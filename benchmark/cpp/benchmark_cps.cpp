#include "../hpp/benchmark_cps.hpp"

NOINLINE(void Cps::initialize())
{
    // NOOP
}
NOINLINE(void Cps::validate_assert(std::size_t N))
{
    return Benchmark<Signal, Cps>::validation_assert(N);
}    
NOINLINE(double Cps::construction(std::size_t N))
{
    return Benchmark<Signal, Cps>::construction(N);
}
NOINLINE(double Cps::destruction(std::size_t N))
{
    return Benchmark<Signal, Cps>::destruction(N);
}
NOINLINE(double Cps::connection(std::size_t N))
{
    return Benchmark<Signal, Cps>::connection(N);
}
NOINLINE(double Cps::emission(std::size_t N))
{
    return Benchmark<Signal, Cps>::emission(N);
}
NOINLINE(double Cps::combined(std::size_t N))
{
    return Benchmark<Signal, Cps>::combined(N);
}
NOINLINE(double Cps::threaded(std::size_t N))
{
    return Benchmark<Signal, Cps>::threaded(N);
}
