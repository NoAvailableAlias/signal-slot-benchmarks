#include "../hpp/benchmark_bs2_st.hpp"

NOINLINE(void Bs2_st::initialize())
{
    // NOOP
}
NOINLINE(void Bs2_st::validate_assert(std::size_t N))
{
    return Benchmark<Signal, Bs2_st>::validation_assert(N);
}    
NOINLINE(double Bs2_st::construction(std::size_t N, std::size_t limit))
{
    return Benchmark<Signal, Bs2_st>::construction(N, limit);
}
NOINLINE(double Bs2_st::destruction(std::size_t N, std::size_t limit))
{
    return Benchmark<Signal, Bs2_st>::destruction(N, limit);
}
NOINLINE(double Bs2_st::connection(std::size_t N, std::size_t limit))
{
    return Benchmark<Signal, Bs2_st>::connection(N, limit);
}
NOINLINE(double Bs2_st::disconnect(std::size_t N, std::size_t limit))
{
    return Benchmark<Signal, Bs2_st>::disconnect(N, limit);
}
NOINLINE(double Bs2_st::reconnect(std::size_t N, std::size_t limit))
{
    return Benchmark<Signal, Bs2_st>::reconnect(N, limit);
}
NOINLINE(double Bs2_st::emission(std::size_t N, std::size_t limit))
{
    return Benchmark<Signal, Bs2_st>::emission(N, limit);
}
NOINLINE(double Bs2_st::combined(std::size_t N, std::size_t limit))
{
    return Benchmark<Signal, Bs2_st>::combined(N, limit);
}
NOINLINE(double Bs2_st::threaded(std::size_t N, std::size_t limit))
{
    // NOT IMPLEMENTED FOR THIS LIB
    return 0.0;
}
