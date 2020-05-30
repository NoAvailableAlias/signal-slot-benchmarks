#include "../hpp/benchmark_vdk_st.hpp"

NOINLINE(void Vdk_st::initialize())
{
    // NOOP
}
NOINLINE(void Vdk_st::validate_assert(std::size_t N))
{
    return Benchmark<Signal, Vdk_st>::validation_assert(N);
}
NOINLINE(double Vdk_st::construction(std::size_t N, std::size_t limit))
{
    return Benchmark<Signal, Vdk_st>::construction(N, limit);
}
NOINLINE(double Vdk_st::destruction(std::size_t N, std::size_t limit))
{
    return Benchmark<Signal, Vdk_st>::destruction(N, limit);
}
NOINLINE(double Vdk_st::connection(std::size_t N, std::size_t limit))
{
    return Benchmark<Signal, Vdk_st>::connection(N, limit);
}
NOINLINE(double Vdk_st::disconnect(std::size_t N, std::size_t limit))
{
    return Benchmark<Signal, Vdk_st>::disconnect(N, limit);
}
NOINLINE(double Vdk_st::reconnect(std::size_t N, std::size_t limit))
{
    return Benchmark<Signal, Vdk_st>::reconnect(N, limit);
}
NOINLINE(double Vdk_st::emission(std::size_t N, std::size_t limit))
{
    return Benchmark<Signal, Vdk_st>::emission(N, limit);
}
NOINLINE(double Vdk_st::combined(std::size_t N, std::size_t limit))
{
    return Benchmark<Signal, Vdk_st>::combined(N, limit);
}
NOINLINE(double Vdk_st::threaded(std::size_t N, std::size_t limit))
{
    // NOT IMPLEMENTED FOR THIS LIB
    return 0.0;
}
