#pragma once

#include "../lib/vdksoft/signals/src/signals.h"

#include "../../benchmark.hpp"

class Vdk : public vdk::context
{
    NOINLINE(void handler(Rng& rng))
    {
        volatile std::size_t a = rng(); (void)a;
    }

    public:

    using Signal = vdk::signal<void(Rng&)>;

    template <typename Subject, typename Foo>
    static void connect_method(Subject& subject, Foo& foo)
    {
        // msvc generates "Error C2039 'generate': is not a member of 'std::tuple<Rng &>'
        // some sort of issue with the threadsafe command / target codepath
        //subject.connect(&foo, &Foo::handler);
    }
    template <typename Subject>
    static void emit_method(Subject& subject, Rng& rng)
    {
        subject.emit(rng);
    }

    // Used for switching policies at runtime
    static void initialize();

    static void validate_assert(std::size_t);
    static double construction(std::size_t, std::size_t);
    static double destruction(std::size_t, std::size_t);
    static double connection(std::size_t, std::size_t);
    static double disconnect(std::size_t, std::size_t);
    static double reconnect(std::size_t, std::size_t);
    static double emission(std::size_t, std::size_t);
    static double combined(std::size_t, std::size_t);

    // NOT IMPLEMENTED FOR THIS LIB
    static double threaded(std::size_t, std::size_t);

    static constexpr const char* C_LIB_NAME = "* vdksoft signals";
    static constexpr const char* C_LIB_SOURCE_URL = "https://github.com/vdksoft/signals";
    static constexpr const char* C_LIB_FILE = "benchmark_vdk";
    static constexpr const char* C_LIB_IS_HEADER_ONLY = "-";
    static constexpr const char* C_LIB_DATA_STRUCTURE = "doubly linked list";
    static constexpr const char* C_LIB_IS_THREAD_SAFE = "X";

    static constexpr const std::size_t C_LIB_SIZEOF_SIGNAL = sizeof(Signal);
};
