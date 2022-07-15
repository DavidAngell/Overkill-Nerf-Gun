# Overkill-Nerf-Gun
<div id="top"></div>

(Unfinished) Documentation and code explanation for a Nerf Gun with a range finder that I made as a final project / friendly competition is AP Physics.



## Design Overview


### Parts

- ****[Daybreak Ultra 2.0 3D Printed Flywheel Cage (41.5mm)](https://outofdarts.com/collections/flywheels-cages/products/daybreak-ultra-2-0-3d-printed-flywheel-cage?variant=31748462051370)****
- ****[OOD Kraken 130 3s Neo Motor for Nerf Blasters](https://outofdarts.com/collections/motors-2019/products/ood-kraken-motor)****
- ****[Containment Crew "CYCLONE" Flywheels (pair)](https://outofdarts.com/collections/flywheels-cages/products/containment-crew-cyclone-flywheels-pair)****
- **[IRLB3034 N-Channel MOSFET](https://www.amazon.com/gp/product/B07Q3S7XSN/ref=ppx_yo_dt_b_search_asin_title?ie=UTF8&psc=1)**
- [Arduino Nano](https://www.amazon.com/dp/B0713XK923?psc=1&ref=ppx_yo2ov_dt_b_product_details)
- [MPU6050 - **3 Axis Accelerometer Gyroscope**](https://www.amazon.com/dp/B01DK83ZYQ?ref=ppx_yo2ov_dt_b_product_details&th=1)
- **[SSD1306 OLED Display](https://www.amazon.com/dp/B072Q2X2LL?psc=1&ref=ppx_yo2ov_dt_b_product_details)**

- Perf Board
- 18 Gauge Wire

<p align="right">(<a href="#top">back to top</a>)</p>




### Gun Overview

<div align="left">
	<img src="README/Gun%20Design/gun_far.png" alt="Logo" width="49%" height="auto">
	<img src="README/Gun%20Design/gun_controller.png" alt="Logo" width="49%" height="auto">
</div>

<div><br /></div>
<p align="right">(<a href="#top">back to top</a>)</p>

### Dart Pusher System
![gun_dart_pusher.mp4](README/Gun%20Design/gun_dart_pusher.mp4)

<p align="right">(<a href="#top">back to top</a>)</p>





## Range Finder: Math

To start off, we are searching for an equation which takes in the range finder data, accelerometer data, and dart velocity and returns the angle at which the gun should be fired to hit the target.

Because geometry, if we know the measured range to a target and the angle at which it was measured,  we can determine the change in horizontal distance ($Δx$) and the change in vertical distance ($Δy$) to the target:

![range_finder_math_triangle.png](README/Range%20Finder%20Math/range_finder_math_triangle.png)

We also know the muzzle velocity ($v$) because we can measure it separately and get a good estimate of the approximate value of $v$. So, we need an equation which combines the change in horizontal distance ($Δx$), the change in vertical distance ($Δy$), the muzzle velocity ($v$), and, most importantly, the firing angle ($θ$). We can uses the kinematic equations for both the $x$ position and $y$ position given time, rearrange the first equation to solve for time, then substitute the result into the second equation, netting us an equation which contains all four of the variables that we either know or seek to know.

$$
\Delta x=vt\sin(\theta)
$$

$$
\Delta y=vt\cos(\theta)+ \frac{1}{2}gt^{2}
$$

$$
...
$$

$$
\Delta y = \Delta x\tan(\theta)+ \frac{{\Delta x}^{2}}{v^{2}{\cos}^{2}(\theta)}
$$

There is just one small problem: to my knowledge, at least, there is no way to solve for $θ$ in this equation. Feel free to give it a try! So, we need some algorithm that will allow us estimate $θ$. If I rearrange the equation, those who have taken a calculus course might see it.

$$
0 = \Delta x\tan(\theta) + \frac{{\Delta x}^{2}}{v^{2}{\cos}^{2}(\theta)} - \Delta y
$$

Newton’s method! A method for obtaining an approximation of the roots of a function. What we are really asking is when $f(θ) = 0$, which is just a root. Furthermore, the equation is pretty easily differentiable, so Newton’s method is perfect for this.

$$
θ_{n+1}=θ_{n}-\frac{f(θ_{n})}{f'(θ_{n})}
$$

$$
θ_{n+1}=θ_{n}-\frac{\Delta x\tan(θ_{n})+ {\sec}^{2}(θ_{n})\frac{{\Delta x}^{2}}{{v}^{2}} - \Delta y}{\Delta x\sec^{2}(θ_{n})+ 2\tan(θ_{n})\sec^{2}(θ_{n})\frac{{\Delta x}^{2}}{{v}^{2}} - \Delta y}
$$

Sure. It’s not the prettiest equation. But in code, there are actually a lot of things which only need to be calculated once per iteration: $tan(θ)$ and $sec^2(θ)$ only need to be calculated once per iteration and $\frac{Δx}{v^2}$ only needs to be calculated once per target. Overall, it could be worse.

<p align="right">(<a href="#top">back to top</a>)</p>





## Arduino Code

If you aren’t familiar with Arduinos, here is a quick rundown of how Arduino code differs from C/C++ code. The key difference is that there is no *main()* function that acts as the single entrance point of your code. Rather, there is *setup()* which contains all of the code that runs at startup and *loop()* which runs after the *setup()* function and, as the name implies, repeats while the Arduino is on. 

So, in *loop()* I display one of three screens: a range finder input screen in which the user points the gun at their chosen target; a height input screen in which the user input’s their height above the ground in feet; and a targeting screen in which the users aims the gun at the location which would hit the target.

## Libraries

These are some of the simple libraries I threw together to help aid code readability. If I have named my functions well, you should not need an understanding of these to understand main.cpp. Regardless, all three of these are structured in the same way: they are namespaces that have a *begin()* function which is run during *setup()* and contain useful functions from their respective devices. 

### Display.cpp

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

### Accelerometer.cpp

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

### RangeFinder.cpp

I’m too lazy to explain this right now :(



## main.cpp

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

<p align="right">(<a href="#top">back to top</a>)</p>





## Electronics


### Arduino Range Finder Circuitry

![Get CDDX File Here: [https://crcit.net/c/0befd373d41f49899402f0b9a3a5f62a](https://crcit.net/c/0befd373d41f49899402f0b9a3a5f62a)](README/Arduino%20Range%20Finder%20Circuitry/range_finder_circuit.png)

Get CDDX File Here: [https://crcit.net/c/0befd373d41f49899402f0b9a3a5f62a](https://crcit.net/c/0befd373d41f49899402f0b9a3a5f62a)

It looks a lot more complicated than it actually is.

### Flywheel Circuitry

![Get CDDX File Here: [https://crcit.net/c/0befd373d41f49899402f0b9a3a5f62a](https://crcit.net/c/0befd373d41f49899402f0b9a3a5f62a)](README/Flywheel%20Circuitry/motor_circuit.png)

Get CDDX File Here: [https://crcit.net/c/0befd373d41f49899402f0b9a3a5f62a](https://crcit.net/c/0befd373d41f49899402f0b9a3a5f62a)

I ended up using an N- Channel MOSFET to switch on and off the motors because well… I gave away my only high current switch to a girl in a futile attempt to win her approval :) In all seriousness, a high current switch like this [21A microswitch](https://outofdarts.com/collections/electronics/products/21a-microswitch-genuine-omron-v-212-1c6-spdt) is both less complicated and probably cheaper than my design; I just happened to have a MOSFET lying around from a previous project that I could use. Also, if you go with a switch, you don’t need the 10k resistor; it is only there because MOSFETs have some capacitance to them that needs to be drained in order to ensure fast and proper switching. I would still recommended the Shockley diode in parallel with the motors to reduce back EMF and save your switch it in the long term.

In case you are wondering wire the wires are crosses for the motors, it’s because they have to spin in opposite directions because the dart goes in between the two motors. If there weren’t crossed, then the motors would be pushing the dart in opposite directions —which I heard isn’t very good for making a gun… gun.

<p align="right">(<a href="#top">back to top</a>)</p>





<div align="center">
	<br />
	<a href="https://paypal.me/davidjangell" align="center" target="_blank">
		<img src="https://viatesting.files.wordpress.com/2020/03/paypal-donate-button.png" 
  			width="200"
 		/>
	</a>
</div>
