#include <fstream>
#include <iomanip>

#include <boost/filesystem.hpp>

#include "benchmark/hpp/benchmark_aco.hpp"
#include "benchmark/hpp/benchmark_asg.hpp"
#include "benchmark/hpp/benchmark_bs2.hpp"
#include "benchmark/hpp/benchmark_cls.hpp"
#include "benchmark/hpp/benchmark_cps.hpp"
#include "benchmark/hpp/benchmark_cps_st.hpp"
#include "benchmark/hpp/benchmark_css.hpp"
#include "benchmark/hpp/benchmark_dob.hpp"
#include "benchmark/hpp/benchmark_evl.hpp"
#include "benchmark/hpp/benchmark_jls.hpp"
#include "benchmark/hpp/benchmark_jos.hpp"
#include "benchmark/hpp/benchmark_ksc.hpp"
#include "benchmark/hpp/benchmark_lfs.hpp"
#include "benchmark/hpp/benchmark_lss.hpp"
#include "benchmark/hpp/benchmark_mws.hpp"
#include "benchmark/hpp/benchmark_nls.hpp"
#include "benchmark/hpp/benchmark_nls_st.hpp"
#include "benchmark/hpp/benchmark_nod.hpp"
#include "benchmark/hpp/benchmark_nod_st.hpp"
#include "benchmark/hpp/benchmark_nss_st.hpp"
#include "benchmark/hpp/benchmark_nss_sts.hpp"
#include "benchmark/hpp/benchmark_nss_ts.hpp"
#include "benchmark/hpp/benchmark_nss_tss.hpp"
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

// Externs defined in benchmark_utility.hpp
std::size_t g_timer_limit = Timer_u(Limit_u(100)).count();
std::size_t g_best_of_limit = 2;
std::size_t g_start_test_size = 8;
std::size_t g_ending_test_size = 256;

//------------------------------------------------------------------------------

template <auto fun_ptr>
double best_of(std::size_t N)
{
    std::vector<double> results;
    for (std::size_t i = 0; i < g_best_of_limit; ++i)
    {
        results.push_back((*fun_ptr)(N));
    }
    return *std::max_element(results.begin(), results.end());
}

template <typename Benchmark>
void run_benchmark_class(BenchmarkClassResults& records, std::size_t N)
{
    try
    {
        // Time this particular benchmark run (for display only)
        auto start = std::chrono::system_clock::now();
        auto start_out = std::chrono::system_clock::to_time_t(start);

        std::cout << std::put_time(std::localtime(&start_out), "%c")
            << " [BEGIN: " << Benchmark::C_LIB_NAME << "]" << std::endl;

        auto& metrics = records[Benchmark::C_LIB_NAME];

        // Used for switching policies at runtime
        Benchmark::initialize();

        metrics[C_CONSTRUCTION].emplace_back(N, best_of<Benchmark::construction>(N));
        metrics[C_DESTRUCTION].emplace_back(N, best_of<Benchmark::destruction>(N));
        metrics[C_CONNECTION].emplace_back(N, best_of<Benchmark::connection>(N));
        metrics[C_DISCONNECT].emplace_back(N, best_of<Benchmark::disconnect>(N));
        metrics[C_RECONNECT].emplace_back(N, best_of<Benchmark::reconnect>(N));
        metrics[C_EMISSION].emplace_back(N, best_of<Benchmark::emission>(N));
        metrics[C_COMBINED].emplace_back(N, best_of<Benchmark::combined>(N));

        // Only thread safe benchmark classes will have this implemented
        metrics[C_THREADED].emplace_back(N, best_of<Benchmark::threaded>(N));

        auto stop = std::chrono::system_clock::now();
        auto stop_out = std::chrono::system_clock::to_time_t(stop);

        std::cout << std::put_time(std::localtime(&stop_out), "%c")
        << " [END: " << Benchmark::C_LIB_NAME << "]" << std::endl;
    }
    catch (std::exception const& error)
    {
        std::cerr << "Exception: " << error.what() << std::endl;
    }
}

//------------------------------------------------------------------------------

BenchmarkClassResults run_all_benchmarks(std::size_t begin, std::size_t end)
{
    BenchmarkClassResults records;

    // Double the input size N for every iteration
    for (auto N = begin; N <= end; N *= 2)
    {
        std::cout << "[BEGIN: Test Size: " << N << "]\n" << std::endl;

        run_benchmark_class<Aco>(records, N);
        run_benchmark_class<Asg>(records, N);
        run_benchmark_class<Bs2>(records, N);
        //run_benchmark_class<Cls>(records, N); // SlotScope possible bug in destroy_connection
        run_benchmark_class<Cps>(records, N);
        run_benchmark_class<Cps_st>(records, N);
        run_benchmark_class<Css>(records, N);
        //run_benchmark_class<Dob>(records, N); // obs::scoped_connection
        run_benchmark_class<Evl>(records, N);
        run_benchmark_class<Jls>(records, N);
        //run_benchmark_class<Jos>(records, N); // TODO remove from project
        run_benchmark_class<Ksc>(records, N);
        run_benchmark_class<Lfs>(records, N);
        //run_benchmark_class<Lss>(records, N); // updated to latest but still fails
        run_benchmark_class<Mws>(records, N);
        run_benchmark_class<Nls>(records, N);
        run_benchmark_class<Nls_st>(records, N);
        run_benchmark_class<Nod>(records, N);
        run_benchmark_class<Nod_st>(records, N);
        run_benchmark_class<Nss_st>(records, N);
        run_benchmark_class<Nss_sts>(records, N);
        run_benchmark_class<Nss_ts>(records, N);
        run_benchmark_class<Nss_tss>(records, N);
        run_benchmark_class<Psg>(records, N);
        run_benchmark_class<Pss>(records, N);
        run_benchmark_class<Pss_st>(records, N);
        run_benchmark_class<Sss>(records, N);
        //run_benchmark_class<Wnk>(records, N); // TODO remove from project
        run_benchmark_class<Wsg>(records, N);
        run_benchmark_class<Yas>(records, N);
        run_benchmark_class<Vdk>(records, N);

        std::cout << "\n[END: Test Size: " << N << "]" << std::endl;
    }
    return records;
}

//------------------------------------------------------------------------------

void run_all_validation_tests(std::size_t N)
{
    Aco::validate_assert(N);
    Asg::validate_assert(N);
    Bs2::validate_assert(N);
    //Cls::validate_assert(N);
    Cps::validate_assert(N);
    Cps_st::validate_assert(N);
    Css::validate_assert(N);
    //Dob::validate_assert(N);
    Evl::validate_assert(N);
    Jls::validate_assert(N);
    //Jos::validate_assert(N);
    Ksc::validate_assert(N);
    Lfs::validate_assert(N);
    //Lss::validate_assert(N);
    Mws::validate_assert(N);
    Nls::validate_assert(N);
    Nls_st::validate_assert(N);
    Nod::validate_assert(N);
    Nod_st::validate_assert(N);
    Nss_st::validate_assert(N);
    Nss_sts::validate_assert(N);
    Nss_ts::validate_assert(N);
    Nss_tss::validate_assert(N);
    Psg::validate_assert(N);
    Pss::validate_assert(N);
    Pss_st::validate_assert(N);
    Sss::validate_assert(N);
    //Wnk::validate_assert(N);
    Wsg::validate_assert(N);
    Yas::validate_assert(N);
    Vdk::validate_assert(N);
}

//------------------------------------------------------------------------------

template <typename T>
void output_perf_report_header(ReportMethodResults const& first_result_row, T& ost)
{
    std::string header_first_row("| Library | ");

    for (auto const& [op_name, result] : first_result_row)
    {
        header_first_row.append(op_name);
        header_first_row.append(" | ");
    }
    header_first_row += "score |";

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
    }
}

template <typename T>
void output_metrics_report(T& ost)
{
    ost << "| Library | Build Size | Header Only | Data Structure | Thread Safe |\n"
        << "| ------- |:----------:|:-----------:| -------------- |:-----------:|\n";

    output_metrics_report_row<Aco>(ost);
    output_metrics_report_row<Asg>(ost);
    output_metrics_report_row<Bs2>(ost);
    //output_metrics_report_row<Cls>(ost);
    output_metrics_report_row<Cps>(ost);
    output_metrics_report_row<Cps_st>(ost);
    output_metrics_report_row<Css>(ost);
    //output_metrics_report_row<Dob>(ost);
    output_metrics_report_row<Evl>(ost);
    output_metrics_report_row<Jls>(ost);
    //output_metrics_report_row<Jos>(ost);
    output_metrics_report_row<Ksc>(ost);
    output_metrics_report_row<Lfs>(ost);
    //output_metrics_report_row<Lss>(ost);
    output_metrics_report_row<Mws>(ost);
    output_metrics_report_row<Nls>(ost);
    output_metrics_report_row<Nls_st>(ost);
    output_metrics_report_row<Nod>(ost);
    output_metrics_report_row<Nod_st>(ost);
    output_metrics_report_row<Nss_st>(ost);
    output_metrics_report_row<Nss_sts>(ost);
    output_metrics_report_row<Nss_ts>(ost);
    output_metrics_report_row<Nss_tss>(ost);
    output_metrics_report_row<Psg>(ost);
    output_metrics_report_row<Pss>(ost);
    output_metrics_report_row<Pss_st>(ost);
    output_metrics_report_row<Sss>(ost);
    //output_metrics_report_row<Wnk>(ost);
    output_metrics_report_row<Wsg>(ost);
    output_metrics_report_row<Yas>(ost);
    output_metrics_report_row<Vdk>(ost);
}

//------------------------------------------------------------------------------

template <typename T>
void output_plotly_reports(BenchmarkClassResults const& records, T& ost)
{
    std::string lib_columns;
    for (auto const& [lib_name, results] : records)
    {
        lib_columns += std::string("\"") + std::string(lib_name) + ", y;\",";
    }
    ost << "\"Test Size N, x;\"," << lib_columns;

    for (auto c = g_start_test_size; c <= g_ending_test_size; c *= 2)
    {
        ost << "\n" << std::to_string(c) << ",";
        for (auto const& [lib_name, results] : records)
        {
            auto score = 0.0;
            auto sum = 0.0;
            auto ops = 0;

            for (auto const& [op_name, vals] : results)
            {
                if (op_name[0] == '[')
                {
                    continue;
                }

                for (auto const& [N, result] : vals)
                {
                    if (c == N)
                    {
                        sum += result;
                    }
                }
                ++ops;
            }
            score = sum / (double)ops;
            ost << std::to_string(score) << ",";
        }
    }
    ost << "\n";
}

//------------------------------------------------------------------------------

template <typename T>
void output_reports(BenchmarkClassResults const& records, T& ost)
{
    ReportClassResults result_average;
    ReportOrderedResults result_order;

    // Process and sort results by total score (sum of column averages)

    for (auto const& [lib_name, results] : records)
    {
        auto score = 0.0;

        for (auto const& [op_name, vals] : results)
        {
            auto sum = 0.0;
            for (auto const& [N, result] : vals)
            {
                sum += result;
            }
            auto average = sum / (double)vals.size();

            result_average[lib_name][op_name] = average;

            if (op_name[0] != '[')
            {
                score += average;
            }
        }
        result_order[score] = ReportOrderedResult { lib_name, &result_average[lib_name] };
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
    output_plotly_reports(records, ost);
}

//------------------------------------------------------------------------------

int main(int argc, char* argv[])
{
    // Jl_signal uses a custom allocator for maximum performance
    jl::StaticSignalConnectionAllocator<C_JLSIGNAL_MAX> signal_con_allocator;
    jl::StaticObserverConnectionAllocator<C_JLSIGNAL_MAX> observer_con_allocator;
    jl::SignalBase::SetCommonConnectionAllocator(&signal_con_allocator);
    jl::SignalObserver::SetCommonConnectionAllocator(&observer_con_allocator);

    std::size_t user_limit = 0;
    std::size_t start_test_size = g_start_test_size;
    std::size_t maximum_test_size = g_ending_test_size;

    std::cout << "Enter the milliseconds per sample (4000 takes several hours): ";

    if (!(std::cin >> user_limit))
    {
        return 1;
    }
    // Must compile as x64 or else wrapping might happen
    g_timer_limit = Timer_u(Limit_u(user_limit)).count();
    std::cin.ignore();

    std::cout << "Enter the number of rounds per benchmark (best of N runs): ";

    if (!(std::cin >> user_limit))
    {
        return 1;
    }
    // Time to start discarding watt hours
    g_best_of_limit = user_limit;
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
