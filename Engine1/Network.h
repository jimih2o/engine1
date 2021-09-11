#pragma once

#include "Engine1.h"
#include "Socket.h"
#include "Queue.h"
#include "Event.h"

#include <iostream>
#include <thread>

namespace engine1 {
  class NetworkClient : public iEngineObject {
  public:
    struct Config {
      const int       port;
      Socket::Address hostAddress;
      std::ostream &  log;
    };

    NetworkClient(Config const& config);

    virtual void Start(void);
    virtual void Stop(void);
    virtual void Join(void);
    virtual bool IsContextValid(void);

  private:
    Config config;
    Socket socket;
    bool   valid = false;
    bool   killThread = false;
    std::thread netThread;
    const enum {
      TX_EVENT = Event::FIRST,
      RX_EVENT
    };
    Event events;

    void commLoop(void);
  };

  class NetworkServer : public iEngineObject {
  public:
    struct Config {
      const int     port;
      std::ostream& log;
    };

    NetworkServer(Config const& config);

    virtual void Start(void);
    virtual void Stop(void);
    virtual void Join(void);
    virtual bool IsContextValid(void);

  private:
    Config config;
    Socket socket;
  };
}
