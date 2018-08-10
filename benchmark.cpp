#include <iostream>
#include <fstream>
#include <iomanip>

#include "benchmark/hpp/benchmark_asg.hpp"
#include "benchmark/hpp/benchmark_bs1.hpp"
#include "benchmark/hpp/benchmark_bs2.hpp"
#include "benchmark/hpp/benchmark_cls.hpp"
#include "benchmark/hpp/benchmark_cps.hpp"
#include "benchmark/hpp/benchmark_evl.hpp"
#include "benchmark/hpp/benchmark_jls.hpp"
#include "benchmark/hpp/benchmark_jos.hpp"
#include "benchmark/hpp/benchmark_ksc.hpp"
#include "benchmark/hpp/benchmark_mws.hpp"
#include "benchmark/hpp/benchmark_nls.hpp"
#include "benchmark/hpp/benchmark_nls_st.hpp"
#include "benchmark/hpp/benchmark_nod.hpp"
#include "benchmark/hpp/benchmark_nss.hpp"
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

template <typename T>
void run_benchmark_class(ImmediateData& records, std::size_t N)
{
    // Time this particular benchmark run (for display only)
    auto start = std::chrono::system_clock::now();
    auto start_out = std::chrono::system_clock::to_time_t(start);

    std::cout << std::put_time(std::localtime(&start_out), "%c")
        << " [BEGIN: " << T::LibraryName << "]" << std::endl;

    auto& metrics = records[T::LibraryName];

    metrics[C_CONSTRUCTION].push_back(T::construction(N));
    metrics[C_DESTRUCTION].push_back(T::destruction(N));
    metrics[C_CONNECTION].push_back(T::connection(N));
    metrics[C_EMISSION].push_back(T::emission(N));
    metrics[C_COMBINED].push_back(T::combined(N));

    // Benchmark class may or may not have this implemented
    metrics[C_THREADED].push_back(T::threaded(N));

    auto stop = std::chrono::system_clock::now();
    auto stop_out = std::chrono::system_clock::to_time_t(stop);

    std::cout << std::put_time(std::localtime(&stop_out), "%c")
        << " [END: " << T::LibraryName << "]" << std::endl;
}

//------------------------------------------------------------------------------

ImmediateData run_all_benchmarks(std::size_t begin, std::size_t end)
{
    ImmediateData records;

    try
    {
        // Double the input size N for every iteration
        for(std::size_t N = begin; N <= end; N *= 2)
        {
            std::cout << "[BEGIN: Test Size: " << N << "]\n" << std::endl;

            run_benchmark_class<Asg>(records, N);
            run_benchmark_class<Bs1>(records, N);
            run_benchmark_class<Bs2>(records, N);
            run_benchmark_class<Cls>(records, N);
            run_benchmark_class<Cps>(records, N);
            run_benchmark_class<Evl>(records, N);
            run_benchmark_class<Jls>(records, N);
            run_benchmark_class<Jos>(records, N);
            run_benchmark_class<Ksc>(records, N);
            run_benchmark_class<Mws>(records, N);
            run_benchmark_class<Nls>(records, N);
            run_benchmark_class<Nls_st>(records, N);
            run_benchmark_class<Nod>(records, N);
            run_benchmark_class<Nss>(records, N);
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
        Evl::validate_assert(N);
        Jls::validate_assert(N);
        Jos::validate_assert(N);
        Ksc::validate_assert(N);
        Mws::validate_assert(N);
        Nls::validate_assert(N);
        Nls_st::validate_assert(N);
        Nod::validate_assert(N);
        Nss::validate_assert(N);
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
void output_report_header(RelativeResults const& first_result_row, T& ost)
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

template <typename T>
void output_reports(ImmediateData const& records, T& ost)
{
    RelativeData resultAverage;
    OrderedData resultOrder;

    // Process and sort results by total score (sum of column averages)

    for (auto const& row : records)
    {
        auto const& libName = row.first;

        double score = 0.0;

        for (auto const& column : row.second)
        {
            auto const& opName = column.first;
            auto const& val = column.second;

            double average = std::accumulate(std::begin(val),
                std::end(val), 1.0) / (double)val.size();

            resultAverage[libName][opName] = average;
            score += average;
        }
        resultOrder[score] = OrderedResults { libName, &resultAverage[libName] };
    }

    // Output in markdown format

    bool show_header = true;

    for (auto const& row : Range(resultOrder.rbegin(), resultOrder.rend()))
    {
        auto const& score = row.first;
        auto const& libName = row.second.first;

        if (show_header)
        {
            output_report_header((*row.second.second), ost);
            show_header = false;
        }

        ost << "| " << libName;

        for (auto const& column : (*row.second.second))
        {
            auto const& opName = column.first;
            auto const& val = column.second;

            ost << " | " << std::setprecision(0) << std::fixed << val;
        }
        ost << " | " << std::setprecision(0) << std::fixed << score << " |\n";
    }
}

//------------------------------------------------------------------------------

int main(int argc, char* argv[])
{
    // Jl_signal uses a static allocator for high performance
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
