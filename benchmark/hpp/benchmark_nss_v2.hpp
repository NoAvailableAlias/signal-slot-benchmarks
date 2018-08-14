#pragma once

#include <mutex>

#include "../lib/NoAvailableAlias/nano-signal-slot-v2x/nano_signal_slot.hpp"
#include "../lib/NoAvailableAlias/nano-signal-slot-v2x/nano_mutex.hpp"

#include "../../benchmark.hpp"

namespace
{
    //using Mutex = std::recursive_mutex;
    using Mutex = Nano::Recursive_Mutex;
}

class Nss_v2 : public Nano::Observer<Mutex>
{
    NOINLINE(void handler(Rng& rng))
    {
        volatile std::size_t a = rng(); (void)a;
    }

    public:

    using Signal = Nano::Signal<void(Rng&), Mutex>;

    template <typename Subject, typename Foo>
    static void connect_method(Subject& subject, Foo& foo)
    {
        subject.connect<&Foo::handler>(foo);
    }
    template <typename Subject>
    static void emit_method(Subject& subject, Rng& rng)
    {
        subject.fire(rng);
    }

    // Used for switching policies at runtime
    static void initialize();

    static void validate_assert(std::size_t);
    static double construction(std::size_t);
    static double destruction(std::size_t);
    static double connection(std::size_t);
    static double emission(std::size_t);
    static double combined(std::size_t);
    static double threaded(std::size_t);

    static constexpr const char* C_LIB_NAME = "* nano-signal-slot v2x";
    static constexpr const char* C_LIB_SOURCE_URL = "https://github.com/NoAvailableAlias/nano-signal-slot/tree/rework";
    static constexpr const char* C_LIB_FILE = "benchmark_nss_v2";
    static constexpr const char* C_LIB_IS_HEADER_ONLY = "X";
    static constexpr const char* C_LIB_DATA_STRUCTURE = "std::forward_list";
    static constexpr const char* C_LIB_IS_THREAD_SAFE = "X";
};
