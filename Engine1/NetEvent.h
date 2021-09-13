#pragma once
#include "Engine1.h"

#include "Event.h"
#include "Queue.h"

#include <map>
#include <vector>

namespace engine1 {

  class iNetEvent {
  protected:
    Queue < std::vector<uint8_t> > rawData;
  public:
    virtual Event::Type GetEventIdentifier() const = 0;
    virtual uint16_t    GetPayloadLength() const = 0;
    virtual bool        IsCritical() const = 0;

    void AsyncOnEventReceived_NetThread(uint8_t const* data, uint16_t len) {

    }
  };

  class iNetEventManager {
  public:

    void RegisterNewEventType(iNetEvent* pEvent) {
      if (registry.find(pEvent->GetEventIdentifier()) != registry.end()) {
        throw String("Double Registration for net event #") + ToString(pEvent->GetEventIdentifier());
      }
      registry[pEvent->GetEventIdentifier()] = pEvent;
    }

  private:
    std::map<Event::Type, iNetEvent*> registry;
  };

}
#pragma once
#include "Engine1.h"

#include "Event.h"
#include "Queue.h"

#include <map>
#include <vector>

namespace engine1 {
  class iNetEventManager;

  class iNetEvent {
    iNetEventManager* eventManager;

  protected:
    Queue < std::vector<uint8_t> > rawData;

  public:
    virtual Event::Type GetEventIdentifier() const = 0;
    virtual uint16_t    GetPayloadLength() const = 0;
    virtual bool        IsCritical() const = 0;
    virtual Event&      GetDispatchEvent() = 0;

    void AsyncOnEventReceived_NetThread(uint8_t const* data, uint16_t len) {
      std::vector<uint8_t> nbuf(len);
      for (uint32_t i = 0; i < len; ++i)
        nbuf.push_back(data[i]);

      rawData.Push(nbuf);
      GetDispatchEvent().Notify(GetEventIdentifier());
    }

    void Send(uint8_t const* data, uint16_t len) {
      eventManager->TransmitEvent(GetEventIdentifier(), data, len);
    }

    void SetManager(iNetEventManager* manager) {
      eventManager = manager;
    }
  };

  class iNetEventManager {
  public:
    void RegisterNewEventType(iNetEvent* pEvent) {
      if (registry.find(pEvent->GetEventIdentifier()) != registry.end()) {
        throw String("Double Registration for net event #") + ToString(pEvent->GetEventIdentifier());
      }

      pEvent->SetManager(this);
      registry[pEvent->GetEventIdentifier()] = pEvent;
    }

    void TransmitEvent(Event::Type identifier, uint8_t const* data, uint32_t len) {
      // TODO critical infrastructure
      AsyncProcessEventTransmission_External(registry[identifier], data, len);
    }

  protected:
    virtual void AsyncProcessEventTransmission_External(iNetEvent* pEvent, uint8_t const* bytes, uint16_t len) = 0;

  private:
    std::map<Event::Type, iNetEvent*> registry;
  };

}
