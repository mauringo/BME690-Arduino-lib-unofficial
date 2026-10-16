/**
 * Copyright (C) 2021 Bosch Sensortec GmbH
 * Modifications (C) 2026 Mauro Riboni
 * SPDX-License-Identifier: BSD-3-Clause
 */
#include <Wire.h>
#include <bme69xLibrary.h>
Bme69x sensor;
bool ready = false;
void setup() {
    Serial.begin(115200);
    const uint32_t start = millis();
    while (!Serial && millis() - start < 3000) delay(10);
    Wire.begin(14, 21, 100000); // Polverine ESP32-S3
    sensor.begin(BME69X_I2C_ADDR_LOW, Wire);
    if (sensor.status < BME69X_OK) { Serial.println(sensor.statusString()); return; }
    sensor.setTPH();
    sensor.setHeaterProf(300, 100);
    ready = sensor.status >= BME69X_OK;
}
void loop() {
    if (!ready) { delay(1000); return; }
    sensor.setOpMode(BME69X_FORCED_MODE);
    delay((sensor.getMeasDur() + 999) / 1000);
    bme69xData data;
    if (sensor.fetchData()) {
        sensor.getData(data);
        Serial.print("Temperature (C): "); Serial.println(data.temperature);
        Serial.print("Pressure (Pa): "); Serial.println(data.pressure);
        Serial.print("Humidity (%): "); Serial.println(data.humidity);
        Serial.print("Gas resistance (ohm): "); Serial.println(data.gas_resistance);
    }
    delay(3000);
}
