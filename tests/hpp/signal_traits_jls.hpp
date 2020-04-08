#pragma once

#include "jeffomatic/jl_signal/src/Signal.h"
#include "jeffomatic/jl_signal/src/StaticSignalConnectionAllocators.h"

#include <algorithm>
#include <memory>

struct signal_traits_jls
{
  static constexpr bool has_signal_empty_test = true;
  static constexpr bool has_connection_connected_test = true;
  static constexpr bool has_disconnect_all = true;
  static constexpr bool has_swap = true;
  static constexpr bool will_deadlock_if_recursively_modified = false;
  
  template<typename Signature>
  using signal = jl::Signal<Signature>;

  using connection = std::shared_ptr<jl::SignalObserver>;

private:
  static std::unique_ptr<jl::StaticSignalConnectionAllocator<16>>
  m_signal_con_allocator;

  static std::unique_ptr<jl::StaticObserverConnectionAllocator<16>>
  m_observer_con_allocator;

public:
  static void initialize()
  {
    printf("yep\n");
    m_signal_con_allocator =
      std::make_unique<decltype(m_signal_con_allocator)::element_type>();
    m_observer_con_allocator =
      std::make_unique<decltype(m_observer_con_allocator)::element_type>();

    jl::SignalBase::SetCommonConnectionAllocator(m_signal_con_allocator.get());
    jl::SignalObserver::SetCommonConnectionAllocator
      (m_observer_con_allocator.get());
  }
  
  static void terminate()
  {
    jl::SignalBase::SetCommonConnectionAllocator(nullptr);
    jl::SignalObserver::SetCommonConnectionAllocator(nullptr);

    m_signal_con_allocator.reset();
    m_observer_con_allocator.reset();
  }
  
  template<typename Signal>
  static bool empty(Signal& s)
  {
    return s.CountConnections() == 0;
  }

  template<typename F, typename... Args>
  static connection connect(signal<void(Args...)>& s, F&& f)
  {
    struct result_type: public jl::SignalObserver
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

    std::shared_ptr<result_type> observer
      (std::make_shared<result_type>(std::forward<F>(f)));
    
    s.Connect(observer.get(), &result_type::call);

    return observer;
  }

  template<typename Signal, typename... Args>
  static void trigger(Signal& s, Args&&... args)
  {
    s(std::forward<Args>(args)...);
  }

  static bool connected(const connection& c)
  {
    return c->CountSignalConnections() == 0;
  }

  template<typename Signal>
  static void disconnect(Signal& s, connection& c)
  {
    s.Disconnect(c.get());
  }

  template<typename Signal>
  static void disconnect_all_slots(Signal& s)
  {
    s.DisconnectAll();
  }
  
  template<typename Signal>
  static void swap(Signal& s1, Signal& s2)
  {
    std::swap(s1, s2);
  }
};
