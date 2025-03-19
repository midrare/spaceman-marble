#include <SPI.h>

#include "PMW3389.h"
#include "Trackball.h"

#define PMW3389_SENSOR_MOT_PIN 3
#define PMW3389_SENSOR_RESET_PIN 4
#define PMW3389_SENSOR_NCS_PIN 5
#include "PMW3389.h"

#define MOUSE_LEFT_BUTTON_PIN 6
#define MOUSE_FORWARD_BUTTON_PIN 7

#define MOUSE_RIGHT_BUTTON_PIN 18
#define MOUSE_BACK_BUTTON_PIN 19

#define MOUSE_MIDDLE_BUTTON_PIN 20
#define MOUSE_EXTRA1_BUTTON_PIN 8
#define MOUSE_EXTRA2_BUTTON_PIN 9

#define DEFAULT_CPI 800u
#define DEBOUNCE_MUS 250


PMW3389 sensor;
PMW3389_DATA sensorData = {};

uint64_t lastUpdateMus = 0;
uint64_t nowMus = 0;


void setup() {
  Serial1.begin(9600);
  for (int i = 0; !Serial1 && i < 10; i++) {
    delay(100);
  }

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
  Trackball.setMoveScale(0.05, 0.05);
  Trackball.setScrollScale(0.05, 0.05);
}


uint8_t lastButtons = 0x00;

void loop() {
  sensorData = sensor.readBurst();
  if (sensorData.isOnSurface && sensorData.isMotion) {
    Trackball.move(-sensorData.dx, sensorData.dy);
  }

  uint8_t buttons = 0b00000000
                    | ((digitalRead(MOUSE_LEFT_BUTTON_PIN) == LOW) << 0)
                    | ((digitalRead(MOUSE_RIGHT_BUTTON_PIN) == LOW) << 1)
                    | ((digitalRead(MOUSE_BACK_BUTTON_PIN) == LOW) << 2)
                    | ((digitalRead(MOUSE_FORWARD_BUTTON_PIN) == LOW) << 3)
                    | ((digitalRead(MOUSE_MIDDLE_BUTTON_PIN) == LOW) << 4)
                    | ((digitalRead(MOUSE_EXTRA1_BUTTON_PIN) == LOW) << 5)
                    | ((digitalRead(MOUSE_EXTRA2_BUTTON_PIN) == LOW) << 6);
  Trackball.set(buttons);

#if defined(DEBOUNCE_MUS) && (DEBOUNCE_MUS > 0)
  nowMus = micros();
  if (nowMus - lastUpdateMus > DEBOUNCE_MUS && Trackball.send()) {
    if (Trackball.send()) {
      if (Serial1) { 
        Serial1.print("(");
        Serial1.print(sensorData.dx);
        Serial1.print(", ");
        Serial1.print(sensorData.dy);
        Serial1.print(") ");
        Serial1.print(buttons);
        Serial1.println();
      }

      lastUpdateMus = nowMus;
    }
  }
#else
  Trackball.send();
#endif
}
