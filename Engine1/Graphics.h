#pragma once

#include "Engine1.h"

#include <thread>

namespace engine1 {
  class Graphics : public iEngineObject {
  public:
    struct Config {
      bool fullscreen = false;
      uint32_t width_px = 800;
      uint32_t height_px = 600;
    };

    Graphics(Config const& config = Config());
    ~Graphics();

    void UpdateConfig(Config const& newConfig);

    virtual void Start(void) override;
    virtual void Stop(void) override;
    virtual bool IsContextValid(void) override;
    virtual void Join(void) override;

  private:
    Config      config;
    bool        started = false;
    bool        renderTargetValid = false;
    bool        killThread = false;
    std::thread drawThread;

    void PauseRender(void);
    void Restart(void);

    void drawLoop(void);
  };
}
