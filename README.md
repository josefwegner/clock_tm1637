# Simple Digital Clock Sketch for Arduino Nano RP2040 Connect

This sketchs shows the current local time on a 4-digit LED display with a
TM1637 driver IC.

It connects to WiFi and requests the current time from a NTP server, sets the
time in the builtin RTC, calculates the local time and displays it on the LED
display.

It might be interesting if you want to see how you can convert an Epoch / Unix
timestamp to local time. The calculation takes leap years in account, but has
the following shortcomings:
- it does not handle leap seconds
- timezone handling is hard coded for CET and CEST
- it is not particular optimized nor pretty

## Wiring
| Nano RP2040 Connect | TM1637 Display |
|---------------------|----------------|
| D2                  | CLK            |
| D3                  | DIO            |
| +3V3V               | VCC            |
| GND                 | GND            |

## WiFi Authentication
Create a file called arduino_secrets.h with the following contents:
``` c
// arduino_secrets.h
#define SECRET_SSID "<YOUR 2.4 GHz WIFI NAME>"
#define SECRET_PASS "<YOUR WIFI PASSWORD>"
```
