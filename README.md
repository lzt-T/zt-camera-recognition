# zt-camera-recognition

跨平台摄像头设备识别库，支持 Windows 和 macOS，基于原生 C++ 实现，提供设备名称、厂商 ID、产品 ID、安装日期等底层硬件信息。

[![npm version](https://img.shields.io/npm/v/zt-camera-recognition.svg)](https://www.npmjs.com/package/zt-camera-recognition)
[![License: MIT](https://img.shields.io/badge/License-MIT-yellow.svg)](https://opensource.org/licenses/MIT)

## 功能特性

- 跨平台支持：Windows 和 macOS
- 获取摄像头设备完整信息（名称、厂商 ID、产品 ID、安装日期等）
- 原生 C++ 实现，性能优越
- 预构建二进制文件，首次使用时自动下载，无需本地编译环境
- 兼容所有主流包管理器（npm / yarn / pnpm）
- 支持 Node.js 14+，兼容所有版本（基于 Node-API）

## 安装

```bash
npm install zt-camera-recognition
# 或
yarn add zt-camera-recognition
# 或
pnpm add zt-camera-recognition
```

> **pnpm 用户注意**：pnpm v10 默认阻止包运行安装脚本。本库已内置自动处理机制，首次 `require()` 时会自动从 GitHub Releases 下载预编译文件，无需任何额外配置。

## 使用方法

### CommonJS

```javascript
const camera = require('zt-camera-recognition');

const devices = camera.getCameraDevices();

console.log(`发现 ${devices.length} 个摄像头设备：`);

devices.forEach((device) => {
  console.log(`- ${device.name}`);
  console.log(`  厂商 ID:   ${device.vendorId || '未知'}`);
  console.log(`  产品 ID:   ${device.productId || '未知'}`);
  console.log(`  安装日期: ${device.installDate || '未知'}`);
  console.log(`  设备类型: ${device.deviceType || '未知'}`);
});
```

### TypeScript / ESM

```typescript
import camera from 'zt-camera-recognition';

const devices: camera.CameraDevice[] = camera.getCameraDevices();

devices.forEach((device) => {
  console.log(device.name);
});
```

### 输出示例（Windows）

```
发现 2 个摄像头设备：
- ASUS FHD Webcam
  厂商 ID:   3277
  产品 ID:   0096
  安装日期: 2025-04-30 18:53:02
  设备类型: 未知
- OBS Virtual Camera
  厂商 ID:   未知
  产品 ID:   未知
  安装日期: 未知
  设备类型: 未知
```

## API

### `getCameraDevices()`

获取当前系统上所有摄像头设备的信息。

**返回值：** `CameraDevice[]`

**类型定义：**

```typescript
interface CameraDevice {
  name: string;         // 设备名称
  vendorId: string;     // 厂商 ID（Windows 返回十进制字符串，macOS 不适用）
  productId: string;    // 产品 ID（Windows 返回十进制字符串，macOS 不适用）
  installDate: string;  // 安装日期，格式 YYYY-MM-DD HH:mm:ss（Windows 专有）
  deviceType: string;   // 设备类型（macOS 专有）
}
```

**平台字段差异：**

| 字段 | Windows | macOS |
|------|---------|-------|
| `name` | ✅ | ✅ |
| `vendorId` | ✅ | — |
| `productId` | ✅ | — |
| `installDate` | ✅ | — |
| `deviceType` | — | ✅ |

## 平台支持

| 系统 | 架构 | 状态 |
|------|------|------|
| Windows | x64 | ✅ |
| Windows | ARM64 | ✅ |
| macOS | x64 | ✅ |
| macOS | ARM64（Apple Silicon）| ✅ |

**Node.js 版本：** >= 14.0.0

## 预构建二进制文件

本库使用基于 [Node-API](https://nodejs.org/api/n-api.html) 的预构建二进制文件，一个文件兼容所有 Node.js 版本，无需为不同运行时单独构建。

**安装时的处理优先级：**

1. 运行 `prebuild-install` 从 GitHub Releases 下载（npm / yarn 用户，安装时完成）
2. 首次 `require()` 时自动从 GitHub Releases 下载并解压（pnpm v10 用户的兜底方案）
3. 如果下载失败，尝试从源码本地编译
4. 编译也失败则抛出明确的错误提示

若需手动触发编译：

```bash
# 需要本地编译环境（见下方"从源码构建"）
node-gyp rebuild
```

## 从源码构建

如果预构建文件下载失败，可手动从源码编译，需提前安装编译工具：

- **Windows**：[Visual Studio 2019+](https://visualstudio.microsoft.com/)（含 C++ 工作负载）和 Python 3.x
- **macOS**：Xcode Command Line Tools（`xcode-select --install`）

```bash
git clone https://github.com/lzt-T/zt-camera-recognition.git
cd zt-camera-recognition
pnpm install
pnpm run compile
```

## 维护者发布流程

### 1. 更新版本号

```bash
# 修改 package.json 中的 version 字段，例如改为 1.0.8
```

### 2. 发布到 npm

```bash
pnpm publish
```

### 3. 在各平台构建预编译文件

分别在以下环境中执行：

| 环境 | 产物 |
|------|------|
| Windows x64 | `zt-camera-recognition-v{version}-napi-v6-win32-x64.tar.gz` |
| macOS x64 | `zt-camera-recognition-v{version}-napi-v6-darwin-x64.tar.gz` |
| macOS ARM64 | `zt-camera-recognition-v{version}-napi-v6-darwin-arm64.tar.gz` |

```bash
pnpm run prebuild:all
```

### 4. 上传到 GitHub Releases

在 [GitHub Releases](https://github.com/lzt-T/zt-camera-recognition/releases) 创建新 Release（Tag 格式：`v{version}`），将 `prebuilds/` 目录下所有 `.tar.gz` 文件上传。

> 若某平台的预编译文件暂未上传，该平台用户首次使用时会自动回退到本地源码编译。

## 常见问题

**Q：安装后运行报错 `Could not locate the bindings file`？**

A：预编译文件下载失败且本地编译环境不满足。检查网络是否能访问 GitHub，或参考"从源码构建"安装编译工具后重新安装。

**Q：pnpm 安装时提示 `Ignored build scripts`？**

A：这是 pnpm v10 的安全机制，不影响使用。本库在首次 `require()` 时会自动处理，无需任何额外操作。

**Q：能在浏览器或 Electron 渲染进程中使用吗？**

A：不能在浏览器中使用（原生 C++ 插件无法在沙箱环境运行）。Electron 主进程中可以正常使用，渲染进程可通过 IPC 调用主进程来获取数据。

## License

[MIT](./LICENSE) © lzt-T
