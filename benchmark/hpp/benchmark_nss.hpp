#pragma once

#include "../lib/NoAvailableAlias/nano-signal-slot/nano_signal_slot.hpp"

#include "../../benchmark.hpp"

class Nss : public Nano_Deprecated::Observer
{
    NOINLINE(void handler(Rng& rng))
    {
        volatile std::size_t a = rng(); (void)a;
    }

    public:

    using Signal = Nano_Deprecated::Signal<void(Rng&)>;

    template <typename Subject, typename Foo>
    static void connect_method(Subject& subject, Foo& foo)
    {
        subject.connect<Foo, &Foo::handler>(foo);
    }
    template <typename Subject>
    static void emit_method(Subject& subject, Rng& rng)
    {
        subject.emit(rng);
    }

    // Used for switching policies at runtime
    static void initialize();

    static void validate_assert(std::size_t);
    static double construction(std::size_t);
    static double destruction(std::size_t);
    static double connection(std::size_t);
    static double emission(std::size_t);
    static double combined(std::size_t);

    // NOT IMPLEMENTED FOR THIS LIB
    static double threaded(std::size_t);

    static constexpr const char* C_LIB_NAME = "nano-signal-slot v1x";
    static constexpr const char* C_LIB_SOURCE_URL = "https://github.com/NoAvailableAlias/nano-signal-slot";
    static constexpr const char* C_LIB_FILE = "benchmark_nss";
    static constexpr const char* C_LIB_IS_HEADER_ONLY = "X";
    static constexpr const char* C_LIB_DATA_STRUCTURE = "singly linked list";
    static constexpr const char* C_LIB_IS_THREAD_SAFE = "-";
};
