#pragma once

#include "jeffomatic/jl_signal/src/Signal.h"

#include <algorithm>

struct signal_traits_jls
{
  static constexpr bool has_signal_empty_test = true;
  static constexpr bool has_connection_connected_test = true;
  
  template<typename Signature>
  using signal = jl::Signal<Signature>;

  using connection = jl::SignalObserver*;
  
  template<typename Signal>
  static bool empty(Signal& s)
  {
    return s.CountConnections() == 0;
  }

  template<typename F, typename... Args>
  static connection connect(signal<void(Args...)>& s, F&& f)
  {
    struct result_type: public jl::SignalObserver
    {
      explicit result_type(F&& f)
        : m_f(std::forward<F>(f))
      {

      }

      void call(Args... args)
      {
        m_f(std::forward<Args>(args)...);
      }
      
      F m_f;
    };

    result_type* const observer(new result_type(std::forward<F>(f)));
    
    s.Connect(observer, &result_type::call);

    return observer;
  }

  template<typename Signal, typename... Args>
  static void trigger(Signal& s, Args&&... args)
  {
    s(std::forward<Args>(args)...);
  }

  static bool connected(const connection& c)
  {
    return c->CountSignalConnections() == 0;
  }

  template<typename Signal>
  static void disconnect(Signal& s, connection& c)
  {
    s.Disconnect(c);
  }

  template<typename Signal>
  static void disconnect_all_slots(Signal& s)
  {
    s.DisconnectAll();
  }
  
  template<typename Signal>
  static void swap(Signal& s1, Signal& s2)
  {
    std::swap(s1, s2);
  }
};
