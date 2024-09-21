
#include <Arduino.h>
#include <microLED.h>

#define PIN_0 2
#define PIN_1 3
#define PIN_2 4
#define PIN_3 5
#define PIN_4 6
#define STRIP_PIN 7
#define NUMLEDS 4

microLED < NUMLEDS, STRIP_PIN, -1, LED_WS2812, ORDER_GRB > strip;

byte status = 0;

byte getBit(byte number, int shift) {
    number = number >> shift;
    number = number & 1;
    return number;
}

void setPins() {
    if (getBit(status, 0) == 1)
        digitalWrite(PIN_0, HIGH);
    else
        digitalWrite(PIN_0, LOW);

    if (getBit(status, 1) == 1)
        digitalWrite(PIN_1, HIGH);
    else
        digitalWrite(PIN_1, LOW);

    if (getBit(status, 2) == 1)
        digitalWrite(PIN_2, HIGH);
    else
        digitalWrite(PIN_2, LOW);

    if (getBit(status, 3) == 1)
        digitalWrite(PIN_3, HIGH);
    else
        digitalWrite(PIN_3, LOW);

    if (getBit(status, 4) == 1)
        digitalWrite(PIN_4, HIGH);
    else
        digitalWrite(PIN_4, LOW);

    mData color = {0};
    if (getBit(status, 6) == 1)
        color = mRGB(255, 255, 255);
    else
        color = mRGB(0, 0, 0);
    for (int i = 0; i < NUMLEDS; i++)
        strip.set(i, color);
    strip.show();
}

void setup() {
    Serial.begin(9600);
    pinMode(PIN_0, OUTPUT);
    pinMode(PIN_1, OUTPUT);
    pinMode(PIN_2, OUTPUT);
    pinMode(PIN_3, OUTPUT);
    pinMode(PIN_4, OUTPUT);
    strip.setBrightness(100);
    for (int i = 0; i < NUMLEDS; i++)
        strip.set(i, mRGB(255, 0, 0));
    strip.show();
}

void loop() {
    if (Serial.available() > 0) {
        status = Serial.read();
        setPins();
    }
}
