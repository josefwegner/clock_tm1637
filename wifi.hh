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

#ifndef _CLOCK_TM1637_WIFI_H
#define _CLOCK_TM1637_WIFI_H

#include <SPI.h>
#include <WiFiNINA.h>

void wifiBegin(void);
void printWifiStatus(void);

#endif
