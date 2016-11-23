CMAKE_MINIMUM_REQUIRED(VERSION 3.0)

set(PROJECT_NAME signal-slot-benchmarks)
project(${PROJECT_NAME})

set(BENCHMARK_DIR "benchmark")
set(BENCHMARK_HEADER_DIR "${BENCHMARK_DIR}/hpp")

include_directories(". ${BENCHMARK_HEADER_DIR} benchmark/lib")

set(BENCHMARKS
    asg
    # TODO: bs1
    # TODO: bs2
    cls
    evl
    jls
    jos
    ksc
    mws
    # TODO: nls
    nod
    nss
    psg
    spp
    sss
    wnk
    wsg
    yas)

set(HEADERS benchmark.hpp
            benchmark_utility.hpp)

set(SOURCES benchmark.cpp)

foreach(benchmark ${BENCHMARKS})
  list(APPEND HEADERS ${BENCHMARK_HEADER_DIR}/benchmark_${benchmark}.hpp)
  list(APPEND SOURCES ${BENCHMARK_DIR}/cpp/benchmark_${benchmark}.cpp)
endforeach()

set(SOURCES ${SOURCES}
            benchmark/lib/jeffomatic/jl_signal/src/ObjectPool.cpp
            benchmark/lib/jeffomatic/jl_signal/src/SignalBase.cpp
            benchmark/lib/mwthinker/Signal/src/mw/signals/connection.cpp
            # TODO: benchmark/lib/neolib_sigslot/io_thread.cpp
            # TODO: benchmark/lib/neolib_sigslot/thread.cpp
            benchmark/lib/supergrover/sigslot/src/signal.cpp
            benchmark/lib/winglot/Signals/W_Slot.cpp)

add_executable(${PROJECT_NAME} ${HEADERS} ${SOURCES})
