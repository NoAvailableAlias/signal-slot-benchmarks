/*
  API issues:
  - empty() is not implemented.
  - connection.connect() is not implemented.
  - recursive access (e.g. connection management during signal execution) hangs.
  - intrusive connection management.
  - clear() is not implemented.
  - swap is not supported.

  dob: connection operator bool() is not const.
 */

#include "tests/hpp/signal_traits_aco.hpp"
#include "tests/hpp/signal_traits_asg.hpp"
#include "tests/hpp/signal_traits_bs2.hpp"
#include "tests/hpp/signal_traits_cls.hpp"
#include "tests/hpp/signal_traits_cps.hpp"
#include "tests/hpp/signal_traits_cps_st.hpp"
#include "tests/hpp/signal_traits_css.hpp"
#include "tests/hpp/signal_traits_dob.hpp"
//#include "tests/hpp/signal_traits_evl.hpp"
#include "tests/hpp/signal_traits_ics.hpp"
#include "tests/hpp/signal_traits_jls.hpp"
//#include "tests/hpp/signal_traits_jos.hpp"
//#include "tests/hpp/signal_traits_ksc.hpp"
//#include "tests/hpp/signal_traits_lfs.hpp"
//#include "tests/hpp/signal_traits_lss.hpp"
//#include "tests/hpp/signal_traits_mws.hpp"
//#include "tests/hpp/signal_traits_nes.hpp"
//#include "tests/hpp/signal_traits_nls.hpp"
//#include "tests/hpp/signal_traits_nls_st.hpp"
//#include "tests/hpp/signal_traits_nod.hpp"
//#include "tests/hpp/signal_traits_nod_st.hpp"
//#include "tests/hpp/signal_traits_nss_st.hpp"
//#include "tests/hpp/signal_traits_nss_sts.hpp"
//#include "tests/hpp/signal_traits_nss_ts.hpp"
//#include "tests/hpp/signal_traits_nss_tss.hpp"
//#include "tests/hpp/signal_traits_psg.hpp"
//#include "tests/hpp/signal_traits_pss.hpp"
//#include "tests/hpp/signal_traits_pss_st.hpp"
//#include "tests/hpp/signal_traits_sss.hpp"
//#include "tests/hpp/signal_traits_wnk.hpp"
//#include "tests/hpp/signal_traits_yas.hpp"
//#include "tests/hpp/signal_traits_vdk.hpp"

// These are required in Main to initialize the jlsignal allocator
#include "jeffomatic/jl_signal/src/Signal.h"
#include "jeffomatic/jl_signal/src/StaticSignalConnectionAllocators.h"

#include <typeinfo>

#include <gtest/gtest.h>

enum class color_t
  {
   red,
   magenta,
   green,
   blue,
   yellow,
   gray
  };
  
struct result_label
{
  color_t color;
  std::string text;
};
  
std::map<std::string, std::map<std::string, result_label>> g_results;

#define SAFE_TYPED_TEST(test_suite, test_name, body)                    \
  TYPED_TEST(test_suite, test_name)                                     \
  {                                                                     \
    const auto test_runner                                              \
      ([this]() -> void                                                 \
       {                                                                \
         body                                                           \
       });                                                              \
                                                                        \
    EXPECT_EXIT(test_runner(); exit(0), testing::ExitedWithCode(0), ""); \
                                                                        \
    if (!this->HasFailure())                                            \
      test_runner();                                                    \
                                                                        \
    if (this->HasFailure())                                             \
      this->store_test_result_failure();                                \
    else                                                                \
      this->store_test_result_success();                                \
  }

template<typename Traits>
class signal_test:
  public testing::Test
{
protected:
  void store_test_result_success()
  {
    store_test_result(color_t::green, "yes");
  }

  void store_test_result_failure()
  {
    store_test_result(color_t::red, "no");
  }

  void store_test_result_not_available()
  {
    store_test_result(color_t::gray, "n/a");
  }

  void store_test_result_other_failure()
  {
    store_test_result(color_t::magenta, "X");
  }

  void store_test_result_cant_tell()
  {
    store_test_result(color_t::yellow, "?");
  }
  
private:
  void store_test_result(color_t color, const std::string& result)
  {
    std::map<std::string, result_label>& test_results
      (g_results
       [testing::UnitTest::GetInstance()->current_test_info()->name()]);

    std::string tag(typeid(Traits).name());
    const std::string needle("signal_traits_");
    tag = tag.substr(tag.find(needle) + needle.size());
    
    const auto it(test_results.find(tag));

    if (it == test_results.end())
      test_results[tag] = result_label{color, result};
  }
};

using all_traits =
  testing::Types
  <
    signal_traits_aco,
    signal_traits_asg,
    signal_traits_bs2,
    signal_traits_cls,
    signal_traits_cps,
    signal_traits_cps_st,
    signal_traits_css,
    signal_traits_dob,
    signal_traits_ics,
    signal_traits_jls
  >;

TYPED_TEST_CASE(signal_test, all_traits);

SAFE_TYPED_TEST(signal_test, initially_empty,
{
  using traits = TypeParam;
  typename traits::template signal<void()> signal;

  if constexpr (traits::has_signal_empty_test)
    EXPECT_TRUE(traits::empty(signal));
  else
    this->store_test_result_cant_tell();
})

SAFE_TYPED_TEST(signal_test, not_empty_when_connected,
{
  using traits = TypeParam;
  typename traits::template signal<void()> signal;

  const typename traits::connection connection
    (traits::connect(signal, []() -> void { }));

  if constexpr (traits::has_signal_empty_test)
    EXPECT_FALSE(traits::empty(signal));
  else
    this->store_test_result_cant_tell();
})

SAFE_TYPED_TEST(signal_test, trigger_connection_kept,
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


SAFE_TYPED_TEST(signal_test, trigger_connection_discarded,
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

  traits::disconnect(signal, connection);
  traits::trigger(signal);
  EXPECT_FALSE(called);
})

SAFE_TYPED_TEST(signal_test, disconnect_all_slots,
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

SAFE_TYPED_TEST(signal_test, call_order,
{
  using traits = TypeParam;
  typename traits::template signal<void()> signal;

  constexpr const int connection_count(10);
  
  int calls(0);
  int call_order[connection_count];
  typename traits::connection connections[connection_count];

  for (int i(0); i!=connection_count; ++i)
    connections[i] =
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

SAFE_TYPED_TEST(signal_test, disconnect_while_triggered_does_not_trigger,
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

SAFE_TYPED_TEST(signal_test, swap_0_0,
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

SAFE_TYPED_TEST(signal_test, swap_0_1,
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

SAFE_TYPED_TEST(signal_test, swap_0_n,
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

SAFE_TYPED_TEST(signal_test, swap_1_1,
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

SAFE_TYPED_TEST(signal_test, swap_1_n,
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

SAFE_TYPED_TEST(signal_test, swap_n_n,
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

SAFE_TYPED_TEST(signal_test, swap_while_triggered,
{
  using traits = TypeParam;
  typename traits::template signal<void()> signal;

  if constexpr (traits::will_deadlock_if_recursively_modified)
    {
      this->store_test_result_other_failure();
      FAIL() << "This signal can't be accessed while triggered.";
    }

  if constexpr (!traits::has_swap)
    this->store_test_result_not_available();
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

SAFE_TYPED_TEST(signal_test, connections_of_swapped_signals,
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

SAFE_TYPED_TEST(signal_test, argument,
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

SAFE_TYPED_TEST(signal_test, recursive,
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

  traits::disconnect(signal, connection_1);

  traits::trigger(signal);
  EXPECT_EQ(3, calls);
})

static void output_padded
(std::ostream& output, const std::string& label, int left, int right)
{
  for (int i(0); i <= left; ++i)
    output << ' ';

  output << label;

  for (int i(0); i <= right; ++i)
    output << ' ';
}

static void output_centered
(std::ostream& output, const std::string& label, int width)
{
  const int label_size(label.size());
  assert(width >= label.size());

  const int left((width - label_size) / 2);
  const int right(width - label_size - left);

  output_padded(output, label, left, right);
}

static void output_left
(std::ostream& output, const std::string& label, int width)
{
  const int label_size(label.size());
  assert(width >= label.size());

  const int left(0);
  const int right(width - label_size);

  output_padded(output, label, left, right);
}

static const char* color_to_term(color_t color)
{
  switch (color)
    {
    case color_t::red:
      return "\033[31m";
    case color_t::magenta:
      return "\033[35m";
    case color_t::green:
      return "\033[32m";
    case color_t::yellow:
      return "\033[33m";
    case color_t::gray:
      return "\033[37m";
    case color_t::blue:
      return "\033[34m";
    }

  assert(false);
  return "\033[01;35m";
}

int main(int argc, char* argv[])
{
  // Jl_signal uses a static allocator for maximum performance
  constexpr const std::size_t C_JLSIGNAL_MAX = 256;
  jl::StaticSignalConnectionAllocator<C_JLSIGNAL_MAX> signal_con_allocator;
  jl::StaticObserverConnectionAllocator<C_JLSIGNAL_MAX> observer_con_allocator;
  jl::SignalBase::SetCommonConnectionAllocator(&signal_con_allocator);
  jl::SignalObserver::SetCommonConnectionAllocator(&observer_con_allocator);

  testing::InitGoogleTest(&argc, argv);
  const int result(RUN_ALL_TESTS());

  const auto& first_row(*g_results.begin());
  std::vector<int> widths;
  widths.reserve(first_row.second.size() + 1);
  widths.emplace_back(first_row.first.size());
  
  for (const auto& entry : first_row.second)
    widths.emplace_back(entry.first.size());

  for (const auto& row : g_results)
    {
      widths[0] = std::max<int>(widths[0], row.first.size());

      int i(1);
      for (const auto& entry : row.second)
        {
          widths[i] = std::max<int>(widths[i], entry.second.text.size());
          ++i;
        }
    }

  std::cout << '|';
  output_left(std::cout, "", widths[0]);
  std::cout << '|';

  int i(1);
  for (const auto& entry : first_row.second)
    {
      output_centered(std::cout, entry.first, widths[i]);
      std::cout << '|';
      ++i;
    }

  std::cout << '\n';

  for (const auto& row : g_results)
    {
      std::cout << '|';
      output_left(std::cout, row.first, widths[0]);
      std::cout << '|';
      
      i = 1;
      for (const auto& entry : row.second)
        {
          std::cout << color_to_term(entry.second.color);
          output_centered(std::cout, entry.second.text, widths[i]);
          std::cout << "\033[39;49m|";
          ++i;
        }

      std::cout << '\n';
    }
  
  return result;
}
