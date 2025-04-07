#define PMW3389_SENSOR_MOT_PIN 3
#define PMW3389_SENSOR_RESET_PIN 4
#define PMW3389_SENSOR_NCS_PIN 5

#define MOUSE_LEFT_BUTTON_PIN 6
#define MOUSE_FORWARD_BUTTON_PIN 7
#define MOUSE_RIGHT_BUTTON_PIN 18
#define MOUSE_MIDDLE_BUTTON_PIN 19
#define MOUSE_BACK_BUTTON_PIN 20
#define MOUSE_EXTRA1_BUTTON_PIN 8
#define MOUSE_EXTRA2_BUTTON_PIN 9


#include <math.h>

#include <EEPROM.h>
#include <Keyhole.h>
#include <SPI.h>

#include "Acceleration.h"
#include "Trackball.h"
#include "PMW3389.h"


// config variables
bool enableMoveAccel = true;
bool enableScrollAccel = true;

uint16_t throttleMus = 10000;
uint16_t sensorCpi = 800;


// state variables
PMW3389 sensor;
PMW3389_DATA sensorData = {};
double sensorScale = 0.1;
double sensorAccumulatedX = 0.0;
double sensorAccumulatedY = 0.0;

uint64_t nowMus = 0;
uint64_t lastLoopMus = 0;
uint64_t lastUpdateMus = 0;

uint8_t buttons = 0b00000000;
uint8_t lastButtons = 0b00000000;


template <typename T, typename ...Args>
void prints(const T& arg, Args... args) {
  if (!Serial1) {
    return;
  }
  
  Serial1.print(arg);

  if constexpr ((sizeof...(Args)) > 0) {
    prints(args...);
  }
}


template <typename T, typename ...Args>
void printsln(const T& arg, Args... args) {
  if (!Serial1) {
    return;
  }
  
  prints(arg, args...);
  Serial1.println();
}


static auto subtractMaxIntegral(double& value, double factor) -> int16_t {
  double valueF = 0.0;
  value = modf(value * factor, &valueF) / factor;
  return static_cast<int16_t>(valueF);
}


static void readConfig() {
  size_t pos = 0;

  EEPROM.get(pos, sensorCpi);
  pos += sizeof(sensorCpi);

  EEPROM.get(pos, throttleMus);
  pos += sizeof(throttleMus);

  EEPROM.get(pos, enableMoveAccel);
  pos += sizeof(enableMoveAccel);

  EEPROM.get(pos, enableScrollAccel);
  pos += sizeof(enableScrollAccel);

  uint8_t buttonMap[8];
  EEPROM.get(pos, buttonMap);
  pos += sizeof(buttonMap);
  Trackball.setMappings(buttonMap, sizeof(buttonMap));
}


static void writeConfig() {
  size_t pos = 0;

  EEPROM.put(pos, sensorCpi);
  pos += sizeof(sensorCpi);

  EEPROM.put(pos, throttleMus);
  pos += sizeof(throttleMus);

  EEPROM.put(pos, enableMoveAccel);
  pos += sizeof(enableMoveAccel);

  EEPROM.put(pos, enableScrollAccel);
  pos += sizeof(enableScrollAccel);

  uint8_t buttonMap[8];
  Trackball.getMappings(buttonMap, sizeof(buttonMap));
  EEPROM.put(pos, buttonMap);
  pos += sizeof(buttonMap);
}


static void resetConfig() {
  enableMoveAccel = true;
  enableScrollAccel = true;

  throttleMus = 10000;
  sensorCpi = 800;

  Trackball.setMapping(MOUSE_LEFT, MOUSE_LEFT);
  Trackball.setMapping(MOUSE_RIGHT, MOUSE_RIGHT);
  Trackball.setMapping(MOUSE_BACK, MOUSE_BACK);
  Trackball.setMapping(MOUSE_MIDDLE, MOUSE_MIDDLE);
  Trackball.setMapping(MOUSE_FORWARD, MOUSE_FORWARD);
  Trackball.setMapping(MOUSE_EXTRA1, MOUSE_EXTRA1);
  Trackball.setMapping(MOUSE_EXTRA2, MOUSE_EXTRA2);

  writeConfig();

  if (sensor.getCPI() != sensorCpi) {
    sensor.setCPI(sensorCpi);
  }
}


void setup() {
  Serial1.begin(9600);
  for (int i = 0; !Serial1 && i < 10; i++) {
    delay(100);
  }


  // DEBUG
  Serial1.println("WARNING: RESETTING CONFIG");
  resetConfig();

  printsln("EEPROM capacity: ", EEPROM.length(), "B");

  prints("Initializing pull-up resistors... ");
  pinMode(MOUSE_LEFT_BUTTON_PIN, INPUT_PULLUP);
  pinMode(MOUSE_RIGHT_BUTTON_PIN, INPUT_PULLUP);
  pinMode(MOUSE_BACK_BUTTON_PIN, INPUT_PULLUP);
  pinMode(MOUSE_FORWARD_BUTTON_PIN, INPUT_PULLUP);
  pinMode(MOUSE_MIDDLE_BUTTON_PIN, INPUT_PULLUP);
  pinMode(MOUSE_EXTRA1_BUTTON_PIN, INPUT_PULLUP);
  pinMode(MOUSE_EXTRA2_BUTTON_PIN, INPUT_PULLUP);
  printsln("done.");

  prints("Reading config from EEPROM... ");
  readConfig();
  printsln("done.");

  prints("Initializing sensor... ");
  pinMode(PMW3389_SENSOR_NCS_PIN, OUTPUT);
  digitalWrite(PMW3389_SENSOR_NCS_PIN, HIGH);
  pinMode(PMW3389_SENSOR_RESET_PIN, OUTPUT);
  digitalWrite(PMW3389_SENSOR_RESET_PIN, HIGH);
  // signature check fails, but device works regardless
  sensor.begin(PMW3389_SENSOR_NCS_PIN, sensorCpi);
  printsln("done.");

  prints("Initializing HID device... ");
  Trackball.begin();
  Trackball.setMoveScale(0.50, 0.50);
  Trackball.setScrollScale(0.50, 0.50);
  printsln("done.");

  printsln("Initialization done. Entering main loop.");

  nowMus = micros();
  lastLoopMus = nowMus;
  lastUpdateMus = nowMus;
}


void loop() {
  nowMus = micros();

  if (nowMus - lastLoopMus > 0) {
    sensorData = sensor.readBurst();
    
    sensorAccumulatedX += sensorData.dx;
    sensorAccumulatedY += sensorData.dy;

    int16_t dx = subtractMaxIntegral(sensorAccumulatedX, sensorScale);
    int16_t dy = subtractMaxIntegral(sensorAccumulatedY, sensorScale);

    Trackball.move(-dx, dy);

    Trackball.set(MOUSE_LEFT, (digitalRead(MOUSE_LEFT_BUTTON_PIN) == LOW));
    Trackball.set(MOUSE_RIGHT, (digitalRead(MOUSE_RIGHT_BUTTON_PIN) == LOW));
    Trackball.set(MOUSE_BACK, (digitalRead(MOUSE_BACK_BUTTON_PIN) == LOW));
    Trackball.set(MOUSE_FORWARD, (digitalRead(MOUSE_FORWARD_BUTTON_PIN) == LOW));
    Trackball.set(MOUSE_MIDDLE, (digitalRead(MOUSE_MIDDLE_BUTTON_PIN) == LOW));
    Trackball.set(MOUSE_EXTRA1, (digitalRead(MOUSE_EXTRA1_BUTTON_PIN) == LOW));
    Trackball.set(MOUSE_EXTRA2, (digitalRead(MOUSE_EXTRA2_BUTTON_PIN) == LOW));
  }

  KEYHOLE keyhole(Serial1);
  if (keyhole.begin()) {
    if (keyhole.command("reset!")) {
      printsln("Resetting config.");
      resetConfig();
    }

    if (keyhole.command("write!")) {
      printsln("Writing config.");
      writeConfig();
    }

    uint8_t buttonMap[8];
    Trackball.getMappings(buttonMap, sizeof(buttonMap));

    keyhole.variable("button_left", buttonMap[MOUSE_LEFT]);
    keyhole.variable("button_right", buttonMap[MOUSE_RIGHT]);
    keyhole.variable("button_back", buttonMap[MOUSE_BACK]);
    keyhole.variable("button_forward", buttonMap[MOUSE_FORWARD]);
    keyhole.variable("button_middle", buttonMap[MOUSE_MIDDLE]);
    keyhole.variable("button_extra1", buttonMap[MOUSE_EXTRA1]);
    keyhole.variable("button_extra2", buttonMap[MOUSE_EXTRA2]);

    keyhole.variable("move_accel", enableMoveAccel);
    keyhole.variable("scroll_accel", enableScrollAccel);
    keyhole.variable("throttle_mus", throttleMus);

    keyhole.variable("sensor_cpi", sensorCpi);

    keyhole.end();

    sensor.setCPI(sensorCpi);
    Trackball.setMappings(buttonMap, sizeof(buttonMap));
  }

  Trackball.send(nowMus);
  lastUpdateMus = nowMus;
  lastLoopMus = nowMus;
}
