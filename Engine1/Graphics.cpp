#include "Graphics.h"

#include <thread>
#include <functional>

namespace engine1 {
  Graphics::Graphics(Config const& config) : config(config) {
  }

  Graphics::~Graphics() {
    if (IsContextValid())
      Stop();
  }

  void Graphics::UpdateConfig(Config const& newConfig) {
    PauseRender();
    config = newConfig;
    Restart();
  }

  bool Graphics::IsContextValid(void) {
    return started && renderTargetValid;
  }


  void Graphics::Start(void) {
    renderTargetValid = true;
    started = true;
    killThread = false;

    drawThread = std::thread(std::bind(&Graphics::drawThread, this));
  }


  void Graphics::drawLoop(void) {
    while (started && renderTargetValid && !killThread) {

    }
  }

  void Graphics::Join(void) {
    drawThread.join();
  }

  void Graphics::Stop(void) {
    killThread = true;
  }

  void Graphics::PauseRender(void) {

  }

  void Graphics::Restart(void) {
    // adjust window as needed

    // begin pipeline rasterization again
  }
}
