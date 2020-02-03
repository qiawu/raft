#ifndef TIMER_H
#define TIMER_H 

#include <functional>

namespace raft {
  class Timer {
    public:
      Timer(): clear(true) {}
      void SetTimeout(std::function<void()> func, int delay);
      void SetInterval(std::function<void()> func, int interval);
      void Stop();
      void Reset();

    private:
      bool clear;

  };

}

#endif /* TIMER_H */
