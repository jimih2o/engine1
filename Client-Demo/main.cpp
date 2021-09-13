
#include "Engine1.h"
#include "Graphics.h"
#include "Network.h"
#include "Timer.h"

#include "NetEventLoopback.h"

#include <iostream>
#include <thread>

int main(int argc, char** argv) {

  using namespace std;

  cout << "Client-Demo:";
  for (int i = 1; i < argc; ++i) {
    cout << " " << argv[i];
  }
  cout << endl;

  engine1::EngineManager engine;

  auto tmr = engine.CreateEngineResource<engine1::TimerManager>("TimerManager");
  auto gfx = engine.CreateEngineResource<engine1::Graphics>("Graphics");
  auto net = engine.CreateEngineResource<engine1::NetworkClient>("Network", engine1::NetworkClient::Config {
      25652,
      25653,
      {127, 0, 0, 1},
      cout
    });

  engine.Start();
  
  engine1::Event event;
  enum {
    LOOPBACK_EVT = 1
  };

  engine1::NetEventLoopback loopback(event, LOOPBACK_EVT, true);

  if (engine.IsContextValid()) {

    // initiate loopback
    loopback.Ping();

    while (1) {
      event.WaitForEvent();

      switch (event.GetNextEvent()) {
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
