#ifndef BENCHMARK_VDK_HPP
#define BENCHMARK_VDK_HPP

#define VDK_SIGNALS_LITE
#include "../lib/vdksoft/signals/include/signals.h"

#include "../../benchmark.hpp"

class Vdk
{
    NOINLINE(void handler(Rng& rng))
    {
        volatile std::size_t a = rng(); (void)a;
    }

    SlotScope reg;

    public:

    using Signal = vdk::signal<void(Rng&)>;

    template <typename Subject, typename Foo>
    static void connect_method(Subject& subject, Foo& foo)
    {
        subject.connect(&foo, &Foo::handler);
        foo.reg = make_slot_scope([&subject, &foo](void*){subject.disconnect(&foo, &Foo::handler);});
    }
    template <typename Subject>
    static void emit_method(Subject& subject, Rng& rng)
    {
        subject.emit(rng);
    }

    static void validate_assert(std::size_t);
    static double construction(std::size_t);
    static double destruction(std::size_t);
    static double connection(std::size_t);
    static double emission(std::size_t);
    static double combined(std::size_t);

    // This may or may not be implemented
    static double threaded(std::size_t);

    static const char* LibraryName;
};

#endif // BENCHMARK_VDK_HPP
