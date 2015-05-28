#ifndef BENCHMARK_WNK_HPP
#define BENCHMARK_WNK_HPP

#include "../lib/miguelmartin/Wink-Signals/wink/signal.hpp"
#include "../lib/miguelmartin/Wink-Signals/wink/slot.hpp"

#include "../../benchmark.hpp"

class Wnk
{
    SlotScope reg;

    NOINLINE(void handler(Rng& rng))
    {
        volatile std::size_t a = rng(); (void)a;
    }

    public:

    using Slot = wink::slot<void(Rng&)>;
    using Signal = wink::signal<Slot>;

    template <typename Subject, typename Foo>
    static void connect_method(Subject& subject, Foo& foo)
    {
        subject.connect(&foo, &Foo::handler);

        // Automatically disconnect when the foo instance is destroyed
        // Benchmarks require connection management
        foo.reg = make_slot_scope([&](void*) { subject.disconnect(&foo, &Foo::handler); });
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

#endif // BENCHMARK_WNK_HPP
