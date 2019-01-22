#include "../hpp/benchmark_sss.hpp"

NOINLINE(void Sss::initialize())
{
    // NOOP
}
NOINLINE(void Sss::validate_assert(std::size_t N))
{
    return Benchmark<Signal, Sss>::validation_assert(N);
}    
NOINLINE(double Sss::construction(std::size_t N))
{
    return Benchmark<Signal, Sss>::construction(N);
}
NOINLINE(double Sss::destruction(std::size_t N))
{
    return Benchmark<Signal, Sss>::destruction(N);
}
NOINLINE(double Sss::connection(std::size_t N))
{
    return Benchmark<Signal, Sss>::connection(N);
}
NOINLINE(double Sss::disconnect(std::size_t N))
{
    return Benchmark<Signal, Sss>::disconnect(N);
}
NOINLINE(double Sss::reconnect(std::size_t N))
{
    return Benchmark<Signal, Sss>::reconnect(N);
}
NOINLINE(double Sss::emission(std::size_t N))
{
    return Benchmark<Signal, Sss>::emission(N);
}
NOINLINE(double Sss::combined(std::size_t N))
{
    return Benchmark<Signal, Sss>::combined(N);
}
NOINLINE(double Sss::threaded(std::size_t N))
{
    // NOT IMPLEMENTED FOR THIS LIB
    return 0.0;
}
