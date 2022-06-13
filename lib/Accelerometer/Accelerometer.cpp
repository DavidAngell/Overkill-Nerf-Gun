#include <MPU6050.h>

namespace Accelerometer {
	MPU6050 accelerometer;

	void begin() {
		while (!accelerometer.begin(MPU6050_SCALE_2000DPS, MPU6050_RANGE_2G)) {
			delay(100);
		}
		
		// Calibrate gyroscope.
		// mpu.calibrateGyro();
	}

	float readPitchAngle() {
		Vector normAccel = accelerometer.readNormalizeAccel();		
		return (atan2(normAccel.YAxis, normAccel.ZAxis) * 180.0) / M_PI;
	}
}