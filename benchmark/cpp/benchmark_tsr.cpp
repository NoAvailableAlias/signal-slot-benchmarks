#include "../hpp/benchmark_tsr.hpp"

NOINLINE(void Tsr::initialize())
{
    // NOOP
}
NOINLINE(void Tsr::validate_assert(std::size_t N))
{
    return Benchmark<Signal, Tsr>::validation_assert(N);
}
NOINLINE(double Tsr::construction(std::size_t N, std::size_t limit))
{
    return Benchmark<Signal, Tsr>::construction(N, limit);
}
NOINLINE(double Tsr::destruction(std::size_t N, std::size_t limit))
{
    return Benchmark<Signal, Tsr>::destruction(N, limit);
}
NOINLINE(double Tsr::connection(std::size_t N, std::size_t limit))
{
    return Benchmark<Signal, Tsr>::connection(N, limit);
}
NOINLINE(double Tsr::disconnect(std::size_t N, std::size_t limit))
{
    return Benchmark<Signal, Tsr>::disconnect(N, limit);
}
NOINLINE(double Tsr::reconnect(std::size_t N, std::size_t limit))
{
    return Benchmark<Signal, Tsr>::reconnect(N, limit);
}
NOINLINE(double Tsr::emission(std::size_t N, std::size_t limit))
{
    return Benchmark<Signal, Tsr>::emission(N, limit);
}
NOINLINE(double Tsr::combined(std::size_t N, std::size_t limit))
{
    return Benchmark<Signal, Tsr>::combined(N, limit);
}
NOINLINE(double Tsr::threaded(std::size_t N, std::size_t limit))
{
    return Benchmark<Signal, Tsr>::threaded(N, limit);
}
