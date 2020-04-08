#pragma once

#include <Montellese/cpp-signal/cpp-signal.h>

#include <algorithm>
#include <memory>

struct signal_traits_cps_st
{
private:
  using cppsignal = cpp_signal<cpp_signal_no_locking>;

public:
  static constexpr bool has_signal_empty_test = false;
  static constexpr bool has_connection_connected_test = false;
  static constexpr bool has_disconnect_all = true;
  static constexpr bool has_swap = true;
  static constexpr bool will_deadlock_if_recursively_modified = false;
  
  template<typename Signature>
  using signal = cppsignal::signal<Signature>;

  using connection = std::shared_ptr<cppsignal::slot_tracker>;

  static void initialize() {}
  static void terminate() {}
  
  template<typename F, typename... Args>
  static connection connect(signal<void(Args...)>& s, F&& f)
  {
    struct result_type: public cppsignal::slot_tracker
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

    s.template connect<result_type, &result_type::call>(*observer);

    return observer;
  }

  template<typename Signal, typename... Args>
  static void trigger(Signal& s, Args&&... args)
  {
    s(std::forward<Args>(args)...);
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

