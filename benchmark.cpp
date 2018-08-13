#include <iostream>
#include <fstream>
#include <iomanip>

#include <boost/filesystem.hpp>

#include "benchmark/hpp/benchmark_asg.hpp"
#include "benchmark/hpp/benchmark_bs1.hpp"
#include "benchmark/hpp/benchmark_bs2.hpp"
#include "benchmark/hpp/benchmark_cls.hpp"
#include "benchmark/hpp/benchmark_cps.hpp"
#include "benchmark/hpp/benchmark_cps_st.hpp"
#include "benchmark/hpp/benchmark_evl.hpp"
#include "benchmark/hpp/benchmark_jls.hpp"
#include "benchmark/hpp/benchmark_jos.hpp"
#include "benchmark/hpp/benchmark_ksc.hpp"
#include "benchmark/hpp/benchmark_lss.hpp"
#include "benchmark/hpp/benchmark_mws.hpp"
#include "benchmark/hpp/benchmark_nls.hpp"
#include "benchmark/hpp/benchmark_nls_st.hpp"
#include "benchmark/hpp/benchmark_nod.hpp"
#include "benchmark/hpp/benchmark_nss.hpp"
#include "benchmark/hpp/benchmark_nss_v2.hpp"
#include "benchmark/hpp/benchmark_nss_v2_st.hpp"
#include "benchmark/hpp/benchmark_psg.hpp"
#include "benchmark/hpp/benchmark_pss.hpp"
#include "benchmark/hpp/benchmark_pss_st.hpp"
#include "benchmark/hpp/benchmark_sss.hpp"
#include "benchmark/hpp/benchmark_wnk.hpp"
#include "benchmark/hpp/benchmark_wsg.hpp"
#include "benchmark/hpp/benchmark_yas.hpp"
#include "benchmark/hpp/benchmark_vdk.hpp"

// These are required in Main to initialize jlsignal
#include "benchmark/lib/jeffomatic/jl_signal/src/Signal.h"
#include "benchmark/lib/jeffomatic/jl_signal/src/StaticSignalConnectionAllocators.h"

#include "benchmark_utility.hpp"

// Extern defined in benchmark_utility.hpp
std::size_t g_timer_limit = Timer_u(Limit_u(4000)).count();

//------------------------------------------------------------------------------

template <typename Benchmark>
void run_benchmark_class(ImmediateData& records, std::size_t N)
{
    // Time this particular benchmark run (for display only)
    auto start = std::chrono::system_clock::now();
    auto start_out = std::chrono::system_clock::to_time_t(start);

    std::cout << std::put_time(std::localtime(&start_out), "%c")
        << " [BEGIN: " << Benchmark::C_LIB_NAME << "]" << std::endl;

    auto& metrics = records[Benchmark::C_LIB_NAME];

    metrics[C_CONSTRUCTION].push_back(Benchmark::construction(N));
    metrics[C_DESTRUCTION].push_back(Benchmark::destruction(N));
    metrics[C_CONNECTION].push_back(Benchmark::connection(N));
    metrics[C_EMISSION].push_back(Benchmark::emission(N));
    metrics[C_COMBINED].push_back(Benchmark::combined(N));

    // T might not have this implemented
    metrics[C_THREADED].push_back(Benchmark::threaded(N));

    auto stop = std::chrono::system_clock::now();
    auto stop_out = std::chrono::system_clock::to_time_t(stop);

    std::cout << std::put_time(std::localtime(&stop_out), "%c")
        << " [END: " << Benchmark::C_LIB_NAME << "]" << std::endl;
}

//------------------------------------------------------------------------------

ImmediateData run_all_benchmarks(std::size_t begin, std::size_t end)
{
    ImmediateData records;

    try
    {
        // Double the input size N for every iteration
        for (auto N = begin; N <= end; N *= 2)
        {
            std::cout << "[BEGIN: Test Size: " << N << "]\n" << std::endl;

            run_benchmark_class<Asg>(records, N);
            run_benchmark_class<Bs1>(records, N);
            run_benchmark_class<Bs2>(records, N);
            run_benchmark_class<Cls>(records, N);
            run_benchmark_class<Cps>(records, N);
            run_benchmark_class<Cps_st>(records, N);
            run_benchmark_class<Evl>(records, N);
            run_benchmark_class<Jls>(records, N);
            run_benchmark_class<Jos>(records, N);
            run_benchmark_class<Ksc>(records, N);
            run_benchmark_class<Lss>(records, N);
            run_benchmark_class<Mws>(records, N);
            run_benchmark_class<Nls>(records, N);
            run_benchmark_class<Nls_st>(records, N);
            run_benchmark_class<Nod>(records, N);
            run_benchmark_class<Nss>(records, N);
            run_benchmark_class<Nss_v2>(records, N);
            run_benchmark_class<Nss_v2_st>(records, N);
            run_benchmark_class<Psg>(records, N);
            run_benchmark_class<Pss>(records, N);
            run_benchmark_class<Pss_st>(records, N);
            run_benchmark_class<Sss>(records, N);
            run_benchmark_class<Wnk>(records, N);
            run_benchmark_class<Wsg>(records, N);
            run_benchmark_class<Yas>(records, N);
            run_benchmark_class<Vdk>(records, N);

            std::cout << "\n[END: Test Size: " << N << "]" << std::endl;
        }
    }
    catch (std::exception const& error)
    {
        std::cerr << "Exception: " << error.what() << std::endl;
        std::cin.get();
    }
    return records;
}

//------------------------------------------------------------------------------

void run_all_validation_tests(std::size_t N)
{
    try
    {
        // Abort if any implementation isn't functioning correctly
        Asg::validate_assert(N);
        Bs1::validate_assert(N);
        Bs2::validate_assert(N);
        Cls::validate_assert(N);
        Cps::validate_assert(N);
        Cps_st::validate_assert(N);
        Evl::validate_assert(N);
        Jls::validate_assert(N);
        Jos::validate_assert(N);
        Ksc::validate_assert(N);
        Lss::validate_assert(N);
        Mws::validate_assert(N);
        Nls::validate_assert(N);
        Nls_st::validate_assert(N);
        Nod::validate_assert(N);
        Nss::validate_assert(N);
        Nss_v2::validate_assert(N);
        Nss_v2_st::validate_assert(N);
        Psg::validate_assert(N);
        Pss::validate_assert(N);
        Pss_st::validate_assert(N);
        Sss::validate_assert(N);
        Wnk::validate_assert(N);
        Wsg::validate_assert(N);
        Yas::validate_assert(N);
        Vdk::validate_assert(N);
    }
    catch (std::exception const& error)
    {
        std::cerr << "Exception: " << error.what() << std::endl;
        std::cin.get();
    }
}

//------------------------------------------------------------------------------

template <typename T>
void output_perf_report_header(RelativeResults const& first_result_row, T& ost)
{
    std::string header_first_row("| Library | ");

    for (auto const& column : first_result_row)
    {
        header_first_row.append(column.first);
        header_first_row.append(" | ");
    }
    header_first_row += "total |";

    ost << "\n" << header_first_row << "\n";

    for (auto const& ch : header_first_row)
    {
        ost << (ch == '|' ? '|' : '-');
    }
    ost << "\n";
}

//------------------------------------------------------------------------------

std::string get_object_file_size(const char* file_stem)
{
    namespace fs = boost::filesystem;

#if defined(__GNUC__) || defined(__GNUG__)

    auto file_name = std::string(file_stem) + ".o";

#else

    auto file_name = std::string(file_stem) + ".obj";

#endif

    const fs::path file { file_name };

    if (fs::exists(file))
    {
        return std::to_string(fs::file_size(file) / 1000) + " kb";
    }
    return "&mdash;";
}

template <typename Benchmark, typename T>
void output_metrics_report_row(T& ost)
{
    try
    {
        ost << "| [" << Benchmark::C_LIB_NAME << "]("
            << Benchmark::C_LIB_SOURCE_URL << ") | "
            << get_object_file_size(Benchmark::C_LIB_FILE) << " | "
            << Benchmark::C_LIB_IS_HEADER_ONLY
            << " | " << Benchmark::C_LIB_DATA_STRUCTURE
            << " | " << Benchmark::C_LIB_IS_THREAD_SAFE << " |\n";
    }
    catch (std::exception const& error)
    {
        std::cerr << "Exception: " << error.what() << std::endl;
        std::cin.get();
    }
}

template <typename T>
void output_metrics_report(T& ost)
{
    ost << "| Library | Build Size | Header Only | Data Structure | Thread Safe |\n"
        << "| ------- |:----------:|:-----------:| -------------- |:-----------:|\n";
    output_metrics_report_row<Asg>(ost);
    output_metrics_report_row<Bs1>(ost);
    output_metrics_report_row<Bs2>(ost);
    output_metrics_report_row<Cls>(ost);
    output_metrics_report_row<Cps>(ost);
    output_metrics_report_row<Cps_st>(ost);
    output_metrics_report_row<Evl>(ost);
    output_metrics_report_row<Jls>(ost);
    output_metrics_report_row<Jos>(ost);
    output_metrics_report_row<Ksc>(ost);
    output_metrics_report_row<Lss>(ost);
    output_metrics_report_row<Mws>(ost);
    output_metrics_report_row<Nls>(ost);
    output_metrics_report_row<Nls_st>(ost);
    output_metrics_report_row<Nod>(ost);
    output_metrics_report_row<Nss>(ost);
    output_metrics_report_row<Nss_v2>(ost);
    output_metrics_report_row<Nss_v2_st>(ost);
    output_metrics_report_row<Psg>(ost);
    output_metrics_report_row<Pss>(ost);
    output_metrics_report_row<Pss_st>(ost);
    output_metrics_report_row<Sss>(ost);
    output_metrics_report_row<Wnk>(ost);
    output_metrics_report_row<Wsg>(ost);
    output_metrics_report_row<Yas>(ost);
    output_metrics_report_row<Vdk>(ost);
}

//------------------------------------------------------------------------------

template <typename T>
void output_reports(ImmediateData const& records, T& ost)
{
    RelativeData result_average;
    OrderedData result_order;

    // Process and sort results by total score (sum of column averages)

    for (auto const& row : records)
    {
        auto const& lib_name = row.first;

        double score = 0.0;

        for (auto const& column : row.second)
        {
            auto const& op_name = column.first;
            auto const& val = column.second;

            double average = std::accumulate(std::begin(val),
                std::end(val), 1.0) / (double)val.size();

            result_average[lib_name][op_name] = average;
            score += average;
        }
        result_order[score] = OrderedResults { lib_name, &result_average[lib_name] };
    }

    // Output in markdown format

    bool show_header = true;

    for (auto const& row : Range(result_order.rbegin(), result_order.rend()))
    {
        auto const& score = row.first;
        auto const& lib_name = row.second.first;

        if (show_header)
        {
            output_perf_report_header((*row.second.second), ost);
            show_header = false;
        }

        ost << "| " << lib_name;

        for (auto const& column : (*row.second.second))
        {
            auto const& val = column.second;

            ost << " | " << std::setprecision(0) << std::fixed << val;
        }
        ost << " | " << std::setprecision(0) << std::fixed << score << " |\n";
    }

    output_metrics_report(ost);
}

//------------------------------------------------------------------------------

int main(int argc, char* argv[])
{
    // Jl_signal uses a compile time allocator for maximum performance
    // (however, this allocator cannot be resized at runtime)
    jl::StaticSignalConnectionAllocator<C_JLSIGNAL_MAX> signal_con_allocator;
    jl::StaticObserverConnectionAllocator<C_JLSIGNAL_MAX> observer_con_allocator;
    jl::SignalBase::SetCommonConnectionAllocator(&signal_con_allocator);
    jl::SignalObserver::SetCommonConnectionAllocator(&observer_con_allocator);

    std::size_t user_limit = 0;
    std::size_t start_test_size = 2;
    std::size_t maximum_test_size = 64;

    std::cout << "Enter the milliseconds per sample (4000 takes several hours): ";

    if (!(std::cin >> user_limit))
    {
        return 1;
    }
    // Must compile as x64 or else wrapping might happen
    g_timer_limit = Timer_u(Limit_u(user_limit)).count();
    std::cin.ignore();

    // Make sure to set process to high priority
    std::cout << "Change the CPU priority now: [paused]" << std::endl;
    std::cin.get();

    // Time the entire duration of the benchmark
    auto start = std::chrono::system_clock::now();
    auto start_c = std::chrono::system_clock::to_time_t(start);

    // Assert that all implementations are functioning correctly
    run_all_validation_tests(maximum_test_size);
    // Run all benchmark operations on each benchmark implementation
    auto records = run_all_benchmarks(start_test_size, maximum_test_size);

    //--------------------------------------------------------------------------

    if (std::ofstream ofs { "report.txt", std::ios::app })
    {
        auto tee = std::tie(std::cout, ofs);
        tee << "\n" << std::put_time(std::localtime(&start_c), "%c") << "\n";

        output_reports(records, tee);

        auto stop = std::chrono::system_clock::now();
        auto stop_c = std::chrono::system_clock::to_time_t(stop);
        tee << "\n" << std::put_time(std::localtime(&stop_c), "%c") << "\n";

        ofs << std::endl;
    }
    else
    {
        std::cerr << "Unable to append report.txt: [error]" << std::endl;
    }
    std::cin.get();
}
