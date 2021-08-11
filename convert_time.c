#include "convert_time.h"

#ifdef STANDALONE
#include <stdio.h>
char* days[] = {"Sun", "Mon", "Tue", "Wed", "Thu", "Fri", "Sat"};
#endif

void convertEpochLocal(const time_t epoch, datetime_t *datetime) {
  // CEST starts last Sunday in March, ends last Sunday in October
  // okay, we are in Germany, so at least add one hour
  convertEpochUTC(epoch + 3600, datetime);
  if (datetime->month == 3 || datetime->month == 10) {
    // difficult case, need to calculate exact start and end date
    // of daylight savings time
    // calculate day of the week for April 1st

  } else if (datetime->month > 3 && datetime->month < 10) {
    // daylights saving time - add another hour
    datetime->hour++;
    if (datetime->hour > 23) {
      datetime->hour -= 24;
      datetime->day++; datetime->dotw = (datetime->dotw + 1) % 7;
      if (datetime->day > 30) {
        // check if we need to increase the month
        if (datetime->month == 4 || datetime->month == 6 || datetime->month == 9) {
          datetime->day -= 30;
          datetime->month++;
        } else {
          if (datetime->day > 31) {
            datetime->day -= 31;
            datetime->month++;
          }
        }
      }
    }
  }
}

void convertEpochUTC(const time_t epoch, datetime_t *datetime) {
  unsigned long tmp = epoch;
  datetime->sec = tmp % 60; tmp /= 60;
  datetime->min = tmp % 60; tmp /= 60;
  datetime->hour = tmp % 24; tmp /= 24;

  unsigned int year;
  unsigned int month;
  unsigned int leapyear;

  if (tmp > 18262) {
    // skip forward to 2020
    tmp -= 18262;
    year = 2020;
    month = 1;
    leapyear = 1;
    datetime->dotw = (tmp + 3) % 7; // 2020-1-1 was a Wednesday
  } else {
    year = 1970;
    month = 1;
    leapyear = 0;
    datetime->dotw = (tmp + 4) % 7; // 1970-1-1 was a Thursday
  }

  while (tmp > 365) {
    // check for leap year
    if (year % 4 == 0 && (year % 100 != 0 || year % 400 == 0)) {
      tmp--;
    }
    tmp -= 365;
    year++;
  }

  leapyear = year % 4 == 0 && (year % 100 != 0 || year % 400 == 0);

  while (tmp >= 28) {
#ifdef STANDALONE
    printf("year: %d, month: %d, is leap year?: %d, tmp: %ld\n", year, month, leapyear, tmp);
#endif
    if (month == 2) {
      if (leapyear) {
        tmp--;
      }
    } else if (month == 1 || month == 3 || month == 5 || month == 7 || month == 8 || month == 10 || month == 12) {
      tmp -= 3;
    } else {
      tmp -= 2;
    }
    tmp -= 28;
    month++;
  }
  datetime->year = year;
  datetime->month = month;
  datetime->day = tmp + 1;
}

unsigned int dayofweek(unsigned y, unsigned int m, unsigned int d) {
    static int t[] = {0, 3, 2, 5, 0, 3, 5, 1, 4, 6, 2, 4};
    if ( m < 3 )
    {
        y -= 1;
    }
    return (y + y/4 - y/100 + y/400 + t[m-1] + d) % 7;
}

#ifdef STANDALONE
int main(void) {
  time_t epoch = time(NULL);

  datetime_t tm;
  convertEpochLocal(epoch, &tm);

  printf("current: %04ld-%02d-%02d (%s) %02d:%02d:%02d\n\n", tm.year, tm.month, tm.day, days[tm.dotw], tm.hour, tm.min, tm.sec);

  // some tests
  convertEpochLocal(1630452461L, &tm);
  printf("returned: %04ld-%02d-%02d (%s) %02d:%02d:%02d\n", tm.year, tm.month, tm.day, days[tm.dotw], tm.hour, tm.min, tm.sec);
  printf("expected: 2021-09-01 (Wed) 01:27:41\n\n");

  convertEpochLocal(1619825261L, &tm);
  printf("returned: %04ld-%02d-%02d (%s) %02d:%02d:%02d\n", tm.year, tm.month, tm.day, days[tm.dotw], tm.hour, tm.min, tm.sec);
  printf("expected: 2021-05-01 (Sat) 01:27:41\n\n");

  convertEpochLocal(1614554861L, &tm);
  printf("returned: %04ld-%02d-%02d (%s) %02d:%02d:%02d\n", tm.year, tm.month, tm.day, days[tm.dotw], tm.hour, tm.min, tm.sec);
  printf("expected: 2021-03-01 (Mon) 00:27:41\n\n");

  convertEpochLocal(1583018861L, &tm);
  printf("returned: %04ld-%02d-%02d (%s) %02d:%02d:%02d\n", tm.year, tm.month, tm.day, days[tm.dotw], tm.hour, tm.min, tm.sec);
  printf("expected: 2020-03-01 (Sun) 00:27:41\n\n");
}
#endif

