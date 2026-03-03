/**
 * @file camera_type2.h
 * @brief macOS 平台摄像头枚举接口声明
 *
 * 通过 AVFoundation 的 AVCaptureDeviceDiscoverySession 枚举视频设备，
 * 支持内置广角与外部摄像头（macOS 10.15+ 的 AVCaptureDeviceTypeExternal）。
 */

#ifndef CAMERA_TYPE2_H
#define CAMERA_TYPE2_H

#include "../common/camera_common.h"

/**
 * @class CameraType
 * @brief macOS 下 CameraInterface 的实现：基于 AVFoundation
 */
class CameraType : public CameraInterface
{
public:
  CameraType();
  virtual ~CameraType();
  virtual std::vector<CameraDevice> getCameraDevices() override;

private:
#ifdef __APPLE__
  /** 将 (__bridge void*)NSString* 转为 UTF-8 的 std::string，nil 返回 "" */
  std::string NSStringToStdString(void *nsString);
#endif
};

#endif
