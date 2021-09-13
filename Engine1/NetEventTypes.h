#pragma once

#include "Engine1.h"

// default net events used by engine1. Nothing prohibits custom event types past those listed here.
namespace engine1 {
  enum NetEventType : uint32_t {
    None = 0,
    Loopback, //NetEventLoopback.h

    TotalEngine1Events
  };
}
