#include "benchmark/hpp/benchmark_asg.hpp"
#include "benchmark/hpp/benchmark_bs1.hpp"
#include "benchmark/hpp/benchmark_bs2.hpp"
#include "benchmark/hpp/benchmark_cls.hpp"
#include "benchmark/hpp/benchmark_evl.hpp"
#include "benchmark/hpp/benchmark_jls.hpp"
#include "benchmark/hpp/benchmark_jos.hpp"
#include "benchmark/hpp/benchmark_ksc.hpp"
#include "benchmark/hpp/benchmark_mws.hpp"
#include "benchmark/hpp/benchmark_nls.hpp"
#include "benchmark/hpp/benchmark_nod.hpp"
#include "benchmark/hpp/benchmark_nss.hpp"
#include "benchmark/hpp/benchmark_psg.hpp"
#include "benchmark/hpp/benchmark_sss.hpp"
#include "benchmark/hpp/benchmark_wnk.hpp"
#include "benchmark/hpp/benchmark_wsg.hpp"
#include "benchmark/hpp/benchmark_yas.hpp"

#include "benchmark/lib/jeffomatic/jl_signal/src/Signal.h"
#include "benchmark/lib/jeffomatic/jl_signal/src/StaticSignalConnectionAllocators.h"

#include <iostream>
#include <fstream>
#include <iomanip>
#include <numeric>
#include <vector>
#include <chrono>
#include <map>

#include "benchmark_utility.hpp"

const char* construction = "construct";
const char* destruction = "destruct";
const char* connection = "connect";
const char* emission = "emission";
const char* combined = "combined";
const char* threaded = "threaded";

using ImmediateResults = std::map<const char*, std::vector<double>>;
using ImmediateData = std::map<const char*, ImmediateResults>;

std::size_t g_limit = Timer_u(Limit_u(4000)).count();

template <typename T> void output_report(ImmediateData const&, T&);
template <typename T> void run_benchmark_class(ImmediateData&, std::size_t);

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
        Evl::validate_assert(N);
        Jls::validate_assert(N);
        Jos::validate_assert(N);
        Ksc::validate_assert(N);
        Mws::validate_assert(N);
        Nls::validate_assert(N);
        Nod::validate_assert(N);
        Nss::validate_assert(N);
        Psg::validate_assert(N);
        Sss::validate_assert(N);
        Wnk::validate_assert(N);
        Wsg::validate_assert(N);
        Yas::validate_assert(N);
    }
    catch (std::exception const& error)
    {
        // Catch something exceptional we would like to know about
        std::cerr << "Exception: " << error.what() << std::endl;
        std::cin.get();
    }
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
            run_benchmark_class<Evl>(records, N);
            run_benchmark_class<Jls>(records, N);
            run_benchmark_class<Jos>(records, N);
            run_benchmark_class<Ksc>(records, N);
            run_benchmark_class<Mws>(records, N);
            run_benchmark_class<Nls>(records, N);
            run_benchmark_class<Nod>(records, N);
            run_benchmark_class<Nss>(records, N);
            run_benchmark_class<Psg>(records, N);
            run_benchmark_class<Sss>(records, N);
            run_benchmark_class<Wnk>(records, N);
            run_benchmark_class<Wsg>(records, N);
            run_benchmark_class<Yas>(records, N);

            std::cout << "\n[END: Test Size: " << N << "]" << std::endl;
        }
    }
    catch (std::exception const& error)
    {
        // Would like to know how we died before we ghost
        std::cerr << "Exception: " << error.what() << std::endl;
        std::cin.get();
    }
    return records;
}

//------------------------------------------------------------------------------

int main(int argc, char* argv[])
{
    // Jl_signal uses a static allocator for high performance
    jl::StaticSignalConnectionAllocator<c_jlsignal_max> signal_con_allocator;
    jl::StaticObserverConnectionAllocator<c_jlsignal_max> observer_con_allocator;
    jl::SignalBase::SetCommonConnectionAllocator(&signal_con_allocator);
    jl::SignalObserver::SetCommonConnectionAllocator(&observer_con_allocator);

    std::size_t limit = 0;
    std::size_t start_test_size = 4;
    std::size_t maximum_test_size = 64;

    std::cout << "Enter the time limit per sample [milliseconds]: ";

    if (!(std::cin >> limit))
    {
        std::cerr << "Invalid time limit entered!" << std::endl;
        return 1;
    }
    // Limit to a sane value (and complain about it)
    if (limit > 4000)
    {
        limit = 4000;
        std::cerr << "Capping sample size to 4000 milliseconds." << std::endl;
    }
    g_limit = Timer_u(Limit_u(limit)).count();
    std::cin.ignore();

    // Make sure to set process to high priority and affinity to 1 core
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

        output_report(records, tee);

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

//------------------------------------------------------------------------------

template <typename T>
void output_report(ImmediateData const& records, T& ost)
{
    using namespace std;

    using RelativeResults = map<const char*, double>;
    using RelativeData = map<const char*, RelativeResults>;
    using OrderedResults = pair<const char*, RelativeResults*>;
    using OrderedData = map<double, OrderedResults>;

    RelativeData resultAverage;
    OrderedData resultOrder;

    // Process and sort results by total score (sum of column averages)

    for(auto const& row : records)
    {
        auto const& libName = row.first;

        double score = 0.0;

        for(auto const& column : row.second)
        {
            auto const& opName = column.first;
            auto const& val = column.second;

            double average = accumulate(begin(val),
                end(val), 1.0) / (double) val.size();

            resultAverage[libName][opName] = average;
            score += average;
        }
        resultOrder[score] = OrderedResults { libName, &resultAverage[libName] };
    }

    // Output in unformatted csv

    ost << "\nLibrary, " << construction << ", " << destruction << ", "
        << connection << ", " << emission << ", " << combined << ", total\n";

    for(auto const& row : Range(resultOrder.rbegin(), resultOrder.rend()))
    {
        auto const& score = row.first;
        auto const& libName = row.second.first;

        ost << libName;

        for(auto const& column : (*row.second.second))
        {
            auto const& opName = column.first;
            auto const& val = column.second;

            ost << ", " << setprecision(0) << fixed << val;
        }
        ost << ", " << setprecision(0) << fixed << score << "\n";
    }
}

//------------------------------------------------------------------------------

template <typename T>
void run_benchmark_class(ImmediateData& records, std::size_t N)
{
    // Time this particular benchmark run (for display only)
    auto start = std::chrono::system_clock::now();
    auto start_c = std::chrono::system_clock::to_time_t(start);

    std::cout << std::put_time(std::localtime(&start_c), "%c")
        << " [BEGIN: " << T::LibraryName << "]" << std::endl;

    auto& metrics = records[T::LibraryName];

    metrics[construction].push_back(T::construction(N));
    metrics[destruction].push_back(T::destruction(N));
    metrics[connection].push_back(T::connection(N));
    metrics[emission].push_back(T::emission(N));
    metrics[combined].push_back(T::combined(N));

    // Benchmark class may or may not have this implemented
    metrics[threaded].push_back(T::threaded(N));

    auto stop = std::chrono::system_clock::now();
    auto stop_c = std::chrono::system_clock::to_time_t(stop);

    std::cout << std::put_time(std::localtime(&stop_c), "%c")
        << " [END: " << T::LibraryName << "]" << std::endl;
}
