/**
 * @file platform_factory.cc
 * @brief 摄像头接口的平台工厂实现
 *
 * 根据编译目标（_WIN32 / __APPLE__）返回对应的 CameraInterface 实现：
 * - Windows: DirectShow + SetupAPI 枚举（CameraWindows）
 * - macOS: AVFoundation 枚举（CameraType）
 * 其他平台编译时报错 "Unsupported platform"。
 */

#include "camera_common.h"

#ifdef _WIN32
#include "../windows/camera_windows.h"
#elif __APPLE__
#include "../macos/camera_type2.h"
#else
#error "Unsupported platform"
#endif

/**
 * 创建当前平台的摄像头枚举接口实例。
 * 调用方必须对返回的指针执行 delete，避免泄漏。
 */
CameraInterface *createCameraInterface()
{
#ifdef _WIN32
    return new CameraWindows();
#elif __APPLE__
    return new CameraType();
#else
    return nullptr;
#endif
}
