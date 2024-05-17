#include <Arduino.h>

const byte BTN_0 = 2;
const byte BTN_1 = 4;
const byte SW_0 = 7;
const byte SW_1 = 8;
const byte SW_2 = 12;
const byte SW_3 = 13;

byte status = 0;

byte getBit(byte number, int shift) {
    number = number >> shift;
    number = number & 1;
    return number;
}

void setPins() {
    Serial.write(status);
    Serial.write(getBit(status, 0));
    Serial.write(getBit(status, 1));
    Serial.write(getBit(status, 2));
    Serial.write(getBit(status, 3));
    Serial.write(getBit(status, 4));
    Serial.write(getBit(status, 5));
    if (getBit(status, 0) == 1)
        digitalWrite(BTN_0, HIGH);
    else
        digitalWrite(BTN_0, LOW);

    if (getBit(status, 1) == 1)
        digitalWrite(BTN_1, HIGH);
    else
        digitalWrite(BTN_1, LOW);

    if (getBit(status, 2) == 1)
        digitalWrite(SW_0, HIGH);
    else
        digitalWrite(SW_0, LOW);

    if (getBit(status, 3) == 1)
        digitalWrite(SW_1, HIGH);
    else
        digitalWrite(SW_1, LOW);

    if (getBit(status, 4) == 1)
        digitalWrite(SW_2, HIGH);
    else
        digitalWrite(SW_2, LOW);

    if (getBit(status, 5) == 1)
        digitalWrite(SW_3, HIGH);
    else
        digitalWrite(SW_3, LOW);
}

void setup() {
    Serial.begin(9600);
    pinMode(BTN_0, OUTPUT);
    pinMode(BTN_1, OUTPUT);
    pinMode(SW_0, OUTPUT);
    pinMode(SW_1, OUTPUT);
    pinMode(SW_2, OUTPUT);
    pinMode(SW_3, OUTPUT);
}

void loop() {
    if (Serial.available() > 0) {
        status = Serial.read();
        setPins();
    }
}
