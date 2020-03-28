#pragma once

#include <Ansoulom/cpp-observe/cpp_observe.hpp>

struct signal_traits_aco
{
  static constexpr bool has_signal_empty_test = false;
  static constexpr bool has_connection_connected_test = true;
  
  template<typename Signature>
  struct resolve_signal;
  
  template<typename... Args>
  struct resolve_signal<void(Args...)>
  {
    using type = observe::subject<Args...>;
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
    template<typename F>
    explicit connection_impl(F&& f)
      : observer(std::forward<F>(f))
    {

    }

    void disconnect(void* s) override
    {
      reinterpret_cast<signal<void(Args...)>*>(s)->remove_observer(observer);
    }
    
    observe::observer<Args...> observer;
  };

  using connection = connection_base*;
  
  template<typename F, typename... Args>
  static connection connect(observe::subject<Args...>& s, F&& f)
  {
    connection_impl<Args...>* const result(new connection_impl<Args...>(f));

    s.add_observer(result->observer);
    
    return result;
  }

  template<typename Signal, typename... Args>
  static void trigger(Signal& s, Args&&... args)
  {
    s(std::forward<Args>(args)...);
  }

  static bool connected(const connection& c)
  {
    return false;
  }

  template<typename Signal>
  static void disconnect(Signal& s, connection& c)
  {
    c->disconnect(&s);
  }
  
  template<typename Signal>
  static void disconnect_all_slots(Signal& s)
  {
    s.clear();
  }
  
  template<typename Signal>
  static void swap(Signal& s1, Signal& s2)
  {
    std::swap(s1, s2);
  }
};

