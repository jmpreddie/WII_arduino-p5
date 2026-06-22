#include <Arduino.h>
#include <Wire.h>
#include <Adafruit_MPU6050.h>
#include <Adafruit_Sensor.h>

// variables
Adafruit_MPU6050 mpu;

struct MyData {
  float ax;
  float ay;
  float az;
  float gx;
  float gy;
  float gz;
};

MyData data;

void setup() {
  Serial.begin(115200);
  Wire.begin();

  if (!mpu.begin()) {
    // Serial.println("Failed to find MPU6050 chip");
    while (1) {
      delay(10);
    }
  }

  // Serial.println("MPU6050 found!");
}

void loop() {
  sensors_event_t a, g, temp;
  mpu.getEvent(&a, &g, &temp);

  data.ax = a.acceleration.x;
  data.ay = a.acceleration.y;
  data.az = a.acceleration.z;
  data.gx = g.gyro.x;
  data.gy = g.gyro.y;
  data.gz = g.gyro.z;

  // Serial.print("Acceleration: ");
  // Serial.print(data.ax);
  // Serial.print(", ");
  // Serial.print(data.ay);
  // Serial.print(", ");
  // Serial.println(data.az);

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

  // Serial.print("Roll: ");
  // if (roll > 15) {
  //   Serial.println("Tilted Right");
  // } else if (roll < -15) {
  //   Serial.println("Tilted Left");
  // } else {
  //   Serial.println("Level_0");
  // }

  // Serial.print("Pitch: ");
  // if (pitch > 15) {
  //   Serial.println("Tilted Forward");
  // } else if (pitch < -15) {
  //   Serial.println("Tilted Backward");
  // } else {
  //   Serial.println("Level_1");
  // }

  // CSV output format:
  // ax,ay,az,gx,gy,gz,roll,pitch,rollDirection,pitchDirection
  String csvLine = String(data.ax, 6) + "," +
                   String(data.ay, 6) + "," +
                   String(data.az, 6) + "," +
                   String(data.gx, 6) + "," +
                   String(data.gy, 6) + "," +
                   String(data.gz, 6) + "," +
                   String(roll, 2) + "," +
                   String(pitch, 2) + "," +
                   rollDirection + "," +
                   pitchDirection;

  Serial.println(csvLine);

  delay(100);
}
