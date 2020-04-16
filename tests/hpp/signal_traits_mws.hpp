#pragma once

#include <mwthinker/Signal/src/mw/signal.h>

#include <memory>

struct signal_traits_mws
{
  static constexpr bool has_signal_empty_test = true;
  static constexpr bool has_connection_connected_test = true;
  static constexpr bool has_disconnect_all = true;
  static constexpr bool has_swap = false;
  static constexpr bool will_deadlock_if_recursively_modified = false;
  static constexpr bool is_intrusive = false;
  
  template<typename Signature>
  struct resolve_signal;
  
  template<typename... Args>
  struct resolve_signal<void(Args...)>
  {
    using type = mw::Signal<Args...>;
  };

  template<typename Signature>
  using signal = typename resolve_signal<Signature>::type;
  
  using connection = mw::signals::Connection;

  static void initialize() {}
  static void terminate() {}
  
  template<typename Signal>
  static bool empty(Signal& s)
  {
    return s.size() == 0;
  }

  template<typename F, typename... Args>
  static connection connect(mw::Signal<Args...>& s, F&& f)
  {
    return s.connect(f);
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
  
  template<typename Signal>
  static void disconnect(Signal& s, connection& c)
  {
    c.disconnect();
  }
  
  template<typename Signal>
  static void disconnect_all_slots(Signal& s)
  {
    s.clear();
  }
};

