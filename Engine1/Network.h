#pragma once

#include "Engine1.h"
#include "Socket.h"
#include "Queue.h"
#include "Event.h"
#include "Packet.h"
#include "NetEvent.h"

#include <iostream>
#include <thread>
#include <sstream>
#include <iomanip>
#include <map>
#include <list>

namespace engine1 {
  struct RoutablePacket {
    Socket::Address addr;
    uint16_t        port = 0;
    Packet          packet;
  };

  class NetworkClient : public iEngineObject, public iNetEventManager {
  public:
    struct Config {
      const uint16_t  txPort;
      const uint16_t  rxPort;
      Socket::Address hostAddress;
      std::ostream &  log;
    };

    NetworkClient(Config const& config);

    virtual void Start(void);
    virtual void Stop(void);
    virtual void Join(void);
    virtual bool IsContextValid(void);

  protected:
    virtual void AsyncProcessEventTransmission_External(iNetEvent* pEvent, uint8_t const* bytes, uint16_t len) override;

  private:
    Config      config;
    Socket      socket;
    bool        valid = false;
    bool        killThread = false;
    std::thread netThread;
    std::thread rxThread;
    const enum {
      TX_EVENT = Event::FIRST,
      RX_EVENT
    };
    Event events;
    Queue<Packet> txQueue;
    Queue<RoutablePacket> rxQueue;

    void commLoop(void);
    void rxLoop(void);
  };

  class NetworkServer : public iEngineObject {
  public:
    struct Config {
      const uint16_t rxPort;
      std::ostream& log;
    };

    NetworkServer(Config const& config);

    virtual void Start(void);
    virtual void Stop(void);
    virtual void Join(void);
    virtual bool IsContextValid(void);

  private:
    Config      config;
    Socket      socket;
    bool        initialized = false;
    bool        killThread = false;
    std::mutex  socketLock;
    std::thread mainThread;
    std::thread rxThread;
    enum {
      TX_EVENT = Event::FIRST,
      RX_EVENT,
    };
    Event events;
    std::map<Socket::Address, uint16_t> clientMap;

    Queue<RoutablePacket> rxQueue;
    Queue<RoutablePacket> txQueue;

    void rxLoop(void);
    void mainLoop(void);
  };
}
