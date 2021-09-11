#pragma once

#include "Engine1.h"

#include <vector>

#ifdef _WIN32
#include "winsock2.h"
#pragma comment( lib, "wsock32.lib" )
#endif

namespace engine1 {
  // low level accessor for portability reasons
  class Socket {
    int handle = -1;
    uint16_t boundPort = 0;

  public:
    struct Address {
      uint8_t a = 127, b = 0, c = 0, d = 1;

      Address(uint8_t a, uint8_t b, uint8_t c, uint8_t d) : a(a), b(b), c(c), d(d) {}
      Address(uint32_t field) : a((field >> 24) & 0xFF), b((field >> 16) & 0xFF), c((field >> 8) & 0xFF), d(field & 0xFF) {}

      operator uint32_t() const {
        return (a << 24) | (b << 16) | (c << 8) | d;
      }
    };

    enum class ReceivedState {
      Success,
      QueueEmpty,
      ErrorDetected
    };

    Socket();
    ~Socket();

    bool BindPort(uint16_t port);
    bool Send(Address const& addr, uint16_t port, std::vector<uint8_t> const& bytes);
    ReceivedState Receive(Address& from, uint16_t& port, uint8_t* bytes, uint32_t& bytesRead, uint32_t maxPacketLength_bytes);
  };
}