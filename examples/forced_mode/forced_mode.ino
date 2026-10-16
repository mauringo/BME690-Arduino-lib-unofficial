/**
 * Copyright (C) 2021 Bosch Sensortec GmbH
 *
 * SPDX-License-Identifier: BSD-3-Clause
 * 
 */

// Mauro Riboni (@mauringo): personal hobby work, made for fun and learning.
// Unofficial; not affiliated with, endorsed by, or supported by Bosch.


// Complete Polverine Arduino + PlatformIO examples, profiles and compensation:
// https://github.com/mauringo/Polverine_Arduino_Examples

#include "Arduino.h"
#include "bme69xLibrary.h"

#ifndef PIN_CS
#define PIN_CS SS
#endif

Bme69x bme;
bool ready = false;

/**
 * @brief Initializes the sensor and hardware settings
 */
void setup(void)
{
	SPI.begin();
	Serial.begin(115200);
	
	const uint32_t serialStarted = millis();
	while (!Serial && millis() - serialStarted < 3000)
		delay(10);
		
	/* initializes the sensor based on SPI library */
	bme.begin(PIN_CS, SPI);

	if(bme.checkStatus())
	{
		if (bme.checkStatus() == BME69X_ERROR)
		{
			Serial.println("Sensor error:" + bme.statusString());
			return;
		}
		else if (bme.checkStatus() == BME69X_WARNING)
		{
			Serial.println("Sensor Warning:" + bme.statusString());
		}
	}
	
	/* Set the default configuration for temperature, pressure and humidity */
	bme.setTPH();
	if (bme.status < BME69X_OK) { Serial.println(bme.statusString()); return; }

	/* Set the heater configuration to 300 deg C for 100ms for Forced mode */
	bme.setHeaterProf(300, 100);

	ready = bme.status >= BME69X_OK;
	Serial.println("TimeStamp(ms), Temperature(deg C), Pressure(Pa), Humidity(%), Gas resistance(ohm), Status");
}

void loop(void)
{
	if (!ready) { delay(1000); return; }
	bme69xData data;

	bme.setOpMode(BME69X_FORCED_MODE);
	delay((bme.getMeasDur() + 999) / 1000 + 100); // T/P/H + 100 ms heater

	if (bme.fetchData())
	{
		bme.getData(data);
		Serial.print(String(millis()) + ", ");
		Serial.print(String(data.temperature) + ", ");
		Serial.print(String(data.pressure) + ", ");
		Serial.print(String(data.humidity) + ", ");
		Serial.print(String(data.gas_resistance) + ", ");
		Serial.println(data.status, HEX);
	}
}
