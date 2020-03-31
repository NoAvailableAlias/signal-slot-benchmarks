#pragma once

#include <joanrieu/signal11/signal11.h>

#include <algorithm>
#include <memory>

struct signal_traits_jos
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
    using type = signal11<Args...>;
  };

  template<typename Signature>
  using signal = typename resolve_signal<Signature>::type;

  struct connection_base
  {
    virtual ~connection_base() {}

    virtual void disconnect(void* s) = 0;
  };

  template<typename... Args>
  struct connection_impl: public connection_base
  {
    explicit connection_impl(typename signal11<Args...>::connection connection)
      : connection(connection)
    {

    }

    void disconnect(void* s) override
    {
      reinterpret_cast<signal11<Args...>*>(s)->disconnect(connection);
    }
    
    typename signal11<Args...>::connection connection;
  };

  using connection = std::shared_ptr<connection_base>;
  
  template<typename F, typename... Args>
  static connection connect(signal11<Args...>& s, F&& f)
  {
    return std::make_shared<connection_impl<Args...>>(s.connect(f));
  }

  template<typename Signal, typename... Args>
  static void trigger(Signal& s, Args&&... args)
  {
    s(std::forward<Args>(args)...);
  }

  template<typename Signal>
  static void disconnect(Signal& s, connection& c)
  {
    c->disconnect(&s);
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

