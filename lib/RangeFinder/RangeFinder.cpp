#include <Arduino.h>
#include <SoftwareSerial.h>

namespace RangeFinder {
	SoftwareSerial laserSerial(2, 3); //Define software serial, 3 is TX, 2 is RX
	char buff[4] = { (char) 0x80, 0x06, 0x03, 0x77};
	unsigned char data[11] = { 0 };

	uint8_t laserPin = 7; //D7
	bool on = false;

	void begin() {
		laserSerial.begin(9600);
		pinMode(laserPin, OUTPUT);
	}

	bool isOn() {
		return on;
	}

	void turnOn() {
		on = true;
		digitalWrite(laserPin, HIGH);
	}

	void turnOff() {
		on = false;
		digitalWrite(laserPin, LOW);
	}

	float getRange() {
		if (!on) turnOn();

		laserSerial.print(buff);

		float distance = 0;
		while (distance <= 0) {
			/*!
			* @copyright  Copyright (c) 2010 DFRobot Co.Ltd (http://www.dfrobot.com)
			* @licence  The MIT License (MIT)
			* @author  [liunian](nian.liu@dfrobot.com)
			*/

			if (laserSerial.available() > 0) { // Determine whether there is data to read on the serial 
				delay(50);
				for (int i = 0; i < 11; i++) {
					data[i] = laserSerial.read();
				}

				unsigned char Check = 0;
				for (int i = 0; i < 10; i++) {
					Check = Check + data[i];
				}

				Check =~ Check + 1;
				if (data[10] == Check) {
					if (data[3] == 'E' && data[4] == 'R' && data[5] == 'R') {
						Serial.println("Out of range");
						distance = -1;
					} else {
						distance = (data[3] - 0x30) * 100
								+ (data[4] - 0x30) * 10
								+ (data[5] - 0x30) * 1
								+ (data[7] - 0x30) * 0.1
								+ (data[8] - 0x30) * 0.01
								+ (data[9] - 0x30) * 0.001;
					}
				} else {
					Serial.println("Invalid Data!");
					distance = -1;
				}
			} else {
				distance = -1;
			}
			
			delay(100);
		}

		return distance;
	}
}
