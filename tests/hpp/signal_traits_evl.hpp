#pragma once

#include <eviltwin/observer.hpp>

#include <cassert>

struct signal_traits_evl
{
  static constexpr bool has_signal_empty_test = false;
  static constexpr bool has_connection_connected_test = true;
  static constexpr bool has_disconnect_all = true;
  static constexpr bool has_swap = true;
  static constexpr bool will_deadlock_if_recursively_modified = false;
  
  template<typename Signature>
  using signal = obs::Subject<Signature>;

  using connection = obs::Registration;

  static void initialize() {}
  static void terminate() {}

  template<typename Signal, typename F>
  static connection connect(Signal& s, F&& f)
  {
    return s.registerObserver(std::forward<F>(f));
  }

  template<typename Signal, typename... Args>
  static void trigger(Signal& s, Args&&... args)
  {
    s(std::forward<Args>(args)...);
  }

  static bool connected(connection& c)
  {
    return c != connection();
  }
  
  template<typename Signal>
  static void disconnect(Signal& s, connection& c)
  {
    assert(c.use_count() <= 1);
    c.reset();
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

