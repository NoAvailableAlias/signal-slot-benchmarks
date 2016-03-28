#include "../hpp/benchmark_spp.hpp"

const char* Spp::LibraryName = "signals-cpp";

NOINLINE(void Spp::validate_assert(std::size_t N))
{
    return Benchmark<SignalType, Spp>::validation_assert(N);
}    
NOINLINE(double Spp::construction(std::size_t N))
{
    return Benchmark<SignalType, Spp>::construction(N);
}
NOINLINE(double Spp::destruction(std::size_t N))
{
    return Benchmark<SignalType, Spp>::destruction(N);
}
NOINLINE(double Spp::connection(std::size_t N))
{
    return Benchmark<SignalType, Spp>::connection(N);
}
NOINLINE(double Spp::emission(std::size_t N))
{
    return Benchmark<SignalType, Spp>::emission(N);
}
NOINLINE(double Spp::combined(std::size_t N))
{
    return Benchmark<SignalType, Spp>::combined(N);
}
NOINLINE(double Spp::threaded(std::size_t N))
{
    return 0.0;
}
