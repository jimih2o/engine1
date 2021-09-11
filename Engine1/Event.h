#pragma once

#include "Engine1.h"
#include "Queue.h"

#include <mutex>
#include <condition_variable>

namespace engine1 {
  class Event {
  public:
    typedef uint8_t Type;

    static const Type ANY   = 0x00;
    static const Type FIRST = 0x01;

    void WaitForEvent() {
      if (events.IsEmpty() == false) return;

      std::unique_lock<std::mutex> lock(sync);
      cv.wait(lock);
    }

    void Notify(uint8_t eventId = ANY) {
      events.Push(eventId);
      std::unique_lock<std::mutex> lock(sync);
      cv.notify_all();
    }

    Type GetNextEvent(void) {
      return events.Pop();
    }

    bool IsEventWaiting() {
      return events.IsEmpty() == false;
    }

  private:
    std::mutex sync;
    std::condition_variable cv;

    Queue<Type> events;
  };
}