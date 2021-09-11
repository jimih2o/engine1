#pragma once

#include <string>
#include <map>
#include <cstdint>

namespace engine1 {

  typedef std::string String;
  template < typename T >
  String ToString(T const&in) {
    return std::to_string(in);
  }

  class iComponent {
    std::map<String, iComponent*> components;

  public:
    virtual ~iComponent() {
      for (auto iter : components)
        delete iter.second;
    }

    template < typename Functor >
    void ForEachComponent(Functor fn)
    {
      for (auto iter : components)
        fn(iter.first, iter.second);
    }

    void AddComponent(String const& name, iComponent* pComponent) {
      components[name] = pComponent;
    }

    iComponent* GetComponent(String const& componentName) {
      auto element = components.find(componentName);
      if (element != components.end())
        return element->second;
      return nullptr;
    }

    template <typename ComponentType>
    ComponentType* GetComponent(String const& componentName) {
      return dynamic_cast<ComponentType*>(GetComponent(componentName));
    }
  };

  // class for major engine feature components: graphics, networking, events, etc.
  class iEngineObject : public iComponent {
  public:
    virtual void Start(void) {}
    virtual void Stop(void) {}
    virtual void Join(void) {}
    virtual bool IsContextValid(void) = 0;
  };

  // first major top level collection for game engine context
  class EngineManager : public iEngineObject {
    std::map<String, iComponent*> namedObjects;

  public:
    template<typename ComponentType, typename... ConstructorArgs>
    ComponentType* CreateObject(String const& name, ConstructorArgs... args) {
      ComponentType* object = new ComponentType(args...);
      namedObjects[name] = object;
      return object;
    }

    template <typename ComponentType>
    ComponentType* GetObject(String const& name) {
      return namedObjects[name];
    }

    template<typename EngineObject, typename... ConstructorArgs>
    EngineObject* CreateEngineResource(String const& name, ConstructorArgs... args) {
      AddComponent(name, new EngineObject(args...));
      return GetComponent<EngineObject>(name);
    }

    template<typename EngineObject>
    EngineObject* GetEngineResource(String const& name) {
      return GetComponent<EngineObject>(name);
    }

    virtual void Start(void) {
      ForEachComponent([](String const& name, iComponent* component) {
        dynamic_cast<iEngineObject*>(component)->Start();
      });
    }

    virtual void Stop(void) {
      ForEachComponent([](String const& name, iComponent* component) {
        dynamic_cast<iEngineObject*>(component)->Stop();
      });
    }

    virtual void Join(void) {
      ForEachComponent([](String const& name, iComponent* component) {
        dynamic_cast<iEngineObject*>(component)->Join();
      });
    }

    virtual bool IsContextValid(void) {
      bool valid = true;

      ForEachComponent([&valid](String const& name, iComponent* component) {
        valid = (valid && dynamic_cast<iEngineObject*>(component)->IsContextValid());
      });

      return valid;
    }
  };
}

