#ifndef _CLOCK_TM1637_NTP_H
#define _CLOCK_TM1637_NTP_H

#include <WiFiUdp.h>

time_t getNtpTime(void);
unsigned long sendNTPpacket(IPAddress& address);
void ntpBegin(void);

#endif
