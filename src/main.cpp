#include <Arduino.h>
#include <Wire.h>
#include <Adafruit_MPU6050.h>
#include <Adafruit_Sensor.h>


const uint8_t JOYSTICK_X_PIN = A0; // Analog pin for joystick X-axis
const uint8_t JOYSTICK_Y_PIN = A1; // Analog pin for joystick Y-axis
const uint8_t JOYSTICK_BUTTON_PIN = 2; // Digital pin for joystick button
const unsigned long BUTTON_DEBOUNCE_MS = 20;

//default values for the joystick
const int JOYSTICK_X_CENTER = 128;
const int JOYSTICK_Y_CENTER = 128;

// variables
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


//functions for orginzation
int16_t readJoystick(uint8_t pin) {
  int16_t xValue = map(analogRead(pin), 0, 1023, -128, 127);
  return xValue;
}


void setup() {
  pinMode(JOYSTICK_BUTTON_PIN, INPUT_PULLUP); // Set the joystick button pin as input with pull-up resistor
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
  static bool lastRawButtonState = false;
  static unsigned long lastDebounceTime = 0;

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



  //joystick code
  //Is the button pressed 
  if(digitalRead(JOYSTICK_BUTTON_PIN) == LOW){
    data.jb = false; 
  }else if(digitalRead(JOYSTICK_BUTTON_PIN) == HIGH){
    data.jb = true; 
  } 
  //giving me the joystick x and y values 
  data.jx = readJoystick(JOYSTICK_X_PIN);
  data.jy = readJoystick(JOYSTICK_Y_PIN);
  //Is the button pressed (with debounce)
  bool rawButtonPressed = (digitalRead(JOYSTICK_BUTTON_PIN) == LOW);

  if (rawButtonPressed != lastRawButtonState) {
    lastDebounceTime = millis();
    lastRawButtonState = rawButtonPressed;
  }

  if (millis() - lastDebounceTime > BUTTON_DEBOUNCE_MS) {
   data.jb = rawButtonPressed;
  }

 








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
                   pitchDirection+","+
                   String(data.jx, 2) + "," +
                   String(data.jy, 2) + "," +
                   (data.jb ? "Pressed" : "Not Pressed"); 

  Serial.println(csvLine);       
  
  
  delay(100);
}
