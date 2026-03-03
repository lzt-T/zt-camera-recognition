export interface CameraDevice {
  name: string;
  vendorId: string;
  productId: string;
  installDate: string;
  deviceType: string;
}

export function getCameraDevices(): CameraDevice[];
