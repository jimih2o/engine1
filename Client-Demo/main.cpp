
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
      25653,
      {127, 0, 0, 1},
      cout
    });

  engine.Start();
  
  if (engine.IsContextValid()) {
    uint8_t DUMMY_PAYLOAD[] = "Test packet infrastructure....\n";

    while (1) {
      //engine.GetEngineResource<engine1::NetworkClient>("Network")->SendRawDataToHost(DUMMY_PAYLOAD, sizeof(DUMMY_PAYLOAD));
      Sleep(5000);
    }

    engine.Join();
  }

  return 0;
}
