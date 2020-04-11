#pragma once

#include <yassi/yassi.h>

#include <memory>

struct signal_traits_yas
{
  static constexpr bool has_signal_empty_test = false;
  static constexpr bool has_connection_connected_test = false;
  static constexpr bool has_disconnect_all = true;
  static constexpr bool has_swap = true;
  static constexpr bool will_deadlock_if_recursively_modified = false;
  
  template<typename Signature>
  using signal = yassi::Emitter<yassi::Signal<Signature>>;

  template<typename S>
  struct extract_event;

  template<typename Signature>
  struct extract_event<signal<Signature>>
  {
    using type = yassi::Signal<Signature>;
  };
    
  struct connection_base
  {
    virtual ~connection_base() {}

    virtual void disconnect(void* s) = 0;
  };

  using connection = std::shared_ptr<connection_base>;

  static void initialize() {}
  static void terminate() {}

  template<typename F, typename... Args>
  static connection connect(signal<void(Args...)>& s, F&& f)
  {
    struct result_type: connection_base
    {
      explicit result_type(F&& f)
        : m_f(std::forward<F>(f))
      {

      }

      void call(Args... args)
      {
        m_f(std::forward<Args>(args)...);
      }

      void disconnect(void* s) override
      {
        reinterpret_cast<signal<void(Args...)>*>(s)->disconnect
          (*this, &result_type::call);
      }
      
      F m_f;
    };

    std::shared_ptr<result_type> result
      (std::make_shared<result_type>(std::forward<F>(f)));

    s.template connect<yassi::Signal<void(Args...)>>
      (*result.get(), &result_type::call);
    
    return result;
  }

  template<typename Signal, typename... Args>
  static void trigger(Signal& s, Args&&... args)
  {
    s.template emit<typename extract_event<Signal>::type>
      (std::forward<Args>(args)...);
  }

  template<typename Signal>
  static void disconnect(Signal& s, connection& c)
  {
    c->disconnect(&s);
  }
  
  template<typename Signal>
  static void disconnect_all_slots(Signal& s)
  {
    s.disconnect();
  }
  
  template<typename Signal>
  static void swap(Signal& s1, Signal& s2)
  {
    std::swap(s1, s2);
  }
};

