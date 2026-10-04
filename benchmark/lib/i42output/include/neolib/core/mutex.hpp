// mutex.hpp
/*
 *  Copyright (c) 2020-2025 Leigh Johnston.
 *
 *  All rights reserved.
 *
 *  Redistribution and use in source and binary forms, with or without
 *  modification, are permitted provided that the following conditions are
 *  met:
 *
 *     * Redistributions of source code must retain the above copyright
 *       notice, this list of conditions and the following disclaimer.
 *
 *     * Redistributions in binary form must reproduce the above copyright
 *       notice, this list of conditions and the following disclaimer in the
 *       documentation and/or other materials provided with the distribution.
 *
 *     * Neither the name of Leigh Johnston nor the names of any
 *       other contributors to this software may be used to endorse or
 *       promote products derived from this software without specific prior
 *       written permission.
 *
 *  THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS
 *  IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO,
 *  THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR
 *  PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT OWNER OR
 *  CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL,
 *  EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO,
 *  PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR
 *  PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF
 *  LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING
 *  NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS
 *  SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
*/

#pragma once

#include <neolib/neolib.hpp>
#include <atomic>
#include <cstdint>
#include <mutex>
#include <thread>
#include <chrono>
#if defined(__x86_64__) || defined(_M_X64) || defined(__i386) || defined(_M_IX86)
#include <immintrin.h>
#endif
#include <boost/unordered/unordered_flat_map.hpp>
#include <boost/thread/locks.hpp>
#include <boost/version.hpp>
#include <boost/lockfree/detail/freelist.hpp>
#include <boost/lockfree/detail/prefix.hpp>
#include <boost/fiber/detail/cpu_relax.hpp>
#include <neolib/core/optional.hpp>
#include <neolib/core/i_mutex.hpp>
#include <neolib/core/service.hpp>

namespace neolib
{
#if BOOST_VERSION >= 108800
    constexpr std::size_t cacheline_bytes = boost::lockfree::detail::cacheline_bytes;
#else
    constexpr std::size_t cacheline_bytes = BOOST_LOCKFREE_CACHELINE_BYTES;
#endif

    class mutex_profiler : public i_mutex_profiler
    {
        struct params
        {
            std::uint32_t timeout_us = 100u;
            std::uint16_t maxCount = 10u;
            bool enabled = false;
            bool enhancedMetrics = false;
        };
        static_assert(std::atomic<params>::is_always_lock_free, "neolib::mutex_profiler::params must be lock-free on all platforms!");
    public:
        bool enabled(std::chrono::microseconds& aTimeout, std::uint32_t& aMaxCount, bool& aEnhancedMetrics) const noexcept final
        {
            auto const p = iParams.load();
            if (p.enabled)
            {
                aTimeout = std::chrono::microseconds{ p.timeout_us };
                aMaxCount = p.maxCount;
                aEnhancedMetrics = p.enhancedMetrics;
            }
            return p.enabled;
        }
        void enable(std::chrono::microseconds aTimeout = std::chrono::microseconds{ 100 }, std::uint32_t aMaxCount = 10u, bool aEnhancedMetrics = false) final
        {
            if (aTimeout.count() > std::numeric_limits<decltype(params{}.timeout_us)>::max() ||
                aMaxCount > std::numeric_limits<decltype(params{}.maxCount)>::max())
                throw std::logic_error("neolib::mutex_profiler::enable: param(s) exceed limit(s)");
            iParams.store(params{
                static_cast<decltype(params{}.timeout_us)>(aTimeout.count()),
                static_cast<decltype(params{}.maxCount)>(aMaxCount),
                true,
                aEnhancedMetrics });
        }
        void disable() noexcept final
        {
            iParams.store(params{});
        }
    public:
        void subscribe(i_mutex_profiler_observer& aObserver) final
        {
            std::unique_lock lock{ iMutex };
            iSubscribers.push_back(&aObserver);
        }
        void unsubscribe(i_mutex_profiler_observer& aObserver) final
        {
            std::unique_lock lock{ iMutex };
            auto existing = std::find(iSubscribers.begin(), iSubscribers.end(), &aObserver);
            if (existing != iSubscribers.end())
                iSubscribers.erase(existing);
        }
    private:
        void notify_contention(
            i_lockable& aMutex, const std::chrono::microseconds& aContendedFor, mutex_lock_info const* aPreviousLocks, std::size_t aPreviousLocksCount) noexcept final
        {
            std::unique_lock lock{ iMutex };
            for (auto& subscriber : iSubscribers)
                subscriber->mutex_contended(aMutex, aContendedFor, aPreviousLocks, aPreviousLocksCount);
        }
    private:
        std::atomic<params> iParams;
        mutable std::recursive_mutex iMutex;
        std::vector<i_mutex_profiler_observer*> iSubscribers;

    };

    struct null_mutex : public i_lockable
    {
        void lock() final {}
        void unlock() final {}
        bool try_lock() noexcept final { return true; }
    };

    template <typename Subject>
    class proxy_mutex : public i_lockable
    {
    public:
        proxy_mutex(Subject& aSubject) :
            iSubject{ &aSubject }
        {
        }
    public:
        void lock() final
        {
            iSubject->lock();
        }
        void unlock() final
        {
            iSubject->unlock();
        }
        bool try_lock() noexcept final
        {
            return iSubject->try_lock();
        }
    private:
        Subject* iSubject;
    };

    template <typename... MutexTypes>
    class multi_mutex : public i_lockable
    {
    public:
        multi_mutex() = default;
        template <typename MutexType, typename... Args>
        explicit multi_mutex(std::in_place_type_t<MutexType>, Args&&... aArgs) :
            iMutex{ std::in_place_type<MutexType>, std::forward<Args>(aArgs)... }
        {
        }
    public:
        template <typename MutexType, typename... Args>
        MutexType& emplace(Args&&... aArgs)
        {
            return iMutex.template emplace<MutexType>(std::forward<Args>(aArgs)...);
        }
    public:
        template <typename MutexType>
        bool is() const
        {
            return std::holds_alternative<MutexType>(iMutex);
        }
    public:
        void lock() final 
        {
            std::visit([](auto& mutex)
                {
                    mutex.lock();
                }, iMutex);
        }
        void unlock() final 
        {
            std::visit([](auto& mutex)
                {
                    mutex.unlock();
                }, iMutex);
        }
        bool try_lock() noexcept final
        {
            return std::visit([](auto& mutex) -> bool
                {
                    return mutex.try_lock();
                }, iMutex);
        }
    private:
        std::variant<MutexTypes...> iMutex;
    };

    namespace this_thread
    {
        namespace lightweight
        {
            using thread_id = std::thread::id const*;

            inline thread_id get_id()
            {
                thread_local std::thread::id tId = std::this_thread::get_id();
                return &tId;
            }
        }
    }

    template <typename ProfilerTag = void, bool Spinlock = false, bool Yield = false>
    class alignas(cacheline_bytes) recursive_mutex : public i_lockable
    {
    private:
        using metrics_list = std::vector<mutex_lock_info>;
#if defined(NEOS_PROFILE_MUTEX)
        struct metrics_key
        {
            recursive_mutex const* ptr;
            std::uint64_t generation;
            friend bool operator==(metrics_key const& a, metrics_key const& b) noexcept
            {
                return a.ptr == b.ptr && a.generation == b.generation;
            }
        };

        struct metrics_key_hash
        {
            std::size_t operator()(metrics_key const& k) const noexcept
            {
                auto const p = reinterpret_cast<std::uintptr_t>(k.ptr);
                auto const g = static_cast<std::uintptr_t>(k.generation);
                return static_cast<std::size_t>(p ^ (g + 0x9e3779b97f4a7c15ULL + (p << 6) + (p >> 2)));
            }
        };
        using metrics_map = boost::unordered_flat_map<metrics_key, metrics_list, metrics_key_hash>;
#else
        using metrics_map = boost::unordered_flat_map<recursive_mutex const*, metrics_list>;
#endif
    private:
#if defined(NEOS_PROFILE_MUTEX)
        static inline int sIcfSingleton;
        static inline std::atomic<std::uint64_t> sGenerationCounter{ 0u };
        void prevent_icf() const noexcept
        {
            static volatile int sink = 42;
            (void)sink;
            (void)sIcfSingleton;
        }
#else
        void prevent_icf() const noexcept
        {
        }
#endif

        struct tuning
        {
            std::uint32_t spinIters;
            std::uint32_t yieldEvery;
        };

        enum class tuning_state : std::uint8_t
        {
            Uninitialized = 0,
            Initializing = 1,
            Initialized = 2
        };

        static inline std::atomic<tuning_state> sTuningState{ tuning_state::Uninitialized };
        static inline tuning sTuning{ 200u, 50u }; // safe fallback

        static tuning compute_tuning() noexcept
        {
            using clock = std::chrono::high_resolution_clock;
            using ns = std::chrono::nanoseconds;

            constexpr ns spinBudget = std::chrono::nanoseconds{ 500 };
            constexpr ns yieldPeriod = std::chrono::microseconds{ 1 };
            constexpr std::uint32_t warmupIters = 128;
            constexpr std::uint32_t sampleIters = 1024;

            for (std::uint32_t i = 0; i < warmupIters; ++i)
                cpu_relax();

            auto const t0 = clock::now();
            for (std::uint32_t i = 0; i < sampleIters; ++i)
                cpu_relax();
            auto const t1 = clock::now();

            auto const total = std::chrono::duration_cast<ns>(t1 - t0);
            auto const perRelax = (total.count() > 0) ? (total / sampleIters) : ns{ 1 };

            std::uint64_t spinIters64 =
                (perRelax.count() > 0) ? static_cast<std::uint64_t>(spinBudget / perRelax) : 200ull;

            // Clamp to avoid pathological CPU burn or weird clock behaviour.
            if (spinIters64 < 50ull)   spinIters64 = 50ull;
            if (spinIters64 > 5000ull) spinIters64 = 5000ull;

            std::uint64_t yieldEvery64 =
                (perRelax.count() > 0) ? static_cast<std::uint64_t>(yieldPeriod / perRelax) : 50ull;

            if (yieldEvery64 < 1ull) yieldEvery64 = 1ull;
            if (yieldEvery64 > spinIters64) yieldEvery64 = spinIters64;

            return tuning{ static_cast<std::uint32_t>(spinIters64), static_cast<std::uint32_t>(yieldEvery64) };
        }

        static tuning get_tuning() noexcept
        {
            if constexpr (!Spinlock)
                return {};
            else
            {

                if (sTuningState.load(std::memory_order_acquire) == tuning_state::Initialized)
                    return sTuning;

                auto expected = tuning_state::Uninitialized;
                if (sTuningState.compare_exchange_strong(expected, tuning_state::Initializing, std::memory_order_acq_rel))
                {
                    sTuning = compute_tuning();
                    sTuningState.store(tuning_state::Initialized, std::memory_order_release);
                    return sTuning;
                }

                while (sTuningState.load(std::memory_order_acquire) != tuning_state::Initialized)
                    cpu_relax();

                return sTuning;
            }
        }

    public:
        recursive_mutex() :
            iLockCount{ 0u },
            iLockingThread{},
            iState{}
#if defined(NEOS_PROFILE_MUTEX)
            , iGeneration{ sGenerationCounter.fetch_add(1u, std::memory_order_relaxed) + 1u }
#endif
        {
        }
        ~recursive_mutex()
        {
            assert(!iState.test(std::memory_order_acquire));
        }
    public:
        void lock() final
        {
            prevent_icf();
            auto const thisThread = this_thread::lightweight::get_id();
            if (iState.test(std::memory_order_relaxed) && 
                iLockingThread.load(std::memory_order_relaxed) == thisThread)
            {
                ++iLockCount;
                return;
            }

            auto acquire_lock = [&](
                auto&& on_contended,
                auto&& on_before_wait,
                auto&& on_after_wait)
                {
                    auto const tune = get_tuning();
                    for (;;)
                    {
                        if (!iState.test(std::memory_order_relaxed) &&
                            !iState.test_and_set(std::memory_order_acquire))
                            return;

                        on_contended();

                        if constexpr (Spinlock)
                        {
                            std::uint32_t untilNextYield = tune.yieldEvery;
                            for (std::uint32_t i = 0; i < tune.spinIters; ++i)
                            {
                                cpu_relax();
                                if constexpr (Yield)
                                {
                                    if (--untilNextYield == 0)
                                    {
                                        untilNextYield = tune.yieldEvery;
                                        std::this_thread::yield();
                                    }
                                }
                                if (!iState.test(std::memory_order_relaxed) &&
                                    !iState.test_and_set(std::memory_order_acquire))
                                {
                                    return;
                                }
                            }
                        }

                        on_before_wait();
                        iState.wait(true, std::memory_order_relaxed);
                        on_after_wait();
                    }
                };
#if !defined(NEOS_PROFILE_MUTEX)
            acquire_lock([]{}, []{}, []{});
#else
            static auto& serviceProfiler = service<i_mutex_profiler>();
            std::chrono::microseconds timeout;
            std::uint32_t maxCount;
            bool enhancedMetrics;
            if (serviceProfiler.enabled(timeout, maxCount, enhancedMetrics))
            {
                static metrics_list sNullMetrics;
                auto& m = (enhancedMetrics ? metrics() : sNullMetrics);
                if (enhancedMetrics)
                    m.clear();
                std::optional<std::chrono::high_resolution_clock::time_point> start;
                std::optional<std::chrono::high_resolution_clock::time_point> next;
                acquire_lock(
                    [&]
                    {
                        if (!start.has_value())
                        {
                            start = std::chrono::high_resolution_clock::now();
                            next = start;
                        }
                    },
                    [&]
                    {
                        if (enhancedMetrics)
                        {
                            auto const lockingThread = iLockingThread.load();
                            m.emplace_back(lockingThread != nullptr ? *lockingThread : std::thread::id{}, std::chrono::microseconds{});
                        }
                    },
                    [&]
                    {
                        if (enhancedMetrics)
                        {
                            auto const now = std::chrono::high_resolution_clock::now();
                            m.back().duration = std::chrono::duration_cast<std::chrono::microseconds>(now - next.value());
                            next = now;
                        }
                    });

                if (start.has_value())
                {
                    auto const end = std::chrono::high_resolution_clock::now();
                    if (end - start.value() > timeout && ++iPathologicalContentionCounter > maxCount)
                    {
                        serviceProfiler.notify_contention(*this, std::chrono::duration_cast<std::chrono::microseconds>(end - start.value()), m.empty() ? nullptr : m.data(), m.size());
                        iPathologicalContentionCounter = 0u;
                    }
                }
            }
            else
            {
                acquire_lock([]{}, []{}, []{});
            }
#endif
            iLockingThread.store(thisThread, std::memory_order_relaxed);
            ++iLockCount;
        }
        void unlock() final
        {
            prevent_icf();
            if (--iLockCount == 0u)
            {
                iLockingThread.store(nullptr, std::memory_order_relaxed);
                iState.clear(std::memory_order_release);
                iState.notify_one();
            }
        }
        bool try_lock() noexcept final
        {
            prevent_icf();
            auto const thisThread = this_thread::lightweight::get_id();
            if (iState.test(std::memory_order_relaxed) && 
                iLockingThread.load(std::memory_order_relaxed) == thisThread)
            {
                ++iLockCount;
                return true;
            }
            if (iState.test_and_set(std::memory_order_acquire))
                return false;
            iLockingThread.store(thisThread, std::memory_order_relaxed);
            ++iLockCount;
            return true;
        }
    private:
        metrics_list& metrics() const noexcept
        {
            thread_local metrics_map tMap;
#if defined(NEOS_PROFILE_MUTEX)
            return tMap[metrics_key{ std::launder(this), iGeneration }];
#else
            return tMap[std::launder(this)];
#endif
        }
    private:
        std::atomic<std::uint32_t> iLockCount;
        std::atomic<this_thread::lightweight::thread_id> iLockingThread;
        std::atomic_flag iState;
#if defined(NEOS_PROFILE_MUTEX)
        std::atomic<std::uint32_t> iPathologicalContentionCounter = 0u;
        std::uint64_t iGeneration = 0u;
#endif
    };

    template <typename ProfilerTag = void>
    class alignas(cacheline_bytes) switchable_mutex : public i_lockable
    {
    public:
        switchable_mutex()
        {
            set_multi_threaded();
        }
    public:
        void set_single_threaded()
        {
            iActiveMutex.template emplace<neolib::null_mutex>();
        }
        void set_multi_threaded()
        {
            iActiveMutex.template emplace<std::recursive_mutex>();
        }
        void set_multi_threaded_profiled()
        {
            iActiveMutex.template emplace<neolib::recursive_mutex<ProfilerTag>>();
        }
        void set_multi_threaded_spinlock()
        {
            iActiveMutex.template emplace<neolib::recursive_mutex<ProfilerTag, true>>();
        }
    public:
        void lock() final
        {
            std::visit([](auto& mutex) { mutex.lock(); }, iActiveMutex);
        }
        void unlock() final
        {
            std::visit([](auto& mutex) { mutex.unlock(); }, iActiveMutex);
        }
        bool try_lock() noexcept final
        {
            bool result = false;
            std::visit([&](auto& mutex) { result = mutex.try_lock(); }, iActiveMutex);
            return result;
        }
    private:
        std::variant<std::recursive_mutex, neolib::recursive_mutex<ProfilerTag>, neolib::recursive_mutex<ProfilerTag, true>, neolib::null_mutex> iActiveMutex;
    };

    template <typename Mutexes>
    class scoped_multi_lock
    {
    public:
        scoped_multi_lock(Mutexes& aMutexes) :
            iMutexes{ aMutexes }
        {
            boost::lock(iMutexes.begin(), iMutexes.end());
        }
        ~scoped_multi_lock()
        {
            for (auto& m : iMutexes)
                m.unlock();
        }
    private:
        Mutexes& iMutexes;
    };
}