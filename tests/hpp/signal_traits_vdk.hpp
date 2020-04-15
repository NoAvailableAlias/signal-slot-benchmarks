#pragma once

#define VDK_SIGNALS_LITE
#include <vdksoft/signals/include/signals.h>

#include <memory>

struct signal_traits_vdk
{
  static constexpr bool has_signal_empty_test = true;
  static constexpr bool has_connection_connected_test = true;
  static constexpr bool has_disconnect_all = true;
  static constexpr bool has_swap = false;
  static constexpr bool will_deadlock_if_recursively_modified = false;
  static constexpr bool is_intrusive = true;
  
  template<typename Signature>
  using signal = vdk::signal<Signature>;
  
  struct connection_base
  {
    virtual ~connection_base() {}

    virtual bool connected() = 0;
    virtual void disconnect() = 0;
  };

  using connection = std::shared_ptr<connection_base>;

  static void initialize() {}
  static void terminate() {}

  template<typename Signal>
  static bool empty(Signal& s)
  {
    return s.empty();
  }
  
  template<typename F, typename... Args>
  static connection connect(signal<void(Args...)>& s, F&& f)
  {
    struct result_type: connection_base
    {
      result_type(signal<void(Args...)>& s, F&& f)
        : m_signal(s), m_f(std::forward<F>(f))
      {

      }

      void call(Args... args)
      {
        m_f(std::forward<Args>(args)...);
      }

      bool connected() override
      {
        return m_signal.connected(this, &result_type::call);
      }
      
      void disconnect() override
      {
        m_signal.disconnect(this, &result_type::call);
      }

      signal<void(Args...)>& m_signal;
      F m_f;
    };

    std::shared_ptr<result_type> result
      (std::make_shared<result_type>(s, std::forward<F>(f)));

    s.connect(result.get(), &result_type::call);
    
    return result;
  }

  template<typename Signal, typename... Args>
  static void trigger(Signal& s, Args&&... args)
  {
    s.emit(std::forward<Args>(args)...);
  }

  static bool connected(const connection& c)
  {
    return c->connected();
  }
  
  template<typename Signal>
  static void disconnect(Signal& s, connection& c)
  {
    c->disconnect();
  }
  
  template<typename Signal>
  static void disconnect_all_slots(Signal& s)
  {
    s.disconnect_all();
  }
};

