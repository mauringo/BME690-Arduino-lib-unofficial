# BME690_unofficial

Unofficial Arduino framework library maintained by **Mauro Riboni** ([@mauringo](https://github.com/mauringo), 58341265+mauringo@users.noreply.github.com).

Based on the Bosch Sensortec Arduino BME68x wrapper and BME69x SensorAPI. Bosch copyright notices and licenses are retained. This project is independent of Bosch Sensortec.

## Install

Clone this repository directly as `Arduino/libraries/BME690_unofficial`. The repository root now contains `library.properties`, `src/`, and `examples/`; do not copy the former nested `BME69x_Sensor_library` directory.

For PlatformIO with `framework = arduino`:

```ini
lib_deps = https://github.com/mauringo/BME690-Arduino-lib-unofficial.git
```

Include `<bme69xLibrary.h>`. The existing `Bme69x` API is preserved. For Polverine, call `Wire.begin(14, 21, 100000)` and `sensor.begin(BME69X_I2C_ADDR_LOW, Wire)` (address `0x76`). See `examples/forced_mode_i2c_custompins/`.

No proprietary binary is needed for raw BME690 readings. IAQ requires the separate [BSEC3_unofficial library](https://github.com/mauringo/Bsec3-ArduinoLibrary-Unofficial).

## Compatibility

Compiled with PlatformIO Arduino ESP32 2.0.17 on ESP32-S3 (Polverine). Other Arduino architectures retain the upstream transport implementation but have not been validated in this release. I2C short reads now return a communication error instead of reporting success with incomplete data. Hardware measurements still require board validation.

Version 1.4.0: installable repository layout, Arduino/PlatformIO metadata and attribution, corrected example extension, I2C short-read detection, and SPI transactions that preserve const write buffers.
