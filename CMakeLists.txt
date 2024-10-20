cmake_minimum_required(VERSION 3.6)
project(signal-slot-benchmarks VERSION 0.0.1 LANGUAGES CXX)

# compiler support
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED YES)

# dependencies
find_package(Boost COMPONENTS system chrono thread REQUIRED)
find_package(Threads)


add_library(signal_libraries
    benchmark/lib/copperspice/cs_signal/src/cs_signal.cpp
    benchmark/lib/copperspice/cs_signal/src/cs_slot.cpp
    benchmark/lib/CppFakeIt/libfastsignals/src/function_detail.cpp
    benchmark/lib/CppFakeIt/libfastsignals/src/lfs_connection.cpp
    benchmark/lib/CppFakeIt/libfastsignals/src/signal_impl.cpp
    benchmark/lib/dacap/obs/dacap_connection.cpp
    benchmark/lib/i42output/src/async_task.cpp
    benchmark/lib/i42output/src/async_thread.cpp
    benchmark/lib/i42output/src/event.cpp
    benchmark/lib/i42output/src/thread.cpp
    benchmark/lib/i42output/src/timer.cpp
    benchmark/lib/i42output/src/waitable_event.cpp
    benchmark/lib/jeffomatic/jl_signal/src/SignalBase.cpp
    benchmark/lib/jeffomatic/jl_signal/src/ObjectPool.cpp
    benchmark/lib/libsigcplusplus/sigc++/connection.cc
    benchmark/lib/libsigcplusplus/sigc++/functors/slot_base.cc
    benchmark/lib/libsigcplusplus/sigc++/signal_base.cc
    benchmark/lib/libsigcplusplus/sigc++/trackable.cc
    benchmark/lib/mwthinker/Signal/src/mw/signals/connection.cpp
    benchmark/lib/supergrover/sigslot/src/signal.cpp
    benchmark/lib/vdksoft/signals/src/signals.cpp
)

target_include_directories (signal_libraries
    PUBLIC
    benchmark/lib/libsigcplusplus
)

target_link_libraries(signal_libraries iscool_signals)


add_subdirectory(benchmark/lib/KDBindings)

# add_library(KDBindings INTERFACE)
# target_include_directories(signal_libraries INTERFACE ${CMAKE_SOURCE_DIR}/benchmark/lib/KDBindings/src)

# target_link_libraries(signal_libraries KDBindings)

# project definition
add_executable(bench
    benchmark.cpp
    benchmark/cpp/benchmark_aco.cpp
    benchmark/cpp/benchmark_asg.cpp
    benchmark/cpp/benchmark_bs2.cpp
    benchmark/cpp/benchmark_bs2_st.cpp
    benchmark/cpp/benchmark_cls.cpp
    benchmark/cpp/benchmark_cps.cpp
    benchmark/cpp/benchmark_cps_st.cpp
    benchmark/cpp/benchmark_css.cpp
    benchmark/cpp/benchmark_dob.cpp
    benchmark/cpp/benchmark_evl.cpp
    benchmark/cpp/benchmark_ics.cpp
    benchmark/cpp/benchmark_jls.cpp
    benchmark/cpp/benchmark_jos.cpp
    benchmark/cpp/benchmark_ksc.cpp
    benchmark/cpp/benchmark_ktn.cpp
    benchmark/cpp/benchmark_lfs.cpp
    benchmark/cpp/benchmark_lcp.cpp
    benchmark/cpp/benchmark_lss.cpp
    benchmark/cpp/benchmark_mws.cpp
    benchmark/cpp/benchmark_nes.cpp
    benchmark/cpp/benchmark_nls.cpp
    benchmark/cpp/benchmark_nls_st.cpp
    benchmark/cpp/benchmark_nod.cpp
    benchmark/cpp/benchmark_nod_st.cpp
    benchmark/cpp/benchmark_nss_st.cpp
    benchmark/cpp/benchmark_nss_sts.cpp
    benchmark/cpp/benchmark_nss_ts.cpp
    benchmark/cpp/benchmark_nss_tss.cpp
    benchmark/cpp/benchmark_psg.cpp
    benchmark/cpp/benchmark_pss.cpp
    benchmark/cpp/benchmark_pss_st.cpp
    benchmark/cpp/benchmark_sss.cpp
    benchmark/cpp/benchmark_tsr.cpp
    benchmark/cpp/benchmark_tsr_st.cpp
    benchmark/cpp/benchmark_vdk.cpp
    benchmark/cpp/benchmark_vdk_st.cpp
    benchmark/cpp/benchmark_wnk.cpp
    benchmark/cpp/benchmark_yas.cpp
    benchmark/cpp/benchmark_kdbindings.cpp
)

target_link_libraries(bench KDBindings)


add_subdirectory(benchmark/lib/iscool)

set_target_properties(bench PROPERTIES CXX_EXTENSIONS NO)
target_include_directories(bench PRIVATE ${CMAKE_CURRENT_SOURCE_DIR})
target_compile_options(bench PRIVATE  "-fdiagnostics-color=always")
target_link_libraries(bench
  signal_libraries Boost::system Boost::chrono Threads::Threads)

#-------------------------------------------------------------------------------
# Tests

find_package(GTest REQUIRED)

add_executable(tests
  tests.cpp
  tests/cpp/exited_with_code_stored.cpp
  tests/cpp/signal_traits_jls.cpp
  tests/cpp/test_non_template_base.cpp
  tests/cpp/test_results.cpp
  )

target_include_directories(tests
  PRIVATE ${CMAKE_CURRENT_SOURCE_DIR} benchmark/lib/)
target_compile_options(tests PRIVATE  "-fdiagnostics-color=always")
target_link_libraries(tests
  signal_libraries Boost::system Boost::chrono Threads::Threads GTest::GTest)
