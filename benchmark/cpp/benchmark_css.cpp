#include "../hpp/benchmark_css.hpp"

NOINLINE(void Css::initialize())
{
    // NOOP
}
NOINLINE(void Css::validate_assert(std::size_t N))
{
    return Benchmark<Signal, Css>::validation_assert(N);
}
NOINLINE(double Css::construction(std::size_t N))
{
    return Benchmark<Signal, Css>::construction(N);
}
NOINLINE(double Css::destruction(std::size_t N))
{
    return Benchmark<Signal, Css>::destruction(N);
}
NOINLINE(double Css::connection(std::size_t N))
{
    return Benchmark<Signal, Css>::connection(N);
}
NOINLINE(double Css::emission(std::size_t N))
{
    return Benchmark<Signal, Css>::emission(N);
}
NOINLINE(double Css::combined(std::size_t N))
{
    return Benchmark<Signal, Css>::combined(N);
}
NOINLINE(double Css::threaded(std::size_t N))
{
    return Benchmark<Signal, Css>::threaded(N);
}
