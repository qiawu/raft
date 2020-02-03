
#include "timer.h"

#include <thread>

void raft::Timer::SetTimeout(std::function<void()> func, int delay) {
  this->clear = false;
  std::thread t([=]() {
        if(this->clear) return;
        std::this_thread::sleep_for(std::chrono::milliseconds(delay));
        if(this->clear) return;
        func();
        this->clear = true;
      });
  t.detach();
}

void raft::Timer::SetInterval(std::function<void()> func, int interval) {
    this->clear = false;
    std::thread t([=]() {
        while(true) {
            if(this->clear) return;
            std::this_thread::sleep_for(std::chrono::milliseconds(interval));
            if(this->clear) return;
            func();
        }
    });
    t.detach();
}

void raft::Timer::Stop() {
  this->clear = true;
}

void raft::Timer::Reset() {
  this->clear = true;
}
