#pragma once

#include "Engine1.h"

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

    virtual void Start(void);
    virtual void Stop(void);
    virtual bool IsContextValid(void);

  private:
    Config config;
    bool   started;
    bool   renderTargetValid;

    void PauseRender(void);
    void Restart(void);
  };
}
