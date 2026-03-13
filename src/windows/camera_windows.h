/**
 * @file camera_windows.h
 * @brief Windows 平台摄像头枚举接口声明
 *
 * 使用 DirectShow (dshow.h) 枚举视频输入设备，使用 SetupAPI + DevPkey
 * 获取设备安装日期等扩展属性。依赖 COM 与 setupapi.lib。
 */

#ifndef CAMERA_WINDOWS_H
#define CAMERA_WINDOWS_H

#include "../common/camera_common.h"
#include <windows.h>
#include <dshow.h>
#include <atlbase.h>
#include <setupapi.h>
#include <initguid.h>
#include <devpkey.h>
#include <wbemidl.h>
#include <comdef.h>

#pragma comment(lib, "setupapi.lib")
#pragma comment(lib, "wbemuuid.lib")

/** 视频设备接口类 GUID（用于 SetupDiGetClassDevsW 枚举视频类设备） */
DEFINE_GUID(GUID_DEVINTERFACE_VIDEO, 0x65E8773D, 0x8F56, 0x11D0, 0xA3, 0xB9, 0x00, 0xA0, 0xC9, 0x22, 0x31, 0x96);

/**
 * @struct AdvancedProperties
 * @brief 通过 SetupAPI 获取的设备扩展属性（当前仅安装日期）
 */
struct AdvancedProperties {
    std::string installDate;  /**< 设备安装日期，格式为 "YYYY-MM-DD HH:MM:SS" */
};

/**
 * @class CameraWindows
 * @brief Windows 下 CameraInterface 的实现：DirectShow + SetupAPI
 */
class CameraWindows : public CameraInterface {
public:
    CameraWindows();
    virtual ~CameraWindows();
    virtual std::vector<CameraDevice> getCameraDevices() override;
    virtual std::string getSystemInstallDate() override;

private:
    /** 将宽字符串转为 UTF-8 的 std::string，空指针返回 "" */
    std::string WCharToChar(const WCHAR* wstr);
    /** 将 FILETIME 转为本地时间的可读字符串 */
    std::string FileTimeToString(const FILETIME& ft);
    /** 根据设备路径在 SetupAPI 中查找并填充安装日期等属性 */
    AdvancedProperties GetAdvancedProperties(const std::string& devicePath);
};

#endif
