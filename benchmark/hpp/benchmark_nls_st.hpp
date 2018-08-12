#pragma once

#include "../lib/i42output/include/neolib/signal.hpp"
#include "../lib/i42output/include/neolib/slot.hpp"

#include "../../benchmark.hpp"

class Nls_st : public neolib::has_slots<neolib::locking_policy_none>
{
    NOINLINE(void handler(Rng& rng))
    {
        volatile std::size_t a = rng(); (void)a;
    }

    public:

    using Signal = neolib::signal<void(Rng&), neolib::locking_policy_none>;

    template <typename Subject, typename Foo>
    static void connect_method(Subject& subject, Foo& foo)
    {
        subject(foo, &Foo::handler);
    }
    template <typename Subject>
    static void emit_method(Subject& subject, Rng& rng)
    {
        subject.trigger(rng);
    }

    static void validate_assert(std::size_t);
    static double construction(std::size_t);
    static double destruction(std::size_t);
    static double connection(std::size_t);
    static double emission(std::size_t);
    static double combined(std::size_t);

    // NOT IMPLEMENTED FOR THIS LIB
    static double threaded(std::size_t);

    static constexpr const char* C_LIB_NAME = "neolib signal";
    static constexpr const char* C_LIB_SOURCE_URL = "https://github.com/i42output/neolib";
    static constexpr const char* C_LIB_FILE = "benchmark_nls_st";
    static constexpr const char* C_LIB_IS_HEADER_ONLY = "X";
    static constexpr const char* C_LIB_DATA_STRUCTURE = "*std::unordered_map";
    static constexpr const char* C_LIB_IS_THREAD_SAFE = "-";
};
