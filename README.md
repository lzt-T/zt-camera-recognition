# zt-camera-recognition

跨平台摄像头设备识别库，支持 Windows 和 macOS，基于原生 C++ 实现，提供设备名称、厂商 ID、产品 ID、安装日期等底层硬件信息。

[![npm version](https://img.shields.io/npm/v/zt-camera-recognition.svg)](https://www.npmjs.com/package/zt-camera-recognition)
[![License: MIT](https://img.shields.io/badge/License-MIT-yellow.svg)](https://opensource.org/licenses/MIT)

## 功能特性

- 跨平台支持：Windows 和 macOS
- 获取摄像头设备完整信息（名称、厂商 ID、产品 ID、安装日期等）
- 获取系统安装日期（Windows 专有），可用于判断摄像头是否为内置设备
- 原生 C++ 实现，性能优越
- 预构建二进制文件，自动从 GitHub Releases 下载，无需本地编译环境
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

> **pnpm 用户注意**：pnpm v10 默认阻止包运行安装脚本。推荐在项目的 `package.json` 中添加以下配置，让 pnpm 在安装时自动下载预编译文件：
>
> ```json
> {
>   "pnpm": {
>     "onlyBuiltDependencies": ["zt-camera-recognition"]
>   }
> }
> ```
>
> 若未添加此配置，本库也会在首次 `require()` 时自动从 GitHub Releases 下载预编译文件。

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

### `getSystemInstallDate()`

获取操作系统的安装日期。

**返回值：** `string`

- **Windows**：返回系统安装日期，格式 `YYYY-MM-DD HH:mm:ss`
- **macOS**：返回空字符串（不支持）

**使用场景：**

结合设备的 `installDate` 和系统安装日期，可以判断摄像头是否为内置设备：

```javascript
const camera = require('zt-camera-recognition');

const systemInstallDate = camera.getSystemInstallDate();
const devices = camera.getCameraDevices();

devices.forEach((device) => {
  if (device.installDate && systemInstallDate) {
    const isBuiltIn = device.installDate <= systemInstallDate;
    console.log(`${device.name}: ${isBuiltIn ? '内置' : '外接'}`);
  }
});
```

> **原理说明：** OEM 品牌电脑在工厂预装系统时会同时安装内置摄像头驱动，因此内置摄像头的安装日期通常早于或等于系统安装日期；而外接摄像头是用户后续插入的，安装日期晚于系统安装日期。

## 平台支持

| 系统 | 架构 | 状态 |
|------|------|------|
| Windows | x64 | ✅ |
| Windows | ARM64 | ✅ |
| macOS | x64 | ✅ |
| macOS | ARM64（Apple Silicon）| ✅ |

**Node.js 版本：** >= 14.0.0

## 预构建二进制文件

本库使用基于 [Node-API](https://nodejs.org/api/n-api.html) 的预构建二进制文件，一个文件兼容所有 Node.js 版本及 Electron，无需为不同运行时单独构建。

**处理优先级：**

1. 安装时运行 `prebuild-install` 从 GitHub Releases 下载（npm / yarn / pnpm + onlyBuiltDependencies）
2. 若安装脚本未执行，首次 `require()` 时自动从 GitHub Releases 下载
3. 下载失败则抛出明确的错误提示

## 在 Electron 中使用

本库基于 Node-API（NAPI），预编译文件天然兼容所有 Electron 版本，**只能在主进程中使用**，渲染进程可通过 IPC 调用主进程来获取数据。

### pnpm + electron-builder 配置

使用 pnpm 的 Electron 项目，在 `package.json` 中添加：

```json
{
  "pnpm": {
    "onlyBuiltDependencies": ["zt-camera-recognition"]
  }
}
```

这样 pnpm 安装时会执行 install 脚本，提前下载好预编译文件。由于发布包中不含 `binding.gyp`，`@electron/rebuild` 不会将其识别为需要重编译的模块，从而避免重编译报错。

## 从源码构建

如需从源码编译，需提前安装编译工具：

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

## 常见问题

**Q：安装后运行报错 `Could not locate the bindings file`？**

A：预编译文件下载失败。检查网络是否能访问 GitHub，或参考"从源码构建"安装编译工具后重新安装。

**Q：pnpm 安装时提示 `Ignored build scripts`？**

A：这是 pnpm v10 的安全机制。在项目 `package.json` 中添加 `onlyBuiltDependencies` 配置（见上方"pnpm 用户注意"），重新安装后即可在安装阶段完成下载。

**Q：在 Electron 项目中 `electron-builder install-app-deps` 报错 `node-gyp failed to rebuild`？**

A：`@electron/rebuild` 在扫描到 `binding.gyp` 时会尝试重编译。本库发布包中不含 `binding.gyp`，升级到最新版本后此问题不再出现。同时确保在 `package.json` 中添加 `onlyBuiltDependencies` 配置，让安装阶段提前下载好预编译文件。

## License

[MIT](./LICENSE) © lzt-T
