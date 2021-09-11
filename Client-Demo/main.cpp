
#include "Engine1.h"
#include "Graphics.h"
#include "Network.h"

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

  auto gfx = engine.CreateEngineResource<engine1::Graphics>("Graphics");
  auto net = engine.CreateEngineResource<engine1::NetworkClient>("Network", engine1::NetworkClient::Config {
      25652,
      {127, 0, 0, 1},
      cout
    });

  engine.Start();
  
  if (engine.IsContextValid())
    engine.Join();

  return 0;
}
