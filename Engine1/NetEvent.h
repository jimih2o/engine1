#pragma once
#include "Engine1.h"

#include "Event.h"
#include "Queue.h"
#include "NetEventTypes.h"

#include <map>
#include <vector>
#include <list>

namespace engine1 {
  class iNetEventManager;

  class iNetEvent {
  protected:
    Queue < std::vector<uint8_t> > rawData;

  public:
    iNetEvent();
    virtual ~iNetEvent();

    virtual NetEventType GetUniqueIdentifier() const = 0;
    virtual Event::Type GetEventType() const = 0;
    virtual bool        IsCritical() const = 0;
    virtual Event&      GetDispatchEvent() = 0;

    void AsyncOnEventReceived_NetThread(uint8_t const* data, uint16_t len);
    void Send(uint8_t const* data, uint16_t len);
  };

  class iNetEventManager {
    std::map< NetEventType, std::list< iNetEvent* > > *pRegistry;

  public:
    iNetEventManager() {
      pRegistry = new std::map< NetEventType, std::list< iNetEvent* > >();
    }

    ~iNetEventManager() {
      delete pRegistry;
    }

    void Register(iNetEvent* pNetEvent) {
      auto listing = pRegistry->find(pNetEvent->GetUniqueIdentifier());
      if (listing != pRegistry->end()) {
        listing->second.push_back(pNetEvent);
      }
      else {
        auto obj = pRegistry->insert(std::make_pair(pNetEvent->GetUniqueIdentifier(), std::list<iNetEvent*>()));
        obj.first->second.push_back(pNetEvent);
      }
    }

    void Unregister(iNetEvent* pNetEvent) {
      auto obj = pRegistry->find(pNetEvent->GetUniqueIdentifier());
      if (obj != pRegistry->end()) {
        std::list<iNetEvent*>& listing = obj->second;

        auto iter = listing.begin();
        for (; iter != listing.end(); ++iter) {
          if (*iter == pNetEvent) {
            break;
          }
        }

        if (iter != listing.end())
          listing.erase(iter);
      }
    }

    void TransmitEvent(iNetEvent* pNetEvent, uint8_t const* data, uint32_t len) {
      // TODO critical infrastructure
      AsyncProcessEventTransmission_External(pNetEvent, data, len);
    }

  protected:
    void Route(NetEventType type, uint8_t const* bytes, uint16_t len) {
      auto listing = pRegistry->at(type);

      for (auto netObj : listing) {
        netObj->AsyncOnEventReceived_NetThread(bytes, len);
      }

    }

    virtual void AsyncProcessEventTransmission_External(iNetEvent* pEvent, uint8_t const* bytes, uint16_t len) = 0;

  };

}
