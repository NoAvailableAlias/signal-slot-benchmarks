/*
  API issues:
  - empty() is not implemented.
  - connection.connect() is not implemented.
  - recursive access (e.g. connection management during signal execution) hangs.
  - intrusive connection management.
  - clear() is not implemented.
  - swap is not supported.
  - disconnecting as a member function of the signal (requires to still have
    access to the signal).

  dob: connection operator bool() is not const.
  jos: connection's type depends on signals's type.
  mws: weird namespaces, mw::Signal<> and mw::signals::Connection;
  nes: painful subscribe syntax. Can't bind lambda? With captures?
  nls: subscribe return a "handle" which is a private type…
  nss: needs to implement an observer.
  psg: signal0, signal1…

  Tests to add:
  - connection that outlives the signal.
  - any way to have a signal returning a value?

  Stuff to add in the summary:
  - a legend,
  - list the libraries that validate all the tests.

  List the pitch and last update of each library.
 */

#include "tests/hpp/signal_traits_aco.hpp"
#include "tests/hpp/signal_traits_asg.hpp"
#include "tests/hpp/signal_traits_bs2.hpp"
#include "tests/hpp/signal_traits_bs2_st.hpp"
#include "tests/hpp/signal_traits_cls.hpp"
#include "tests/hpp/signal_traits_cps.hpp"
#include "tests/hpp/signal_traits_cps_st.hpp"
#include "tests/hpp/signal_traits_css.hpp"
#include "tests/hpp/signal_traits_dob.hpp"
#include "tests/hpp/signal_traits_evl.hpp"
#include "tests/hpp/signal_traits_ics.hpp"
#include "tests/hpp/signal_traits_jls.hpp"
#include "tests/hpp/signal_traits_jos.hpp"
#include "tests/hpp/signal_traits_ksc.hpp"
#include "tests/hpp/signal_traits_lfs.hpp"
#include "tests/hpp/signal_traits_lss.hpp"
#include "tests/hpp/signal_traits_mws.hpp"
#include "tests/hpp/signal_traits_nes.hpp"
#include "tests/hpp/signal_traits_nls.hpp"
#include "tests/hpp/signal_traits_nls_st.hpp"
#include "tests/hpp/signal_traits_nod.hpp"
#include "tests/hpp/signal_traits_nod_st.hpp"
#include "tests/hpp/signal_traits_nss_st.hpp"
#include "tests/hpp/signal_traits_nss_sts.hpp"
#include "tests/hpp/signal_traits_nss_ts.hpp"
#include "tests/hpp/signal_traits_nss_tss.hpp"
#include "tests/hpp/signal_traits_psg.hpp"
#include "tests/hpp/signal_traits_pss.hpp"
#include "tests/hpp/signal_traits_pss_st.hpp"
#include "tests/hpp/signal_traits_sss.hpp"
#include "tests/hpp/signal_traits_vdk.hpp"
#include "tests/hpp/signal_traits_wnk.hpp"
#include "tests/hpp/signal_traits_yas.hpp"

#include "tests/hpp/fenced_typed_test.hpp"
#include "tests/hpp/test_non_template_base.hpp"
#include "tests/hpp/test_results.hpp"

#include <typeinfo>

results_per_category g_results;

template<typename Traits>
class signal_test:
  public test_non_template_base
{
public:
  void SetUp() override
  {
    std::string library_tag(typeid(Traits).name());
    const std::string needle("signal_traits_");
    store_library_tag
      (library_tag.substr(library_tag.find(needle) + needle.size()));
    clear_test_result();
    
    Traits::initialize();
  }

  void TearDown() override
  {
    Traits::terminate();
    commit_test_result(g_results);
  }
};

using all_traits =
  testing::Types
  <
    signal_traits_aco,
    signal_traits_asg,
    signal_traits_bs2,
    signal_traits_bs2_st,
    signal_traits_cls,
    signal_traits_cps,
    signal_traits_cps_st,
    signal_traits_css,
    signal_traits_dob,
    signal_traits_evl,
    signal_traits_ics,
    signal_traits_jls,
    signal_traits_jos,
    signal_traits_ksc,
    signal_traits_lfs,
    signal_traits_lss,
    signal_traits_mws,
    signal_traits_nes,
    signal_traits_nls,
    signal_traits_nls_st,
    signal_traits_nod,
    signal_traits_nod_st,
    signal_traits_nss_st,
    signal_traits_nss_sts,
    signal_traits_nss_ts,
    signal_traits_nss_tss,
    signal_traits_psg,
    signal_traits_pss,
    signal_traits_pss_st,
    signal_traits_sss,
    signal_traits_vdk,
    signal_traits_wnk,
    signal_traits_yas
  >;

TYPED_TEST_CASE(signal_test, all_traits);

FENCED_TYPED_TEST
(signal_test, initially_empty,
 "Connection management", "Is the signal empty when constructed?",
{
  using traits = TypeParam;
  typename traits::template signal<void()> signal;

  if constexpr (traits::has_signal_empty_test)
    EXPECT_TRUE(traits::empty(signal));
  else
    this->store_test_result_not_available();
})

FENCED_TYPED_TEST
(signal_test, not_empty_when_connected,
 "Connection management",
 "Is the signal not empty when a callback is registered?",
{
  using traits = TypeParam;
  typename traits::template signal<void()> signal;

  const typename traits::connection connection
    (traits::connect(signal, []() -> void { }));

  if constexpr (traits::has_signal_empty_test)
    EXPECT_FALSE(traits::empty(signal));
  else
    this->store_test_result_not_available();
})

FENCED_TYPED_TEST
(signal_test, trigger_connection_kept,
 "Activation", "Will the callback be called if the signal is triggered?",
{
  using traits = TypeParam;
  typename traits::template signal<void()> signal;

  traits::trigger(signal);
  
  bool called(false);

  const typename traits::connection connection
    (traits::connect
     (signal,
      [ &called ]() -> void
      {
        called = true;
      }));

  traits::trigger(signal);
  EXPECT_TRUE(called);
})

FENCED_TYPED_TEST
(signal_test, trigger_connection_discarded,
 "Connection management",
 "Will the callback be called if the signal is triggered even if the connection"
 " is not stored?",
{
  using traits = TypeParam;
  typename traits::template signal<void()> signal;

  traits::trigger(signal);
  
  bool called(false);

  traits::connect
    (signal,
     [ &called ]() -> void
     {
       called = true;
     });

  traits::trigger(signal);
  EXPECT_TRUE(called);
})

FENCED_TYPED_TEST
(signal_test, disconnect,
 "Connection management",
 "Will the callback not be called if the signal is triggered once the"
 " connection has been cut?",
{
  using traits = TypeParam;
  typename traits::template signal<void()> signal;

  bool called(false);

  typename traits::connection connection
    (traits::connect
     (signal,
      [ &called ]() -> void
      {
        called = true;
      }));

  traits::disconnect(signal, connection);
  traits::trigger(signal);
  EXPECT_FALSE(called);
})

FENCED_TYPED_TEST
(signal_test, disconnect_all_slots,
 "Connection management",
 "Will the callback not be called if the signal is cleared?",
{
  using traits = TypeParam;
  typename traits::template signal<void()> signal;

  bool called(false);

  const typename traits::connection connection
    (traits::connect
     (signal,
      [ &called ]() -> void
      {
        called = true;
      }));

  if constexpr (traits::has_disconnect_all)
    {
      traits::disconnect_all_slots(signal);
      traits::trigger(signal);
  
      EXPECT_FALSE(called);
    }
  else
    this->store_test_result_not_available();
})

FENCED_TYPED_TEST
(signal_test, call_order,
 "Activation",
 "Are the callbacks called in the order they are registered?",
{
  using traits = TypeParam;
  typename traits::template signal<void()> signal;

  constexpr const int connection_count(10);
  
  int calls(0);
  int call_order[connection_count];
  typename traits::connection connections[connection_count];

  for (int i(0); i!=connection_count; ++i)
    {
      call_order[i] = -1;
      
      connections[i] =
        traits::connect
        (signal,
         [ &calls, &call_order, i ]() -> void
         {
           ++calls;
           call_order[i] = calls;
         });
    }

  traits::trigger(signal);
  
  EXPECT_EQ(1, call_order[0]);
  EXPECT_EQ(2, call_order[1]);
  EXPECT_EQ(3, call_order[2]);
  EXPECT_EQ(4, call_order[3]);
  EXPECT_EQ(5, call_order[4]);
  EXPECT_EQ(6, call_order[5]);
  EXPECT_EQ(7, call_order[6]);
  EXPECT_EQ(8, call_order[7]);
  EXPECT_EQ(9, call_order[8]);
  EXPECT_EQ(10, call_order[9]);
})

FENCED_TYPED_TEST
(signal_test, connect_while_triggered_does_not_trigger,
 "Activation",
 "Will the signal not execute a callback registered while the signal is"
 " triggered?",
{
  using traits = TypeParam;
  typename traits::template signal<void()> signal;

  if constexpr (traits::will_deadlock_if_recursively_modified)
    {
      this->store_test_result_other_failure();
      FAIL() << "This signal can't be connected while triggered.";
    }
  
  bool called(false);
  
  auto set_call
    ([ &called ]() -> void
     {
       called = true;
     });
    
  typename traits::connection connection_in;
  const typename traits::connection connection
    (traits::connect
     (signal,
      [ &signal, &connection_in, set_call ]() -> void
      {
        connection_in = traits::connect(signal, set_call);
      }));

  traits::trigger(signal);
  EXPECT_FALSE(called);

  traits::trigger(signal);
  EXPECT_TRUE(called);
})

FENCED_TYPED_TEST
(signal_test, disconnect_while_triggered_does_not_trigger,
 "Activation",
 "Will the signal not execute a callback unregistered while the signal is"
 " triggered?",
{
  using traits = TypeParam;
  typename traits::template signal<void()> signal;

  if constexpr (traits::will_deadlock_if_recursively_modified)
    {
      this->store_test_result_other_failure();
      FAIL() << "This signal can't be accessed while triggered.";
    }
  
  bool called_1(false);
  typename traits::connection connection_2;
    
  const typename traits::connection connection_1
    (traits::connect
     (signal,
      [ &called_1, &signal, &connection_2 ]() -> void
      {
        called_1 = true;
        traits::disconnect(signal, connection_2);
      }));

  bool called_2(false);
  
  connection_2 =
    traits::connect
    (signal,
     [ &called_2 ]() -> void
     {
       called_2 = true;
     });
    
  traits::trigger(signal);
  
  EXPECT_TRUE(called_1);
  EXPECT_FALSE(called_2);
})

FENCED_TYPED_TEST
(signal_test, swap_0_0,
 "Swap", "Can an empty signal be swapped with another empty signal?",
{
  using traits = TypeParam;
  typename traits::template signal<void()> signal;
  typename traits::template signal<void()> signal_alt;

  if constexpr (traits::has_swap)
    traits::swap(signal, signal_alt);
  else
    {
      this->store_test_result_not_available();
      return;
    }

  // Emptyness is the sole characteristic to test here. If it is not available
  // then we'll let the other tests validate the swap.
  if constexpr (traits::has_signal_empty_test)
    {
      EXPECT_TRUE(traits::empty(signal));
      EXPECT_TRUE(traits::empty(signal_alt));
    }
})

FENCED_TYPED_TEST
(signal_test, swap_0_1,
 "Swap", "Can an empty signal be swapped with a signal with one callback?",
{
  using traits = TypeParam;
  typename traits::template signal<void()> signal;

  if constexpr (!traits::has_swap)
    this->store_test_result_not_available();
  else
    {
      bool called(false);
      const typename traits::connection connection
        (traits::connect
         (signal,
          [ &called ]() -> void
          {
            called = true;
          }));

      typename traits::template signal<void()> signal_alt;
      traits::swap(signal_alt, signal);
    
      traits::trigger(signal);
      EXPECT_FALSE(called);

      traits::trigger(signal_alt);
      EXPECT_TRUE(called);
    }
})

FENCED_TYPED_TEST
(signal_test, swap_0_n,
 "Swap", "Can an empty signal be swapped with a signal with two callbacks?",
{
  using traits = TypeParam;
  typename traits::template signal<void()> signal;

  if constexpr (!traits::has_swap)
    this->store_test_result_not_available();
  else
    {
      bool called_1(false);
      const typename traits::connection connection_1
        (traits::connect
         (signal,
          [ &called_1 ]() -> void
          {
            called_1 = true;
          }));

      bool called_2(false);
      const typename traits::connection connection
        (traits::connect
         (signal,
          [ &called_2 ]() -> void
          {
            called_2 = true;
          }));

      typename traits::template signal<void()> signal_alt;
      traits::swap(signal_alt, signal);
    
      traits::trigger(signal);

      EXPECT_FALSE(called_1);
      EXPECT_FALSE(called_2);

      traits::trigger(signal_alt);
  
      EXPECT_TRUE(called_1);
      EXPECT_TRUE(called_2);
    }
})

FENCED_TYPED_TEST
(signal_test, swap_1_1,
 "Swap",
 "Can a signal with one callback be swapped with another signal with one"
 " callback?",
{
  using traits = TypeParam;
  typename traits::template signal<void()> signal;

  if constexpr (!traits::has_swap)
    this->store_test_result_not_available();
  else
    {
      bool called(false);
      const typename traits::connection connection
        (traits::connect
         (signal,
          [ &called ]() -> void
          {
            called = true;
          }));

      typename traits::template signal<void()> signal_alt;

      bool called_alt(false);
  
      const typename traits::connection connection_alt
        (traits::connect
         (signal_alt,
          [ &called_alt ]() -> void
          {
            called_alt = true;
          }));

      traits::swap(signal_alt, signal);
    
      traits::trigger(signal);
  
      EXPECT_FALSE(called);
      EXPECT_TRUE(called_alt);

      called_alt = false;
      traits::trigger(signal_alt);
    
      EXPECT_TRUE(called);
      EXPECT_FALSE(called_alt);
    }
})

FENCED_TYPED_TEST
(signal_test, swap_1_n,
 "Swap",
 "Can a signal with one callback be swapped with another signal with two"
 " callbacks?",
{
  using traits = TypeParam;
  typename traits::template signal<void()> signal;

  if constexpr (!traits::has_swap)
    this->store_test_result_not_available();
  else
    {
      bool called(false);

      const typename traits::connection connection
        (traits::connect
         (signal,
          [ &called ]() -> void
          {
            called = true;
          }));

      typename traits::template signal<void()> signal_alt;

      bool called_alt_1(false);
  
      const typename traits::connection connection_alt_1
        (traits::connect
         (signal_alt,
          [ &called_alt_1 ]() -> void
          {
            called_alt_1 = true;
          }));

      bool called_alt_2(false);
  
      const typename traits::connection connection_alt_2
        (traits::connect
         (signal_alt,
          [ &called_alt_2 ]() -> void
          {
            called_alt_2 = true;
          }));

      traits::swap(signal_alt, signal);
    
      traits::trigger(signal);
      EXPECT_FALSE(called);
      EXPECT_TRUE(called_alt_1);
      EXPECT_TRUE(called_alt_2);

      called_alt_1 = false;
      called_alt_2 = false;
      traits::trigger(signal_alt);
    
      EXPECT_TRUE(called);
      EXPECT_FALSE(called_alt_1);
      EXPECT_FALSE(called_alt_2);
    }
})

FENCED_TYPED_TEST
(signal_test, swap_n_n,
 "Swap",
 "Can a signal with two callbacks be swapped with another signal with two"
 " callbacks?",
{
  using traits = TypeParam;
  typename traits::template signal<void()> signal;

  if constexpr (!traits::has_swap)
    this->store_test_result_not_available();
  else
    {
      bool called_1(false);

      const typename traits::connection connection_1
        (traits::connect
         (signal,
          [ &called_1 ]() -> void
          {
            called_1 = true;
          }));

      bool called_2(false);

      const typename traits::connection connection_2
        (traits::connect
         (signal,
          [ &called_2 ]() -> void
          {
            called_2 = true;
          }));

      typename traits::template signal<void()> signal_alt;

      bool called_alt_1(false);

      const typename traits::connection connection_alt_1
        (traits::connect
         (signal_alt,
          [ &called_alt_1 ]() -> void
          {
            called_alt_1 = true;
          }));

      bool called_alt_2(false);

      const typename traits::connection connection_alt_2
        (traits::connect
         (signal_alt,
          [ &called_alt_2 ]() -> void
          {
            called_alt_2 = true;
          }));

      traits::swap(signal_alt, signal);
    
      traits::trigger(signal);
      EXPECT_FALSE(called_1);
      EXPECT_FALSE(called_2);
      EXPECT_TRUE(called_alt_1);
      EXPECT_TRUE(called_alt_2);

      called_alt_1 = false;
      called_alt_2 = false;
      traits::trigger(signal_alt);
    
      EXPECT_TRUE(called_1);
      EXPECT_TRUE(called_2);
      EXPECT_FALSE(called_alt_1);
      EXPECT_FALSE(called_alt_2);
    }
})

FENCED_TYPED_TEST
(signal_test, swap_while_triggered,
 "Swap", "Can a signal be swapped with another signal while it is triggered?",
{
  using traits = TypeParam;
  typename traits::template signal<void()> signal;

  if constexpr (!traits::has_swap)
      this->store_test_result_not_available();
  else if constexpr (traits::will_deadlock_if_recursively_modified)
    {
      this->store_test_result_other_failure();
      FAIL() << "This signal can't be accessed while triggered.";
    }
  else
    {
      int calls_1(0);

      const typename traits::connection connection_1
        (traits::connect
         (signal,
          [ &calls_1 ]() -> void
          {
            ++calls_1;
          }));

      int calls_2(0);

      const typename traits::connection connection_2
        (traits::connect
         (signal,
          [ &calls_2 ]() -> void
          {
            ++calls_2;
          }));

      typename traits::template signal<void()> signal_alt;

      const typename traits::connection connection_alt_1
        (traits::connect
         (signal_alt,
          [ &signal_alt, &signal ]() -> void
          {
            traits::swap(signal_alt, signal);
          }));
    
      int calls_alt(0);

      const typename traits::connection connection_alt_2
        (traits::connect
         (signal_alt,
          [ &calls_alt ]() -> void
          {
            ++calls_alt;
          }));
    
      traits::trigger(signal_alt);
      EXPECT_EQ(0, calls_1);
      EXPECT_EQ(0, calls_2);
      EXPECT_EQ(1, calls_alt);
  
      traits::trigger(signal_alt);
      EXPECT_EQ(1, calls_1);
      EXPECT_EQ(1, calls_2);
      EXPECT_EQ(1, calls_alt);
    }
})

FENCED_TYPED_TEST
(signal_test, connections_of_swapped_signals,
 "Swap", "When swapping two signals, will the connections be swapped too?",
{
  using traits = TypeParam;
  typename traits::template signal<void()> signal_1;

  if constexpr (!traits::has_swap)
    this->store_test_result_not_available();
  else
    {
      bool called_1(false);

      typename traits::connection connection_1
        (traits::connect
         (signal_1,
          [ &called_1 ]() -> void
          {
            called_1 = true;
          }));

      typename traits::template signal<void()> signal_2;

      bool called_2(false);

      typename traits::connection connection_2
        (traits::connect
         (signal_2,
          [ &called_2 ]() -> void
          {
            called_2 = true;
          }));

      traits::swap(signal_1, signal_2);
      traits::disconnect_all_slots(signal_1);

      if constexpr (traits::has_connection_connected_test)
        {
          EXPECT_TRUE(traits::connected(connection_1));
          EXPECT_FALSE(traits::connected(connection_2));
        }
  
      traits::trigger(signal_1);
  
      EXPECT_FALSE(called_1);
      EXPECT_FALSE(called_2);
  
      traits::trigger(signal_2);

      EXPECT_TRUE(called_1);
      EXPECT_FALSE(called_2);
    }
})

FENCED_TYPED_TEST
(signal_test, argument,
 "Activation with argument", "Can the callbacks have an argument?",
{
  using traits = TypeParam;
  typename traits::template signal<void(int)> signal;

  int arg(0);

  const typename traits::connection connection
    (traits::connect
     (signal,
      [ &arg ](int v) -> void
      {
        arg = v;
      }));

  traits::trigger(signal, 24);
  EXPECT_EQ(24, arg);
})

FENCED_TYPED_TEST
(signal_test, argument_no_copies,
 "Activation with argument",
 "Will triggering the signal not make unecessary copies of its argument?",
{
  struct copy_counter
  {
    explicit copy_counter(int* c)
      : count(c)
    {}

    copy_counter(const copy_counter& that)
      : count(that.count)
    {
      ++*count;
    }
    
    int* count;
  };

  static_assert(std::is_move_constructible_v<copy_counter>);
  static_assert(std::is_move_assignable_v<copy_counter>);
  
  using traits = TypeParam;
  typename traits::template signal<void(const copy_counter&)> signal;

  int count_in_callback(~0);
  
  const typename traits::connection connection
    (traits::connect
     (signal,
      [ &count_in_callback ](const copy_counter& c) -> void
      {
        count_in_callback = *c.count;
      }));

  int count(0);
  traits::trigger(signal, copy_counter(&count));
  
  EXPECT_EQ(0, count_in_callback);
})

FENCED_TYPED_TEST
(signal_test, recursive,
 "Activation", "Can a signal be triggered while it is triggered?",
{
  using traits = TypeParam;
  typename traits::template signal<void(int)> signal;

  if constexpr (traits::will_deadlock_if_recursively_modified)
    {
      this->store_test_result_other_failure();
      FAIL() << "This signal can't be accessed while triggered.";
    }

  int calls(0);

  const typename traits::connection connection
    (traits::connect
     (signal,
      [ &calls, &signal ](int v) -> void
      {
        ++calls;

        if (v > 0)
          traits::trigger(signal, v - 1);
      }));

  traits::trigger(signal, 3);
  EXPECT_EQ(4, calls);
})

FENCED_TYPED_TEST
(signal_test, same_function_connected_twice,
 "Activation",
 "Will the registration of the same callback twice will cause the callback to"
 " be called twice by triggering the signal?",
{
  using traits = TypeParam;
  typename traits::template signal<void()> signal;

  if constexpr (traits::is_intrusive)
    this->store_test_result_not_available();
  else
    {
      int calls(0);

      const auto callback
        ([ &calls ]() -> void
         {
           ++calls;
         });

      typename traits::connection connection_1
        (traits::connect(signal, callback));
      typename traits::connection connection_2
        (traits::connect(signal, callback));
  
      traits::trigger(signal);

      EXPECT_EQ(2, calls);

      traits::disconnect(signal, connection_1);

      traits::trigger(signal);
      EXPECT_EQ(3, calls);
    }
})

int main(int argc, char* argv[])
{
  testing::InitGoogleTest(&argc, argv);
  testing::GTEST_FLAG(death_test_style) = "threadsafe";
  
  const int result(RUN_ALL_TESTS());

  output_results(std::cout, g_results);
  
  return result;
}
