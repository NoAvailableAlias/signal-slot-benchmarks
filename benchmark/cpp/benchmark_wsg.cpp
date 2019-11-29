#include "../hpp/benchmark_wsg.hpp"

NOINLINE(void Wsg::initialize())
{
    // NOOP
}
NOINLINE(void Wsg::validate_assert(std::size_t N))
{
    return Benchmark<Signal, Wsg>::validation_assert(N);
}    
NOINLINE(double Wsg::construction(std::size_t N, std::size_t limit))
{
    return Benchmark<Signal, Wsg>::construction(N, limit);
}
NOINLINE(double Wsg::destruction(std::size_t N, std::size_t limit))
{
    return Benchmark<Signal, Wsg>::destruction(N, limit);
}
NOINLINE(double Wsg::connection(std::size_t N, std::size_t limit))
{
    return Benchmark<Signal, Wsg>::connection(N, limit);
}
NOINLINE(double Wsg::disconnect(std::size_t N, std::size_t limit))
{
    return Benchmark<Signal, Wsg>::disconnect(N, limit);
}
NOINLINE(double Wsg::reconnect(std::size_t N, std::size_t limit))
{
    return Benchmark<Signal, Wsg>::reconnect(N, limit);
}
NOINLINE(double Wsg::emission(std::size_t N, std::size_t limit))
{
    return Benchmark<Signal, Wsg>::emission(N, limit);
}
NOINLINE(double Wsg::combined(std::size_t N, std::size_t limit))
{
    return Benchmark<Signal, Wsg>::combined(N, limit);
}
NOINLINE(double Wsg::threaded(std::size_t N, std::size_t limit))
{
    return Benchmark<Signal, Wsg>::threaded(N, limit);
}
