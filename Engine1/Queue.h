#pragma once

#include "Engine1.h"

#include <queue>
#include <mutex>

namespace engine1 {
  template < typename T >
  class Queue : std::queue<T> {
    std::mutex lock;
  public:

    void Push(const std::queue<T>::value_type& _Val) {
      std::lock_guard<std::mutex> guard(lock);
      std::queue<T>::push(_Val);
    }

    void Push(std::queue<T>::value_type&& _Val) {
      std::lock_guard<std::mutex> guard(lock);
      std::queue<T>::push(_Val);
    }

    T Pop(void) {
      std::lock_guard<std::mutex> guard(lock);
      T element = std::queue<T>::front();
      std::queue<T>::pop();
      return element;
    }

    bool IsEmpty(void) {
      std::lock_guard<std::mutex> guard(lock);
      return std::queue<T>::empty();
    }
  };
}