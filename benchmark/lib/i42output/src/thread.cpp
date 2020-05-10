// thread.cpp - v3.0
/*
 *  Copyright (c) 2007 Leigh Johnston.
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

#include <neolib/neolib.hpp>
#include <iostream>
#include <chrono>
#include <functional>
#include <boost/chrono/thread_clock.hpp>
#include <neolib/singleton.hpp>
#include <neolib/thread.hpp>

namespace neolib
{
    thread::thread(const std::string& aName, bool aAttachToCurrentThread) : 
        iName(aName), 
        iUsingExistingThread(aAttachToCurrentThread), 
        iState(ReadyToStart), 
        iId{ aAttachToCurrentThread ? std::this_thread::get_id() : std::thread::id{} },
        iBlockedCount(0)
    {
    }

    thread::thread(std::function<void()> aExecFunction, const std::string& aName) : 
        thread{ aName, false }
    {
        iExecFunction = aExecFunction;
    }

    thread::~thread()
    {
        if (!finished() && !using_existing_thread())
            abort();
        if (has_thread_object() && thread_object().joinable())
            thread_object().join();
    }

    const std::string& thread::name() const
    {
        return iName;
    }

    bool thread::using_existing_thread() const
    {
        return iUsingExistingThread;
    }

    void thread::start()
    {
        std::optional<std::scoped_lock<std::recursive_mutex>> lock{ iMutex };
        if (started())
        {
            throw thread_already_started();
        }
        try
        {
            iState = Starting;
            if (!iUsingExistingThread)
            {
                iThreadObject = std::make_unique<std::thread>(std::bind(&thread::entry_point, this));
                lock = std::nullopt;
            }
            else
            {
                lock = std::nullopt;
                entry_point();
            }
        }
        catch(const std::exception& aException)
        {
            iState = Error;
            std::cerr << std::string("Error starting thread due to exception being thrown (") + aException.what() + ")." << std::endl;
            throw;
        }
        catch(...)
        {
            iState = Error;
            std::cerr << std::string("Error starting thread due to exception of unknown type being thrown.") << std::endl;
            throw;
        }
    }

    void thread::cancel()
    {
        std::optional<std::scoped_lock<std::recursive_mutex>> lock{ iMutex };
        if (finished())
        {
            return;
        }
        lock = std::nullopt;
        if (!in())
        {
            abort();
            return;
        }
        bool canCancel = false;
        lock.emplace(iMutex);
        if (started())
        {
            iState = Cancelled;
            canCancel = true;
        }
        if (canCancel)
            throw cancellation();
    }

    void thread::abort(bool aWait)
    {
        std::optional<std::scoped_lock<std::recursive_mutex>> lock{ iMutex };
        if (finished())
        {
            return;
        }
        lock = std::nullopt;
        if (in())
        {
            cancel();
            return;
        }
        lock.emplace(iMutex);
        if (started())
        {
            iState = Aborted;
        }
        lock = std::nullopt;
        if (aWait)
            wait();
    }

    void thread::wait() const
    {
        if (!started())
            throw thread_not_started();
        if (in())
            throw cannot_wait_on_self();
        if (thread_object().joinable())
            thread_object().join();
    }

    wait_result thread::wait(const waitable_event_list& aEventList) const
    {
        if (!started())
            throw thread_not_started();
        if (in())
            throw cannot_wait_on_self();
        return aEventList.wait(*this);
    }

    bool thread::msg_wait(const message_queue& aMessageQueue) const
    {
        if (!started())
            throw thread_not_started();
        if (in())
            throw cannot_wait_on_self();
        for(;;)
        {
            if (waitable_ready())
                return true;
            if (aMessageQueue.have_message())
                return false;
            yield();
        }
    }

    wait_result thread::msg_wait(const message_queue& aMessageQueue, const waitable_event_list& aEventList) const
    {
        if (!started())
            throw thread_not_started();
        if (in())
            throw cannot_wait_on_self();
        return aEventList.msg_wait(aMessageQueue, *this);
    }

    void thread::block()
    {
        if (!in())
            throw not_in_thread();
        bool shouldCancel = false;
        {
            std::scoped_lock<std::recursive_mutex> lock{ iMutex };
            ++iBlockedCount;
            shouldCancel = aborted();
        }
        if (shouldCancel)
            throw cancellation();
    }

    void thread::unblock()
    {
        if (!in())
            throw not_in_thread();
        bool shouldCancel = false;
        {
            std::scoped_lock<std::recursive_mutex> lock{ iMutex };
            --iBlockedCount;
            shouldCancel = aborted();
        }
        if (shouldCancel)
            throw cancellation();
    }

    bool thread::started() const 
    { 
        return iState != ReadyToStart; 
    }

    bool thread::running() const
    {
        return iState == Started;
    }
    
    bool thread::finished() const 
    { 
        return iState != ReadyToStart && iState != Starting && iState != Started; 
    }
    
    bool thread::aborted() const 
    { 
        return iState == Aborted; 
    }
    
    bool thread::cancelled() const 
    { 
        return iState == Cancelled; 
    }
    
    bool thread::error() const 
    { 
        return iState == Error; 
    }
    
    thread::id_type thread::id() const 
    { 
        return iId; 
    }

    bool thread::in() const 
    {
        if (!started() && !using_existing_thread())
            throw thread_not_started();
        return std::this_thread::get_id() == iId;
    }

    bool thread::blocked() const 
    { 
        return iBlockedCount != 0; 
    }

    bool thread::has_thread_object() const
    {
        return iThreadObject != nullptr;
    }

    thread::thread_object_type& thread::thread_object() const
    { 
        if (!has_thread_object()) 
            throw no_thread_object(); 
        return *iThreadObject; 
    }

    void thread::sleep(uint32_t aDelayInMilleseconds)
    {
        std::this_thread::sleep_for(std::chrono::milliseconds(aDelayInMilleseconds));
    }

    void thread::yield()
    {
        std::this_thread::yield();
    }

    uint64_t thread::elapsed_ms()
    {
        return elapsed_us() / 1000;
    }

    uint64_t thread::elapsed_us()
    {
        return elapsed_ns() / 1000;
    }

    uint64_t thread::elapsed_ns()
    {
        using namespace boost::chrono;
        return duration_cast<nanoseconds>(thread_clock::time_point(thread_clock::now()).time_since_epoch()).count();
    }

    uint64_t thread::program_elapsed_ms()
    {
        return program_elapsed_us() / 1000;
    }

    uint64_t thread::program_elapsed_us()
    {
        return program_elapsed_ns() / 1000;
    }

    namespace
    {
        uint64_t sProgramStartTime_ns = std::chrono::duration_cast<std::chrono::nanoseconds>(std::chrono::high_resolution_clock::time_point(std::chrono::high_resolution_clock::now()).time_since_epoch()).count();
    }

    uint64_t thread::program_elapsed_ns()
    {
        return std::chrono::duration_cast<std::chrono::nanoseconds>(std::chrono::high_resolution_clock::time_point(std::chrono::high_resolution_clock::now()).time_since_epoch()).count() - sProgramStartTime_ns;
    }

    bool thread::waitable_ready() const
    {
        return thread_object().get_id() == std::thread::id();
    }

    void thread::exec_preamble()
    {
    }

    void thread::exec(yield_type)
    {
        if (iExecFunction != std::nullopt)
            (*iExecFunction)();
        else
            throw nothing_to_do();
    }

    void thread::entry_point()
    {
        {
            std::scoped_lock<std::recursive_mutex> lock{ iMutex };
            iState = Started;
            iId = std::this_thread::get_id();
        }
        try
        {
            exec_preamble();
            exec();
            if (!iUsingExistingThread)
            {
                std::scoped_lock<std::recursive_mutex> lock{ iMutex };
                if (iState == Started)
                    iState = Finished;
            }
        }
        catch(const std::exception& aException)
        {
            std::cerr << std::string("Thread terminating due to an uncaught exception was being thrown (") + aException.what() + ")." << std::endl;
            throw;
        }
        catch(...)
        {
            std::cerr << "Thread terminating due to an uncaught exception of unknown type being thrown." << std::endl;
            throw;
        }
    }
} // namespace neolib
