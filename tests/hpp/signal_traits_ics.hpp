#pragma once

#include <iscool/signals/signal.h>
#include <iscool/signals/signal.impl.tpp>

struct signal_traits_ics
{
  template<typename Signature>
  using signal = iscool::signals::signal<Signature>;

  using connection = iscool::signals::connection;
  
  template<typename Signal>
  static bool empty(Signal& s)
  {
    return s.empty();
  }

  template<typename Signal, typename F>
  static connection connect(Signal& s, F&& f)
  {
    return s.connect(std::forward<F>(f));
  }

  template<typename Signal, typename... Args>
  static void trigger(Signal& s, Args&&... args)
  {
    s(std::forward<Args>(args)...);
  }

  static bool connected(const connection& c)
  {
    return c.connected();
  }
  
  static void disconnect(connection& c)
  {
    c.disconnect();
  }
  
  template<typename Signal>
  static void disconnect_all_slots(Signal& s)
  {
    s.disconnect_all_slots();
  }
  
  template<typename Signal>
  static void swap(Signal& s1, Signal& s2)
  {
    s1.swap(s2);
  }
};

