// SPECIAL 13C: Orientation Colors
//
// WIRING DIAGRAM
// XIAO ESP32-C3 D8  -> NeoPixel DIN
// XIAO ESP32-C3 5V/VBUS -> NeoPixel 5V
// XIAO ESP32-C3 GND -> NeoPixel GND
// XIAO ESP32-C3 D0  -> MPU6050 SCL
// XIAO ESP32-C3 D10 -> MPU6050 SDA
// XIAO ESP32-C3 3V3 -> MPU6050 VCC
// XIAO ESP32-C3 GND -> MPU6050 GND
//
// The blade color changes with orientation.
// The middle two LEDs turn white when the saber is held very cleanly
// horizontal or vertical.

#include <Adafruit_MPU6050.h>
#include <Adafruit_NeoPixel.h>
#include <Adafruit_Sensor.h>
#include <Wire.h>

#define USE_SUPER_MINI_PINS 0

#if USE_SUPER_MINI_PINS
const int LED_STRIP_PIN = 2;
const int GYRO_SCL_PIN = 5;
const int GYRO_SDA_PIN = 4;
#else
const int LED_STRIP_PIN = D8;
const int GYRO_SCL_PIN = D0;
const int GYRO_SDA_PIN = D10;
#endif

enum AxisName {
  AXIS_X,
  AXIS_Y,
  AXIS_Z
};

enum OrientationState {
  FACE_UP_STATE,
  FACE_DOWN_STATE,
  RIGHT_STATE,
  LEFT_STATE,
  TIP_DOWN_STATE,
  TIP_UP_STATE,
  BETWEEN_STATE
};

const int LED_COUNT = 60;
const int SAFE_BRIGHTNESS = 25;
const float ORIENTATION_THRESHOLD = 7.0;
const float ALIGNMENT_THRESHOLD = 2.0;
const AxisName SABER_RIGHT_AXIS = AXIS_Y;
const AxisName SABER_TIP_AXIS = AXIS_Z;
const AxisName SABER_UP_AXIS = AXIS_X;
const int SABER_RIGHT_SIGN = -1;
const int SABER_TIP_SIGN = -1;
const int SABER_UP_SIGN = 1;

Adafruit_MPU6050 mpu;
Adafruit_NeoPixel strip(LED_COUNT, LED_STRIP_PIN, NEO_GRBW + NEO_KHZ800);

float getMappedAxis(float x, float y, float z, AxisName axisName, int axisSign);
OrientationState getOrientationState(float saberRight, float saberTip, float saberUp);
bool isPerfectHorizontal(float saberRight, float saberTip, float saberUp);
bool isPerfectVertical(float saberRight, float saberTip, float saberUp);
void showOrientationColor(OrientationState orientationState, bool showCenterWhite);
void fillBlade(uint32_t colorValue);
void setMiddleWhite();
void printOrientation(OrientationState orientationState, bool perfectHorizontal, bool perfectVertical);

void setup() {
  Serial.begin(115200);
  Wire.begin(GYRO_SDA_PIN, GYRO_SCL_PIN);

  strip.begin();
  strip.setBrightness(SAFE_BRIGHTNESS);
  strip.clear();
  strip.show();

  if (!mpu.begin()) {
    Serial.println("MPU6050 not found. Check wiring.");
    while (true) {
      delay(10);
    }
  }

  Serial.println("Orientation color demo ready.");
}

void loop() {
  sensors_event_t accelEvent;
  sensors_event_t gyroEvent;
  sensors_event_t tempEvent;

  mpu.getEvent(&accelEvent, &gyroEvent, &tempEvent);

  float saberRight = getMappedAxis(
    accelEvent.acceleration.x,
    accelEvent.acceleration.y,
    accelEvent.acceleration.z,
    SABER_RIGHT_AXIS,
    SABER_RIGHT_SIGN
  );
  float saberTip = getMappedAxis(
    accelEvent.acceleration.x,
    accelEvent.acceleration.y,
    accelEvent.acceleration.z,
    SABER_TIP_AXIS,
    SABER_TIP_SIGN
  );
  float saberUp = getMappedAxis(
    accelEvent.acceleration.x,
    accelEvent.acceleration.y,
    accelEvent.acceleration.z,
    SABER_UP_AXIS,
    SABER_UP_SIGN
  );

  OrientationState orientationState = getOrientationState(saberRight, saberTip, saberUp);
  bool perfectHorizontal = isPerfectHorizontal(saberRight, saberTip, saberUp);
  bool perfectVertical = isPerfectVertical(saberRight, saberTip, saberUp);

  showOrientationColor(orientationState, perfectHorizontal || perfectVertical);
  printOrientation(orientationState, perfectHorizontal, perfectVertical);

  delay(120);
}

float getMappedAxis(float x, float y, float z, AxisName axisName, int axisSign) {
  float axisValue = 0.0;

  if (axisName == AXIS_X) {
    axisValue = x;
  } else if (axisName == AXIS_Y) {
    axisValue = y;
  } else {
    axisValue = z;
  }

  return axisValue * axisSign;
}

OrientationState getOrientationState(float saberRight, float saberTip, float saberUp) {
  if (saberUp > ORIENTATION_THRESHOLD) {
    return FACE_UP_STATE;
  }

  if (saberUp < -ORIENTATION_THRESHOLD) {
    return FACE_DOWN_STATE;
  }

  if (saberRight > ORIENTATION_THRESHOLD) {
    return RIGHT_STATE;
  }

  if (saberRight < -ORIENTATION_THRESHOLD) {
    return LEFT_STATE;
  }

  if (saberTip > ORIENTATION_THRESHOLD) {
    return TIP_DOWN_STATE;
  }

  if (saberTip < -ORIENTATION_THRESHOLD) {
    return TIP_UP_STATE;
  }

  return BETWEEN_STATE;
}

bool isPerfectHorizontal(float saberRight, float saberTip, float saberUp) {
  return abs(saberRight) < ALIGNMENT_THRESHOLD &&
         abs(saberTip) < ALIGNMENT_THRESHOLD &&
         abs(saberUp) > ORIENTATION_THRESHOLD;
}

bool isPerfectVertical(float saberRight, float saberTip, float saberUp) {
  return abs(saberRight) < ALIGNMENT_THRESHOLD &&
         abs(saberUp) < ALIGNMENT_THRESHOLD &&
         abs(saberTip) > ORIENTATION_THRESHOLD;
}

void showOrientationColor(OrientationState orientationState, bool showCenterWhite) {
  uint32_t colorValue = strip.Color(0, 0, 0, 10);

  if (orientationState == FACE_UP_STATE) {
    colorValue = strip.Color(0, 0, 255, 0);
  } else if (orientationState == FACE_DOWN_STATE) {
    colorValue = strip.Color(255, 0, 0, 0);
  } else if (orientationState == RIGHT_STATE) {
    colorValue = strip.Color(0, 255, 0, 0);
  } else if (orientationState == LEFT_STATE) {
    colorValue = strip.Color(180, 0, 180, 0);
  } else if (orientationState == TIP_DOWN_STATE) {
    colorValue = strip.Color(255, 80, 0, 0);
  } else if (orientationState == TIP_UP_STATE) {
    colorValue = strip.Color(0, 180, 180, 0);
  }

  fillBlade(colorValue);

  if (showCenterWhite) {
    setMiddleWhite();
  }

  strip.show();
}

void fillBlade(uint32_t colorValue) {
  for (int i = 0; i < LED_COUNT; i++) {
    strip.setPixelColor(i, colorValue);
  }
}

void setMiddleWhite() {
  int leftCenter = (LED_COUNT / 2) - 1;
  int rightCenter = LED_COUNT / 2;

  strip.setPixelColor(leftCenter, strip.Color(0, 0, 0, 80));
  strip.setPixelColor(rightCenter, strip.Color(0, 0, 0, 80));
}

void printOrientation(OrientationState orientationState, bool perfectHorizontal, bool perfectVertical) {
  if (orientationState == FACE_UP_STATE) {
    Serial.println("Orientation: flat, face up");
  } else if (orientationState == FACE_DOWN_STATE) {
    Serial.println("Orientation: flat, face down");
  } else if (orientationState == RIGHT_STATE) {
    Serial.println("Orientation: tilted right");
  } else if (orientationState == LEFT_STATE) {
    Serial.println("Orientation: tilted left");
  } else if (orientationState == TIP_DOWN_STATE) {
    Serial.println("Orientation: tip down");
  } else if (orientationState == TIP_UP_STATE) {
    Serial.println("Orientation: tip up");
  } else {
    Serial.println("Orientation: between directions");
  }

  if (perfectHorizontal) {
    Serial.println("Alignment: perfectly horizontal");
  } else if (perfectVertical) {
    Serial.println("Alignment: perfectly vertical");
  }
}
