#pragma once

#include <cpp11nullptr/lsignal/lsignal.h>
#include <algorithm>

struct signal_traits_cls
{
  static constexpr bool has_signal_empty_test = false;
  static constexpr bool has_connection_connected_test = false;
  static constexpr bool will_deadlock_if_recursively_modified = true;

  template<typename Signature>
  using signal = lsignal::signal<Signature>;

  using connection = lsignal::slot;
  
  template<typename Signal, typename F>
  static connection connect(Signal& s, F&& f)
  {
    connection result;
    s.connect(std::forward<F>(f), &result);

    return result;
  }

  template<typename Signal, typename... Args>
  static void trigger(Signal& s, Args&&... args)
  {
    s(std::forward<Args>(args)...);
  }

  template<typename Signal>
  static void disconnect(Signal& s, connection& c)
  {
    c.disconnect();
  }
  
  template<typename Signal>
  static void disconnect_all_slots(Signal& s)
  {
    s.disconnect_all();
  }
  
  template<typename Signal>
  static void swap(Signal& s1, Signal& s2)
  {
    std::swap(s1, s2);
  }
};

