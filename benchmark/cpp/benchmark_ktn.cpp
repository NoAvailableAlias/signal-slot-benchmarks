#include "../hpp/benchmark_ktn.hpp"

NOINLINE(void Ktnss::initialize())
{
    // NOOP
}
NOINLINE(void Ktnss::validate_assert(std::size_t N))
{
    return Benchmark<Signal, Ktnss>::validation_assert(N);
}
NOINLINE(double Ktnss::construction(std::size_t N, std::size_t limit))
{
    return Benchmark<Signal, Ktnss>::construction(N, limit);
}
NOINLINE(double Ktnss::destruction(std::size_t N, std::size_t limit))
{
    return Benchmark<Signal, Ktnss>::destruction(N, limit);
}
NOINLINE(double Ktnss::connection(std::size_t N, std::size_t limit))
{
    return Benchmark<Signal, Ktnss>::connection(N, limit);
}
NOINLINE(double Ktnss::disconnect(std::size_t N, std::size_t limit))
{
    return Benchmark<Signal, Ktnss>::disconnect(N, limit);
}
NOINLINE(double Ktnss::reconnect(std::size_t N, std::size_t limit))
{
    return Benchmark<Signal, Ktnss>::reconnect(N, limit);
}
NOINLINE(double Ktnss::emission(std::size_t N, std::size_t limit))
{
    return Benchmark<Signal, Ktnss>::emission(N, limit);
}
NOINLINE(double Ktnss::combined(std::size_t N, std::size_t limit))
{
    return Benchmark<Signal, Ktnss>::combined(N, limit);
}
NOINLINE(double Ktnss::threaded(std::size_t N, std::size_t limit))
{
    return Benchmark<Signal, Ktnss>::threaded(N, limit);
}
