#include <SPI.h>
#include <Wire.h>

#include "PMW3389.h"
#include "Trackball.h"

#define PMW3389_SENSOR_RESET_PIN 4
#define PMW3389_SENSOR_NCS_PIN 5
#include "PMW3389.h"

#define SERIAL_TIMEOUT 5000
#define SERIAL_BAUD 9600

#define MOUSE_LEFT_BUTTON_PIN 18
#define MOUSE_RIGHT_BUTTON_PIN 19
#define MOUSE_BACK_BUTTON_PIN 20
#define MOUSE_FORWARD_BUTTON_PIN 21
#define MOUSE_MIDDLE_BUTTON_PIN 7
#define MOUSE_EXTRA1_BUTTON_PIN 8
#define MOUSE_EXTRA2_BUTTON_PIN 9

#define DEFAULT_CPI 100


PMW3389 sensor;
PMW3389_DATA sensorData = {};

void setup() {
    if (SERIAL_BAUD >= 0) {
        Serial.begin(SERIAL_BAUD);
        if (SERIAL_TIMEOUT < 0) {
            while (!Serial) {
                delay(50);
            }
        } else {
            for (int i=0; i<SERIAL_TIMEOUT && !Serial; i+=50) {
              delay(50);
            }
        }
    }

    Serial.println("Initializing...");

    Serial.println("Initializing button pins...");
    pinMode(MOUSE_LEFT_BUTTON_PIN, INPUT_PULLUP);
    pinMode(MOUSE_RIGHT_BUTTON_PIN, INPUT_PULLUP);
    pinMode(MOUSE_BACK_BUTTON_PIN, INPUT_PULLUP);
    pinMode(MOUSE_FORWARD_BUTTON_PIN, INPUT_PULLUP);
    pinMode(MOUSE_MIDDLE_BUTTON_PIN, INPUT_PULLUP);
    pinMode(MOUSE_EXTRA1_BUTTON_PIN, INPUT_PULLUP);
    pinMode(MOUSE_EXTRA2_BUTTON_PIN, INPUT_PULLUP);


    Serial.println("Initializing sensor...");
    pinMode(PMW3389_SENSOR_NCS_PIN, OUTPUT);
    digitalWrite(PMW3389_SENSOR_NCS_PIN, HIGH);
    pinMode(PMW3389_SENSOR_RESET_PIN, OUTPUT);
    digitalWrite(PMW3389_SENSOR_RESET_PIN, HIGH);
    // signature check fails, but device works regardless
    sensor.begin(PMW3389_SENSOR_NCS_PIN, DEFAULT_CPI);

    Serial.println("Initializing mouse...");
    Trackball.begin();
    Trackball.setMoveScale(0.1, 0.1);
    Trackball.setScrollScale(0.01, 0.01);
    Serial.println("Initialization done. Entering main loop...");
}


void loop() {
    sensorData = sensor.readBurst();
    if (sensorData.isOnSurface && sensorData.isMotion) {
        Trackball.move(sensorData.dx, sensorData.dy);
    }

    Trackball.send();
}
