#pragma once

#include <palacaze/sigslot/include/sigslot/signal.hpp>

#include <algorithm>
#include <memory>

struct signal_traits_pss
{
  static constexpr bool has_signal_empty_test = false;
  static constexpr bool has_connection_connected_test = true;
  static constexpr bool has_disconnect_all = true;
  static constexpr bool has_swap = true;
  static constexpr bool will_deadlock_if_recursively_modified = false;
  static constexpr bool is_intrusive = false;
  
  template<typename Signature>
  struct resolve_signal;
  
  template<typename... Args>
  struct resolve_signal<void(Args...)>
  {
    using type = sigslot::signal<Args...>;
  };

  template<typename Signature>
  using signal = typename resolve_signal<Signature>::type;
  
  using connection = sigslot::connection;

  static void initialize() {}
  static void terminate() {}
  
  template<typename F, typename... Args>
  static connection connect(sigslot::signal<Args...>& s, F&& f)
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
    s.disconnect_all();
  }
  
  template<typename Signal>
  static void swap(Signal& s1, Signal& s2)
  {
    std::swap(s1, s2);
  }
};

