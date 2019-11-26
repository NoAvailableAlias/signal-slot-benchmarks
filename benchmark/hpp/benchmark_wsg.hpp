#pragma once

#include "../lib/winglot/Signals/W_Signal.h"

#include "../../benchmark.hpp"

class Wsg : public W::Slot
{
    NOINLINE(void handler(Rng& rng))
    {
        volatile std::size_t a = rng(); (void)a;
    }

    public:

    using Signal = W::Signal<Rng&>;

    template <typename Subject, typename Foo>
    static void connect_method(Subject& subject, Foo& foo)
    {
        subject.connect(&foo, &Foo::handler);
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
    static double disconnect(std::size_t);
    static double reconnect(std::size_t);
    static double emission(std::size_t);
    static double combined(std::size_t);
    static double threaded(std::size_t);

    static constexpr const char* C_LIB_NAME = "! winglot Signals";
    static constexpr const char* C_LIB_SOURCE_URL = "https://github.com/winglot/Signals";
    static constexpr const char* C_LIB_FILE = "benchmark_wsg";
    static constexpr const char* C_LIB_IS_HEADER_ONLY = "-";
    static constexpr const char* C_LIB_DATA_STRUCTURE = "**std::list";
    static constexpr const char* C_LIB_IS_THREAD_SAFE = "!";

    static constexpr const std::size_t C_LIB_SIZEOF_SIGNAL = sizeof(Signal);
};
