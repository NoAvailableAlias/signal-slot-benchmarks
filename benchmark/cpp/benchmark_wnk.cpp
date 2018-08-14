#include "../hpp/benchmark_wnk.hpp"

NOINLINE(void Wnk::initialize())
{
    // NOOP
}
NOINLINE(void Wnk::validate_assert(std::size_t N))
{
    return Benchmark<Signal, Wnk>::validation_assert(N);
}    
NOINLINE(double Wnk::construction(std::size_t N))
{
    return Benchmark<Signal, Wnk>::construction(N);
}
NOINLINE(double Wnk::destruction(std::size_t N))
{
    return Benchmark<Signal, Wnk>::destruction(N);
}
NOINLINE(double Wnk::connection(std::size_t N))
{
    return Benchmark<Signal, Wnk>::connection(N);
}
NOINLINE(double Wnk::emission(std::size_t N))
{
    return Benchmark<Signal, Wnk>::emission(N);
}
NOINLINE(double Wnk::combined(std::size_t N))
{
    return Benchmark<Signal, Wnk>::combined(N);
}
NOINLINE(double Wnk::threaded(std::size_t N))
{
    // NOT IMPLEMENTED FOR THIS LIB
    return 0.0;
}
