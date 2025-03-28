#include <SPI.h>

#include "PMW3389.h"
#include "Scroll.h"
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

ScrollOffsets scrollOff;
ScrollAcceleration scroll;

PMW3389 sensor;
PMW3389_DATA sensorData = {};

uint64_t throttleMus = 5000;

uint64_t nowMus = 0;
uint8_t buttons = 0b00000000;

uint64_t lastLoopMus = 0;
uint64_t lastUpdateMus = 0;
uint8_t lastButtons = 0x00;


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
  Trackball.setScrollScale(0.01, 0.01);

  nowMus = micros();
  lastLoopMus = nowMus;
  lastUpdateMus = nowMus;
}


void loop() {
  nowMus = micros();

  if (nowMus - lastLoopMus > 0) {
    sensorData = sensor.readBurst();
    if (sensorData.isOnSurface && sensorData.isMotion) {
      // scrollOff = scroll.scroll(sensorData.dx, sensorData.dy, nowMus / 1000, false);
      // Trackball.scroll(-scrollOff.dx, scrollOff.dy);
      Trackball.move(sensorData.dx, sensorData.dy);
    }

    buttons = 0b00000000
      | ((digitalRead(MOUSE_LEFT_BUTTON_PIN) == LOW) << 0)
      | ((digitalRead(MOUSE_RIGHT_BUTTON_PIN) == LOW) << 1)
      | ((digitalRead(MOUSE_BACK_BUTTON_PIN) == LOW) << 2)
      | ((digitalRead(MOUSE_FORWARD_BUTTON_PIN) == LOW) << 3)
      | ((digitalRead(MOUSE_MIDDLE_BUTTON_PIN) == LOW) << 4)
      | ((digitalRead(MOUSE_EXTRA1_BUTTON_PIN) == LOW) << 5)
      | ((digitalRead(MOUSE_EXTRA2_BUTTON_PIN) == LOW) << 6);
    Trackball.set(buttons);
  }

  if ((throttleMus <= 0 || ((nowMus - lastUpdateMus) > throttleMus)) && Trackball.send()) {
    lastUpdateMus = nowMus;
  }
  
  lastLoopMus = nowMus;
}
