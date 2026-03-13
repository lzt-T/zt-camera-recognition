/**
 * @file camera_common.cc
 * @brief 摄像头 N-API 绑定与跨平台调度实现
 *
 * 负责：通过 createCameraInterface() 获取平台实现，调用 getCameraDevices()
 * 并将 C++ 的 CameraDevice 向量转换为 N-API 数组导出给 Node.js。
 */

#include "camera_common.h"
#include <memory>

/**
 * 获取当前系统所有视频输入设备，并转换为 N-API 数组。
 * 异常时向 JS 抛出错误并返回空数组（调用方仍会收到异常）。
 */
Napi::Array GetCameraDevices(const Napi::CallbackInfo &info)
{
    Napi::Env env = info.Env();
    Napi::Array result = Napi::Array::New(env);

    try
    {
        std::unique_ptr<CameraInterface> cameraInterface(createCameraInterface());
        if (!cameraInterface)
        {
            Napi::Error::New(env, "Failed to create camera interface").ThrowAsJavaScriptException();
            return result;
        }

        std::vector<CameraDevice> devices = cameraInterface->getCameraDevices();
        result = Napi::Array::New(env, static_cast<uint32_t>(devices.size()));

        for (size_t i = 0; i < devices.size(); ++i)
        {
            Napi::Object deviceObj = Napi::Object::New(env);
            deviceObj.Set("name", Napi::String::New(env, devices[i].name));
            deviceObj.Set("vendorId", Napi::String::New(env, devices[i].vendorId));
            deviceObj.Set("productId", Napi::String::New(env, devices[i].productId));
            deviceObj.Set("installDate", Napi::String::New(env, devices[i].installDate));
            deviceObj.Set("deviceType", Napi::String::New(env, devices[i].deviceType));
            result.Set(i, deviceObj);
        }
    }
    catch (const std::exception &e)
    {
        Napi::Error::New(env, "Error getting camera devices: " + std::string(e.what())).ThrowAsJavaScriptException();
        return result;
    }
    catch (...)
    {
        Napi::Error::New(env, "Unknown error occurred").ThrowAsJavaScriptException();
        return result;
    }

    return result;
}

/**
 * 获取操作系统安装日期，返回 N-API 字符串。
 * Windows 返回格式化的日期字符串，macOS 返回空字符串。
 */
Napi::String GetSystemInstallDate(const Napi::CallbackInfo &info)
{
    Napi::Env env = info.Env();

    try
    {
        std::unique_ptr<CameraInterface> cameraInterface(createCameraInterface());
        if (!cameraInterface)
        {
            return Napi::String::New(env, "");
        }

        std::string installDate = cameraInterface->getSystemInstallDate();
        return Napi::String::New(env, installDate);
    }
    catch (const std::exception &e)
    {
        Napi::Error::New(env, "Error getting system install date: " + std::string(e.what())).ThrowAsJavaScriptException();
        return Napi::String::New(env, "");
    }
    catch (...)
    {
        Napi::Error::New(env, "Unknown error occurred").ThrowAsJavaScriptException();
        return Napi::String::New(env, "");
    }
}

/**
 * N-API 模块入口：将 getCameraDevices 和 getSystemInstallDate 注册到 exports。
 */
Napi::Object Init(Napi::Env env, Napi::Object exports)
{
    exports.Set(Napi::String::New(env, "getCameraDevices"),
                Napi::Function::New(env, GetCameraDevices));
    exports.Set(Napi::String::New(env, "getSystemInstallDate"),
                Napi::Function::New(env, GetSystemInstallDate));
    return exports;
}

/** 声明本模块名为 "camera"，初始化函数为 Init */
NODE_API_MODULE(camera, Init)
