// i_mutex.hpp
/*
 *  Copyright (c) 2020 Leigh Johnston.
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
#include <thread>
#include <chrono>
#include <neolib/core/i_optional.hpp>
#include <neolib/core/i_service.hpp>

#if !defined(NEOS_DISABLE_PROFILE_MUTEX)
#define NEOS_PROFILE_MUTEX
#endif

namespace neolib
{
    struct i_lockable
    {
        virtual void lock() = 0;
        virtual void unlock() = 0;
        virtual bool try_lock() noexcept = 0;
    };

    struct mutex_lock_info
    {
        std::thread::id threadId;
        std::chrono::microseconds duration;
    };

    class i_mutex_profiler_observer
    {
    public:
        virtual void mutex_contended(i_lockable& aMutex, const std::chrono::microseconds& aContendedFor, mutex_lock_info const* aPreviousLocks, std::size_t aPreviousLocksCount) noexcept = 0;
    };

    struct i_mutex_profiler : i_service
    {
        template <typename ProfilerTag, bool Spinlock, bool Yield>
        friend class recursive_mutex;
    public:
        virtual bool enabled(std::chrono::microseconds& aTimeout, std::uint32_t& aMaxCount, bool& aEnhancedMetrics) const noexcept = 0;
        virtual void enable(std::chrono::microseconds aTimeout = std::chrono::microseconds{ 100 }, std::uint32_t aMaxCount = 10u, bool aEnhancedMetrics = false) = 0;
        virtual void disable() noexcept = 0;
    public:
        virtual void subscribe(i_mutex_profiler_observer& aObserver) = 0;
        virtual void unsubscribe(i_mutex_profiler_observer& aObserver) = 0;
    private:
        virtual void notify_contention(i_lockable& aMutex, const std::chrono::microseconds& aContendedFor, mutex_lock_info const* aPreviousLocks, std::size_t aPreviousLocksCount) noexcept = 0;
    public:
        static uuid const& iid() { static uuid const sIid{ 0xc1546ec1, 0x9cfb, 0x4fe7, 0xb93e, { 0x1, 0xc1, 0x2a, 0x5f, 0xf1, 0x62 } }; return sIid; }
    };

}
