#include "NetEvent.h"

namespace engine1 {
  iNetEvent::iNetEvent() {
    GetResource<iNetEventManager>("Network")->Register(this);
  }
  iNetEvent::~iNetEvent() {
    GetResource<iNetEventManager>("Network")->Unregister(this);
  }

  void iNetEvent::AsyncOnEventReceived_NetThread(uint8_t const* data, uint16_t len) {
    std::vector<uint8_t> nbuf(len);
    for (uint32_t i = 0; i < len; ++i)
      nbuf.push_back(data[i]);

    rawData.Push(nbuf);
    GetDispatchEvent().Notify(GetUniqueIdentifier());
  }

  void iNetEvent::Send(uint8_t const* data, uint16_t len) {
    GetResource<iNetEventManager>("Network")->TransmitEvent(this, data, len);
  }
}
