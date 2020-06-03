#include "../hpp/benchmark_tsr_st.hpp"

NOINLINE(void Tsr_st::initialize())
{
    // NOOP
}
NOINLINE(void Tsr_st::validate_assert(std::size_t N))
{
    return Benchmark<Signal, Tsr_st>::validation_assert(N);
}
NOINLINE(double Tsr_st::construction(std::size_t N, std::size_t limit))
{
    return Benchmark<Signal, Tsr_st>::construction(N, limit);
}
NOINLINE(double Tsr_st::destruction(std::size_t N, std::size_t limit))
{
    return Benchmark<Signal, Tsr_st>::destruction(N, limit);
}
NOINLINE(double Tsr_st::connection(std::size_t N, std::size_t limit))
{
    return Benchmark<Signal, Tsr_st>::connection(N, limit);
}
NOINLINE(double Tsr_st::disconnect(std::size_t N, std::size_t limit))
{
    return Benchmark<Signal, Tsr_st>::disconnect(N, limit);
}
NOINLINE(double Tsr_st::reconnect(std::size_t N, std::size_t limit))
{
    return Benchmark<Signal, Tsr_st>::reconnect(N, limit);
}
NOINLINE(double Tsr_st::emission(std::size_t N, std::size_t limit))
{
    return Benchmark<Signal, Tsr_st>::emission(N, limit);
}
NOINLINE(double Tsr_st::combined(std::size_t N, std::size_t limit))
{
    return Benchmark<Signal, Tsr_st>::combined(N, limit);
}
NOINLINE(double Tsr_st::threaded(std::size_t N, std::size_t limit))
{
    // NOT IMPLEMENTED FOR THIS LIB
    return 0.0;
}
