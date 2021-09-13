
#include "Engine1.h"
#include "Network.h"
#include "Timer.h"
#include "NetEventLoopback.h"

#include <iostream>

int main(int argc, char** argv) {
  using namespace std;

  engine1::EngineManager engine;

  auto tmr = engine.CreateEngineResource<engine1::TimerManager>("TimerManager");
  auto net = engine.CreateEngineResource<engine1::NetworkServer>("Network", engine1::NetworkServer::Config{
      25652,
      cout
    });

  engine.Start();

  engine1::Event events;
  enum {
    LOOPBACK_EVT = 1,
  };

  engine1::NetEventLoopback loopback(events, LOOPBACK_EVT, false);

  if (engine.IsContextValid()) {

    while (1) {
      events.WaitForEvent();

      switch (events.GetNextEvent()) {
      case LOOPBACK_EVT:
        loopback.Process();
        break;

      default:
        break;
      }
    }

    engine.Join();
  }
  return 0;
}
