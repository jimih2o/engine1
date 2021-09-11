#include "Graphics.h"

#include <thread>

namespace engine1 {
  Graphics::Graphics(Config const& config) : config(config) {
    started = false;
    renderTargetValid = false;
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
  }

  void Graphics::Stop(void) {

  }

  void Graphics::PauseRender(void) {

  }

  void Graphics::Restart(void) {
    // adjust window as needed

    // begin pipeline rasterization again
  }
}
