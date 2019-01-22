#include "../hpp/benchmark_psg.hpp"

NOINLINE(void Psg::initialize())
{
    // NOOP
}
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
NOINLINE(double Psg::disconnect(std::size_t N))
{
    return Benchmark<Signal, Psg>::disconnect(N);
}
NOINLINE(double Psg::reconnect(std::size_t N))
{
    return Benchmark<Signal, Psg>::reconnect(N);
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
    // NOT IMPLEMENTED FOR THIS LIB
    return 0.0;
}
