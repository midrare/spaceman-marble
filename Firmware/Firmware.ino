#include <SPI.h>
#include <Wire.h>

#include "PMW3389.h"
#include "Trackball.h"

#define PMW3389_SENSOR_RESET_PIN 4
#define PMW3389_SENSOR_NCS_PIN 5
#include "PMW3389.h"

#define SERIAL_ENABLED 1
#define SERIAL_TIMEOUT 5000
#define SERIAL_BAUD 9600

#define MOUSE_LEFT_BUTTON_PIN 18
#define MOUSE_RIGHT_BUTTON_PIN 19
#define MOUSE_BACK_BUTTON_PIN 20
#define MOUSE_FORWARD_BUTTON_PIN 21
#define MOUSE_MIDDLE_BUTTON_PIN 7
#define MOUSE_EXTRA1_BUTTON_PIN 8
#define MOUSE_EXTRA2_BUTTON_PIN 9



PMW3389 sensor;
PMW3389_DATA sensorData = {};

void setup() {
    #if defined(SERIAL_ENABLED) && (SERIAL_ENABLED > 0)
    Serial.begin(SERIAL_BAUD);
    for (int i=0; i<SERIAL_TIMEOUT; i+=50) {
      if (Serial) {
        break;
      }
      delay(50);
    }
    Serial.println("Initializing...");
    #endif

    #if defined(SERIAL_ENABLED) && (SERIAL_ENABLED > 0)
    Serial.println("Initializing button pins...");
    #endif
    pinMode(MOUSE_LEFT_BUTTON_PIN, INPUT_PULLUP);
    pinMode(MOUSE_RIGHT_BUTTON_PIN, INPUT_PULLUP);
    pinMode(MOUSE_BACK_BUTTON_PIN, INPUT_PULLUP);
    pinMode(MOUSE_FORWARD_BUTTON_PIN, INPUT_PULLUP);
    pinMode(MOUSE_MIDDLE_BUTTON_PIN, INPUT_PULLUP);
    pinMode(MOUSE_EXTRA1_BUTTON_PIN, INPUT_PULLUP);
    pinMode(MOUSE_EXTRA2_BUTTON_PIN, INPUT_PULLUP);


    #if defined(SERIAL_ENABLED) && (SERIAL_ENABLED > 0)
    Serial.println("Initializing sensor...");
    #endif
    pinMode(PMW3389_SENSOR_NCS_PIN, OUTPUT);
    digitalWrite(PMW3389_SENSOR_NCS_PIN, HIGH);
    pinMode(PMW3389_SENSOR_RESET_PIN, OUTPUT);
    digitalWrite(PMW3389_SENSOR_RESET_PIN, HIGH);
    // signature check fails, but device works regardless
    sensor.begin(PMW3389_SENSOR_NCS_PIN);

    #if defined(SERIAL_ENABLED) && (SERIAL_ENABLED > 0)
    Serial.println("Initializing mouse...");
    #endif
    Trackball.begin();
    #if defined(SERIAL_ENABLED) && (SERIAL_ENABLED > 0)
    Serial.println("Initialization done. Entering main loop...");
    #endif
}


void loop() {
    sensorData = sensor.readBurst();
    if (sensorData.isOnSurface && sensorData.isMotion) {
        #if defined(SERIAL_ENABLED) && (SERIAL_ENABLED > 0)
        Serial.print("Motion: ");
        Serial.print(sensorData.dx);
        Serial.print(", ");
        Serial.println(sensorData.dy);
        #endif
    }
}
