#include "../hpp/benchmark_nss_sts.hpp"

NOINLINE(void Nss_sts::initialize())
{
    // NOOP
}
NOINLINE(void Nss_sts::validate_assert(std::size_t N))
{
    return Benchmark<Signal, Nss_sts>::validation_assert(N);
}
NOINLINE(double Nss_sts::construction(std::size_t N))
{
    return Benchmark<Signal, Nss_sts>::construction(N);
}
NOINLINE(double Nss_sts::destruction(std::size_t N))
{
    return Benchmark<Signal, Nss_sts>::destruction(N);
}
NOINLINE(double Nss_sts::connection(std::size_t N))
{
    return Benchmark<Signal, Nss_sts>::connection(N);
}
NOINLINE(double Nss_sts::disconnect(std::size_t N))
{
    return Benchmark<Signal, Nss_sts>::disconnect(N);
}
NOINLINE(double Nss_sts::reconnect(std::size_t N))
{
    return Benchmark<Signal, Nss_sts>::reconnect(N);
}
NOINLINE(double Nss_sts::emission(std::size_t N))
{
    return Benchmark<Signal, Nss_sts>::emission(N);
}
NOINLINE(double Nss_sts::combined(std::size_t N))
{
    return Benchmark<Signal, Nss_sts>::combined(N);
}
NOINLINE(double Nss_sts::threaded(std::size_t N))
{
    // NOT IMPLEMENTED FOR THIS LIB
    return 0.0;
}
