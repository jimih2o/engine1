#include "NetEvent.h"

namespace engine1 {

  void iNetEvent::AsyncOnEventReceived_NetThread(uint8_t const* data, uint16_t len) {
    std::vector<uint8_t> nbuf(len);
    for (uint32_t i = 0; i < len; ++i)
      nbuf.push_back(data[i]);

    rawData.Push(nbuf);
    GetDispatchEvent().Notify(GetEventIdentifier());
  }

  void iNetEvent::Send(uint8_t const* data, uint16_t len) {
    eventManager->TransmitEvent(GetEventIdentifier(), data, len);
  }

  void iNetEvent::SetManager(iNetEventManager* manager) {
    eventManager = manager;
  }
}
