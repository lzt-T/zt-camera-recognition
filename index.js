const path = require('path');
const fs = require('fs');
const zlib = require('zlib');
const { spawnSync } = require('child_process');
const bindings = require('bindings');

const bindingDir = path.join(__dirname, 'build', 'Release');
const bindingPath = path.join(bindingDir, 'zt-camera-recognition.node');

if (!fs.existsSync(bindingPath)) {
  ensureBinding();
}

function ensureBinding() {
  const pkg = require('./package.json');
  const name = pkg.binary.module_name;
  const napiVersions = pkg.binary.napi_versions || [6];
  const { version } = pkg;
  const platform = process.platform;
  const arch = process.arch;

  for (const napiVer of napiVersions) {
    const tarName = `${name}-v${version}-napi-v${napiVer}-${platform}-${arch}.tar.gz`;
    const downloadUrl = `${pkg.binary.host}v${version}/${tarName}`;
    fs.mkdirSync(bindingDir, { recursive: true });
    const tmpTarPath = path.join(bindingDir, tarName);

    process.stdout.write(`zt-camera-recognition: downloading prebuilt binary from ${downloadUrl}\n`);
    const ok = downloadFileSync(downloadUrl, tmpTarPath);

    if (ok) {
      extractNodeFromTarGz(tmpTarPath, bindingDir);
      fs.unlinkSync(tmpTarPath);
      return;
    }

    process.stderr.write(`zt-camera-recognition: prebuilt binary not available for ${platform}-${arch} (${downloadUrl})\n`);
  }

  throw new Error(
    `zt-camera-recognition: no prebuilt binary available for ${platform}-${arch} v${require('./package.json').version}.\n` +
    `  - Supported platforms: Windows x64/arm64, macOS x64/arm64\n` +
    `  - Check available releases: ${require('./package.json').binary.host}`
  );
}

function downloadFileSync(url, destPath) {
  const script = `
    const https = require('https');
    const http = require('http');
    const fs = require('fs');

    function download(url, dest) {
      const client = url.startsWith('https') ? https : http;
      client.get(url, (res) => {
        if (res.statusCode === 301 || res.statusCode === 302) {
          download(res.headers.location, dest);
          return;
        }
        if (res.statusCode !== 200) {
          process.exit(1);
        }
        const file = fs.createWriteStream(dest);
        res.pipe(file);
        file.on('finish', () => { file.close(); process.exit(0); });
        file.on('error', () => process.exit(1));
      }).on('error', () => process.exit(1));
    }

    download(${JSON.stringify(url)}, ${JSON.stringify(destPath)});
  `;

  const result = spawnSync(process.execPath, ['-e', script], { timeout: 60000 });
  return result.status === 0;
}

function extractNodeFromTarGz(tarGzPath, destDir) {
  const buffer = fs.readFileSync(tarGzPath);
  const decompressed = zlib.gunzipSync(buffer);

  fs.mkdirSync(destDir, { recursive: true });

  let offset = 0;
  while (offset + 512 <= decompressed.length) {
    const header = decompressed.slice(offset, offset + 512);
    if (header.every(b => b === 0)) break;

    const filename = header.slice(0, 100).toString('utf8').replace(/\0/g, '').trim();
    const sizeStr = header.slice(124, 136).toString('ascii').replace(/\0/g, '').trim();
    const size = parseInt(sizeStr, 8) || 0;

    offset += 512;

    if (filename.endsWith('.node')) {
      const nodeData = decompressed.slice(offset, offset + size);
      fs.writeFileSync(path.join(destDir, path.basename(filename)), nodeData);
    }

    offset += Math.ceil(size / 512) * 512;
  }
}

const camera = bindings('zt-camera-recognition');
module.exports = camera;
