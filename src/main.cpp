#include <Arduino.h>
#include <SoftwareSerial.h>
#include <RangeFinder.h>
#include "Accelerometer.h"
#include "Display.h" 

const float MUZZLE_VELOCITY = 39.5; // in m/s

// Controller Pins and Stuff
uint8_t changeScreenPin = 5; //D5
uint8_t confirmInputPin = 6; //D6
int screen = 0; // 0: range finder, 1: height input, 2: firing screen

// Math and stuffs
float firingDistance = 1;
int16_t measuredAngle = 1;
float firingHeight = 1;

float oldFiringDistance = 0;
int16_t oldMeasuredAngle = 0;
float oldFiringHeight = 0;

float calculatedFiringAngle = 0;

String getAnalogHeightInFeet() {
	uint16_t sensorVal = analogRead(A0); // returns value from 0 to 1023
	uint8_t heightInInches = round(sensorVal / 12); // max 85 inches
	uint8_t feet = heightInInches / 12;
	uint8_t inches = heightInInches % 12;

	return (String) feet + "' " + inches + '"'; 
}

float getAngalogHeightInMeters() {
	uint16_t sensorVal = analogRead(A0); // returns value from 0 to 1023
	float heightInMeters = 0.0254 * sensorVal / 12.0; // max 2.165 metes
	return heightInMeters;
}

namespace WeirdBSMagicMath {
	float newtonIteration(float prevAngle, float v_0, float delta_x, float delta_y) {
		float tanAngle = (float) tan(prevAngle);
		float secSquaredAngle = (float) (1 / (cos(prevAngle) * cos(prevAngle)));
		float deltaXOverVelocity = 4.9 * delta_x * delta_x / (v_0 * v_0);

		return prevAngle - (delta_x * tanAngle - deltaXOverVelocity * secSquaredAngle - delta_y)
						/ (delta_x * secSquaredAngle - 2 * deltaXOverVelocity * secSquaredAngle * tanAngle);
	}

	float calculateFiringAngle(float hypotenuse, float measurmentsAngleInDegrees, float v_0, float y_0) {
		float measurmentsAngleInRadians = measurmentsAngleInDegrees * M_PI / 180;
		float delta_x = hypotenuse * ((float) cos(measurmentsAngleInRadians));
		float delta_y = hypotenuse * ((float) sin(measurmentsAngleInRadians)) - y_0;

		float angle = 0;
		for (uint8_t i = 0; i < 4; i++) {
			angle = newtonIteration(angle, v_0, delta_x, delta_y);
		}

		return angle * 180 / M_PI;
	}
}

namespace Screens {
	void showDistanceScreen() {
		if (!RangeFinder::isOn()) RangeFinder::turnOn();

		float range = RangeFinder::getRange();
		float angle = Accelerometer::readPitchAngle();
		Display::write((String) range + "M " + (String) angle + "deg", 1);

		if (digitalRead(confirmInputPin) == HIGH) {
			firingDistance = range;
			measuredAngle = angle;
			Display::write("Confirmed!");
			RangeFinder::turnOff();
			delay(2000);
			screen++;
		}
	}

	void showHeightInputScreen() {
		String height = getAnalogHeightInFeet();
		Display::write("Height: " + height, 1);

		if (digitalRead(confirmInputPin) == HIGH) {
			firingHeight = getAngalogHeightInMeters();
			Display::write("Confirmed!");
			delay(2000);
			screen++;
		}
	}

	void showFiringScreen() {
		bool firingInfoChanged = !(
				firingDistance == oldFiringDistance 
				&& measuredAngle == oldMeasuredAngle
			 	&& firingHeight == oldFiringHeight
			);

		if (firingInfoChanged) {
			oldFiringDistance = firingDistance;
			oldMeasuredAngle = measuredAngle;
			oldFiringHeight = firingHeight;
			calculatedFiringAngle = 
				WeirdBSMagicMath::calculateFiringAngle(
					firingDistance, 
					measuredAngle, 
					MUZZLE_VELOCITY, 
					firingHeight
				);
		}


	 	float currentAngleDifference = Accelerometer::readPitchAngle() - calculatedFiringAngle;
		float absCurrentAngleDifference = abs(currentAngleDifference);
		if (absCurrentAngleDifference < 5) { //if the are withing 10 degrees of the correct firing angle
			Display::write("Fire!");

		} else if (currentAngleDifference > 0) {
			Display::write("Down: " + (String) absCurrentAngleDifference + "deg", 1);

		} else if (currentAngleDifference < 0) {
			Display::write("Up: " + (String) absCurrentAngleDifference + "deg", 1);

		}
		
		Serial.println("\n");

	}

	void setScreen(int screenNum) {
		switch (screen % 3) {
			case 0:
				showDistanceScreen();
				break;

			case 1:
				showHeightInputScreen();
				break;

			case 2:
				showFiringScreen();
				break;
		}
	}

}

void setup() {
	Serial.begin(115200);

	RangeFinder::begin();
	Accelerometer::begin();
	Display::begin();

	// Set pin mode of controller pins
	pinMode(changeScreenPin, INPUT_PULLUP);
	pinMode(confirmInputPin, INPUT_PULLUP);
}

void loop() {
	if (digitalRead(changeScreenPin) == HIGH) {
		screen++;
		Screens::setScreen(screen);
		delay(1000);
	}

	Screens::setScreen(screen);
}
