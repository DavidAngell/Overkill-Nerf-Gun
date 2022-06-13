#include <Arduino.h>
#include <SoftwareSerial.h>
#include <Adafruit_SSD1306.h>

namespace Display {
	Adafruit_SSD1306 display = Adafruit_SSD1306();
	uint8_t Buf[14];

	void begin() {
		// Initialize display
		if (!display.begin(SSD1306_SWITCHCAPVCC, 0x3C)) {
			Serial.println(F("SSD1306 allocation failed"));
			for(;;); // Don't proceed, loop forever
		}

		display.display();
		delay(2000);
		display.clearDisplay();
		display.setTextSize(2);
		display.setTextColor(WHITE);
		display.print("fuck you");
		display.display();
		delay(250);
	}

	void write(String contents, int textSize) {
		display.clearDisplay();
		display.setCursor(0, 10);
		display.setTextSize(textSize);
		display.setTextColor(WHITE);
		display.print(contents);
		display.display();
		delay(150);
	}

	void write(String contents) {
		write(contents, 2);
	}

	void write(float contents) {
		write((String) contents, 2);
	}

	void write(int contents) {
		write((String) contents, 2);
	}
}