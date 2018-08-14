#pragma once

#include "../lib/yassi/yassi.h"

#include "../../benchmark.hpp"

class Yas
{
    SlotScope reg;

    NOINLINE(void handler(Rng& rng))
    {
        volatile std::size_t a = rng(); (void)a;
    }

    public:

    using Event = yassi::Signal<void(Rng&)>;
    using Signal = yassi::Emitter<Event>;

    template <typename Subject, typename Foo>
    static void connect_method(Subject& subject, Foo& foo)
    {
        subject.template connect<Event>(foo, &Foo::handler);

        // Automatically disconnect when the foo instance is destroyed
        // Benchmarks require connection management
        foo.reg = make_slot_scope([&](void*) { subject.disconnect(foo, &Foo::handler); });
    }
    template <typename Subject>
    static void emit_method(Subject& subject, Rng& rng)
    {
        subject.template emit<Event>(rng);
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

    static constexpr const char* C_LIB_NAME = "Yassi";
    static constexpr const char* C_LIB_SOURCE_URL = "http://www.codeproject.com/Articles/867044/Yassi-Yet-Another-Signal-Slot-Implementation";
    static constexpr const char* C_LIB_FILE = "benchmark_yas";
    static constexpr const char* C_LIB_IS_HEADER_ONLY = "X";
    static constexpr const char* C_LIB_DATA_STRUCTURE = "std::vector";
    static constexpr const char* C_LIB_IS_THREAD_SAFE = "-";
};
