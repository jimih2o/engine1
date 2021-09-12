#pragma once

#include "Engine1.h"

#include <iostream>
#include <sstream>
#include <iomanip>
#include <vector>

namespace engine1 {

  uint32_t crc32(std::vector<uint8_t> const& bytes);

  // GUID {F06A8644-4BCB-4862-BBCC-635F8BC9D29C}
  static const uint8_t packetKey[] =
  { 0xf0, 0x6a, 0x86, 0x44, 0x4b, 0xcb, 0x48, 0x62, 0xbb, 0xcc, 0x63, 0x5f, 0x8b, 0xc9, 0xd2, 0x9c };

  static const uint16_t MAX_PAYLOAD_SIZE = 0xFF00;

  class Packet {
    static const uint32_t keyOffset = 0;
    static const uint32_t lengthOffset = keyOffset + sizeof(packetKey);
    static const uint32_t crcOffset = lengthOffset + sizeof(uint16_t);
    static const uint32_t payloadOffset = crcOffset + sizeof(uint32_t);

    std::vector<uint8_t> bytes;

    void ComputeHeader() {
      // determine length
      uint16_t length = static_cast<uint16_t>(bytes.size()) - payloadOffset;
      bytes[lengthOffset] = length & 0xFF;
      bytes[lengthOffset + 1] = length & 0xFF00;

      // determine crc
      // first write 0xFF to CRC location for determinism
      for (int i = 0; i < sizeof(uint32_t); ++i) {
        bytes[crcOffset + i] = 0xFF;
      }

      // compute crc
      uint32_t crc = crc32(bytes);

      // emplace crc
      for (int i = 0; i < sizeof(uint32_t); ++i) {
        bytes[crcOffset + i] = (crc >> (8 * i)) & 0xFF;
      }

    }

  public:
    static const uint16_t MAX_SIZE = payloadOffset + MAX_PAYLOAD_SIZE;

    Packet(uint8_t const* raw, uint16_t len) {
      // blindly form a packet given data contents
      bytes.resize(len);

      for (uint16_t i = 0; i < len; ++i)
        bytes[i] = raw[i];
    }

    Packet(uint16_t size = 0) {
      bytes.reserve(payloadOffset);

      for (uint8_t byte : packetKey)
        bytes.push_back(byte);

      // fill with 0's
      while (bytes.size() < payloadOffset)
        bytes.push_back(0x00);

      if (size > MAX_PAYLOAD_SIZE)
        size = MAX_PAYLOAD_SIZE;

      for (uint16_t i = 0; i < size; ++i)
        bytes.push_back(0x00);

      ComputeHeader();
    }

    static bool IsValid(uint8_t const* bytes, uint16_t len) {
      bool valid = true;

      // packet is not large enough
      if (len < payloadOffset) return false;

      for (uint32_t i = 0; valid && i < sizeof(packetKey); ++i) {
        if (bytes[i] == packetKey[i])
          continue;

        valid = false;
      }

      // validate transmited length
      uint16_t length = (bytes[lengthOffset]) | (bytes[lengthOffset + 1] << 8);
      if (length != len - payloadOffset)
        valid = false;

      // validate CRC
      if (valid) {
        uint32_t crc =
          bytes[crcOffset] | (bytes[crcOffset + 1] << 8) | (bytes[crcOffset + 2] << 16) | (bytes[crcOffset + 3] << 24);

        // compute local CRC equivalent: copy array but insert FF's for CRC computation
        std::vector<uint8_t> local(len);
        for (uint16_t i = 0; i < len; ++i)
          local[i] = bytes[i];

        for (int i = 0; i < sizeof(uint32_t); ++i) {
          local[crcOffset + i] = 0xFF;
        }

        if (crc != crc32(local))
          valid = false;
      }

      return valid;
    }

    static bool IsValid(std::vector<uint8_t> const& byteStream) {
      bool valid = true;

      // packet is not large enough
      if (byteStream.size() < payloadOffset) return false;

      for (uint32_t i = 0; valid && i < sizeof(packetKey); ++i) {
        if (byteStream[i] == packetKey[i])
          continue;

        valid = false;
      }

      // validate transmited length
      uint16_t length = (byteStream[lengthOffset]) | (byteStream[lengthOffset + 1] << 8);
      if (length != byteStream.size() - payloadOffset)
        valid = false;

      // validate CRC
      if (valid) {
        uint32_t crc =
          byteStream[crcOffset] | (byteStream[crcOffset + 1] << 8) | (byteStream[crcOffset + 2] << 16) | (byteStream[crcOffset + 3] << 24);

        // compute local CRC equivalent: copy array but insert FF's for CRC computation
        std::vector<uint8_t> local(byteStream);
        for (int i = 0; i < sizeof(uint32_t); ++i) {
          local[crcOffset + i] = 0xFF;
        }

        if (crc != crc32(local))
          valid = false;
      }

      return valid;
    }

    std::vector<uint8_t> const& GetRawPacket(void) const { return bytes; }

    void WritePayload(uint32_t index, uint8_t const* data, uint16_t len) {
      if (len > MAX_PAYLOAD_SIZE)
        len = MAX_PAYLOAD_SIZE;

      if (bytes.size() - payloadOffset < len) {
        bytes.resize(payloadOffset + len);
      }

      for (uint32_t i = 0; i < len; ++i) {
        bytes[i + payloadOffset] = data[i];
      }

      ComputeHeader();
    }

    void Append(std::vector<uint8_t> const& data) {
      // TODO validate paylaod size
      bytes.reserve(bytes.size() + data.size());

      for (uint8_t byte : data)
        bytes.push_back(byte);

      ComputeHeader();
    }

    void Append(uint8_t const* data, uint16_t length) {
      // todo validate payload size
      bytes.reserve(bytes.size() + length);

      for (uint32_t i = 0; i < length; ++i)
        bytes.push_back(data[i]);

      ComputeHeader();
    }
  };

  template<>
  inline String ToString(Packet const& packet) {
    std::stringstream ss;

    ss.width(2);

    for (uint8_t b : packet.GetRawPacket()) {
      ss << "0x" << std::left << std::hex << static_cast<uint32_t>(b) << ",";
    }

    return ss.str();
  }
}