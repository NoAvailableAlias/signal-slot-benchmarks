#pragma once

#define BOOST_SIGNALS_NO_DEPRECATION_WARNING

#include <boost/signals.hpp>

#include "../../benchmark.hpp"

class Bs1 : public boost::signals::trackable
{
    NOINLINE(void handler(Rng& rng))
    {
        volatile std::size_t a = rng(); (void)a;
    }

    public:

    using Signal = boost::signal<void(Rng&)>;

    template <typename Subject, typename Foo>
    static void connect_method(Subject& subject, Foo& foo)
    {
        subject.connect(std::bind(&Foo::handler, &foo, std::placeholders::_1));
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

    // NOT IMPLEMENTED FOR THIS LIB
    static double threaded(std::size_t);

    // The following is used for report outputs
    static constexpr const char* C_LIB_NAME = "Boost Signals";
    static constexpr const char* C_LIB_SOURCE_URL = "http://www.boost.org/doc/libs/1_56_0/doc/html/signals.html";
    static constexpr const char* C_LIB_FILE = "benchmark_bs1";
    static constexpr const char* C_LIB_IS_HEADER_ONLY = "-";
    static constexpr const char* C_LIB_DATA_STRUCTURE = "?";
    static constexpr const char* C_LIB_IS_THREAD_SAFE = "-";
};
