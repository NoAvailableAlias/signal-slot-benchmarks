#pragma once

#include <NoAvailableAlias/nano-signal-slot/nano_signal_slot.hpp>
#include <NoAvailableAlias/nano-signal-slot/nano_mutex.hpp>

#include <memory>

struct signal_traits_nss_sts
{
  static constexpr bool has_signal_empty_test = true;
  static constexpr bool has_connection_connected_test = false;
  static constexpr bool has_disconnect_all = true;
  static constexpr bool has_swap = false;
  static constexpr bool will_deadlock_if_recursively_modified = false;
  static constexpr bool is_intrusive = true;

  using policy = Nano::ST_Policy_Safe;
  
  template<typename Signature>
  using signal = Nano::Signal<Signature, policy>;

  struct connection_base: Nano::Observer<policy>
  {
    virtual ~connection_base() {}

    virtual void disconnect(void* s) = 0;
  };
  
  using connection = std::shared_ptr<connection_base>;

  static void initialize() {}
  static void terminate() {}
  
  template<typename Signal>
  static bool empty(Signal& s)
  {
    return s.is_empty();
  }

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
        reinterpret_cast<signal<void(Args...)>*>(s)->
          template disconnect<&result_type::call>(this);
      }
      
      F m_f;
    };

    std::shared_ptr<result_type> observer
      (std::make_shared<result_type>(std::forward<F>(f)));
    
    s.template connect<&result_type::call>(observer.get());

    return observer;
  }

  template<typename Signal, typename... Args>
  static void trigger(Signal& s, Args&&... args)
  {
    s.fire(std::forward<Args>(args)...);
  }

  static bool connected(const connection& c)
  {
    return c->is_empty();
  }

  template<typename Signal>
  static void disconnect(Signal& s, connection& c)
  {
    c->disconnect(&s);
  }

  template<typename Signal>
  static void disconnect_all_slots(Signal& s)
  {
    s.disconnect_all();
  }
};
