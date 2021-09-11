#pragma once

#include "Engine1.h"

#include <iostream>

namespace engine1 {
  class NetworkClient : public iEngineObject {
  public:
    struct Config {
      const int     port;
      String        hostname;
      std::ostream &log;
    };

    NetworkClient(Config const& config);

    virtual void Start(void);
    virtual void Stop(void);
    virtual void Join(void);
    virtual bool IsContextValid(void);
  };

  class NetworkServer : public iEngineObject {
  public:
    struct Config {
      const int     port;
      std::ostream& log;
    };

    NetworkServer(Config const& config);

    virtual void Start(void);
    virtual void Stop(void);
    virtual void Join(void);
    virtual bool IsContextValid(void);
  };
}
