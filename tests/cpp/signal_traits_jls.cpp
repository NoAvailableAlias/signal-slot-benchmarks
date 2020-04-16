#include "tests/hpp/signal_traits_jls.hpp"

std::unique_ptr<jl::StaticSignalConnectionAllocator<16>>
signal_traits_jls::m_signal_con_allocator;

std::unique_ptr<jl::StaticObserverConnectionAllocator<16>>
signal_traits_jls::m_observer_con_allocator;
