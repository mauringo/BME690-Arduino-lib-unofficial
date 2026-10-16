/**
 * Copyright (C) 2021 Bosch Sensortec GmbH
 * Modifications (C) 2026 Mauro Riboni (@mauringo)
 * SPDX-License-Identifier: BSD-3-Clause
 *
 * My personal hobby project, made for fun and learning. Unofficial;
 * not affiliated with, endorsed by, or supported by Bosch Sensortec.
 * Original Bosch credits and licenses are retained. Provided as-is.
 * Full Polverine examples: https://github.com/mauringo/Polverine_Arduino_Examples
 */
// START HERE: wire the module, choose its address, then edit ESP32 pins below.
// This example reads raw physical values; it does not calculate IAQ.
#include <Arduino.h>
#include <Wire.h>
#include <bme69xLibrary.h>

// Choose the I2C ADDRESS separately from the GPIO pins.
// Most modules use 0x76; change to BME69X_I2C_ADDR_HIGH for a module at 0x77.
constexpr uint8_t SENSOR_ADDRESS = BME69X_I2C_ADDR_LOW;

#if defined(ARDUINO_ARCH_ESP32)
// SDA/SCL here are the defaults of the BOARD selected in Arduino IDE.
// To use custom ESP32 pins, replace SDA and SCL with your wired GPIO numbers.
// Example: Polverine -> 14, 21; generic classic ESP32 -> commonly 21, 22.
// ESP32-S3/C3 boards can use different pins. Check your board's pinout.
constexpr int I2C_SDA_PIN = SDA;
constexpr int I2C_SCL_PIN = SCL;
#endif

void startI2c() {
#if defined(ARDUINO_ARCH_ESP32)
    // Only the ESP32 branch uses this pin-selecting overload.
    if (!Wire.begin(I2C_SDA_PIN, I2C_SCL_PIN)) {
        Serial.println("Could not start I2C. Check the ESP32 GPIO choices.");
        while (true) delay(1000);
    }
#else
    // Other Arduino cores: use the board's labeled SDA and SCL pins.
    // Do NOT pass ESP32 GPIO numbers here. Wire.begin(address) on some cores
    // configures an I2C SLAVE address, not the SDA pin!
    // For an alternate bus/pin mapping, follow that board core's Wire API.
    Wire.begin();
#endif
    Wire.setClock(100000); // Conservative 100 kHz bus speed.
    // Start Wire once, then pass that same object to the sensor library.
    // A board with Wire1 can use that object instead, if its core provides it.
}

Bme69x sensor;
constexpr uint16_t HEATER_DURATION_MS = 100;

void setup() {
    Serial.begin(115200);
    const uint32_t started = millis();
    while (!Serial && millis() - started < 3000) delay(10);
    startI2c();

    sensor.begin(SENSOR_ADDRESS, Wire); // Attach to the bus already started above.
    if (sensor.status < BME69X_OK) {
        Serial.print("Sensor initialization failed: ");
        Serial.println(sensor.statusString());
        while (true) delay(1000); // Fix the wiring/address and reset to retry.
    }
    sensor.setTPH(); // Default temperature, pressure and humidity oversampling.
    if (sensor.status >= BME69X_OK) sensor.setHeaterProf(300, HEATER_DURATION_MS);
    if (sensor.status < BME69X_OK) {
        Serial.println(sensor.statusString());
        while (true) delay(1000);
    }
    Serial.println("temperature_C,pressure_Pa,humidity_percent,gas_ohm,gas_valid");
}

void loop() {
    sensor.setOpMode(BME69X_FORCED_MODE); // Trigger ONE measurement.
    if (sensor.status < BME69X_OK) { Serial.println(sensor.statusString()); delay(3000); return; }

    // getMeasDur() is microseconds for T/P/H. Add the heater's duration in ms.
    // delay() also yields to the ESP32 scheduler; no long busy wait is needed.
    delay((sensor.getMeasDur() + 999) / 1000 + HEATER_DURATION_MS);
    if (sensor.fetchData()) {
        bme69xData data;
        sensor.getData(data);
        Serial.print(data.temperature); Serial.print(',');
        Serial.print(data.pressure); Serial.print(',');
        Serial.print(data.humidity); Serial.print(',');
        Serial.print(data.gas_resistance); Serial.print(',');
        // A number alone does not guarantee a usable gas reading.
        const bool gasValid = (data.status & BME69X_GASM_VALID_MSK) &&
                              (data.status & BME69X_HEAT_STAB_MSK);
        Serial.println(gasValid ? "true" : "false");
    } else if (sensor.status < BME69X_OK) {
        Serial.println(sensor.statusString());
    }
    delay(3000); // Simple raw example; BSEC scheduling is different (see its example).
}
