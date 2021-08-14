/*
 Epoch -> datetime_t conversion

 dayofweek code
 by Tomohiko Sakamoto

 other code
 created 5 Aug 2021
 by Josef Wegner

 This code is in the public domain.
 */
#include "convert_time.h"

#ifdef STANDALONE
#include <stdio.h>
char* weekday[] = {"Sun", "Mon", "Tue", "Wed", "Thu", "Fri", "Sat"};
#endif

static unsigned int days[2][12] = {
  {31, 28, 31, 30, 31, 30, 31, 31, 30, 31, 30, 31}, // normal year
  {31, 29, 31, 30, 31, 30, 31, 31, 30, 31, 30, 31}  // leap year
};

void convertEpochLocal(const time_t epoch, datetime_t *datetime) {
  // CEST starts last Sunday in March, ends last Sunday in October
  // okay, we are in Germany, so at least add one hour
  convertEpochUTC(epoch + 3600, datetime);
  if (isDayLightSavingTime(datetime)) {
      addOffset(datetime);
  }
}

int isDayLightSavingTime(const datetime_t *datetime) {
  // easy cases - always normal time
  if (datetime->month < 3 || datetime->month > 10) {
    return 0;
  }

  // always daylight savings time in Germany
  if (datetime->month > 3 && datetime->month < 10) {
    return 1;
  }

  // difficult case, need to calculate exact start and end date
  // of daylight savings time
  // calculate day of the week for April 1st and November 1st
  int lastSundayOfMarch = 31 - ((dayofweek(datetime->year, 4, 1) + 6) % 7);
  int lastSundayOfOctober = 31 - dayofweek(datetime->year, 10, 31);
#if defined(STANDALONE) && defined(DEBUG)
  printf("day of the week April 1st: %d, day of the week October 31st: %d\n", dayofweek(datetime->year, 4, 1), dayofweek(datetime->year, 10, 31));
  printf("lastSundayOfMarch: %d, lastSundayOfOctober: %d\n", lastSundayOfMarch, lastSundayOfOctober);
#endif
  if (datetime->month == 3) {
    if (datetime->day < lastSundayOfMarch) {
      return 0;
    }
    if (datetime->day > lastSundayOfMarch) {
      return 1;
    }

    // complicate case, compare hours
    if (datetime->hour >= 2) {
      return 1;
    } else {
      return 0;
    }
  }

  if (datetime->day < lastSundayOfOctober) {
    return 1;
  }
  if (datetime->day > lastSundayOfOctober) {
    return 0;
  }

  // complicate case, compare hours
  if (datetime->hour < 3) {
#if defined(STANDALONE) && defined(DEBUG)
    printf("still summer time in October after checking hour; day = %d, hour = %d\n", datetime->day, datetime->hour);
#endif
    return 1;
  } else {
#if defined(STANDALONE) && defined(DEBUG)
    printf("again default time in October after checking hour; day = %d, hour = %d\n", datetime->day, datetime->hour);
#endif
    return 0;
  }
}

void addOffset(datetime_t *datetime) {
  datetime->hour++;
  if (datetime->hour > 23) {
    datetime->hour = 0;
    datetime->day++;
    datetime->dotw = (datetime->dotw + 1) % 7;
    if (datetime->day > days[isLeapYear(datetime->year)][datetime->month - 1]) {
      datetime->day = 1;
      datetime->month++;
      if (datetime->month > 12) {
        datetime->month = 1;
        datetime->year++;
      }
    }
  }
}

int isLeapYear(const unsigned int year) {
  return year % 4 == 0 && (year % 100 != 0 || year % 400 == 0);
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

  leapyear = isLeapYear(year);

  while (tmp >= days[leapyear][month - 1]) {
#if defined(STANDALONE) && defined(DEBUG)
    printf("year: %d, month: %d, is leap year?: %d, tmp: %ld\n", year, month, leapyear, tmp);
#endif
    tmp -= days[leapyear][month - 1];
    month++;
  }
#if defined(STANDALONE) && defined(DEBUG)
    printf(" year: %d, month: %d, is leap year?: %d, tmp: %ld\n", year, month, leapyear, tmp);
#endif
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

void convertToLocalTime(datetime_t *datetime) {
  // add one hour for CET
  addOffset(datetime);
  if (isDayLightSavingTime(datetime)) {
    // add another hour for CEST
    addOffset(datetime);
  }
}

#ifdef STANDALONE
int main(void) {
  time_t epoch = time(NULL);

  datetime_t tm;
  convertEpochLocal(epoch, &tm);

  printf("current: %04ld-%02d-%02d (%s) %02d:%02d:%02d\n\n", tm.year, tm.month, tm.day, weekday[tm.dotw], tm.hour, tm.min, tm.sec);

  // some tests
  convertEpochLocal(0L, &tm);
  printf("returned: %04ld-%02d-%02d (%s) %02d:%02d:%02d\n", tm.year, tm.month, tm.day, weekday[tm.dotw], tm.hour, tm.min, tm.sec);
  printf("expected: 1970-01-01 (Thu) 01:00:00\n\n");

  // some tests
  convertEpochLocal(26179200L, &tm);
  printf("returned: %04ld-%02d-%02d (%s) %02d:%02d:%02d\n", tm.year, tm.month, tm.day, weekday[tm.dotw], tm.hour, tm.min, tm.sec);
  printf("expected: 1970-10-31 (Sat) 01:00:00\n\n");

  convertEpochLocal(1630452461L, &tm);
  printf("returned: %04ld-%02d-%02d (%s) %02d:%02d:%02d\n", tm.year, tm.month, tm.day, weekday[tm.dotw], tm.hour, tm.min, tm.sec);
  printf("expected: 2021-09-01 (Wed) 01:27:41\n\n");

  convertEpochLocal(1619825261L, &tm);
  printf("returned: %04ld-%02d-%02d (%s) %02d:%02d:%02d\n", tm.year, tm.month, tm.day, weekday[tm.dotw], tm.hour, tm.min, tm.sec);
  printf("expected: 2021-05-01 (Sat) 01:27:41\n\n");

  convertEpochLocal(1614554861L, &tm);
  printf("returned: %04ld-%02d-%02d (%s) %02d:%02d:%02d\n", tm.year, tm.month, tm.day, weekday[tm.dotw], tm.hour, tm.min, tm.sec);
  printf("expected: 2021-03-01 (Mon) 00:27:41\n\n");

  convertEpochLocal(1583018861L, &tm);
  printf("returned: %04ld-%02d-%02d (%s) %02d:%02d:%02d\n", tm.year, tm.month, tm.day, weekday[tm.dotw], tm.hour, tm.min, tm.sec);
  printf("expected: 2020-03-01 (Sun) 00:27:41\n\n");

  convertEpochLocal(1616865701L, &tm);
  printf("returned: %04ld-%02d-%02d (%s) %02d:%02d:%02d\n", tm.year, tm.month, tm.day, weekday[tm.dotw], tm.hour, tm.min, tm.sec);
  printf("expected: 2021-03-27 (Sat) 18:21:41\n\n");

  convertEpochLocal(1616890901L, &tm);
  printf("returned: %04ld-%02d-%02d (%s) %02d:%02d:%02d\n", tm.year, tm.month, tm.day, weekday[tm.dotw], tm.hour, tm.min, tm.sec);
  printf("expected: 2021-03-28 (Sun) 01:21:41\n\n");

  convertEpochLocal(1616894501L, &tm);
  printf("returned: %04ld-%02d-%02d (%s) %02d:%02d:%02d\n", tm.year, tm.month, tm.day, weekday[tm.dotw], tm.hour, tm.min, tm.sec);
  printf("expected: 2021-03-28 (Sun) 03:21:41\n\n");

  convertEpochLocal(1635646901L, &tm);
  printf("returned: %04ld-%02d-%02d (%s) %02d:%02d:%02d\n", tm.year, tm.month, tm.day, weekday[tm.dotw], tm.hour, tm.min, tm.sec);
  printf("expected: 2021-10-31 (Sun) 03:21:41\n\n");

  convertEpochLocal(1635639701L, &tm);
  printf("returned: %04ld-%02d-%02d (%s) %02d:%02d:%02d\n", tm.year, tm.month, tm.day, weekday[tm.dotw], tm.hour, tm.min, tm.sec);
  printf("expected: 2021-10-31 (Sun) 02:21:41\n\n");

  convertEpochLocal(1603498901L, &tm);
  printf("returned: %04ld-%02d-%02d (%s) %02d:%02d:%02d\n", tm.year, tm.month, tm.day, weekday[tm.dotw], tm.hour, tm.min, tm.sec);
  printf("expected: 2020-10-24 (Sat) 02:21:41\n\n");
}
#endif

