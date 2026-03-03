# md-camera-recognition

跨平台摄像头设备识别库 - 支持 Windows 和 macOS

## 功能特性

- ✅ 跨平台支持：Windows 和 macOS
- ✅ 获取摄像头设备信息（名称、厂商ID、产品ID、安装日期等）
- ✅ 原生 C++ 性能优化
- ✅ 简单一行 API 调用
- ✅ 预构建二进制文件，快速安装

## 安装

```bash
npm install md-camera-recognition
# 或
pnpm install md-camera-recognition
```

## 使用方法

```javascript
const camera = require('md-camera-recognition');

const devices = camera.getCameraDevices();

console.log(`发现 ${devices.length} 个摄像头设备：`);

devices.forEach((device) => {
  console.log(`- ${device.name}`);
  console.log(`  厂商 ID: ${device.vendorId || '未知'}`);
  console.log(`  产品 ID: ${device.productId || '未知'}`);
  console.log(`  安装日期: ${device.installDate || '未知'}`);
  console.log(`  设备类型: ${device.deviceType || '未知'}`);
});
```

## API

### getCameraDevices()

返回摄像头设备对象数组。

**返回值:** `Array<CameraDevice>`

**CameraDevice 对象:**
```typescript
interface CameraDevice {
  name: string;           // 设备名称
  vendorId: string;       // 厂商 ID（十六进制，Windows）
  productId: string;      // 产品 ID（十六进制，Windows）
  installDate: string;    // 安装日期（Windows）
  deviceType: string;     // 设备类型（macOS）
}
```

## 平台差异

- **Windows**: 返回 `vendorId`, `productId`, `installDate`
- **macOS**: 返回 `deviceType`

## TypeScript 支持

本库提供 TypeScript 类型定义：

```typescript
import camera from 'md-camera-recognition';

const devices: camera.CameraDevice[] = camera.getCameraDevices();
```

## 平台支持

| 系统 | 架构 | 状态 |
|------|------|------|
| Windows | x64, ARM64 | ✅ |
| macOS | x64, ARM64 | ✅ |

## 预构建二进制文件

本库使用预构建二进制文件，无需编译环境即可安装。

如果预构建文件下载失败，会自动回退到源码编译，需要：
- **Windows**: Visual Studio 2019+ 和 Python 3.x
- **macOS**: Xcode Command Line Tools

## 从源码构建

```bash
git clone https://github.com/YOUR_USERNAME/md-camera-recognition.git
cd md-camera-recognition
pnpm install
pnpm run build
```

## 预构建流程（维护者）

### 1. 在各平台构建预构建文件

**Windows x64:**
```bash
pnpm run prebuild
```

**macOS x64:**
```bash
pnpm run prebuild
```

**macOS ARM64:**
```bash
pnpm run prebuild
```

### 2. 上传到 GitHub Releases

访问 [GitHub Releases](https://github.com/YOUR_USERNAME/md-camera-recognition/releases)，创建新版本并上传所有 `prebuilds/` 目录下的 `.tar.gz` 文件。

### 3. 更新 package.json

确保 `binary.host` 指向正确的 GitHub 仓库地址。

### 4. 发布到 npm

```bash
pnpm publish
```

## License

MIT License

## 贡献

欢迎提交 Issue 和 Pull Request！
