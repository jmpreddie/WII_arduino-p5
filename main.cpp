// Wii Remote IR sensor test sample code by kako http://www.kako.com
// modified output for Wii-BlobTrack program by RobotFreak http://www.letsmakerobots.com/user/1433
// modified for https://dfrobot.com by Lumi, Jan. 2014
// further modified with corrected validity detection
// MODERATE sensitivity preset

#include <Arduino.h>
#include <Wire.h>

int IRsensorAddress = 0xB0;
int slaveAddress;
int ledPin = 13;
boolean ledState = false;
byte data_buf[16];
int i;

int Ix[4];
int Iy[4];
int s;

void Write_2bytes(byte d1, byte d2)
{
    Wire.beginTransmission(slaveAddress);
    Wire.write(d1); Wire.write(d2);
    Wire.endTransmission();
}

void setup()
{
    slaveAddress = IRsensorAddress >> 1;
    Serial.begin(115200);
    pinMode(ledPin, OUTPUT);
    Wire.begin();

    // --- MODERATE sensitivity preset ---
    Write_2bytes(0x30,0x01); delay(10);
    Write_2bytes(0x30,0x08); delay(10);
    Write_2bytes(0x06,0xC0); delay(10);
    Write_2bytes(0x08,0xFD); delay(10);
    Write_2bytes(0x1A,0x40); delay(10);
    Write_2bytes(0x33,0x96); delay(10);
    delay(100);
}

void loop()
{
    ledState = !ledState;
    digitalWrite(ledPin, ledState ? HIGH : LOW);

    Wire.beginTransmission(slaveAddress);
    Wire.write(0x36);
    Wire.endTransmission();

    Wire.requestFrom(slaveAddress, 16);
    for (i = 0; i < 16; i++) { data_buf[i] = 0; }
    i = 0;
    while (Wire.available() && i < 16) {
        data_buf[i] = Wire.read();
        i++;
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
    for (i = 0; i < 4; i++)
    {
        bool visible = !(Ix[i] == 1023 && Iy[i] == 1023);
        if (visible) visibleCount++;

        Serial.print("Blob ");
        Serial.print(i + 1);
        Serial.print(": X=");
        Serial.print(int(Ix[i]));
        Serial.print(" Y=");
        Serial.print(int(Iy[i]));
        Serial.print(" (valid=");
        Serial.print(visible ? "yes" : "no");
        Serial.print(")");
        if (i < 3) Serial.print(" | ");
    }

    Serial.print(" | IR_STATUS: ");
    if (visibleCount > 0) {
        Serial.print("seen ");
        Serial.print(visibleCount);
        Serial.print(" LED");
        if (visibleCount != 1) Serial.print("s");
    } else {
        Serial.print("no IR LED detected");
    }
    Serial.println("");
    delay(15);
}