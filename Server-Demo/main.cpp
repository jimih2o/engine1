
#include "Engine1.h"
#include "Network.h"

#include <iostream>

int main(int argc, char** argv) {
  using namespace std;

  engine1::EngineManager engine;

  auto net = engine.CreateEngineResource<engine1::NetworkServer>("Network", engine1::NetworkServer::Config{
      25652,
      cout
    });

  engine.Start();

  if (engine.IsContextValid()) {
    engine.Join();
  }
  return 0;
}
