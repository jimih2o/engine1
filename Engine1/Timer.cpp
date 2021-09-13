#include "Timer.h"

#include <functional>

namespace engine1 {

  Timer::Timer() {

  }

  void Timer::Start(uint32_t timeout_mS, Event::Type onElapse, bool autoRestart) {
    std::unique_lock<std::mutex> lock(sync);
    this->timeout_mS = timeout_mS;
    notification = onElapse;
    restartOnElapse = autoRestart;
    running = true;

    pManager->restartTimer(this);
  }

  void Timer::Stop() {
    std::unique_lock<std::mutex> lock(sync);
    running = false;
  }

  std::chrono::milliseconds Timer::GetTimeout() const {
    return std::chrono::milliseconds(timeout_mS);
  }

  Timer* TimerManager::Create(Event* pEvent) {
    Timer* pTimer = new Timer();

    timers.push_back(pTimer);

    pTimer->timerIdentifier = static_cast<Event::Type>(timers.size() - 1);
    pTimer->pEvent = pEvent;
    pTimer->pManager = this;

    return pTimer;
  }

  void TimerManager::Start(void) {
    mainThread = std::thread(std::bind(&TimerManager::mainLoop, this));
  }

  void TimerManager::mainLoop(void) {
    while (!killThread) {
      events.WaitForEvent();

      while (events.IsEventWaiting()) {
        Event::Type timerId = events.GetNextEvent();
        Timer* pTimer = timers[timerId];

        std::unique_lock<std::mutex> lock(pTimer->sync);
        if (pTimer->restartOnElapse && pTimer->running) {
          restartTimer(pTimer);
        }

        if (pTimer->running)
          pTimer->pEvent->Notify(pTimer->notification);
      }

    }
  }

  void TimerManager::restartTimer(Timer* pTimer) {
    struct timerFnCb {
      Event& events;
      Event::Type id;
      std::chrono::milliseconds timeout;
    };

    timerFnCb cb = {
      events,
      pTimer->timerIdentifier,
      pTimer->GetTimeout()
    };

    auto timerFn = [cb]() {
      std::this_thread::sleep_for(cb.timeout);
      cb.events.Notify(cb.id);
    };

    // TODO replace this with an ACTIVE and INACTIVE timer list!
    std::thread spinOff = std::thread(timerFn);

    spinOff.detach();
  }

  void TimerManager::Stop(void) {
    killThread = true;
  }

  void TimerManager::Join(void) {
    mainThread.join();

    for (Timer* pTimer : timers)
      delete pTimer;
  }

  bool TimerManager::IsContextValid(void) {
    return !killThread;
  }
}

