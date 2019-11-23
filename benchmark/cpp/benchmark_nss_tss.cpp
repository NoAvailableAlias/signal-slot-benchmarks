#include "../hpp/benchmark_nss_tss.hpp"

NOINLINE(void Nss_tss::initialize())
{
    // NOOP
}
NOINLINE(void Nss_tss::validate_assert(std::size_t N))
{
    return Benchmark<Signal, Nss_tss>::validation_assert(N);
}
NOINLINE(double Nss_tss::construction(std::size_t N))
{
    return Benchmark<Signal, Nss_tss>::construction(N);
}
NOINLINE(double Nss_tss::destruction(std::size_t N))
{
    return Benchmark<Signal, Nss_tss>::destruction(N);
}
NOINLINE(double Nss_tss::connection(std::size_t N))
{
    return Benchmark<Signal, Nss_tss>::connection(N);
}
NOINLINE(double Nss_tss::disconnect(std::size_t N))
{
    return Benchmark<Signal, Nss_tss>::disconnect(N);
}
NOINLINE(double Nss_tss::reconnect(std::size_t N))
{
    return Benchmark<Signal, Nss_tss>::reconnect(N);
}
NOINLINE(double Nss_tss::emission(std::size_t N))
{
    return Benchmark<Signal, Nss_tss>::emission(N);
}
NOINLINE(double Nss_tss::combined(std::size_t N))
{
    return Benchmark<Signal, Nss_tss>::combined(N);
}
NOINLINE(double Nss_tss::threaded(std::size_t N))
{
    return Benchmark<Signal, Nss_tss>::threaded(N);
}
