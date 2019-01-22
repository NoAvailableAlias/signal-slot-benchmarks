#include "../hpp/benchmark_yas.hpp"

NOINLINE(void Yas::initialize())
{
    // NOOP
}
NOINLINE(void Yas::validate_assert(std::size_t N))
{
    return Benchmark<Signal, Yas>::validation_assert(N);
}    
NOINLINE(double Yas::construction(std::size_t N))
{
    return Benchmark<Signal, Yas>::construction(N);
}
NOINLINE(double Yas::destruction(std::size_t N))
{
    return Benchmark<Signal, Yas>::destruction(N);
}
NOINLINE(double Yas::connection(std::size_t N))
{
    return Benchmark<Signal, Yas>::connection(N);
}
NOINLINE(double Yas::disconnect(std::size_t N))
{
    return Benchmark<Signal, Yas>::disconnect(N);
}
NOINLINE(double Yas::reconnect(std::size_t N))
{
    return Benchmark<Signal, Yas>::reconnect(N);
}
NOINLINE(double Yas::emission(std::size_t N))
{
    return Benchmark<Signal, Yas>::emission(N);
}
NOINLINE(double Yas::combined(std::size_t N))
{
    return Benchmark<Signal, Yas>::combined(N);
}
NOINLINE(double Yas::threaded(std::size_t N))
{
    // NOT IMPLEMENTED FOR THIS LIB
    return 0.0;
}
