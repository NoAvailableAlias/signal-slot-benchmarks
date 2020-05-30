#include "../hpp/benchmark_lcp.hpp"

NOINLINE(void Lcp::initialize())
{
    // NOOP
}
NOINLINE(void Lcp::validate_assert(std::size_t N))
{
    return Benchmark<Signal, Lcp>::validation_assert(N);
}
NOINLINE(double Lcp::construction(std::size_t N, std::size_t limit))
{
    return Benchmark<Signal, Lcp>::construction(N, limit);
}
NOINLINE(double Lcp::destruction(std::size_t N, std::size_t limit))
{
    return Benchmark<Signal, Lcp>::destruction(N, limit);
}
NOINLINE(double Lcp::connection(std::size_t N, std::size_t limit))
{
    return Benchmark<Signal, Lcp>::connection(N, limit);
}
NOINLINE(double Lcp::disconnect(std::size_t N, std::size_t limit))
{
    return Benchmark<Signal, Lcp>::disconnect(N, limit);
}
NOINLINE(double Lcp::reconnect(std::size_t N, std::size_t limit))
{
    return Benchmark<Signal, Lcp>::reconnect(N, limit);
}
NOINLINE(double Lcp::emission(std::size_t N, std::size_t limit))
{
    return Benchmark<Signal, Lcp>::emission(N, limit);
}
NOINLINE(double Lcp::combined(std::size_t N, std::size_t limit))
{
    return Benchmark<Signal, Lcp>::combined(N, limit);
}
NOINLINE(double Lcp::threaded(std::size_t N, std::size_t limit))
{
    // NOT IMPLEMENTED FOR THIS LIB
    return 0.0;
}
