#include "../hpp/benchmark_dob.hpp"

NOINLINE(void Dob::initialize())
{
    // NOOP
}
NOINLINE(void Dob::validate_assert(std::size_t N))
{
    return Benchmark<Signal, Dob>::validation_assert(N);
}
NOINLINE(double Dob::construction(std::size_t N))
{
    return Benchmark<Signal, Dob>::construction(N);
}
NOINLINE(double Dob::destruction(std::size_t N))
{
    return Benchmark<Signal, Dob>::destruction(N);
}
NOINLINE(double Dob::connection(std::size_t N))
{
    return Benchmark<Signal, Dob>::connection(N);
}
NOINLINE(double Dob::emission(std::size_t N))
{
    return Benchmark<Signal, Dob>::emission(N);
}
NOINLINE(double Dob::combined(std::size_t N))
{
    return Benchmark<Signal, Dob>::combined(N);
}
NOINLINE(double Dob::threaded(std::size_t N))
{
    // TODO This lib is currently not thread safe
    //return Benchmark<Signal, Dob>::threaded(N);
    return 0.0;
}
