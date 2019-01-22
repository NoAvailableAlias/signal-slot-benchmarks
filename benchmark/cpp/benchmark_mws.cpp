#include "../hpp/benchmark_mws.hpp"

NOINLINE(void Mws::initialize())
{
    // NOOP
}
NOINLINE(void Mws::validate_assert(std::size_t N))
{
    return Benchmark<Signal, Mws>::validation_assert(N);
}    
NOINLINE(double Mws::construction(std::size_t N))
{
    return Benchmark<Signal, Mws>::construction(N);
}
NOINLINE(double Mws::destruction(std::size_t N))
{
    return Benchmark<Signal, Mws>::destruction(N);
}
NOINLINE(double Mws::connection(std::size_t N))
{
    return Benchmark<Signal, Mws>::connection(N);
}
NOINLINE(double Mws::disconnect(std::size_t N))
{
    return Benchmark<Signal, Mws>::disconnect(N);
}
NOINLINE(double Mws::reconnect(std::size_t N))
{
    return Benchmark<Signal, Mws>::reconnect(N);
}
NOINLINE(double Mws::emission(std::size_t N))
{
    return Benchmark<Signal, Mws>::emission(N);
}
NOINLINE(double Mws::combined(std::size_t N))
{
    return Benchmark<Signal, Mws>::combined(N);
}
NOINLINE(double Mws::threaded(std::size_t N))
{
    // NOT IMPLEMENTED FOR THIS LIB
    return 0.0;
}
