#include <SPI.h>

#include "PMW3389.h"
#include "Trackball.h"

#define PMW3389_SENSOR_RESET_PIN 4
#define PMW3389_SENSOR_NCS_PIN 5
#include "PMW3389.h"

#define MOUSE_LEFT_BUTTON_PIN 18
#define MOUSE_RIGHT_BUTTON_PIN 19
#define MOUSE_BACK_BUTTON_PIN 20
#define MOUSE_FORWARD_BUTTON_PIN 21
#define MOUSE_MIDDLE_BUTTON_PIN 7
#define MOUSE_EXTRA1_BUTTON_PIN 8
#define MOUSE_EXTRA2_BUTTON_PIN 9

#define DEFAULT_CPI 100
#define DEBOUNCE_MUS 10000


PMW3389 sensor;
PMW3389_DATA sensorData = {};

uint64_t lastUpdateMus = 0;
uint64_t nowMus = 0;


void setup() {
    pinMode(MOUSE_LEFT_BUTTON_PIN, INPUT_PULLUP);
    pinMode(MOUSE_RIGHT_BUTTON_PIN, INPUT_PULLUP);
    pinMode(MOUSE_BACK_BUTTON_PIN, INPUT_PULLUP);
    pinMode(MOUSE_FORWARD_BUTTON_PIN, INPUT_PULLUP);
    pinMode(MOUSE_MIDDLE_BUTTON_PIN, INPUT_PULLUP);
    pinMode(MOUSE_EXTRA1_BUTTON_PIN, INPUT_PULLUP);
    pinMode(MOUSE_EXTRA2_BUTTON_PIN, INPUT_PULLUP);


    pinMode(PMW3389_SENSOR_NCS_PIN, OUTPUT);
    digitalWrite(PMW3389_SENSOR_NCS_PIN, HIGH);
    pinMode(PMW3389_SENSOR_RESET_PIN, OUTPUT);
    digitalWrite(PMW3389_SENSOR_RESET_PIN, HIGH);
    // signature check fails, but device works regardless
    sensor.begin(PMW3389_SENSOR_NCS_PIN, DEFAULT_CPI);

    Trackball.begin();
}


void loop() {
    sensorData = sensor.readBurst();
    if (sensorData.isOnSurface && sensorData.isMotion) {
        Trackball.move(sensorData.dx, sensorData.dy);
    }

    nowMus = micros();
    if (nowMus - lastUpdateMus > DEBOUNCE_MUS && Trackball.send()) {
        lastUpdateMus = nowMus;
    }
}
