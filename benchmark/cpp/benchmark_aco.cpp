#include "../hpp/benchmark_aco.hpp"

NOINLINE(void Aco::initialize())
{
    // NOOP
}
NOINLINE(void Aco::validate_assert(std::size_t N))
{
    return Benchmark<Signal, Aco>::validation_assert(N);
}
NOINLINE(double Aco::construction(std::size_t N))
{
    return Benchmark<Signal, Aco>::construction(N);
}
NOINLINE(double Aco::destruction(std::size_t N))
{
    return Benchmark<Signal, Aco>::destruction(N);
}
NOINLINE(double Aco::connection(std::size_t N))
{
    return Benchmark<Signal, Aco>::connection(N);
}
NOINLINE(double Aco::emission(std::size_t N))
{
    return Benchmark<Signal, Aco>::emission(N);
}
NOINLINE(double Aco::combined(std::size_t N))
{
    return Benchmark<Signal, Aco>::combined(N);
}
NOINLINE(double Aco::threaded(std::size_t N))
{
    // NOT IMPLEMENTED FOR THIS LIB
    return 0.0;
}
