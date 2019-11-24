#include "../hpp/benchmark_nes.hpp"

NOINLINE(void Nes::initialize())
{
    // NOOP
}
NOINLINE(void Nes::validate_assert(std::size_t N))
{
    return Benchmark<Signal, Nes>::validation_assert(N);
}
NOINLINE(double Nes::construction(std::size_t N))
{
    return Benchmark<Signal, Nes>::construction(N);
}
NOINLINE(double Nes::destruction(std::size_t N))
{
    return Benchmark<Signal, Nes>::destruction(N);
}
NOINLINE(double Nes::connection(std::size_t N))
{
    return Benchmark<Signal, Nes>::connection(N);
}
NOINLINE(double Nes::disconnect(std::size_t N))
{
    return Benchmark<Signal, Nes>::disconnect(N);
}
NOINLINE(double Nes::reconnect(std::size_t N))
{
    return Benchmark<Signal, Nes>::reconnect(N);
}
NOINLINE(double Nes::emission(std::size_t N))
{
    return Benchmark<Signal, Nes>::emission(N);
}
NOINLINE(double Nes::combined(std::size_t N))
{
    return Benchmark<Signal, Nes>::combined(N);
}
NOINLINE(double Nes::threaded(std::size_t N))
{
    // NOT IMPLEMENTED FOR THIS LIB
    return 0.0;
}
