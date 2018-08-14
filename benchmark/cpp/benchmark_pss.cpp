#include "../hpp/benchmark_pss.hpp"

NOINLINE(void Pss::initialize())
{
    // NOOP
}
NOINLINE(void Pss::validate_assert(std::size_t N))
{
    return Benchmark<Signal, Pss>::validation_assert(N);
}
NOINLINE(double Pss::construction(std::size_t N))
{
    return Benchmark<Signal, Pss>::construction(N);
}
NOINLINE(double Pss::destruction(std::size_t N))
{
    return Benchmark<Signal, Pss>::destruction(N);
}
NOINLINE(double Pss::connection(std::size_t N))
{
    return Benchmark<Signal, Pss>::connection(N);
}
NOINLINE(double Pss::emission(std::size_t N))
{
    return Benchmark<Signal, Pss>::emission(N);
}
NOINLINE(double Pss::combined(std::size_t N))
{
    return Benchmark<Signal, Pss>::combined(N);
}
NOINLINE(double Pss::threaded(std::size_t N))
{
    return Benchmark<Signal, Pss>::threaded(N);
}
