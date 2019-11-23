#include "../hpp/benchmark_asg.hpp"

NOINLINE(void Asg::initialize())
{
    // NOOP
}
NOINLINE(void Asg::validate_assert(std::size_t N))
{
    return Benchmark<Signal, Asg>::validation_assert(N);
}    
NOINLINE(double Asg::construction(std::size_t N))
{
    return Benchmark<Signal, Asg>::construction(N);
}
NOINLINE(double Asg::destruction(std::size_t N))
{
    return Benchmark<Signal, Asg>::destruction(N);
}
NOINLINE(double Asg::connection(std::size_t N))
{
    return Benchmark<Signal, Asg>::connection(N);
}
NOINLINE(double Asg::disconnect(std::size_t N))
{
    return Benchmark<Signal, Asg>::disconnect(N);
}
NOINLINE(double Asg::reconnect(std::size_t N))
{
    return Benchmark<Signal, Asg>::reconnect(N);
}
NOINLINE(double Asg::emission(std::size_t N))
{
    return Benchmark<Signal, Asg>::emission(N);
}
NOINLINE(double Asg::combined(std::size_t N))
{
    return Benchmark<Signal, Asg>::combined(N);
}
NOINLINE(double Asg::threaded(std::size_t N))
{
    // NOT IMPLEMENTED FOR THIS LIB
    return 0.0;
}
