# Arduino Code

If you aren’t familiar with Arduinos, here is a quick rundown of how Arduino code differs from C/C++ code. The key difference is that there is no *main()* function that acts as the single entrance point of your code. Rather, there is *setup()* which contains all of the code that runs at startup and *loop()* which runs after the *setup()* function and, as the name implies, repeats while the Arduino is on. 

So, in *loop()* I display one of three screens: a range finder input screen in which the user points the gun at their chosen target; a height input screen in which the user input’s their height above the ground in feet; and a targeting screen in which the users aims the gun at the location which would hit the target.

# Libraries

These are some of the simple libraries I threw together to help aid code readability. If I have named my functions well, you should not need an understanding of these to understand main.cpp. Regardless, all three of these are structured in the same way: they are namespaces that have a *begin()* function which is run during *setup()* and contain useful functions from their respective devices. 

## Display.cpp

This one is probably the easiest to understand. It is basically a wrapper for the Adafruit SSD1306 display library. Normally, if one wanted to write something to the display, they would need to do this:

```cpp
display.clearDisplay();
display.setCursor(0, 10);
display.setTextSize(2); // optional
display.setTextColor(WHITE); // optional
display.print("Example text");
display.display();
delay(150);
```

This is a pain. 

None of us high-level programmers ever wanna look at that ugly mess all over our code. Instead, *Display.cpp* contains this:

```cpp
namespace Display {
	...
	void begin();

	void write(String contents, int textSize) {
		display.clearDisplay();
		display.setCursor(0, 10);
		display.setTextSize(textSize);
		display.setTextColor(WHITE);
		display.print(contents);
		display.display();
		delay(150);
	}

	void write(String contents);
	void write(float contents);
	void write(int contents);
}
```

Now, any text or numbers can easily be written to the display like this:

```cpp
#include "Display.h"
...
Display::write("Example text");
```

## Accelerometer.cpp

```cpp
#include <MPU6050.h>

namespace Accelerometer {
	MPU6050 accelerometer;

	void begin() {
		while (!accelerometer.begin(MPU6050_SCALE_2000DPS, MPU6050_RANGE_2G)) {
			delay(100);
		}
	}

	float readPitchAngle() {
		Vector normAccel = accelerometer.readNormalizeAccel();		
		return (atan2(normAccel.YAxis, normAccel.ZAxis) * 180.0) / M_PI;
	}
}
```

*readPitchAngle()* just reads the $y$ and $z$ acceleration from the accelerometer, plugs them into *atan2()* —a function in *math.h* that computes the inverse tangent in the range $[-\pi, +\pi]$ radians— and converts the output into degrees.

## RangeFinder.cpp

I’m too lazy to explain this right now :(



# main.cpp

The *main.cpp* file starts by running the *begin()* method for the ranger finder, accelerometer, and display inside of *setup()*. Then, it sets the pin mode of the “confirm” and “change screen” pins on the gun controller as INPUT_PULLUP; meaning that it reads whether the pin is pulled to +5v or ground and uses the Arduino Nano’s built-in pull-up resistors to do so.

```cpp
uint8_t changeScreenPin = 5; //D5
uint8_t confirmInputPin = 6; //D6

void setup() {
	Serial.begin(115200);

	RangeFinder::begin();
	Accelerometer::begin();
	Display::begin();

	pinMode(changeScreenPin, INPUT_PULLUP);
	pinMode(confirmInputPin, INPUT_PULLUP);
}
```

After this, the Arduino runs the *loop()* for the rest of the on-time of the device. Within *loop()*, we check whether the “change screen” pin has been pressed (pulled up to +5v), and if it has, the current screen number gets updated accordingly. Otherwise, the Arduino just checks what the screen number mod 3 is because there are 3 screens. This method will cause an overflow after every 256 screen changes, but (1) it won’t really be too noticeable to the user because they will just have to press the button again and (2) I’m too lazy to fix it —if you want it fixed, make a pull request so I can ignore it :)

```cpp
int screen = 0; // 0: range finder, 1: height input, 2: firing screen

namespace Screens {
	void showDistanceScreen() {
		...
	}

	void showHeightInputScreen() {
		...
	}

	void showFiringScreen() {
		...
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

void loop() {
	if (digitalRead(changeScreenPin) == HIGH) {
		screen++;
		Screens::setScreen(screen);
		delay(1000);
	}

	Screens::setScreen(screen);
}
```

The next major part of *main.cpp* are the two functions which read the firing height pot and translate the resistance of it into a height in feet and meters respectively. The math itself is a tad arbitrary —I just divided *sensorVal* by 12 because it gave me a nice result of an 85 inch / 2.165 meter max.

```cpp
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
```

Finally, we have the application of the math —[explained here](Range%20Finder%20Math.md)— that is used to calculate the firing angle of the gun.

```cpp
namespace WeirdBSMagicMath {
	float newtonIteration(float prevAngle, float v_0, float delta_x, float delta_y) {
		float tanAngle = (float) tan(prevAngle);
		float secSquaredAngle = (float) (1 / (cos(prevAngle) * cos(prevAngle)));
		float deltaXOverVelocity = 4.9 * delta_x * delta_x / (v_0 * v_0);

		return prevAngle - (delta_x * tanAngle - deltaXOverVelocity * secSquaredAngle - delta_y)
						/ (delta_x * secSquaredAngle - 2 * deltaXOverVelocity * secSquaredAngle * tanAngle);
	}

	float calculateFiringAngle(float hypotenuse, float measurmentsAngleInDegrees, 
														 float v_0, float y_0) 
	{
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
```