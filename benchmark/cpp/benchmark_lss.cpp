#include "../hpp/benchmark_lss.hpp"

NOINLINE(void Lss::initialize())
{
    // NOOP
}
NOINLINE(void Lss::validate_assert(std::size_t N))
{
    return Benchmark<Signal, Lss>::validation_assert(N);
}
NOINLINE(double Lss::construction(std::size_t N))
{
    return Benchmark<Signal, Lss>::construction(N);
}
NOINLINE(double Lss::destruction(std::size_t N))
{
    return Benchmark<Signal, Lss>::destruction(N);
}
NOINLINE(double Lss::connection(std::size_t N))
{
    return Benchmark<Signal, Lss>::connection(N);
}
NOINLINE(double Lss::emission(std::size_t N))
{
    return Benchmark<Signal, Lss>::emission(N);
}
NOINLINE(double Lss::combined(std::size_t N))
{
    return Benchmark<Signal, Lss>::combined(N);
}
NOINLINE(double Lss::threaded(std::size_t N))
{
    // NOT IMPLEMENTED FOR THIS LIB
    return 0.0;
}
