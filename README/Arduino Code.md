# Arduino Code

# Overview

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

# main.cpp