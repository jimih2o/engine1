#include "Socket.h"

namespace engine1 {
  static uint32_t runtimeCounter = 0;

  Socket::Socket() {
    if (runtimeCounter == 0) {
#ifdef _WIN32
      WSADATA WsaData;
      (void)WSAStartup(MAKEWORD(2, 2), &WsaData);
#endif
    }
    handle = socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP);

    ++runtimeCounter;
  }

  bool Socket::BindPort(uint16_t port, bool waitForMessage) {
    sockaddr_in address;
    address.sin_family = AF_INET;
    address.sin_addr.s_addr = INADDR_ANY;
    address.sin_port = htons((unsigned short)port);

    if (bind(handle, (const sockaddr*)&address, sizeof(sockaddr_in)) < 0) {
      return false;
    }

    boundPort = port;

#ifdef _WIN32
    DWORD nonBlocking;

    if (waitForMessage)
      nonBlocking = 0;
    else
      nonBlocking = 1;

    if (ioctlsocket(handle, FIONBIO, &nonBlocking) != 0) {
      return false;
    }
#endif

    return true;
  }

  Socket::~Socket() {
    --runtimeCounter;

#ifdef _WIN32
    closesocket(handle);
#endif

    if (runtimeCounter == 0) {
      WSACleanup();
    }
  }

  bool Socket::Send(Address const& addr, uint16_t port, std::vector<uint8_t> const& bytes) {
    sockaddr_in address_in;
    address_in.sin_family = AF_INET;
    address_in.sin_addr.s_addr = htonl(addr);
    address_in.sin_port = htons(port);

    int len = sendto(handle, (const char*)bytes.data(), bytes.size(), 0, (sockaddr*)&address_in, sizeof(address_in));

    return len == bytes.size();
  }


  Socket::ReceivedState Socket::Receive(Address& from, uint16_t& port, uint8_t* bytes, uint32_t& bytesRead, uint32_t maxPacketLength_bytes) {
    sockaddr_in addr_in;
    int addr_in_size = sizeof(addr_in);

    int ret = recvfrom(handle, (char*)bytes, maxPacketLength_bytes, 0, (sockaddr*)&addr_in, &addr_in_size);

    if (ret < 0) {
#ifdef _WIN32
      int errorCode = WSAGetLastError();
      printf("SOCKET::RECEIVE ERROR CODE %d\n", errorCode);
#endif
      return ReceivedState::ErrorDetected;
    }
    if (ret == 0) return ReceivedState::QueueEmpty;

    bytesRead = ret;

    from = (uint32_t)ntohl(addr_in.sin_addr.s_addr);
    port = (uint16_t)ntohs(addr_in.sin_port);

    return ReceivedState::Success;
  }
}
