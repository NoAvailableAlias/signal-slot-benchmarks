#include "../hpp/benchmark_nss.hpp"

const char* Nss::LibraryName = "nano-signal-slot v1x";

NOINLINE(void Nss::validate_assert(std::size_t N))
{
    return Benchmark<Signal, Nss>::validation_assert(N);
}    
NOINLINE(double Nss::construction(std::size_t N))
{
    return Benchmark<Signal, Nss>::construction(N);
}
NOINLINE(double Nss::destruction(std::size_t N))
{
    return Benchmark<Signal, Nss>::destruction(N);
}
NOINLINE(double Nss::connection(std::size_t N))
{
    return Benchmark<Signal, Nss>::connection(N);
}
NOINLINE(double Nss::emission(std::size_t N))
{
    return Benchmark<Signal, Nss>::emission(N);
}
NOINLINE(double Nss::combined(std::size_t N))
{
    return Benchmark<Signal, Nss>::combined(N);
}
NOINLINE(double Nss::threaded(std::size_t N))
{
    return 0.0;
}
