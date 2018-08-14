#include "../hpp/benchmark_jos.hpp"

NOINLINE(void Jos::initialize())
{
    // NOOP
}
NOINLINE(void Jos::validate_assert(std::size_t N))
{
    return Benchmark<Signal, Jos>::validation_assert(N);
}    
NOINLINE(double Jos::construction(std::size_t N))
{
    return Benchmark<Signal, Jos>::construction(N);
}
NOINLINE(double Jos::destruction(std::size_t N))
{
    return Benchmark<Signal, Jos>::destruction(N);
}
NOINLINE(double Jos::connection(std::size_t N))
{
    return Benchmark<Signal, Jos>::connection(N);
}
NOINLINE(double Jos::emission(std::size_t N))
{
    return Benchmark<Signal, Jos>::emission(N);
}
NOINLINE(double Jos::combined(std::size_t N))
{
    return Benchmark<Signal, Jos>::combined(N);
}
NOINLINE(double Jos::threaded(std::size_t N))
{
    // NOT IMPLEMENTED FOR THIS LIB
    return 0.0;
}
