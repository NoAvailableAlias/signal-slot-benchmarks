#include "../hpp/benchmark_cls.hpp"

const char* Cls::LibraryName = "* lsignal";

NOINLINE(void Cls::validate_assert(std::size_t N))
{
    return Benchmark<SignalType, Cls>::validation_assert(N);
}    
NOINLINE(double Cls::construction(std::size_t N))
{
    return Benchmark<SignalType, Cls>::construction(N);
}
NOINLINE(double Cls::destruction(std::size_t N))
{
    return Benchmark<SignalType, Cls>::destruction(N);
}
NOINLINE(double Cls::connection(std::size_t N))
{
    return Benchmark<SignalType, Cls>::connection(N);
}
NOINLINE(double Cls::emission(std::size_t N))
{
    return Benchmark<SignalType, Cls>::emission(N);
}
NOINLINE(double Cls::combined(std::size_t N))
{
    return Benchmark<SignalType, Cls>::combined(N);
}
NOINLINE(double Cls::threaded(std::size_t N))
{
    return Benchmark<SignalType, Cls>::threaded(N);
}
