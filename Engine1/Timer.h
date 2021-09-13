#pragma once

#include "Engine1.h"
#include "Event.h"

#include <vector>
#include <chrono>
#include <mutex>

namespace engine1 {
  class TimerManager;

  class Timer : public iComponent {
  protected:
    std::mutex  sync;
    Event*      pEvent = nullptr;
    Event::Type notification;
    bool        restartOnElapse = false;
    uint32_t    timeout_mS;
    Event::Type timerIdentifier;
    TimerManager* pManager;
    bool        running = false;

    Timer();
  public:
    void Start(uint32_t timeout_mS, Event::Type onElapse, bool autoRestart = false);
    void Stop();

    std::chrono::milliseconds GetTimeout() const;

    friend class TimerManager;
  };


  class TimerManager : public iEngineObject {
    std::thread mainThread;
    bool        killThread;
    Event       events;

    std::vector<Timer*> timers;
    void mainLoop(void);
    void restartTimer(Timer* pTimer);

    friend Timer;

  public:
    Timer* Create(Event* pEvent);

    virtual void Start(void) override;
    virtual void Stop(void) override;
    virtual void Join(void) override;
    virtual bool IsContextValid(void) override;
  };
}
