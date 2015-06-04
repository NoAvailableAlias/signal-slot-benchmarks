#ifndef BENCHMARK_SPP_HPP
#define BENCHMARK_SPP_HPP

#include "../lib/martinkallman/signals-cpp/include/signal.h"

#include "../../benchmark.hpp"

// FAILS ASSERT

class Spp
{
    SlotScope reg;

    NOINLINE(void handler(Rng& rng))
    {
        volatile std::size_t a = rng(); (void)a;
    }

    public:
    
    // Had to add namespace, always namespace
    using SignalType = SPP::Signal<Rng&>;

    template <typename Subject, typename Foo>
    static void connect_method(Subject& subject, Foo& foo)
    {
        auto reg = subject.Add(
            std::bind(&Foo::handler, &foo, std::placeholders::_1)
        );
        // Automatically disconnect when the foo instance is destroyed
        // Benchmarks require connection management
        foo.reg = make_slot_scope([reg, &subject](void*) { subject.Remove(reg); });
    }
    template <typename Subject>
    static void emit_method(Subject& subject, Rng& rng)
    {
        subject(rng);
    }

    static void validate_assert(std::size_t);
    static double construction(std::size_t);
    static double destruction(std::size_t);
    static double connection(std::size_t);
    static double emission(std::size_t);
    static double combined(std::size_t);

    static const char* LibraryName;
};

#endif // BENCHMARK_SPP_HPP