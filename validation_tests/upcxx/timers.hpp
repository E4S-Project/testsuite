#ifndef __TIMERS_H
#define __TIMERS_H

#include <chrono>

inline std::chrono::time_point<std::chrono::high_resolution_clock> timer_start(void)
{
  return std::chrono::high_resolution_clock::now();
}


inline double timer_elapsed(std::chrono::time_point<std::chrono::high_resolution_clock> t)
{
  std::chrono::duration<double> t_elapsed = std::chrono::high_resolution_clock::now() - t;
  return t_elapsed.count();
}

#endif
