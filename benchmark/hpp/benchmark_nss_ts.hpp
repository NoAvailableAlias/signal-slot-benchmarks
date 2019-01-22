#pragma once

#include "../lib/NoAvailableAlias/nano-signal-slot/nano_signal_slot.hpp"
#include "../lib/NoAvailableAlias/nano-signal-slot/nano_mutex.hpp"

#include "../../benchmark.hpp"

class Nss_ts : public Nano::Observer<Nano::TS_Policy<>>
{
    NOINLINE(void handler(Rng& rng))
    {
        volatile std::size_t a = rng(); (void)a;
    }

    public:

    using Signal = Nano::Signal<void(Rng&), Nano::TS_Policy<>>;

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
    static double disconnect(std::size_t);
    static double reconnect(std::size_t);
    static double emission(std::size_t);
    static double combined(std::size_t);
    static double threaded(std::size_t);

    static constexpr const char* C_LIB_NAME = "* nano-signal-slot ts";
    static constexpr const char* C_LIB_SOURCE_URL = "https://github.com/NoAvailableAlias/nano-signal-slot/tree/rework";
    static constexpr const char* C_LIB_FILE = "benchmark_nss_ts";
    static constexpr const char* C_LIB_IS_HEADER_ONLY = "X";
    static constexpr const char* C_LIB_DATA_STRUCTURE = "std::vector";
    static constexpr const char* C_LIB_IS_THREAD_SAFE = "X";
};