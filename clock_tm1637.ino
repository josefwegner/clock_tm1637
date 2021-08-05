/*
 Udp NTP Client

 Get the time from a Network Time Protocol (NTP) time server
 Demonstrates use of UDP sendPacket and ReceivePacket
 For more on NTP time servers and the messages needed to communicate with them,
 see http://en.wikipedia.org/wiki/Network_Time_Protocol

 created 4 Sep 2010
 by Michael Margolis
 modified 9 Apr 2012
 by Tom Igoe

 This code is in the public domain.

 */

extern "C"
{
  void rtc_init           (void);
  bool rtc_get_datetime(datetime_t *t);
  bool rtc_set_datetime(datetime_t *t);
}

#include <TM1637Display.h>
#include <mbed.h>
#include "hardware/rtc.h"
#include "ntp.hh"
#include "wifi.hh"

using namespace std::chrono;
mbed::Ticker displayTicker;

// Instantiation and pins configurations
// Pin 3 - > DIO
// Pin 2 - > CLK
TM1637Display tm1637(2, 3);

void setup() {
  // Open serial communications and wait for port to open:
  Serial.begin(9600);
  while (!Serial) {
    ; // wait for serial port to connect. Needed for native USB port only
  }

  Serial.println("Init display");
  tm1637.setBrightness(5);
  tm1637.clear();

  Serial.println("Init RTC");
  rtc_init();

  Serial.println("Init Wifi");
  wifiBegin();
  ntpBegin();

  //Serial.println("Attaching display function");
  //displayTicker.attach(&displayTickerFunc, 200ms);
}

void loop() {
  datetime_t currTime;
  time_t epoch = getNtpTime();
  if (epoch > 0) {
    convertEpoch(epoch, &currTime);
    if (!rtc_set_datetime(&currTime)) {
      Serial.println("Error while setting rtc");
    }
  }
  delay(1000*60*60);
}

void convertEpoch(const time_t epoch, datetime_t *datetime) {
  unsigned long tmp = epoch;
  datetime->sec = tmp % 60; tmp /= 60;
  datetime->min = tmp % 60; tmp /= 60;
  datetime->hour = tmp % 24; tmp /= 24;

  unsigned int year = 1970;
  unsigned int dotw = 4; // 1970-1-1 was a Thursday
  unsigned int month = 1;
  unsigned int leapyear = false;
  while (tmp > 365) {
    // check for leap year
    if (year % 4 == 0 && (year % 100 != 0 || year % 400 == 0)) {
      tmp--;
      dotw += 2;
    } else {
      dotw++;
    }
    tmp -= 365;
    year++;
  }

  leapyear = year % 4 == 0 && (year % 100 != 0 || year % 400 == 0);

  while (tmp > 28) {
    if (month == 2) {
      if (leapyear) {
        tmp--;
        dotw++;
      }
    } else if (month == 1 || month == 3 || month == 5 || month == 7 || month == 8 || month == 10 || month == 12) {
      tmp -= 3;
      dotw += 3;
    } else {
      tmp -= 2;
      dotw += 2;
    }
    tmp -= 28;
    month++;
  }
  datetime->year = year;
  datetime->month = month;
  datetime->day = tmp + 1;
  datetime->dotw = (dotw + tmp) % 7;
}

void displayTickerFunc(void) {
  datetime_t currTime;
  if (rtc_get_datetime(&currTime)) {
    tm1637.showNumberDecEx(currTime.hour * 100 + currTime.min, (currTime.sec % 2) << 6, true);
  } else {
    Serial.println("Error while reading rtc");
  }
}

