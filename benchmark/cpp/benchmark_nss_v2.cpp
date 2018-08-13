#include "../hpp/benchmark_nss_v2.hpp"

NOINLINE(void Nss_v2::validate_assert(std::size_t N))
{
    return Benchmark<Signal, Nss_v2>::validation_assert(N);
}
NOINLINE(double Nss_v2::construction(std::size_t N))
{
    return Benchmark<Signal, Nss_v2>::construction(N);
}
NOINLINE(double Nss_v2::destruction(std::size_t N))
{
    return Benchmark<Signal, Nss_v2>::destruction(N);
}
NOINLINE(double Nss_v2::connection(std::size_t N))
{
    return Benchmark<Signal, Nss_v2>::connection(N);
}
NOINLINE(double Nss_v2::emission(std::size_t N))
{
    return Benchmark<Signal, Nss_v2>::emission(N);
}
NOINLINE(double Nss_v2::combined(std::size_t N))
{
    return Benchmark<Signal, Nss_v2>::combined(N);
}
NOINLINE(double Nss_v2::threaded(std::size_t N))
{
    return Benchmark<Signal, Nss_v2>::threaded(N);
}
