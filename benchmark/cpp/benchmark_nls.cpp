#include "../hpp/benchmark_nls.hpp"

NOINLINE(void Nls::initialize())
{
    if (!neolib::services::service_provider_allocated())
        neolib::services::allocate_service_provider();
    neolib::services::service<neolib::i_event_system>().set_locking_strategy(neolib::event_system_locking_strategy::MultiThreaded);
}
NOINLINE(void Nls::validate_assert(std::size_t N))
{
    initialize();
    return Benchmark<Signal, Nls>::validation_assert(N);
}
NOINLINE(double Nls::construction(std::size_t N, std::size_t limit))
{
    return Benchmark<Signal, Nls>::construction(N, limit);
}
NOINLINE(double Nls::destruction(std::size_t N, std::size_t limit))
{
    return Benchmark<Signal, Nls>::destruction(N, limit);
}
NOINLINE(double Nls::connection(std::size_t N, std::size_t limit))
{
    return Benchmark<Signal, Nls>::connection(N, limit);
}
NOINLINE(double Nls::disconnect(std::size_t N, std::size_t limit))
{
    return Benchmark<Signal, Nls>::disconnect(N, limit);
}
NOINLINE(double Nls::reconnect(std::size_t N, std::size_t limit))
{
    return Benchmark<Signal, Nls>::reconnect(N, limit);
}
NOINLINE(double Nls::emission(std::size_t N, std::size_t limit))
{
    return Benchmark<Signal, Nls>::emission(N, limit);
}
NOINLINE(double Nls::combined(std::size_t N, std::size_t limit))
{
    return Benchmark<Signal, Nls>::combined(N, limit);
}
NOINLINE(double Nls::threaded(std::size_t N, std::size_t limit))
{
    return Benchmark<Signal, Nls>::threaded(N, limit);
}
