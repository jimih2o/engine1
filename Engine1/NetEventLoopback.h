#pragma once

#include "Engine1.h"
#include "NetEvent.h"
#include "NetEventTypes.h"
#include "Timer.h"

#include <iostream>

namespace engine1 {
  class NetEventLoopback : public iNetEvent {
    Event& event;
    Event::Type id;
    bool isClient;
    uint32_t msgsSent = 0;
    Timer* pTimer;

  public:
    NetEventLoopback(Event& threadContextEvent, Event::Type id, bool isClient) : event(threadContextEvent), id(id), isClient(isClient) {
      pTimer = GetResource<TimerManager>("TimerManager")->Create(&event);
    }

    void Ping() {
      String message = String("Ping ") + ToString(msgsSent) + String("!\n");
      Send((uint8_t const*)message.c_str(), (uint16_t)message.length() + 1); // +1 to include null
      ++msgsSent;
    }

    // invoke when event is received
    void Process() {
      if (rawData.IsEmpty() && isClient)
        Ping();

      while (rawData.IsEmpty() == false) {
        auto telegram = rawData.Pop();

        std::cout << "Received \"";
        for (auto c : telegram) {
          std::cout << (char)c;
        }
        std::cout << "\"\n";

        // only loop back from server
        if (isClient == false) {
          Send(telegram.data(), (uint16_t)telegram.size());
        }
        else {
          // queue up next event
          // 1 second, invoke process, one-shot
          pTimer->Start(1000, id, false);
        }
      }

    }

    virtual NetEventType GetUniqueIdentifier() const override {
      return NetEventType::Loopback;
    }

    virtual Event::Type GetEventType() const override {
      return id;
    }

    virtual bool IsCritical() const override {
      return false; // allow errors to determine error rate
    }

    virtual Event& GetDispatchEvent() override {
      return event;
    }
  };
}
