#pragma once

#include <copperspice/cs_signal/src/cs_signal.h>
#include <copperspice/cs_signal/src/cs_slot.h>

#include <algorithm>
#include <cassert>
#include <memory>

struct signal_traits_css
{
  static constexpr bool has_signal_empty_test = false;
  static constexpr bool has_connection_connected_test = false;
  static constexpr bool has_disconnect_all = false;
  static constexpr bool has_swap = false;
  static constexpr bool will_deadlock_if_recursively_modified = false;
  
  template<typename... Args>
  class signal_impl: public CsSignal::SignalBase
  {
  public:

    SIGNAL_1(void trigger(Args... args))
    SIGNAL_2(trigger, args...)
  };
  
  template<typename Signature>
  struct resolve_signal;
  
  template<typename... Args>
  struct resolve_signal<void(Args...)>
  {
    using type = signal_impl<Args...>;
  };

  template<typename Signature>
  using signal = typename resolve_signal<Signature>::type;

  using connection = std::shared_ptr<CsSignal::SlotBase>;
  
  template<typename F, typename... Args>
  static connection connect(signal_impl<Args...>& s, F&& f)
  {
    struct result_type: public CsSignal::SlotBase
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

    std::shared_ptr<result_type> const observer
      (std::make_shared<result_type>(std::forward<F>(f)));

    CsSignal::connect
      (s, &signal<void(Args...)>::trigger, *observer, &result_type::call);

    return observer;
  }

  template<typename Signal, typename... Args>
  static void trigger(Signal& s, Args&&... args)
  {
    s.trigger(std::forward<Args>(args)...);
  }

  template<typename Signal>
  static void disconnect(Signal& s, connection& c)
  {
    assert(c.use_count() <= 1);
    c.reset();
  }
};

