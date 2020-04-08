#pragma once

#include <larspensjo/SimpleSignal/SimpleSignal.h>

struct signal_traits_lss
{
  static constexpr bool has_signal_empty_test = true;
  static constexpr bool has_connection_connected_test = false;
  static constexpr bool has_disconnect_all = false;
  static constexpr bool has_swap = false;
  static constexpr bool will_deadlock_if_recursively_modified = false;
  
  template<typename Signature>
  using signal = Simple::Signal<Signature>;

  using connection = std::size_t;

  static void initialize() {}
  static void terminate() {}
  
  template<typename Signal>
  static bool empty(Signal& s)
  {
    return s.size() == 0;
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

  template<typename Signal>
  static void disconnect(Signal& s, connection& c)
  {
    s.disconnect(c);
  }
};

