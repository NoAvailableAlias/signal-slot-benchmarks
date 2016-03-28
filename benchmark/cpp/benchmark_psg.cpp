#include "../hpp/benchmark_psg.hpp"

const char* Psg::LibraryName = "pbhogan Signals";

NOINLINE(void Psg::validate_assert(std::size_t N))
{
    return Benchmark<Signal, Psg>::validation_assert(N);
}    
NOINLINE(double Psg::construction(std::size_t N))
{
    return Benchmark<Signal, Psg>::construction(N);
}
NOINLINE(double Psg::destruction(std::size_t N))
{
    return Benchmark<Signal, Psg>::destruction(N);
}
NOINLINE(double Psg::connection(std::size_t N))
{
    return Benchmark<Signal, Psg>::connection(N);
}
NOINLINE(double Psg::emission(std::size_t N))
{
    return Benchmark<Signal, Psg>::emission(N);
}
NOINLINE(double Psg::combined(std::size_t N))
{
    return Benchmark<Signal, Psg>::combined(N);
}
NOINLINE(double Psg::threaded(std::size_t N))
{
    return 0.0;
}
