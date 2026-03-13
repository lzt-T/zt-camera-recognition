const camera = require('../index');

console.log('=== Camera Device Recognition Test ===\n');

try {
  const systemInstallDate = camera.getSystemInstallDate();
  console.log(`System Install Date: ${systemInstallDate || 'Not available (macOS)'}\n`);

  const devices = camera.getCameraDevices();

  console.log(`Found ${devices.length} camera device(s):\n`);

  if (devices.length === 0) {
    console.log('No camera devices found.');
  } else {
    devices.forEach((device, index) => {
      const isBuiltIn = device.installDate && systemInstallDate && device.installDate <= systemInstallDate;
      const deviceCategory = device.installDate 
        ? (isBuiltIn ? 'Built-in' : 'External') 
        : 'Unknown';

      console.log(`--- Device ${index + 1}: ${device.name} ---`);
      console.log(`  Vendor ID: ${device.vendorId || 'Unknown'}`);
      console.log(`  Product ID: ${device.productId || 'Unknown'}`);
      console.log(`  Install Date: ${device.installDate || 'Unknown'}`);
      console.log(`  Device Type: ${device.deviceType || 'Unknown'}`);
      console.log(`  Category: ${deviceCategory}`);
      console.log('');
    });
  }
} catch (error) {
  console.error('Error:', error.message);
  console.error('Stack:', error.stack);
  process.exit(1);
}

console.log('=== Test Complete ===');
process.exit(0);
