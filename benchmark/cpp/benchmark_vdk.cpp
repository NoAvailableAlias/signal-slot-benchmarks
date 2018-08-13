#include "../hpp/benchmark_vdk.hpp"

NOINLINE(void Vdk::validate_assert(std::size_t N))
{
    return Benchmark<Signal, Vdk>::validation_assert(N);
}    
NOINLINE(double Vdk::construction(std::size_t N))
{
    return Benchmark<Signal, Vdk>::construction(N);
}
NOINLINE(double Vdk::destruction(std::size_t N))
{
    return Benchmark<Signal, Vdk>::destruction(N);
}
NOINLINE(double Vdk::connection(std::size_t N))
{
    return Benchmark<Signal, Vdk>::connection(N);
}
NOINLINE(double Vdk::emission(std::size_t N))
{
    return Benchmark<Signal, Vdk>::emission(N);
}
NOINLINE(double Vdk::combined(std::size_t N))
{
    return Benchmark<Signal, Vdk>::combined(N);
}
NOINLINE(double Vdk::threaded(std::size_t N))
{
    // NOT IMPLEMENTED FOR THIS LIB
    return 0.0;
}
