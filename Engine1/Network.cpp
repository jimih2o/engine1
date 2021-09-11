#include "Network.h"

#include <vector>

namespace engine1 {
  // GUID {F06A8644-4BCB-4862-BBCC-635F8BC9D29C}
  static const uint8_t packetKey[] =
  { 0xf0, 0x6a, 0x86, 0x44, 0x4b, 0xcb, 0x48, 0x62, 0xbb, 0xcc, 0x63, 0x5f, 0x8b, 0xc9, 0xd2, 0x9c };

  class Packet {
    const uint32_t keyOffset = 0;
    const uint32_t lengthOffset = keyOffset + sizeof(packetKey);
    const uint32_t sNoOffset = lengthOffset + sizeof(uint16_t);
    const uint32_t crcOffset = sNoOffset + sizeof(uint64_t);
    const uint32_t payloadOffset = crcOffset + sizeof(uint32_t);

    std::vector<uint8_t> bytes;

    void ComputeHeader() {

    }

  public:

    Packet() {
      bytes.reserve(payloadOffset);
      
      for (uint8_t byte : packetKey)
        bytes.push_back(byte);

      // fill Sno, length with 0's
      while (bytes.size() < payloadOffset)
        bytes.push_back(0);
    }

    void AssignSequenceNumber(uint64_t sno) {
      for (uint32_t i = sNoOffset; i < sizeof(uint64_t); ++i, sno >>= 8)
        bytes[i] = (sno & 0xFF);
    }

    std::vector<uint8_t> const& GetRawPacket(void) const { return bytes; }

    template< typename T >
    void Append(T data) {
      bytes.reserve(bytes.size() + sizeof(data));
      
      for (uint32_t i = 0; i < sizeof(data); ++i) {
        bytes.push_back(((uint8_t*)&data)[i]);
      }

      ComputeHeader();
    }
  };
}
