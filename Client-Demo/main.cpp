
#include "Engine1.h"
#include "Graphics.h"

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

  engine.Start();

  while (engine.IsContextValid()) {
    // Todo anything? If so move join out of here and add logic (+ stop if needed)
    engine.Join();
  }

  return 0;
}
