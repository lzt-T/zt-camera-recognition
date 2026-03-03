/**
 * @file camera_common.h
 * @brief 摄像头模块公共接口与数据结构定义
 *
 * 本头文件定义跨平台摄像头枚举所需的通用类型与抽象接口，
 * 供 Node.js N-API 绑定与各平台实现（Windows/macOS）共同使用。
 */

#ifndef CAMERA_COMMON_H
#define CAMERA_COMMON_H

#include <napi.h>
#include <string>
#include <vector>

/**
 * @struct CameraDevice
 * @brief 单个摄像头设备的描述信息
 *
 * 与平台无关的设备元数据，用于在 JavaScript 层统一展示。
 */
struct CameraDevice
{
    std::string name;        /**< 设备友好名称（如 "Integrated Camera"） */
    std::string vendorId;   /**< USB 厂商 ID（十六进制字符串，如 "0x0bda"） */
    std::string productId;  /**< USB 产品 ID（十六进制字符串） */
    std::string installDate;/**< 设备安装日期（可读字符串，平台实现可选） */
    std::string deviceType; /**< 设备类型（如 macOS 的 BuiltInWideAngleCamera） */
};

/**
 * @class CameraInterface
 * @brief 摄像头枚举的抽象接口（策略/工厂模式中的产品接口）
 *
 * 各平台通过实现此接口提供本机摄像头列表，
 * 由 platform_factory 在运行时根据编译宏选择具体实现。
 */
class CameraInterface
{
public:
    virtual ~CameraInterface() = default;

    /**
     * @brief 枚举当前系统上可用的视频输入设备
     * @return 设备列表，每个元素为 CameraDevice
     */
    virtual std::vector<CameraDevice> getCameraDevices() = 0;
};

/**
 * @brief 创建当前平台的摄像头接口实例（工厂函数）
 * @return 平台相关的 CameraInterface*，调用方负责 delete
 */
CameraInterface *createCameraInterface();

/**
 * @brief N-API 导出：获取摄像头设备列表（对应 JS 的 getCameraDevices()）
 * @param info N-API 回调信息（未使用参数，保持签名一致）
 * @return 设备对象数组，每项含 name/vendorId/productId/installDate/deviceType
 */
Napi::Array GetCameraDevices(const Napi::CallbackInfo &info);

/**
 * @brief N-API 模块初始化，将 getCameraDevices 挂载到 exports
 * @param env N-API 环境
 * @param exports 模块导出对象
 * @return 填充后的 exports（通常原样返回）
 */
Napi::Object Init(Napi::Env env, Napi::Object exports);

#endif
