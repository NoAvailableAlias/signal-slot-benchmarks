#pragma once

#include <Nuclex/Events/Event.h>

#include <algorithm>
#include <functional>

struct signal_traits_nes
{
  static constexpr bool has_signal_empty_test = true;
  static constexpr bool has_connection_connected_test = false;
  static constexpr bool has_disconnect_all = true;
  static constexpr bool has_swap = true;
  static constexpr bool will_deadlock_if_recursively_modified = false;
  
  template<typename Signature>
  using signal = Nuclex::Support::Events::Event<Signature>;

  using connection = std::function<void()>;

  static void initialize() {}
  static void terminate() {}
  
  template<typename Signal>
  static bool empty(Signal& s)
  {
    return s.CountSubscribers() == 0;
  }
  
  template<typename F, typename... Args>
  static connection connect(signal<void(Args...)>& s, F&& f)
  {
    struct caller_type
    {
      explicit caller_type(F&& f)
        : m_f(std::forward<F>(f))
      {

      }

      void call(Args... args)
      {
        m_f(std::forward<Args>(args)...);
      }
      
      F m_f;
    };

    std::shared_ptr<caller_type> caller
      (std::make_shared<caller_type>(std::forward<F>(f)));
    
    s.template Subscribe<caller_type, &caller_type::call>(caller.get());
    
    return
      [caller, &s]() -> void
      {
        s.template Unsubscribe<caller_type, &caller_type::call>(caller.get());
      };
  }

  template<typename Signal, typename... Args>
  static void trigger(Signal& s, Args&&... args)
  {
    s(std::forward<Args>(args)...);
  }

  template<typename Signal>
  static void disconnect(Signal& s, connection& c)
  {
    c();
  }
  
  template<typename Signal>
  static void disconnect_all_slots(Signal& s)
  {
    s = Signal();
  }
  
  template<typename Signal>
  static void swap(Signal& s1, Signal& s2)
  {
    std::swap(s1, s2);
  }
};

