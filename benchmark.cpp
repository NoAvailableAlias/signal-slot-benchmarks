#include "benchmark/hpp/benchmark_asg.hpp"
#include "benchmark/hpp/benchmark_bs1.hpp"
#include "benchmark/hpp/benchmark_bs2.hpp"
#include "benchmark/hpp/benchmark_evl.hpp"
#include "benchmark/hpp/benchmark_jls.hpp"
#include "benchmark/hpp/benchmark_jos.hpp"
#include "benchmark/hpp/benchmark_ksc.hpp"
#include "benchmark/hpp/benchmark_mws.hpp"
#include "benchmark/hpp/benchmark_nls.hpp"
#include "benchmark/hpp/benchmark_nod.hpp"
#include "benchmark/hpp/benchmark_nss.hpp"
#include "benchmark/hpp/benchmark_psg.hpp"
#include "benchmark/hpp/benchmark_spp.hpp"
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

using ImmediateResults = std::map<const char*, std::vector<double>>;
using ImmediateData = std::map<const char*, ImmediateResults>;

std::size_t g_limit = Timer_u(Limit_u(4000)).count();

template <typename T> void outputReport(ImmediateData const&, T&);

//------------------------------------------------------------------------------

void run_all_validation_tests(std::size_t N)
{
    try
    {
        // Abort if any implementation isn't functioning correctly
        Asg::validate_assert(N);
        Bs1::validate_assert(N);
        Bs2::validate_assert(N);
        Evl::validate_assert(N);
        Jls::validate_assert(N);
        Jos::validate_assert(N);
        Ksc::validate_assert(N);//<- ASSERT FAIL
        Mws::validate_assert(N);
        Nls::validate_assert(N);
        Nod::validate_assert(N);
        Nss::validate_assert(N);
        Psg::validate_assert(N);
        //Spp::validate_assert(N);//<- ASSERT FAIL
        Sss::validate_assert(N);
        Wnk::validate_assert(N);
        Wsg::validate_assert(N);
        Yas::validate_assert(N);
    }
    catch (std::exception const& error)
    {
        // Catch something exceptional we would like to know about
        std::cerr << "Exception encountered: " << error.what() << std::endl;
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
            std::cout << "[Test Size: " << N << "] BEGIN\n" << std::endl;

            std::cout << "[BEGIN: " << Asg::LibraryName << "]" << std::endl;

            auto& asg = records[Asg::LibraryName];
            asg[construction].push_back(Asg::construction(N));
            asg[destruction].push_back(Asg::destruction(N));
            asg[connection].push_back(Asg::connection(N));
            asg[emission].push_back(Asg::emission(N));
            asg[combined].push_back(Asg::combined(N));

            std::cout << "[BEGIN: " << Bs1::LibraryName << "]" << std::endl;

            auto& bs1 = records[Bs1::LibraryName];
            bs1[construction].push_back(Bs1::construction(N));
            bs1[destruction].push_back(Bs1::destruction(N));
            bs1[connection].push_back(Bs1::connection(N));
            bs1[emission].push_back(Bs1::emission(N));
            bs1[combined].push_back(Bs1::combined(N));

            std::cout << "[BEGIN: " << Bs2::LibraryName << "]" << std::endl;

            auto& bs2 = records[Bs2::LibraryName];
            bs2[construction].push_back(Bs2::construction(N));
            bs2[destruction].push_back(Bs2::destruction(N));
            bs2[connection].push_back(Bs2::connection(N));
            bs2[emission].push_back(Bs2::emission(N));
            bs2[combined].push_back(Bs2::combined(N));

            std::cout << "[BEGIN: " << Evl::LibraryName << "]" << std::endl;

            auto& evl = records[Evl::LibraryName];
            evl[construction].push_back(Evl::construction(N));
            evl[destruction].push_back(Evl::destruction(N));
            evl[connection].push_back(Evl::connection(N));
            evl[emission].push_back(Evl::emission(N));
            evl[combined].push_back(Evl::combined(N));

            std::cout << "[BEGIN: " << Jls::LibraryName << "]" << std::endl;

            auto& jls = records[Jls::LibraryName];
            jls[construction].push_back(Jls::construction(N));
            jls[destruction].push_back(Jls::destruction(N));
            jls[connection].push_back(Jls::connection(N));
            jls[emission].push_back(Jls::emission(N));
            jls[combined].push_back(Jls::combined(N));

            std::cout << "[BEGIN: " << Jos::LibraryName << "]" << std::endl;

            auto& jos = records[Jos::LibraryName];
            jos[construction].push_back(Jos::construction(N));
            jos[destruction].push_back(Jos::destruction(N));
            jos[connection].push_back(Jos::connection(N));
            jos[emission].push_back(Jos::emission(N));
            jos[combined].push_back(Jos::combined(N));

            std::cout << "[BEGIN: " << Ksc::LibraryName << "]" << std::endl;

            auto& ksc = records[Ksc::LibraryName];
            ksc[construction].push_back(Ksc::construction(N));
            ksc[destruction].push_back(Ksc::destruction(N));
            ksc[connection].push_back(Ksc::connection(N));
            ksc[emission].push_back(Ksc::emission(N));
            ksc[combined].push_back(Ksc::combined(N));

            std::cout << "[BEGIN: " << Mws::LibraryName << "]" << std::endl;

            auto& mws = records[Mws::LibraryName];
            mws[construction].push_back(Mws::construction(N));
            mws[destruction].push_back(Mws::destruction(N));
            mws[connection].push_back(Mws::connection(N));
            mws[emission].push_back(Mws::emission(N));
            mws[combined].push_back(Mws::combined(N));

            std::cout << "[BEGIN: " << Nls::LibraryName << "]" << std::endl;

            auto& nls = records[Nls::LibraryName];
            nls[construction].push_back(Nls::construction(N));
            nls[destruction].push_back(Nls::destruction(N));
            nls[connection].push_back(Nls::connection(N));
            nls[emission].push_back(Nls::emission(N));
            nls[combined].push_back(Nls::combined(N));

            std::cout << "[BEGIN: " << Nod::LibraryName << "]" << std::endl;

            auto& nod = records[Nod::LibraryName];
            nod[construction].push_back(Nod::construction(N));
            nod[destruction].push_back(Nod::destruction(N));
            nod[connection].push_back(Nod::connection(N));
            nod[emission].push_back(Nod::emission(N));
            nod[combined].push_back(Nod::combined(N));

            std::cout << "[BEGIN: " << Nss::LibraryName << "]" << std::endl;

            auto& nss = records[Nss::LibraryName];
            nss[construction].push_back(Nss::construction(N));
            nss[destruction].push_back(Nss::destruction(N));
            nss[connection].push_back(Nss::connection(N));
            nss[emission].push_back(Nss::emission(N));
            nss[combined].push_back(Nss::combined(N));

            std::cout << "[BEGIN: " << Psg::LibraryName << "]" << std::endl;

            auto& psg = records[Psg::LibraryName];
            psg[construction].push_back(Psg::construction(N));
            psg[destruction].push_back(Psg::destruction(N));
            psg[connection].push_back(Psg::connection(N));
            psg[emission].push_back(Psg::emission(N));
            psg[combined].push_back(Psg::combined(N));

            //std::cout << "[BEGIN: " << Spp::LibraryName << "]" << std::endl;

            //auto& spp = records[Spp::LibraryName];
            //spp[construction].push_back(Spp::construction(N));
            //spp[destruction].push_back(Spp::destruction(N));
            //spp[connection].push_back(Spp::connection(N));
            //spp[emission].push_back(Spp::emission(N));
            //spp[combined].push_back(Spp::combined(N));

            std::cout << "[BEGIN: " << Sss::LibraryName << "]" << std::endl;

            auto& sss = records[Sss::LibraryName];
            sss[construction].push_back(Sss::construction(N));
            sss[destruction].push_back(Sss::destruction(N));
            sss[connection].push_back(Sss::connection(N));
            sss[emission].push_back(Sss::emission(N));
            sss[combined].push_back(Sss::combined(N));

            std::cout << "[BEGIN: " << Wnk::LibraryName << "]" << std::endl;

            auto& wnk = records[Wnk::LibraryName];
            wnk[construction].push_back(Wnk::construction(N));
            wnk[destruction].push_back(Wnk::destruction(N));
            wnk[connection].push_back(Wnk::connection(N));
            wnk[emission].push_back(Wnk::emission(N));
            wnk[combined].push_back(Wnk::combined(N));

            std::cout << "[BEGIN: " << Wsg::LibraryName << "]" << std::endl;

            auto& wsg = records[Wsg::LibraryName];
            wsg[construction].push_back(Wsg::construction(N));
            wsg[destruction].push_back(Wsg::destruction(N));
            wsg[connection].push_back(Wsg::connection(N));
            wsg[emission].push_back(Wsg::emission(N));
            wsg[combined].push_back(Wsg::combined(N));

            std::cout << "[BEGIN: " << Yas::LibraryName << "]" << std::endl;

            auto& yas = records[Yas::LibraryName];
            yas[construction].push_back(Yas::construction(N));
            yas[destruction].push_back(Yas::destruction(N));
            yas[connection].push_back(Yas::connection(N));
            yas[emission].push_back(Yas::emission(N));
            yas[combined].push_back(Yas::combined(N));

            std::cout << "\n[Test Size: " << N << "] END" << std::endl;
        }
    }
    catch (std::exception const& error)
    {
        // Would like to know how we died before we die
        std::cerr << "Exception encountered: " << error.what() << std::endl;
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

        outputReport(records, tee);

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
void outputReport(ImmediateData const& records, T& ost)
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
