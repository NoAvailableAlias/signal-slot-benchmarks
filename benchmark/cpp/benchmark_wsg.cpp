#include "../hpp/benchmark_wsg.hpp"

NOINLINE(void Wsg::initialize())
{
    // NOOP
}
NOINLINE(void Wsg::validate_assert(std::size_t N))
{
    return Benchmark<Signal, Wsg>::validation_assert(N);
}    
NOINLINE(double Wsg::construction(std::size_t N))
{
    return Benchmark<Signal, Wsg>::construction(N);
}
NOINLINE(double Wsg::destruction(std::size_t N))
{
    return Benchmark<Signal, Wsg>::destruction(N);
}
NOINLINE(double Wsg::connection(std::size_t N))
{
    return Benchmark<Signal, Wsg>::connection(N);
}
NOINLINE(double Wsg::disconnect(std::size_t N))
{
    return Benchmark<Signal, Wsg>::disconnect(N);
}
NOINLINE(double Wsg::reconnect(std::size_t N))
{
    return Benchmark<Signal, Wsg>::reconnect(N);
}
NOINLINE(double Wsg::emission(std::size_t N))
{
    return Benchmark<Signal, Wsg>::emission(N);
}
NOINLINE(double Wsg::combined(std::size_t N))
{
    return Benchmark<Signal, Wsg>::combined(N);
}
NOINLINE(double Wsg::threaded(std::size_t N))
{
    return Benchmark<Signal, Wsg>::threaded(N);
}
