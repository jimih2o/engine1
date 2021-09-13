#include "Network.h"

#include <vector>
#include <algorithm>
#include <functional>

namespace engine1 {
  NetworkServer::NetworkServer(Config const& config) : config(config) {

  }

  void NetworkServer::Start(void) {
    initialized = socket.BindPort(config.rxPort, true); // we want a blocking recvfrom on server
    if (!initialized) {
      config.log << "Error, could not bind to port " << config.rxPort << "!\n";
      return;
    }

    mainThread = std::thread(std::bind(&NetworkServer::mainLoop, this));
    rxThread = std::thread(std::bind(&NetworkServer::rxLoop, this));
  }

  void NetworkServer::mainLoop(void) {
    while (initialized && !killThread) {
      events.WaitForEvent();

      while (events.IsEventWaiting()) {
        Event::Type event = events.GetNextEvent();

        switch (event) {
        case TX_EVENT:
          while (txQueue.IsEmpty() == false) {
            RoutablePacket rp = txQueue.Pop();

            if (!socket.Send(rp.addr, clientMap[rp.addr], rp.packet.GetRawPacket())) {
              config.log << "Error, could not send to " << ToString(rp.addr) << ":" << clientMap[rp.addr] << "!\n";
            }
          }
          break;

        case RX_EVENT:
          while (rxQueue.IsEmpty() == false) {
            // handle new rx data
            RoutablePacket p = rxQueue.Pop();
            Route(*reinterpret_cast<NetEventType const *>(p.packet.Payload()), p.packet.Payload() + sizeof(NetEventType), p.packet.PayloadSize() - sizeof(NetEventType));
          }
          break;

        default:
          config.log << "Error, unhandled event in NetworkServer::mainLoop " << ToString(event) << "!\n";
          break;
        }
      }
    }
  }

  void NetworkServer::rxLoop(void) {
    uint8_t* buffer = new uint8_t[Packet::MAX_SIZE];

    while (initialized && !killThread) {
      Socket::Address from;
      uint16_t        port;
      uint32_t        len;

      switch (socket.Receive(from, port, buffer, len, Packet::MAX_SIZE)) {
      case Socket::ReceivedState::Success:
        if (Packet::IsValid(buffer, len)) {
          clientMap[from] = port;
          rxQueue.Push({
            from,
            port,
            Packet(buffer, len)
            });
          events.Notify(RX_EVENT);
        }
        break;

      default:
        config.log << "Error, bad return value from Socket.Receive in NetworkServer::rxLoop\n";
        break;
      }
    }

    delete[]buffer;
  }


  void NetworkServer::AsyncProcessEventTransmission_External(iNetEvent* pEvent, uint8_t const* bytes, uint16_t len) {
    // broadcast to all registered clients
    for (auto addrPort : clientMap) {
      RoutablePacket rp;

      rp.addr = addrPort.first;
      rp.port = addrPort.second;

      NetEventType e = pEvent->GetUniqueIdentifier();
      rp.packet.Append(&e, sizeof(Event::Type));
      rp.packet.Append(bytes, len);

      txQueue.Push(rp);
    }
    // set TX_EVENT only after filling the queue (to avoid unnecessary packet traffic)
    events.Notify(TX_EVENT);
  }

  void NetworkServer::Stop(void) {
    killThread = true;
  }

  void NetworkServer::Join(void) {
    rxThread.join();
    mainThread.join();
  }

  bool NetworkServer::IsContextValid(void) {
    return initialized;
  }
}