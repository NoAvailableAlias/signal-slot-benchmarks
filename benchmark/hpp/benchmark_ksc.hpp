#ifndef BENCHMARK_KSC_HPP
#define BENCHMARK_KSC_HPP

#include "../lib/Kosta-Github/signals-cpp/signals-cpp/signals.hpp"

#include "../../benchmark.hpp"

class Ksc
{
    sigs::connection reg;

    NOINLINE(void handler(Rng& rng))
    {
        volatile std::size_t a = rng(); (void)a;
    }

    public:

    Ksc() = default;
    ~Ksc() { reg.disconnect(); }

    using Signal = sigs::signal<void(Rng&)>;

    template <typename Subject, typename Foo>
    static void connect_method(Subject& subject, Foo& foo)
    {
        //foo.reg = subject.connect(std::bind(&Foo::handler, &foo, std::placeholders::_1));
        foo.reg = subject.connect(&foo, &Foo::handler);
    }
    template <typename Subject>
    static void emit_method(Subject& subject, Rng& rng)
    {
        subject.fire(rng);
    }

    static void validate_assert(std::size_t);
    static double construction(std::size_t);
    static double destruction(std::size_t);
    static double connection(std::size_t);
    static double emission(std::size_t);
    static double combined(std::size_t);

    static const char* LibraryName;
};

#endif // BENCHMARK_KSC_HPP
