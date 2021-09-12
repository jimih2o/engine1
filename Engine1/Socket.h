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

      Address(uint8_t a = 127, uint8_t b = 0, uint8_t c = 0, uint8_t d = 1) : a(a), b(b), c(c), d(d) {}
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

    bool BindPort(uint16_t port, bool waitForMessage);
    bool Send(Address const& addr, uint16_t port, std::vector<uint8_t> const& bytes);
    ReceivedState Receive(Address& from, uint16_t& port, uint8_t* bytes, uint32_t& bytesRead, uint32_t maxPacketLength_bytes);
  };

  template<> inline String ToString(Socket::Address const& ref) {
    return ToString(static_cast<uint32_t>(ref.a)) + "." +
      ToString(static_cast<uint32_t>(ref.b)) + "." +
      ToString(static_cast<uint32_t>(ref.c)) + "." +
      ToString(static_cast<uint32_t>(ref.d));
  }
}