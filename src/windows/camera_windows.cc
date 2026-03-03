/**
 * @file camera_windows.cc
 * @brief Windows 平台摄像头枚举实现（DirectShow + SetupAPI）
 *
 * 流程：COM 初始化 → 创建系统设备枚举器 → 枚举视频输入类别 →
 * 从 IPropertyBag 读取 FriendlyName/DevicePath → 从 DevicePath 解析 VID/PID →
 * 通过 SetupAPI 匹配设备并读取 DEVPKEY_Device_InstallDate。
 */

#include "camera_windows.h"
#include <iostream>
#include <stdio.h>

CameraWindows::CameraWindows() {
}

CameraWindows::~CameraWindows() {
}

/**
 * 使用 WideCharToMultiByte(CP_UTF8) 将 Windows 宽字符转为 UTF-8 字符串。
 * 空指针或转换失败返回空字符串。
 */
std::string CameraWindows::WCharToChar(const WCHAR* wstr) {
    if (!wstr) return "";
    int len = WideCharToMultiByte(CP_UTF8, 0, wstr, -1, NULL, 0, NULL, NULL);
    if (len <= 0) return "";
    std::string str(len - 1, 0);
    WideCharToMultiByte(CP_UTF8, 0, wstr, -1, &str[0], len, NULL, NULL);
    return str;
}

/**
 * 将 FILETIME（UTC）转为本地时间的 "YYYY-MM-DD HH:MM:SS" 字符串。
 * 转换失败返回 "Invalid Time" 或 "Invalid Timezone"。
 */
std::string CameraWindows::FileTimeToString(const FILETIME& ft) {
    SYSTEMTIME stUTC, stLocal;
    char buffer[128];

    if (!FileTimeToSystemTime(&ft, &stUTC)) {
        return "Invalid Time";
    }
    if (!SystemTimeToTzSpecificLocalTime(NULL, &stUTC, &stLocal)) {
        return "Invalid Timezone";
    }

    sprintf_s(buffer, sizeof(buffer), "%04d-%02d-%02d %02d:%02d:%02d",
        stLocal.wYear, stLocal.wMonth, stLocal.wDay,
        stLocal.wHour, stLocal.wMinute, stLocal.wSecond);

    return std::string(buffer);
}

/**
 * 枚举 GUID_DEVINTERFACE_VIDEO 设备，按 devicePath 匹配当前设备，
 * 通过 SetupDiGetDevicePropertyW(DEVPKEY_Device_InstallDate) 读取安装日期。
 * 未匹配或无安装日期时返回空 AdvancedProperties。
 */
AdvancedProperties CameraWindows::GetAdvancedProperties(const std::string& devicePath) {
    AdvancedProperties props;
    
    HDEVINFO hDevInfo = SetupDiGetClassDevsW(&GUID_DEVINTERFACE_VIDEO, NULL, NULL, DIGCF_PRESENT | DIGCF_DEVICEINTERFACE);
    if (hDevInfo == INVALID_HANDLE_VALUE) {
        return props;
    }

    SP_DEVICE_INTERFACE_DATA devInterfaceData = { sizeof(SP_DEVICE_INTERFACE_DATA) };
    
    for (DWORD i = 0; SetupDiEnumDeviceInterfaces(hDevInfo, NULL, &GUID_DEVINTERFACE_VIDEO, i, &devInterfaceData); ++i) {
        DWORD detailSize = 0;
        SetupDiGetDeviceInterfaceDetailW(hDevInfo, &devInterfaceData, NULL, 0, &detailSize, NULL);
        
        if (detailSize == 0) continue;
        
        PSP_DEVICE_INTERFACE_DETAIL_DATA_W detailData = (PSP_DEVICE_INTERFACE_DETAIL_DATA_W) new char[detailSize];
        detailData->cbSize = sizeof(SP_DEVICE_INTERFACE_DETAIL_DATA_W);

        SP_DEVINFO_DATA devInfoData = { sizeof(SP_DEVINFO_DATA) };

        if (SetupDiGetDeviceInterfaceDetailW(hDevInfo, &devInterfaceData, detailData, detailSize, NULL, &devInfoData)) {
            std::string currentDevicePath = WCharToChar(detailData->DevicePath);
            
            if (devicePath.find(currentDevicePath) != std::string::npos || currentDevicePath.find(devicePath) != std::string::npos) {
                DEVPROPTYPE propType;
                FILETIME installDate;
                if (SetupDiGetDevicePropertyW(hDevInfo, &devInfoData, &DEVPKEY_Device_InstallDate, &propType, (PBYTE)&installDate, sizeof(installDate), NULL, 0)) {
                    if (propType == DEVPROP_TYPE_FILETIME) {
                        props.installDate = FileTimeToString(installDate);
                    }
                }
                
                delete[] detailData;
                SetupDiDestroyDeviceInfoList(hDevInfo);
                return props;
            }
        }
        delete[] detailData;
    }

    SetupDiDestroyDeviceInfoList(hDevInfo);
    return props;
}

/**
 * 枚举所有视频输入设备：COM 初始化后使用 DirectShow 的
 * CLSID_VideoInputDeviceCategory 枚举器，读取 FriendlyName/DevicePath，
 * 从 DevicePath 解析 vid_/pid_，并调用 GetAdvancedProperties 补全安装日期。
 * 失败时抛出 std::runtime_error 或传播 COM 错误。
 */
std::vector<CameraDevice> CameraWindows::getCameraDevices() {
    std::vector<CameraDevice> devices;

    HRESULT hr = CoInitializeEx(NULL, COINIT_APARTMENTTHREADED);
    if (FAILED(hr)) {
        throw std::runtime_error("Failed to initialize COM");
    }

    try {
        CComPtr<ICreateDevEnum> pDevEnum;
        hr = pDevEnum.CoCreateInstance(CLSID_SystemDeviceEnum, NULL, CLSCTX_INPROC_SERVER);
        if (FAILED(hr)) {
            throw std::runtime_error("Failed to create system device enumerator");
        }

        CComPtr<IEnumMoniker> pEnumCat;
        hr = pDevEnum->CreateClassEnumerator(CLSID_VideoInputDeviceCategory, &pEnumCat, 0);
        if (FAILED(hr) || !pEnumCat) {
            CoUninitialize();
            return devices;
        }

        IMoniker *pMoniker = NULL;
        ULONG cFetched;
        
        while (pEnumCat->Next(1, &pMoniker, &cFetched) == S_OK) {
            CComPtr<IPropertyBag> pPropBag;
            hr = pMoniker->BindToStorage(NULL, NULL, IID_IPropertyBag, (void**)&pPropBag);
            if (SUCCEEDED(hr)) {
                VARIANT var;
                VariantInit(&var);
                
                std::string deviceName, devicePath;

                hr = pPropBag->Read(L"FriendlyName", &var, 0);
                if (SUCCEEDED(hr) && var.vt == VT_BSTR) {
                    deviceName = WCharToChar(var.bstrVal);
                }
                VariantClear(&var);

                hr = pPropBag->Read(L"DevicePath", &var, 0);
                if (SUCCEEDED(hr) && var.vt == VT_BSTR) {
                    devicePath = WCharToChar(var.bstrVal);
                }
                VariantClear(&var);
                
                if (!deviceName.empty()) {
                    CameraDevice device;
                    device.name = deviceName;
                    device.deviceType = "";

                    if (!devicePath.empty()) {
                        size_t vidPos = devicePath.find("vid_");
                        if (vidPos != std::string::npos) {
                            device.vendorId = devicePath.substr(vidPos + 4, 4);
                        }
                        
                        size_t pidPos = devicePath.find("pid_");
                        if (pidPos != std::string::npos) {
                            device.productId = devicePath.substr(pidPos + 4, 4);
                        }

                        AdvancedProperties advancedProps = GetAdvancedProperties(devicePath);
                        device.installDate = advancedProps.installDate;
                    }
                    
                    devices.push_back(device);
                }
            }
            if (pMoniker) {
                pMoniker->Release();
                pMoniker = NULL;
            }
        }
    } catch (...) {
        CoUninitialize();
        throw;
    }

    CoUninitialize();
    return devices;
}


