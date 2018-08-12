#ifndef BENCHMARK_NSS_V2_HPP
#define BENCHMARK_NSS_V2_HPP

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

#endif // BENCHMARK_NSS_V2_HPP
