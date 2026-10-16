# BME690_unofficial

Unofficial Arduino framework library maintained by **Mauro Riboni** ([@mauringo](https://github.com/mauringo), 58341265+mauringo@users.noreply.github.com).

## Origin and personal-project disclaimer

This library started from the official [Bosch Sensortec BME68x Arduino library](https://github.com/boschsensortec/Bosch-BME68x-Library) and was adapted by **Mauro Riboni (@mauringo)** for BME690.

**This is my personal hobby project, made privately for fun and learning.** It is shared as unofficial experimental work, not as an official Bosch product. I am not affiliated with Bosch Sensortec, and this project is not endorsed, certified, or supported by Bosch. No promise of reliability, fitness for a particular purpose, continued maintenance, or support is made; use it at your own risk under the applicable licenses.

Bosch Sensortec remains the author of the original libraries. Original copyright notices and licenses are retained; my changes do not imply ownership of Bosch's work.

## Start with the generic I2C example

In Arduino IDE choose **File → Examples → BME690_unofficial → generic_i2c**. This is the recommended first example. It explains every setup step and prints temperature (°C), pressure (Pa), relative humidity (%), gas resistance (Ω), and whether the gas reading is valid.

## Install

Clone or copy this repository directly to `<your Arduino sketchbook>/libraries/BME690_unofficial`, then restart Arduino IDE. The root contains `library.properties`, `src/` and `examples/`; do not use the old nested `BME69x_Sensor_library` folder. No proprietary binary or extra Arduino library is needed for raw readings.

For PlatformIO:

```ini
framework = arduino
lib_deps = https://github.com/mauringo/BME690-Arduino-lib-unofficial.git
```

## Choose I2C pins and bus

The library uses the `TwoWire` object you pass it. **Your sketch starts the bus and selects its pins before calling `sensor.begin(address, Wire)`.**

| Board | Start the bus | Pin choice |
| --- | --- | --- |
| ESP32 family | `Wire.begin(sdaPin, sclPin)` | Set the two constants in `generic_i2c.ino` to your wired GPIOs |
| ESP32 with board defaults | `Wire.begin()` | Uses the selected board variant's SDA/SCL defaults |
| Other Arduino cores | `Wire.begin()` | Wire to the board's labeled SDA/SCL pins; consult its pinout |

The generic sketch's ESP32 constants initially use `SDA` and `SCL`, not hardcoded GPIO numbers. Classic ESP32 boards commonly use GPIO21/GPIO22; these are not universal ESP32-family defaults. Polverine specifically uses **SDA 14, SCL 21**. ESP32-S3/C3 boards may use other pins.

On other cores, do not copy the ESP32 overload blindly: `Wire.begin(address)` can start a slave device. If a board offers alternate pins, configure them with that core's documented API. If it provides a second bus (`Wire1`), start that object and pass the same object to `sensor.begin()`. Not all boards have a second I2C controller.

Example for Polverine/ESP32:

```cpp
Wire.begin(14, 21);
Wire.setClock(100000);
sensor.begin(BME69X_I2C_ADDR_LOW, Wire);
```

`0x76`/`0x77` are the **sensor address**, not GPIO numbers. Use `BME69X_I2C_ADDR_LOW` or `BME69X_I2C_ADDR_HIGH` to match the module's address selection. Use suitable sensor supply/logic levels and I2C pull-ups; a bare BME690 must not be assumed to tolerate a 5 V Arduino bus. A breakout may provide regulation/level shifting—check its documentation.

## Measurement flow

1. `begin()` initializes the sensor on the bus already configured by your sketch.
2. `setTPH()` selects oversampling; `setHeaterProf(300, 100)` selects 300 °C for 100 ms.
3. `setOpMode(BME69X_FORCED_MODE)` starts one measurement.
4. Wait for `getMeasDur()` **plus the heater duration**. The former is in microseconds; the heater setting is milliseconds.
5. `fetchData()` and `getData()` retrieve the result. Check gas-valid and heater-stable flags before interpreting gas resistance.

Negative `sensor.status` values are errors; positive values are warnings. If initialization fails, check wiring, the board selection, and the module's I2C address. The example stops until reset instead of reading an uninitialized sensor. Serial waiting is bounded so it can run without an attached monitor.

## Examples and full project

- `generic_i2c`: portable starting point with explicit ESP32/default-pin branches.
- `forced_mode_i2c_custompins`: compact Polverine example with GPIO14/GPIO21.
- `forced_mode`: SPI example; use your board's SPI pins and a suitable chip-select.

For IAQ install [BSEC3_unofficial](https://github.com/mauringo/Bsec3-ArduinoLibrary-Unofficial). For both sensors, BlackIoT compensation tables and Serial JSON, see [Polverine Arduino Examples](https://github.com/mauringo/Polverine_Arduino_Examples) and [its Arduino IDE sketch](https://github.com/mauringo/Polverine_Arduino_Examples/tree/main/Arduino_project).

## Version 1.4.1

Adds the commented generic I2C example, clearer setup instructions and project links. Fixes heater waiting in raw examples and keeps a bounded Serial wait. The raw Arduino driver has no Bosch algorithm binary dependency. Compilation checks are documented with the release; physical communication and measurement quality still require hardware validation.

Pin API references: [Espressif Arduino I2C](https://docs.espressif.com/projects/arduino-esp32/en/latest/api/i2c.html), [Arduino Wire.begin](https://docs.arduino.cc/language-reference/en/functions/communication/wire/begin/).

Verified generic-example compilation: **ESP32-S3 with ESP32 core 3.3.11, and Arduino Uno with AVR core 1.8.7**. These checks did not upload firmware or validate physical measurements.
