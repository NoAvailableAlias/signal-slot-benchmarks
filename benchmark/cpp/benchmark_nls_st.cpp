#include "../hpp/benchmark_nls_st.hpp"

NOINLINE(void Nls_st::initialize())
{
    if (!neolib::services::service_provider_allocated())
        neolib::services::allocate_service_provider();
    neolib::services::service<neolib::i_event_system>().set_locking_strategy(neolib::event_system_locking_strategy::SingleThreaded);
}
NOINLINE(void Nls_st::validate_assert(std::size_t N))
{
    initialize();
    return Benchmark<Signal, Nls_st>::validation_assert(N);
}
NOINLINE(double Nls_st::construction(std::size_t N, std::size_t limit))
{
    return Benchmark<Signal, Nls_st>::construction(N, limit);
}
NOINLINE(double Nls_st::destruction(std::size_t N, std::size_t limit))
{
    return Benchmark<Signal, Nls_st>::destruction(N, limit);
}
NOINLINE(double Nls_st::connection(std::size_t N, std::size_t limit))
{
    return Benchmark<Signal, Nls_st>::connection(N, limit);
}
NOINLINE(double Nls_st::disconnect(std::size_t N, std::size_t limit))
{
    return Benchmark<Signal, Nls_st>::disconnect(N, limit);
}
NOINLINE(double Nls_st::reconnect(std::size_t N, std::size_t limit))
{
    return Benchmark<Signal, Nls_st>::reconnect(N, limit);
}
NOINLINE(double Nls_st::emission(std::size_t N, std::size_t limit))
{
    return Benchmark<Signal, Nls_st>::emission(N, limit);
}
NOINLINE(double Nls_st::combined(std::size_t N, std::size_t limit))
{
    return Benchmark<Signal, Nls_st>::combined(N, limit);
}
NOINLINE(double Nls_st::threaded(std::size_t N, std::size_t limit))
{
    // NOT IMPLEMENTED FOR THIS LIB
    return 0.0;
}
