#pragma once

#include <i42output/include/neolib/task/event.hpp>

#include <functional>

struct signal_traits_nls
{
  static constexpr bool has_signal_empty_test = true;
  static constexpr bool has_connection_connected_test = false;
  static constexpr bool has_disconnect_all = true;
  static constexpr bool has_swap = false;
  static constexpr bool will_deadlock_if_recursively_modified = false;
  static constexpr bool is_intrusive = false;

  template<typename Signature>
  struct resolve_signal;

  template<typename... Args>
  struct resolve_signal<void(Args...)>
  {
    using type = neolib::event<Args...>;
  };

  template<typename Signature>
  using signal = typename resolve_signal<Signature>::type;

  using connection = neolib::ref_ptr<neolib::i_slot_base>;

  static void initialize()
  {
    if (!neolib::services::service_provider_allocated())
      neolib::services::allocate_service_provider();
    neolib::services::service<neolib::i_event_system>().set_locking_strategy
      (neolib::event_system_locking_strategy::MultiThreaded);
  }

  static void terminate() {}

  template<typename Signal>
  static bool empty(Signal& s)
  {
    return !s.has_slots();
  }

  template<typename F, typename... Args>
  static connection connect(neolib::event<Args...>& s, F&& f)
  {
    return (~s(std::function<void(Args...)>{ std::forward<F>(f) })).slot;
  }

  template<typename Signal, typename... Args>
  static void trigger(Signal& s, Args&&... args)
  {
    s.trigger(std::forward<Args>(args)...);
  }

  template<typename Signal>
  static void disconnect(Signal& s, connection& c)
  {
    c->remove();
  }

  template<typename Signal>
  static void disconnect_all_slots(Signal& s)
  {
    s = Signal();
  }
};
