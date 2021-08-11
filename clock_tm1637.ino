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
#ifdef __cplusplus
extern "C"
{
  void rtc_init           (void);
  bool rtc_get_datetime(datetime_t *t);
  bool rtc_set_datetime(datetime_t *t);
}
#endif

#include <TM1637Display.h>
#include <mbed.h>
#include <hardware/rtc.h>
#include "ntp.hh"
#include "wifi.hh"
#include "convert_time.h"

using namespace std::chrono;
mbed::Ticker displayTicker;

// Instantiation and pins configurations
// Pin 3 - > DIO
// Pin 2 - > CLK
TM1637Display tm1637(2, 3);

void setup() {
  // Open serial communications and wait for port to open:
  Serial1.begin(9600);
  //while (!Serial1) {
  //  ; // wait for serial port to connect. Needed for native USB port only
  //}

  Serial1.println("Init display");
  tm1637.setBrightness(5);
  tm1637.clear();

  Serial1.println("Init RTC");
  rtc_init();

  Serial1.println("Init Wifi");
  wifiBegin();
  ntpBegin();

  Serial1.println("Attaching display function");
  displayTicker.attach(&displayTickerFunc, 200ms);
}

void loop() {
  datetime_t currTime;
  time_t epoch = getNtpTime();
  if (epoch > 0) {
    convertEpochLocal(epoch, &currTime);
    if (!rtc_set_datetime(&currTime)) {
      Serial1.println("Error while setting rtc");
    }
  }
  delay(1000*60);
}

void displayTickerFunc(void) {
  datetime_t currTime;
  if (rtc_get_datetime(&currTime)) {
    tm1637.showNumberDecEx(currTime.hour * 100 + currTime.min, (currTime.sec % 2) << 6, true);
  }
}
