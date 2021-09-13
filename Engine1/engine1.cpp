
#include "Engine1.h"

namespace engine1 {
  static EngineManager* pEngineInstance;

  void SetEngineInstance(EngineManager* pEngine) {
    pEngineInstance = pEngine;
  }

  template<>
  iEngineObject* GetResource(String const& name) {
    if (pEngineInstance != nullptr)
      return pEngineInstance->GetEngineResource<iEngineObject>(name);

    return nullptr;
  }

  template<>
  iComponent* GetObject(String const& name) {
    if (pEngineInstance != nullptr)
      return pEngineInstance->GetObject<iComponent>(name);
    return nullptr;
  }
}