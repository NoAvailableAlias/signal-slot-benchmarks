#include "../hpp/benchmark_evl.hpp"

NOINLINE(void Evl::initialize())
{
    // NOOP
}
NOINLINE(void Evl::validate_assert(std::size_t N))
{
    return Benchmark<Signal, Evl>::validation_assert(N);
}    
NOINLINE(double Evl::construction(std::size_t N))
{
    return Benchmark<Signal, Evl>::construction(N);
}
NOINLINE(double Evl::destruction(std::size_t N))
{
    return Benchmark<Signal, Evl>::destruction(N);
}
NOINLINE(double Evl::connection(std::size_t N))
{
    return Benchmark<Signal, Evl>::connection(N);
}
NOINLINE(double Evl::disconnect(std::size_t N))
{
    return Benchmark<Signal, Evl>::disconnect(N);
}
NOINLINE(double Evl::reconnect(std::size_t N))
{
    return Benchmark<Signal, Evl>::reconnect(N);
}
NOINLINE(double Evl::emission(std::size_t N))
{
    return Benchmark<Signal, Evl>::emission(N);
}
NOINLINE(double Evl::combined(std::size_t N))
{
    return Benchmark<Signal, Evl>::combined(N);
}
NOINLINE(double Evl::threaded(std::size_t N))
{
    // NOT IMPLEMENTED FOR THIS LIB
    return 0.0;
}
