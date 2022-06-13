#pragma once

namespace Display {
	void begin();
	void write(String contents, int textSize);
	void write(String contents);
	void write(float contents);
	void write(int contents);
}