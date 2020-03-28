#pragma once

#include <boost/signals2/signal.hpp>

struct signal_traits_bs2
{
  static constexpr bool has_signal_empty_test = true;
  static constexpr bool has_connection_connected_test = true;
  
  template<typename Signature>
  using signal = boost::signals2::signal<Signature>;

  using connection = boost::signals2::connection;
  
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

  template<typename Signal>
  static void disconnect(Signal& s, connection& c)
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

