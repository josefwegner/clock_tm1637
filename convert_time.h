#ifndef CONVERTEPOCH_H
#define CONVERTEPOCH_H

#include <time.h>

#ifdef STANDALONE
typedef struct {
    long year;    ///< 0..4095
    int month;    ///< 1..12, 1 is January
    int day;      ///< 1..28,29,30,31 depending on month
    int dotw;     ///< 0..6, 0 is Sunday
    int hour;     ///< 0..23
    int min;      ///< 0..59
    int sec;      ///< 0..59
} datetime_t;
#else
#include <hardware/rtc.h>
#endif

#ifdef __cplusplus
extern "C"
{
#endif
  void convertEpochUTC(const time_t epoch, datetime_t *datetime);

  void convertEpochLocal(const time_t epoch, datetime_t *datetime);

  unsigned int dayofweek(unsigned y, unsigned int m, unsigned int d);
#ifdef __cplusplus
}
#endif

#endif
