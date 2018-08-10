#include "../hpp/benchmark_nls_st.hpp"

const char* Nls_st::LibraryName = "neolib signal";

NOINLINE(void Nls_st::validate_assert(std::size_t N))
{
    return Benchmark<Signal, Nls_st>::validation_assert(N);
}
NOINLINE(double Nls_st::construction(std::size_t N))
{
    return Benchmark<Signal, Nls_st>::construction(N);
}
NOINLINE(double Nls_st::destruction(std::size_t N))
{
    return Benchmark<Signal, Nls_st>::destruction(N);
}
NOINLINE(double Nls_st::connection(std::size_t N))
{
    return Benchmark<Signal, Nls_st>::connection(N);
}
NOINLINE(double Nls_st::emission(std::size_t N))
{
    return Benchmark<Signal, Nls_st>::emission(N);
}
NOINLINE(double Nls_st::combined(std::size_t N))
{
    return Benchmark<Signal, Nls_st>::combined(N);
}
NOINLINE(double Nls_st::threaded(std::size_t N))
{
    return Benchmark<Signal, Nls_st>::threaded(N);
}
