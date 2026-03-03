/**
 * @file camera_type2.mm
 * @brief macOS 平台摄像头枚举实现（AVFoundation）
 *
 * 使用 AVCaptureDeviceDiscoverySession 发现 BuiltInWideAngleCamera 与
 * External 类型设备，填充 name/deviceType；vendorId/productId/installDate
 * 在 macOS 上不提供，留空。
 */

#include "camera_type2.h"

#ifdef __APPLE__
#import <Foundation/Foundation.h>
#import <AVFoundation/AVFoundation.h>

/** 弱引用：旧版 macOS 可能无此符号，需在运行时检查 &AVCaptureDeviceTypeExternal != NULL */
extern AVCaptureDeviceType const AVCaptureDeviceTypeExternal __attribute__((weak_import));
#endif

CameraType::CameraType() {
}

CameraType::~CameraType() {
}

#ifdef __APPLE__
/**
 * 将 ObjC NSString（通过 __bridge void* 传入）转为 std::string(UTF-8)。
 * 传入 nil 时返回空字符串。
 */
std::string CameraType::NSStringToStdString(void *nsString) {
    NSString *str = (__bridge NSString *)nsString;
    if (!str) return "";
    return std::string([str UTF8String]);
}
#endif

/**
 * 枚举当前可用的视频采集设备：在 @autoreleasepool 内创建发现会话，
 * 设备类型包含内置广角；macOS 10.15+ 且符号存在时加入外部设备类型。
 * 每个 AVCaptureDevice 映射为一个 CameraDevice（name、deviceType 有值；
 * vendorId/productId/installDate 在 AVFoundation 中无对应 API，置空）。
 */
std::vector<CameraDevice> CameraType::getCameraDevices() {
    std::vector<CameraDevice> devices;

#ifdef __APPLE__
    @autoreleasepool {
        NSMutableArray *deviceTypes = [NSMutableArray arrayWithObject:AVCaptureDeviceTypeBuiltInWideAngleCamera];

        /* macOS 10.15+ 且 SDK 提供 External 类型时，一并枚举外接摄像头 */
        if (@available(macOS 10.15, *)) {
            if (&AVCaptureDeviceTypeExternal != NULL) {
                [deviceTypes addObject:AVCaptureDeviceTypeExternal];
            }
        }

        AVCaptureDeviceDiscoverySession *discoverySession = [AVCaptureDeviceDiscoverySession
            discoverySessionWithDeviceTypes:deviceTypes
            mediaType:AVMediaTypeVideo
            position:AVCaptureDevicePositionUnspecified];

        NSArray<AVCaptureDevice *> *cameras = discoverySession.devices;

        for (AVCaptureDevice *camera in cameras) {
            CameraDevice device;

            device.name = NSStringToStdString((__bridge void *)camera.localizedName);
            device.deviceType = NSStringToStdString((__bridge void *)camera.deviceType);
            device.vendorId = "";
            device.productId = "";
            device.installDate = "";

            devices.push_back(device);
        }
    }
#endif

    return devices;
}