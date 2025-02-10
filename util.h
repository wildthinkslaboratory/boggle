#ifndef UTIL
#define UTIL

#include <iomanip>
#include <iostream>

using namespace std;

// convert letters to array indexes and back
#define IDX(a) ((a) - 'a')
#define CHR(i) char((i) + 'a')

inline size_t random_int(size_t range) { return rand() % range; }

inline double get_cpu_time() {
  struct rusage ru;
  getrusage(RUSAGE_SELF, &ru);
  return (ru.ru_utime.tv_sec + ru.ru_utime.tv_usec / 1000000.0 +
          ru.ru_stime.tv_sec + ru.ru_stime.tv_usec / 1000000.0);
}

#endif