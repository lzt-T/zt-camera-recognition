const camera = require('../index');

console.log('=== Camera Device Recognition Test ===\n');

try {
  const devices = camera.getCameraDevices();

  console.log(`Found ${devices.length} camera device(s):\n`);

  if (devices.length === 0) {
    console.log('No camera devices found.');
  } else {
    devices.forEach((device, index) => {
      console.log(`--- Device ${index + 1}: ${device.name} ---`);
      console.log(`  Vendor ID: ${device.vendorId || 'Unknown'}`);
      console.log(`  Product ID: ${device.productId || 'Unknown'}`);
      console.log(`  Install Date: ${device.installDate || 'Unknown'}`);
      console.log(`  Device Type: ${device.deviceType || 'Unknown'}`);
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
