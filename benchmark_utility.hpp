#ifndef MISCELLANEOUS_HPP
#define MISCELLANEOUS_HPP

#include <algorithm>
#include <cassert>
#include <chrono>
#include <future>
#include <memory>
#include <random>
#include <thread>
#include <vector>
#include <tuple>

#ifdef _WIN32
#define NOINLINE(s) __declspec(noinline) s
#elif __unix__
#define NOINLINE(s) __attribute__ ((noinline)) s
#else
#define NOINLINE(s) s
#endif

extern std::size_t g_limit;
const std::size_t c_jlsignal_max = 1024;

typedef std::minstd_rand Rng;

typedef std::chrono::nanoseconds Timer_u;
typedef std::chrono::milliseconds Limit_u;
typedef std::chrono::duration<double, std::milli> Delta_u;

//------------------------------------------------------------------------------

typedef std::shared_ptr<void> SlotScope;

template <typename Deleter>
SlotScope make_slot_scope(Deleter&& deleter)
{
    return SlotScope((void*)0xDEADC0DE, deleter);
}

//------------------------------------------------------------------------------

// based stackoverflow

template <unsigned int N>
struct tee_stream
{
    template <typename ...Args, typename T>
    static std::tuple<Args...>& print(std::tuple<Args...>& t, T&& x)
    {
        std::get<sizeof...(Args) - N>(t) << x;
        tee_stream<N - 1>::print(t, std::forward<T>(x));
        return t;
    }
};
template <>
struct tee_stream<0>
{
    template <typename ...Args, typename T>
    static std::tuple<Args...>& print(std::tuple<Args...>& t, T&& x)
    {
        return t;
    }
};
template <typename ...Args, typename T>
std::tuple<Args...>& operator<< (std::tuple<Args...>& t, T&& x)
{
    return tee_stream<sizeof...(Args)>::print(t, std::forward<T>(x));
}
template <typename ...Args, typename T>
std::tuple<Args...>& operator<< (std::tuple<Args...>&& t, T&& x)
{
    return tee_stream<sizeof...(Args)>::print(t, std::forward<T>(x));
}

//------------------------------------------------------------------------------

template <typename Iterator> struct RangeHelper
{
    Iterator lhs;
    Iterator rhs;

    Iterator cbegin() const { return lhs; }
    Iterator begin() const { return lhs; }
    Iterator cend() const { return rhs; }
    Iterator end() const { return rhs; }
};
template <typename Iterator>
static RangeHelper<Iterator> Range(Iterator lhs,
                                   Iterator rhs)
{
    return { lhs, rhs };
}

//------------------------------------------------------------------------------

class ChronoTimer
{
    std::chrono::time_point<std::chrono::high_resolution_clock> m_start;

    public:

    ChronoTimer() : m_start(std::chrono::high_resolution_clock::now())
    {
    
    }

    void reset()
    {
        new (this) ChronoTimer;
    }
    template <typename T>
    std::size_t count() const
    {
        return std::chrono::duration_cast<T>
          (std::chrono::high_resolution_clock::now() - m_start).count();
    }
};

//------------------------------------------------------------------------------

struct IncrementFill
{
    std::size_t i = 0;
    std::size_t operator()() { return i++; }
};


#endif // MISCELLANEOUS_HPP