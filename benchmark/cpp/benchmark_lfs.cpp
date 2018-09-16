#include "../hpp/benchmark_lfs.hpp"

NOINLINE(void Lfs::initialize())
{
    // NOOP
}
NOINLINE(void Lfs::validate_assert(std::size_t N))
{
    return Benchmark<Signal, Lfs>::validation_assert(N);
}
NOINLINE(double Lfs::construction(std::size_t N))
{
    return Benchmark<Signal, Lfs>::construction(N);
}
NOINLINE(double Lfs::destruction(std::size_t N))
{
    return Benchmark<Signal, Lfs>::destruction(N);
}
NOINLINE(double Lfs::connection(std::size_t N))
{
    return Benchmark<Signal, Lfs>::connection(N);
}
NOINLINE(double Lfs::emission(std::size_t N))
{
    return Benchmark<Signal, Lfs>::emission(N);
}
NOINLINE(double Lfs::combined(std::size_t N))
{
    return Benchmark<Signal, Lfs>::combined(N);
}
NOINLINE(double Lfs::threaded(std::size_t N))
{
    return Benchmark<Signal, Lfs>::threaded(N);
}
