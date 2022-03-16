/*
   Simple Digital Clock Sketch for Arduino Nano RP2040 Connect

   This sketchs shows the current local time on a 4-digit LED display with a
   TM1637 driver IC.

   WiFi and NTP code
   created 4 Sep 2010
   by Michael Margolis
   modified 9 Apr 2012
   by Tom Igoe

   dayofweek code
   by Tomohiko Sakamoto

   other code
   created 5 Aug 2021
   by Josef Wegner

   This code is in the public domain.
 */

// workaround for missing extern "C" in pico-sdk 1.0 used by
// ArduinoCore-mbed (https://github.com/arduino/ArduinoCore-mbed/issues/300)
#ifdef __cplusplus
extern "C"
{
  void rtc_init           (void);
  bool rtc_get_datetime(datetime_t *t);
  bool rtc_set_datetime(datetime_t *t);
}
#endif

const uint8_t numbers[10] = {0x3F, 0x06, 0x5B, 0x4F, 0x66, 0x6D, 0x7D, 0x07, 0x7F, 0x6F};
const uint8_t weekdays[7][2] = {{0x2C, 0x5C}, {0x55, 0x5C}, {0x5E, 0x11}, {0x55, 0x11}, {0x5E, 0x5C}, {0x71, 0x50}, {0x2D, 0x5F}};

#include <TM1637.h>
#include <RotaryEncoder.h>
#include <mbed.h>
#include "ntp.hh"
#include "wifi.hh"
#include "convert_time.h"

using namespace std::chrono;

unsigned int counter = 0;

// Rotary Encoder Inputs
#define INPUT_CLK 4
#define INPUT_DT 5
#define INPUT_T 6
RotaryEncoder *encoder = nullptr;
volatile boolean pressed = false;

// Instantiation and pins configurations
// Pin 3 - > DIO
// Pin 2 - > CLK
TM1637 tm1637(2, 3);

void setup() {
  // Set encoder pins as inputs
  pinMode (INPUT_T, INPUT);

  // Open serial communications and wait for port to open:
  Serial1.begin(9600);

  Serial1.println("Init display");
  tm1637.init();
  tm1637.setBrightness(5);
  tm1637.clearScreen();

  Serial1.println("Init RTC");
  rtc_init();

  Serial1.println("Init Wifi");
  wifiBegin();
  ntpBegin();

  Serial1.println("Setting up interrupts for rotary encode");
  encoder = new RotaryEncoder(INPUT_CLK, INPUT_DT, RotaryEncoder::LatchMode::FOUR3);
  attachInterrupt(digitalPinToInterrupt(INPUT_CLK), checkPosition, CHANGE);
  attachInterrupt(digitalPinToInterrupt(INPUT_DT), checkPosition, CHANGE);
  attachInterrupt(digitalPinToInterrupt(INPUT_T), doPress, CHANGE);
}

void loop() {
  datetime_t currTime;
  uint8_t buffer[4] = {0, 0, 0, 0};
  static int pos = 0;
  static int option = 0;

  encoder->tick();

  int newPos = encoder->getPosition();

  if (pos != newPos) {
    option = abs(newPos % 5);
    Serial1.print("option is ");
    Serial1.println(option);
    pos = newPos;
  }

  if (counter == 0) {
    time_t epoch = getNtpTime();
    if (epoch > 0) {
      epoch_to_utc(epoch, &currTime);
      if (!rtc_set_datetime(&currTime)) {
        Serial1.println("Error while setting rtc");
      }
    }
  }

  if (rtc_get_datetime(&currTime)) {
    utc_to_local(&currTime);
    switch (option) {
      case 1:
        // minutes and seconds
        if (currTime.sec % 2) {
          tm1637.colonOff();
        } else {
          tm1637.colonOn();
        }
        if (pressed) {
          buffer[0] = numbers[currTime.hour / 10];
          buffer[1] = numbers[currTime.hour % 10];
          buffer[2] = numbers[currTime.min / 10];
          buffer[3] = numbers[currTime.min % 10];
        } else {
          buffer[0] = numbers[currTime.min / 10];
          buffer[1] = numbers[currTime.min % 10];
          buffer[2] = numbers[currTime.sec / 10];
          buffer[3] = numbers[currTime.sec % 10];
        }
        break;
      case 2:
        //day of week
        tm1637.colonOff();
        buffer[0] = weekdays[currTime.dotw][0];
        buffer[1] = weekdays[currTime.dotw][1];
        buffer[2] = numbers[currTime.day / 10];
        buffer[3] = numbers[currTime.day % 10];
        break;
      case 3:
        // days and month
        tm1637.colonOn();
        buffer[0] = numbers[currTime.day / 10];
        buffer[1] = numbers[currTime.day % 10];
        buffer[2] = numbers[currTime.month / 10];
        buffer[3] = numbers[currTime.month % 10];
        break;
      case 4:
        // year
        tm1637.colonOff();
        buffer[0] = numbers[currTime.year / 1000];
        buffer[1] = numbers[(currTime.year / 100) % 10];
        buffer[2] = numbers[(currTime.year / 10) % 10];
        buffer[3] = numbers[currTime.year % 10];
        break;
      default:
        // hours and minutes
        if (currTime.sec % 2) {
          tm1637.colonOff();
        } else {
          tm1637.colonOn();
        }
        if (pressed) {
          buffer[0] = 0;
          buffer[1] = 0;
          buffer[2] = numbers[currTime.sec / 10];
          buffer[3] = numbers[currTime.sec % 10];
        } else {
          buffer[0] = numbers[currTime.hour / 10];
          buffer[1] = numbers[currTime.hour % 10];
          buffer[2] = numbers[currTime.min / 10];
          buffer[3] = numbers[currTime.min % 10];
        }
    }
    tm1637.displayRawBytes(buffer, 4)->refresh();
  }

  counter++;
  // reset counter after roughly 1h to refresh time
  if (counter > 1000 * 60 * 60 * 5) {
    counter = 0;
  }

  delay(200);
}

void checkPosition() {
  encoder->tick(); // just call tick() to check the state.
}

void doPress() {
  pressed = !pressed;
}
