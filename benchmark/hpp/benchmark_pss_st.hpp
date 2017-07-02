#ifndef BENCHMARK_PSS_ST_HPP
#define BENCHMARK_PSS_ST_HPP

#include "../lib/palacaze/sigslot/signal.hpp"

#include "../../benchmark.hpp"

class Pss_st
{
    sigslot::scoped_connection conn;

    NOINLINE(void handler(Rng& rng))
    {
        volatile std::size_t a = rng(); (void)a;
    }

    public:

    using Signal = sigslot::signal_st<Rng&>;

    template <typename Subject, typename Foo>
    static void connect_method(Subject& subject, Foo& foo)
    {
        foo.conn = subject.connect_scoped(&Foo::handler, &foo);
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

    // This may or may not be implemented
    static double threaded(std::size_t);

    static const char* LibraryName;
};

#endif // BENCHMARK_PSS_ST_HPP
