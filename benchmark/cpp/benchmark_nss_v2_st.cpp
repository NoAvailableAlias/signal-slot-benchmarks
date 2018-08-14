#include "../hpp/benchmark_nss_v2_st.hpp"

NOINLINE(void Nss_v2_st::initialize())
{
    // NOOP
}
NOINLINE(void Nss_v2_st::validate_assert(std::size_t N))
{
    return Benchmark<Signal, Nss_v2_st>::validation_assert(N);
}
NOINLINE(double Nss_v2_st::construction(std::size_t N))
{
    return Benchmark<Signal, Nss_v2_st>::construction(N);
}
NOINLINE(double Nss_v2_st::destruction(std::size_t N))
{
    return Benchmark<Signal, Nss_v2_st>::destruction(N);
}
NOINLINE(double Nss_v2_st::connection(std::size_t N))
{
    return Benchmark<Signal, Nss_v2_st>::connection(N);
}
NOINLINE(double Nss_v2_st::emission(std::size_t N))
{
    return Benchmark<Signal, Nss_v2_st>::emission(N);
}
NOINLINE(double Nss_v2_st::combined(std::size_t N))
{
    return Benchmark<Signal, Nss_v2_st>::combined(N);
}
NOINLINE(double Nss_v2_st::threaded(std::size_t N))
{
    // NOT IMPLEMENTED FOR THIS LIB
    return 0.0;
}
