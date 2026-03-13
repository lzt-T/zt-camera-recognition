export interface CameraDevice {
  name: string;
  vendorId: string;
  productId: string;
  installDate: string;
  deviceType: string;
}

export function getCameraDevices(): CameraDevice[];

/**
 * 获取操作系统安装日期
 * @returns Windows 返回格式化的日期字符串 (YYYY-MM-DD HH:mm:ss)，macOS 返回空字符串
 */
export function getSystemInstallDate(): string;
