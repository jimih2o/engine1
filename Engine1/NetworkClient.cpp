#include "Network.h"

#include <vector>
#include <algorithm>
#include <functional>

namespace engine1 {

  NetworkClient::NetworkClient(Config const& config) : config(config) {
  }

  void NetworkClient::Start(void) {
    config.log << "Network Client Starting...\n";

    if (socket.BindPort(config.rxPort, true) == false) {
      config.log << "Could not bind to port " << config.rxPort << "\n";
      return;
    }

    valid = true;

    netThread = std::thread(std::bind(&NetworkClient::commLoop, this));
    rxThread = std::thread(std::bind(&NetworkClient::rxLoop, this));
  }

  void NetworkClient::commLoop(void) {
    while (valid && !killThread) {
      events.WaitForEvent();

      while (events.IsEventWaiting()) {
        Event::Type event = events.GetNextEvent();

        switch (event) {
        case TX_EVENT:
          while (txQueue.IsEmpty() == false) {
            Packet packet = txQueue.Pop();
            if (socket.Send(config.hostAddress, config.txPort, packet.GetRawPacket()) == false) {
              config.log << "Error, network client could not transmit to " << ToString(config.hostAddress) + ":" + ToString(config.txPort) << "!\n";
            }
          }
          break;

        case RX_EVENT:
          while (rxQueue.IsEmpty() == false) {
            RoutablePacket rp = rxQueue.Pop();
            config.log << ToString(rp.packet) << "\n\n";

            Route(*reinterpret_cast<NetEventType const*>(rp.packet.Payload()), rp.packet.Payload() + sizeof(NetEventType), rp.packet.PayloadSize() - sizeof(NetEventType));
          }
          break;

        default:
          config.log << "Error, unhandled network event type " << static_cast<uint32_t>(event) << "!\n";
          break;
        }
      }
    }
  }
  void NetworkClient::rxLoop(void) {
    RoutablePacket rp = {
      config.hostAddress,
      config.rxPort
    };
    uint8_t* buffer = new uint8_t[Packet::MAX_SIZE];
    uint32_t       len;
    while (valid && !killThread) {
      switch (socket.Receive(rp.addr, rp.port, buffer, len, Packet::MAX_SIZE)) {
      case Socket::ReceivedState::Success:
        if (Packet::IsValid(buffer, len)) {
          rp.packet = Packet(buffer, len);
          rxQueue.Push(rp);
          events.Notify(RX_EVENT);
        }
        break;

      default:
        config.log << "Error, socket receive error!\n";
        break;
      }
    }

    delete[]buffer;
  }

  void NetworkClient::AsyncProcessEventTransmission_External(iNetEvent* pEvent, uint8_t const* bytes, uint16_t len) {
    // we are the client, so we only send data to the server node
    Packet p;
    NetEventType e = pEvent->GetUniqueIdentifier();
    p.Append(&e, sizeof(Event::Type));
    p.Append(bytes, len);

    txQueue.Push(p);
    events.Notify(TX_EVENT);
  }

  void NetworkClient::Stop(void) {
    killThread = true;
  }

  void NetworkClient::Join(void) {
    netThread.join();
    rxThread.join();
  }

  bool NetworkClient::IsContextValid(void) {
    return valid;
  }
}
