#pragma once

#include <amc522/signal11/Signal.h>

#include <cstdlib>

struct signal_traits_asg
{
  template<typename Signature>
  using signal = Signal11::Signal<Signature>;

  using connection = Signal11::ConnectionRef;
  
  template<typename Signal>
  static bool empty(Signal& s)
  {
    std::abort();
    return false;
  }

  template<typename Signal, typename F>
  static connection connect(Signal& s, F&& f)
  {
    return s.connect(std::forward<F>(f));
  }

  template<typename Signal, typename... Args>
  static void trigger(Signal& s, Args&&... args)
  {
    s.emit(std::forward<Args>(args)...);
  }

  static bool connected(const connection& c)
  {
    return c.isValid();
  }
  
  template<typename Signal>
  static void disconnect(Signal& s, connection& c)
  {
    c.disconnect();
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

