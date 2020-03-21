#include "tests/hpp/signal_traits_ics.hpp"
#include "tests/hpp/signal_traits_jls.hpp"

// These are required in Main to initialize the jlsignal allocator
#include "jeffomatic/jl_signal/src/Signal.h"
#include "jeffomatic/jl_signal/src/StaticSignalConnectionAllocators.h"

#include <gtest/gtest.h>

#define SAFE_TYPED_TEST(test_suite, test_name, body)                    \
  TYPED_TEST(test_suite, test_name)                                     \
  {                                                                     \
    const auto test_runner                                              \
      ([]() -> void                                                     \
       {                                                                \
         body                                                           \
           });                                                          \
                                                                        \
    ASSERT_EXIT(test_runner(); exit(0), testing::ExitedWithCode(0), ""); \
    test_runner();                                                      \
  }

template<typename Traits>
class signal_test:
  public testing::Test
{

};

using all_traits = testing::Types<signal_traits_ics, signal_traits_jls>;

TYPED_TEST_CASE(signal_test, all_traits);

SAFE_TYPED_TEST(signal_test, initially_empty,
{
  using traits = TypeParam;
  typename traits::template signal<void()> signal;
  
  EXPECT_TRUE(traits::empty(signal));
})

SAFE_TYPED_TEST(signal_test, connect,
{
  using traits = TypeParam;
  typename traits::template signal<void()> signal;

  bool called(false);

  traits::connect
    (signal,
     [ &called ]() -> void
     {
       called = true;
     });

  EXPECT_FALSE(traits::empty(signal));
    
  traits::trigger(signal);
  EXPECT_TRUE(called);
 })

SAFE_TYPED_TEST(signal_test, disconnect,
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

  traits::disconnect(connection);
  traits::trigger(signal);
  EXPECT_FALSE(called);
})

SAFE_TYPED_TEST(signal_test, disconnect_all_slots,
{
  using traits = TypeParam;
  typename traits::template signal<void()> signal;

  bool called(false);

  traits::connect
    (signal,
     [ &called ]() -> void
     {
       called = true;
     });

  traits::disconnect_all_slots(signal);
  traits::trigger(signal);
  
  EXPECT_FALSE(called);
 })

SAFE_TYPED_TEST(signal_test, call_order,
{
  using traits = TypeParam;
  typename traits::template signal<void()> signal;

  constexpr const int connection_count(10);
  
  int calls(0);
  int call_order[connection_count];

  for (int i(0); i!=connection_count; ++i)
    traits::connect
      (signal,
       [ &calls, &call_order, i ]() -> void
       {
         ++calls;
         call_order[i] = calls;
       });

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

SAFE_TYPED_TEST(signal_test, connect_while_triggered_does_not_trigger,
{
  using traits = TypeParam;
  typename traits::template signal<void()> signal;

  bool called(false);
  
  auto set_call
    ([ &called ]() -> void
     {
       called = true;
     });
    
  traits::connect
    (signal,
     [ &signal, set_call ]() -> void
     {
       traits::connect(signal, set_call);
     });

  traits::trigger(signal);
  EXPECT_FALSE(called);

  traits::trigger(signal);
  EXPECT_TRUE(called);
 })

SAFE_TYPED_TEST(signal_test, disconnect_while_triggered_does_not_trigger,
{
  using traits = TypeParam;
  typename traits::template signal<void()> signal;

  bool called_1(false);
  typename traits::connection connection;
    
  traits::connect
    (signal,
     [ &called_1, &connection ]() -> void
     {
       called_1 = true;
       traits::disconnect(connection);
     });

  bool called_2(false);
  
  connection =
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

SAFE_TYPED_TEST(signal_test, swap_0_0,
{
  using traits = TypeParam;
  typename traits::template signal<void()> signal;
  typename traits::template signal<void()> signal_alt;

  traits::swap(signal, signal_alt);

  EXPECT_TRUE(traits::empty(signal));
  EXPECT_TRUE(traits::empty(signal_alt));
 })

SAFE_TYPED_TEST(signal_test, swap_0_1,
{
  using traits = TypeParam;
  typename traits::template signal<void()> signal;

  bool called(false);
  traits::connect
    (signal,
     [ &called ]() -> void
     {
       called = true;
     });

  typename traits::template signal<void()> signal_alt;
  traits::swap(signal_alt, signal);
    
  traits::trigger(signal);
  EXPECT_FALSE(called);

  traits::trigger(signal_alt);
  EXPECT_TRUE(called);
 })

SAFE_TYPED_TEST(signal_test, swap_0_n,
{
  using traits = TypeParam;
  typename traits::template signal<void()> signal;

  bool called_1(false);
  traits::connect
    (signal,
     [ &called_1 ]() -> void
     {
       called_1 = true;
     });

  bool called_2(false);
  traits::connect
    (signal,
     [ &called_2 ]() -> void
     {
       called_2 = true;
     });

  typename traits::template signal<void()> signal_alt;
  traits::swap(signal_alt, signal);
    
  traits::trigger(signal);

  EXPECT_FALSE(called_1);
  EXPECT_FALSE(called_2);

  traits::trigger(signal_alt);
  
  EXPECT_TRUE(called_1);
  EXPECT_TRUE(called_2);
 })

SAFE_TYPED_TEST(signal_test, swap_1_1,
{
  using traits = TypeParam;
  typename traits::template signal<void()> signal;

  bool called(false);
  traits::connect
    (signal,
     [ &called ]() -> void
     {
       called = true;
     });

  typename traits::template signal<void()> signal_alt;

  bool called_alt(false);
  
  traits::connect
    (signal_alt,
     [ &called_alt ]() -> void
     {
       called_alt = true;
     });

  traits::swap(signal_alt, signal);
    
  traits::trigger(signal);
  
  EXPECT_FALSE(called);
  EXPECT_TRUE(called_alt);

  called_alt = false;
  traits::trigger(signal_alt);
    
  EXPECT_TRUE(called);
  EXPECT_FALSE(called_alt);
})

SAFE_TYPED_TEST(signal_test, swap_1_n,
{
  using traits = TypeParam;
  typename traits::template signal<void()> signal;

  bool called(false);

  traits::connect
    (signal,
     [ &called ]() -> void
     {
       called = true;
     });

  typename traits::template signal<void()> signal_alt;

  bool called_alt_1(false);
  
  traits::connect
    (signal_alt,
     [ &called_alt_1 ]() -> void
     {
       called_alt_1 = true;
     });

  bool called_alt_2(false);
  
  traits::connect
    (signal_alt,
     [ &called_alt_2 ]() -> void
     {
       called_alt_2 = true;
     });

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
 })

SAFE_TYPED_TEST(signal_test, swap_n_n,
{
  using traits = TypeParam;
  typename traits::template signal<void()> signal;

  bool called_1(false);

  traits::connect
    (signal,
     [ &called_1 ]() -> void
     {
       called_1 = true;
     });

  bool called_2(false);

  traits::connect
    (signal,
     [ &called_2 ]() -> void
     {
       called_2 = true;
     });

  typename traits::template signal<void()> signal_alt;

  bool called_alt_1(false);

  traits::connect
    (signal_alt,
     [ &called_alt_1 ]() -> void
     {
       called_alt_1 = true;
     });

  bool called_alt_2(false);

  traits::connect
    (signal_alt,
     [ &called_alt_2 ]() -> void
     {
       called_alt_2 = true;
     });

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
})

SAFE_TYPED_TEST(signal_test, swap_while_triggered,
{
  using traits = TypeParam;
  typename traits::template signal<void()> signal;

  int calls_1(0);
  traits::connect
    (signal,
     [ &calls_1 ]() -> void
     {
       ++calls_1;
     });

  int calls_2(0);
  traits::connect
    (signal,
     [ &calls_2 ]() -> void
     {
       ++calls_2;
     });

  typename traits::template signal<void()> signal_alt;

  traits::connect
    (signal_alt,
     [ &signal_alt, &signal ]() -> void
     {
       traits::swap(signal_alt, signal);
     });
    
  int calls_alt(0);

  traits::connect
    (signal_alt,
     [ &calls_alt ]() -> void
     {
       ++calls_alt;
     });
    
  traits::trigger(signal_alt);
  EXPECT_EQ(0, calls_1);
  EXPECT_EQ(0, calls_2);
  EXPECT_EQ(1, calls_alt);
  
  traits::trigger(signal_alt);
  EXPECT_EQ(1, calls_1);
  EXPECT_EQ(1, calls_2);
  EXPECT_EQ(1, calls_alt);
})

SAFE_TYPED_TEST(signal_test, connections_of_swapped_signals,
{
  using traits = TypeParam;
  typename traits::template signal<void()> signal_1;

  bool called_1(false);

  const typename traits::connection connection_1
    (traits::connect
     (signal_1,
      [ &called_1 ]() -> void
      {
        called_1 = true;
      }));

  typename traits::template signal<void()> signal_2;

  bool called_2(false);

  const typename traits::connection connection_2
    (traits::connect
     (signal_2,
      [ &called_2 ]() -> void
      {
        called_2 = true;
      }));

  traits::swap(signal_1, signal_2);
  traits::disconnect_all_slots(signal_1);

  EXPECT_TRUE(traits::empty(signal_1));
  EXPECT_FALSE(traits::empty(signal_2));

  EXPECT_TRUE(traits::connected(connection_1));
  EXPECT_FALSE(traits::connected(connection_2));

  traits::trigger(signal_2);

  EXPECT_TRUE(called_1);
  EXPECT_FALSE(called_2);
})

SAFE_TYPED_TEST(signal_test, argument,
{
  using traits = TypeParam;
  typename traits::template signal<void(int)> signal;

  int arg(0);
  traits::connect
    (signal,
     [ &arg ](int v) -> void
     {
       arg = v;
     });

  traits::trigger(signal, 24);
  EXPECT_EQ(24, arg);
})

SAFE_TYPED_TEST(signal_test, recursive,
{
  using traits = TypeParam;
  typename traits::template signal<void(int)> signal;

  int calls(0);

  traits::connect
    (signal,
     [ &calls, &signal ](int v) -> void
     {
       ++calls;

       if (v > 0)
         signal(v - 1);
     });

  traits::trigger(signal, 3);
  EXPECT_EQ(4, calls);
})

SAFE_TYPED_TEST(signal_test, same_function_connected_twice,
{
  using traits = TypeParam;
  typename traits::template signal<void()> signal;

  int calls(0);

  const auto callback
    ([ &calls ]() -> void
     {
       ++calls;
     });

  typename traits::connection connection_1(traits::connect(signal, callback));
  typename traits::connection connection_2(traits::connect(signal, callback));
  
  traits::trigger(signal);

  EXPECT_EQ(2, calls);

  traits::disconnect(connection_1);

  traits::trigger(signal);
  EXPECT_EQ(3, calls);
})

int main(int argc, char* argv[])
{
  // Jl_signal uses a static allocator for maximum performance
  constexpr const std::size_t C_JLSIGNAL_MAX = 256;
  jl::StaticSignalConnectionAllocator<C_JLSIGNAL_MAX> signal_con_allocator;
  jl::StaticObserverConnectionAllocator<C_JLSIGNAL_MAX> observer_con_allocator;
  jl::SignalBase::SetCommonConnectionAllocator(&signal_con_allocator);
  jl::SignalObserver::SetCommonConnectionAllocator(&observer_con_allocator);

  testing::InitGoogleTest(&argc, argv);
  return RUN_ALL_TESTS();
}
