#pragma once

#include <supergrover/sigslot/src/signal.h>

#include <memory>

struct signal_traits_sss
{
  static constexpr bool has_signal_empty_test = false;
  static constexpr bool has_connection_connected_test = false;
  static constexpr bool has_disconnect_all = true;
  static constexpr bool has_swap = true;
  static constexpr bool will_deadlock_if_recursively_modified = false;
  
  template<typename Signature>
  struct resolve_signal;
  
  template<typename... Args>
  struct resolve_signal<void(Args...)>
  {
    using type = ::Signal<Args...>;
  };

  template<typename Signature>
  using signal = typename resolve_signal<Signature>::type;
  
  using connection = std::shared_ptr<SigSlotBase>;

  static void initialize() {}
  static void terminate() {}
  
  template<typename F, typename... Args>
  static connection connect(::Signal<Args...>& s, F&& f)
  {
    struct result_type: SigSlotBase
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

    std::shared_ptr<result_type> result
      (std::make_shared<result_type>(std::forward<F>(f)));

    s.bind(&result_type::call, result.get());

    return result;
  }

  template<typename Signal, typename... Args>
  static void trigger(Signal& s, Args&&... args)
  {
    s.emit(std::forward<Args>(args)...);
  }

  template<typename Signal>
  static void disconnect(Signal& s, connection& c)
  {
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

