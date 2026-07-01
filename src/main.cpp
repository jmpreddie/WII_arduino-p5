#include <Arduino.h>
#include <Wire.h>
#include <Adafruit_MPU6050.h>
#include <Adafruit_Sensor.h>

// ---------- Joystick ----------
const uint8_t JOYSTICK_X_PIN = A0; // Analog pin for joystick X-axis
const uint8_t JOYSTICK_Y_PIN = A1; // Analog pin for joystick Y-axis
const uint8_t JOYSTICK_BUTTON_PIN = 2; // Digital pin for joystick button
const unsigned long BUTTON_DEBOUNCE_MS = 20;

// default values for the joystick
const int JOYSTICK_X_CENTER = 128;
const int JOYSTICK_Y_CENTER = 128;

// ---------- MPU6050 ----------
Adafruit_MPU6050 mpu;
struct MyData {
  float ax;
  float ay;
  float az;
  float gx;
  float gy;
  float gz;
  float jx;
  float jy;
  bool jb;
};

MyData data;

// ---------- Wii IR Camera ----------
// Wii Remote IR sensor test sample code by kako http://www.kako.com
// modified output for Wii-BlobTrack program by RobotFreak http://www.letsmakerobots.com/user/1433
// modified for https://dfrobot.com by Lumi, Jan. 2014
// further modified with corrected validity detection
// MODERATE sensitivity preset
//
int IRsensorAddress = 0xB0;
int slaveAddress;
int ledPin = 13;
boolean ledState = false;
byte data_buf[16];
int ir_i;

int Ix[4];
int Iy[4];
int s;

void Write_2bytes(byte d1, byte d2)
{
    Wire.beginTransmission(slaveAddress);
    Wire.write(d1); Wire.write(d2);
    Wire.endTransmission();
}

// ---------- functions for organization ----------
int16_t readJoystick(uint8_t pin) {
  int16_t xValue = map(analogRead(pin), 0, 1023, -128, 127);
  return xValue;
}

void setup() {
  pinMode(JOYSTICK_BUTTON_PIN, INPUT_PULLUP); // Set the joystick button pin as input with pull-up resistor
  pinMode(ledPin, OUTPUT);
  Serial.begin(115200);
  Wire.begin();

  if (!mpu.begin()) {
    // Serial.println("Failed to find MPU6050 chip");
    while (1) {
      delay(10);
    }
  }
  // Serial.println("MPU6050 found!");

  // --- IR camera init: MODERATE sensitivity preset ---
  slaveAddress = IRsensorAddress >> 1;
  Write_2bytes(0x30,0x01); delay(10);
  Write_2bytes(0x30,0x08); delay(10);
  Write_2bytes(0x06,0xC0); delay(10);
  Write_2bytes(0x08,0xFD); delay(10);
  Write_2bytes(0x1A,0x40); delay(10);
  Write_2bytes(0x33,0x96); delay(10);
  delay(100);
}

void loop() {
  static bool lastRawButtonState = false;
  static unsigned long lastDebounceTime = 0;

  // ---------- MPU6050 read ----------
  sensors_event_t a, g, temp;
  mpu.getEvent(&a, &g, &temp);

  data.ax = a.acceleration.x;
  data.ay = a.acceleration.y;
  data.az = a.acceleration.z;
  data.gx = g.gyro.x;
  data.gy = g.gyro.y;
  data.gz = g.gyro.z;

  float roll = atan2(data.ay, data.az) * 180.0 / PI;
  float pitch = atan2(-data.ax, sqrt(data.ay * data.ay + data.az * data.az)) * 180.0 / PI;

  String rollDirection;
  if (roll > 15) {
    rollDirection = "Tilted Right";
  } else if (roll < -15) {
    rollDirection = "Tilted Left";
  } else {
    rollDirection = "Level_0";
  }

  String pitchDirection;
  if (pitch > 15) {
    pitchDirection = "Tilted Forward";
  } else if (pitch < -15) {
    pitchDirection = "Tilted Backward";
  } else {
    pitchDirection = "Level_1";
  }

  // ---------- Joystick read ----------
  // Is the button pressed
  if (digitalRead(JOYSTICK_BUTTON_PIN) == LOW) {
    data.jb = false;
  } else if (digitalRead(JOYSTICK_BUTTON_PIN) == HIGH) {
    data.jb = true;
  }
  data.jx = readJoystick(JOYSTICK_X_PIN);
  data.jy = readJoystick(JOYSTICK_Y_PIN);

  // Is the button pressed (with debounce)
  bool rawButtonPressed = (digitalRead(JOYSTICK_BUTTON_PIN) == LOW);

  if (rawButtonPressed != lastRawButtonState) {
    lastDebounceTime = millis();
    lastRawButtonState = rawButtonPressed;
  }

  if (millis() - lastDebounceTime > BUTTON_DEBOUNCE_MS) {
    data.jb = rawButtonPressed;
  }

  // ---------- IR camera read ----------
  ledState = !ledState;
  digitalWrite(ledPin, ledState ? HIGH : LOW);

  Wire.beginTransmission(slaveAddress);
  Wire.write(0x36);
  Wire.endTransmission();

  Wire.requestFrom(slaveAddress, 16);
  for (ir_i = 0; ir_i < 16; ir_i++) { data_buf[ir_i] = 0; }
  ir_i = 0;
  while (Wire.available() && ir_i < 16) {
      data_buf[ir_i] = Wire.read();
      ir_i++;
  }

  Ix[0] = data_buf[1];
  Iy[0] = data_buf[2];
  s = data_buf[3];
  Ix[0] += (s & 0x30) << 4;
  Iy[0] += (s & 0xC0) << 2;

  Ix[1] = data_buf[4];
  Iy[1] = data_buf[5];
  s = data_buf[6];
  Ix[1] += (s & 0x30) << 4;
  Iy[1] += (s & 0xC0) << 2;

  Ix[2] = data_buf[7];
  Iy[2] = data_buf[8];
  s = data_buf[9];
  Ix[2] += (s & 0x30) << 4;
  Iy[2] += (s & 0xC0) << 2;

  Ix[3] = data_buf[10];
  Iy[3] = data_buf[11];
  s = data_buf[12];
  Ix[3] += (s & 0x30) << 4;
  Iy[3] += (s & 0xC0) << 2;

  int visibleCount = 0;
  bool blobValid[4];
  for (ir_i = 0; ir_i < 4; ir_i++) {
      blobValid[ir_i] = !(Ix[ir_i] == 1023 && Iy[ir_i] == 1023);
      if (blobValid[ir_i]) visibleCount++;
  }

  // ---------- Combined CSV output ----------
  // ax,ay,az,gx,gy,gz,roll,pitch,rollDirection,pitchDirection,jx,jy,jb,
  // blob1_x,blob1_y,blob1_valid,blob2_x,blob2_y,blob2_valid,
  // blob3_x,blob3_y,blob3_valid,blob4_x,blob4_y,blob4_valid,ir_visible_count
  String csvLine = String(data.ax, 6) + "," +
                   String(data.ay, 6) + "," +
                   String(data.az, 6) + "," +
                   String(data.gx, 6) + "," +
                   String(data.gy, 6) + "," +
                   String(data.gz, 6) + "," +
                   String(roll, 2) + "," +
                   String(pitch, 2) + "," +
                   rollDirection + "," +
                   pitchDirection + "," +
                   String(data.jx, 2) + "," +
                   String(data.jy, 2) + "," +
                   (data.jb ? "Pressed" : "Not Pressed") + "," +
                   String(Ix[0]) + "," + String(Iy[0]) + "," + (blobValid[0] ? "yes" : "no") + "," +
                   String(Ix[1]) + "," + String(Iy[1]) + "," + (blobValid[1] ? "yes" : "no") + "," +
                   String(Ix[2]) + "," + String(Iy[2]) + "," + (blobValid[2] ? "yes" : "no") + "," +
                   String(Ix[3]) + "," + String(Iy[3]) + "," + (blobValid[3] ? "yes" : "no") + "," +
                   String(visibleCount);

  Serial.println(csvLine);

  delay(100);
}