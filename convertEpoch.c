#ifdef STANDALONE
#include <time.h>
#include <stdio.h>

typedef struct {
    long year;    ///< 0..4095
    int month;    ///< 1..12, 1 is January
    int day;      ///< 1..28,29,30,31 depending on month
    int dotw;     ///< 0..6, 0 is Sunday
    int hour;     ///< 0..23
    int min;      ///< 0..59
    int sec;      ///< 0..59
} datetime_t;

char* days[] = {"Sun", "Mon", "Tue", "Wed", "Thu", "Fri", "Sat"};
#define false 0
#define true 1
#endif

void convertEpoch(const time_t epoch, datetime_t *datetime) {
  unsigned long tmp = epoch;
  datetime->sec = tmp % 60; tmp /= 60;
  datetime->min = tmp % 60; tmp /= 60;
  datetime->hour = tmp % 24; tmp /= 24;
  datetime->dotw = ((tmp + 4) % 7); // 1970-1-1 was a Thursday

  unsigned int year = 1970;
  unsigned int month = 1;
  unsigned int leapyear = false;
  while (tmp > 365) {
    // check for leap year
    if (year % 4 == 0 && (year % 100 != 0 || year % 400 == 0)) {
      tmp--;
    }
    tmp -= 365;
    year++;
  }

  leapyear = year % 4 == 0 && (year % 100 != 0 || year % 400 == 0);

  while (tmp > 28) {
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


#ifdef STANDALONE
int main(void) {
  time_t epoch = time(NULL);

  datetime_t tm;
  convertEpoch(epoch, &tm);

  printf("%04d-%02d-%02d (%s) %02d:%02d:%02d\n", tm.year, tm.month, tm.day, days[tm.dotw], tm.hour, tm.min, tm.sec);
}
#endif
