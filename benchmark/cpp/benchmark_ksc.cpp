#include "../hpp/benchmark_ksc.hpp"

const char* Ksc::LibraryName = "* Kosta signals-cpp";

NOINLINE(void Ksc::validate_assert(std::size_t N))
{
    return Benchmark<Signal, Ksc>::validation_assert(N);
}    
NOINLINE(double Ksc::construction(std::size_t N))
{
    return Benchmark<Signal, Ksc>::construction(N);
}
NOINLINE(double Ksc::destruction(std::size_t N))
{
    return Benchmark<Signal, Ksc>::destruction(N);
}
NOINLINE(double Ksc::connection(std::size_t N))
{
    return Benchmark<Signal, Ksc>::connection(N);
}
NOINLINE(double Ksc::emission(std::size_t N))
{
    return Benchmark<Signal, Ksc>::emission(N);
}
NOINLINE(double Ksc::combined(std::size_t N))
{
    return Benchmark<Signal, Ksc>::combined(N);
}
