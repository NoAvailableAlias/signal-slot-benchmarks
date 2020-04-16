#pragma once

#include <pbhogan/Signals/Signal.h>

#include <algorithm>
#include <memory>

namespace detail_psg
{
  template<typename Signature>
  struct resolve_signal;

  template<>
  struct resolve_signal<void()>
  {
    using type = Gallant::Signal0<>;
  };

  template<typename A>
  struct resolve_signal<void(A)>
  {
    using type = Gallant::Signal1<A>;
  };
}

struct signal_traits_psg
{
  static constexpr bool has_signal_empty_test = true;
  static constexpr bool has_connection_connected_test = false;
  static constexpr bool has_disconnect_all = true;
  static constexpr bool has_swap = true;
  static constexpr bool will_deadlock_if_recursively_modified = false;
  static constexpr bool is_intrusive = true;

  template<typename Signature>
  using signal = typename detail_psg::resolve_signal<Signature>::type;

  struct connection_base
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
    return s.Empty();
  }

  template<typename F>
  static connection connect(Gallant::Signal0<>& s, F&& f)
  {
    return connect_internal<signal<void()>, F>(s, std::forward<F>(f));
  }

  template<typename F, typename A>
  static connection connect(Gallant::Signal1<A>& s, F&& f)
  {
    return connect_internal<signal<void(A)>, F, A>(s, std::forward<F>(f));
  }
  
  template<typename Signal, typename F, typename... Args>
  static connection connect_internal(Signal& s, F&& f)
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
        reinterpret_cast<signal<void(Args...)>*>(s)->Disconnect
          (this, &result_type::call);
      }
      
      F m_f;
    };

    std::shared_ptr<result_type> observer
      (std::make_shared<result_type>(std::forward<F>(f)));
    
    s.Connect(observer.get(), &result_type::call);

    return observer;
  }

  template<typename Signal, typename... Args>
  static void trigger(Signal& s, Args&&... args)
  {
    s.Emit(std::forward<Args>(args)...);
  }

  template<typename Signal>
  static void disconnect(Signal& s, connection& c)
  {
    c->disconnect(&s);
  }

  template<typename Signal>
  static void disconnect_all_slots(Signal& s)
  {
    s.Clear();
  }
  
  template<typename Signal>
  static void swap(Signal& s1, Signal& s2)
  {
    std::swap(s1, s2);
  }
};
