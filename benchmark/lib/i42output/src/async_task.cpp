// async_task.cpp
/*
 *  Copyright (c) 2007, 2020 Leigh Johnston.
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
#include <neolib/thread.hpp>
#include <neolib/async_task.hpp>
#include <neolib/timer_object.hpp>

#ifdef _WIN32
#include <neolib/win32_message_queue.hpp>
#endif

namespace neolib
{
    timer_service::timer_service(async_task& aTask, bool aMultiThreaded) :
        iTask{ aTask }
    {
    }

    bool timer_service::poll(bool aProcessEvents, std::size_t aMaximumPollCount)
    {
        std::size_t iterationsLeft = aMaximumPollCount;
        bool didSome = false;
        scoped_dirty sd{ iDirtyObjectList };
        do
        {
            if (iTask.halted())
                return didSome;
            bool didSomeThisIteration = false;
            if (aProcessEvents)
                didSomeThisIteration = (iTask.pump_messages() || didSomeThisIteration);
            for (auto o = iObjects.begin(); o != iObjects.end();)
            {
                if (!*o)
                {
                    o = iObjects.erase(o);
                    continue;
                }
                auto& object = **o;
                if (object.poll())
                {
                    didSomeThisIteration = true;
                    if (aMaximumPollCount != 0 && --iterationsLeft == 0)
                        break;
                }
                if (!iDirtyObjectList.is_dirty())
                    ++o;
                else
                {
                    o = iObjects.begin();
                    iDirtyObjectList.clean();
                }
            }
            if (!didSomeThisIteration)
                break;
            didSome = true;
        } while (aMaximumPollCount != 0 && iterationsLeft > 0);
        return didSome;
    }

    i_timer_object& timer_service::create_timer_object()
    {
        iObjects.push_back(make_ref<timer_object>(*this));
        iDirtyObjectList.dirty();
        return *iObjects.back();
    }

    void timer_service::remove_timer_object(i_timer_object& aObject)
    {
        auto existing = std::find_if(iObjects.begin(), iObjects.end(), [&aObject](auto&& o) { return o == &aObject; });
        if (existing != iObjects.end())
        {
            auto existingRef = std::move(*existing);
            iObjects.erase(existing);
            iDirtyObjectList.dirty();
        }
    }

    io_service::io_service(async_task& aTask, bool aMultiThreaded) :
        iTask{ aTask },
        iNativeIoService{ aMultiThreaded ? BOOST_ASIO_CONCURRENCY_HINT_DEFAULT : BOOST_ASIO_CONCURRENCY_HINT_1 }
    {
    }

    bool io_service::poll(bool aProcessEvents, std::size_t aMaximumPollCount)
    {
        std::size_t iterationsLeft = aMaximumPollCount;
        bool didSome = false;
        iNativeIoService.restart();
        do
        {
            if (iTask.halted())
                return didSome;
            bool didSomeThisIteration = false;
            if (aProcessEvents)
                didSomeThisIteration = (iTask.pump_messages() || didSomeThisIteration);
            didSomeThisIteration = ((aMaximumPollCount == 0 ? iNativeIoService.poll() : iNativeIoService.poll_one()) != 0 || didSomeThisIteration);
            if (!didSomeThisIteration)
                break;
            didSome = true;
        } while (aMaximumPollCount != 0 && --iterationsLeft > 0);
        return didSome;
    }

    async_task::async_task(const std::string& aName) :
        task{ aName }, iThread{ nullptr }, iHalted{ false }
    {
        Destroying.ignore_errors();
    }

    async_task::async_task(i_thread& aThread, const std::string& aName) :
        task{ aName }, iThread{ &aThread }, iHalted{ false }
    {
        Destroying.ignore_errors();
    }

    async_task::~async_task()
    {
        set_destroying();
        if (joined())
            thread().abort();
    }

    i_thread& async_task::thread() const
    {
        if (iThread != nullptr)
            return *iThread;
        throw no_thread();
    }

    bool async_task::joined() const
    {
        return iThread != nullptr;
    }

    void async_task::join(i_thread& aThread)
    {
        iThread = &aThread;
    }

    void async_task::detach()
    {
        iThread = nullptr;
    }

    timer_service& async_task::timer_service()
    {
        if (!iTimerService)
            iTimerService.emplace(*this);
        return *iTimerService;
    }

    io_service& async_task::io_service()
    {
        if (!iIoService)
            iIoService.emplace(*this);
        return *iIoService;
    }

    bool async_task::do_work(yield_type aYieldIfNoWork)
    {
        if (iHalted)
            return false;
        bool didSome = false;
        didSome = (pump_messages() || didSome);
        if (iTimerService)
            didSome = (iTimerService->poll() || didSome);
        if (iIoService)
            didSome = (iIoService->poll() || didSome);
        if (!didSome && aYieldIfNoWork != yield_type::NoYield)
        {
            if (aYieldIfNoWork == yield_type::Yield)
                thread::yield();
            else if (aYieldIfNoWork == yield_type::Sleep)
                thread::sleep(1);
        }
        return didSome;
    }

    bool async_task::have_message_queue() const
    {
        return iMessageQueue != nullptr;
    }

    bool async_task::have_messages() const
    {
        return have_message_queue() && message_queue().have_message();
    }

    i_message_queue& async_task::create_message_queue(std::function<bool()> aIdleFunction)
    {
        #ifdef _WIN32
        iMessageQueue = std::make_unique<win32_message_queue>(*this, aIdleFunction);
        #endif
        return message_queue();
    }

    const i_message_queue& async_task::message_queue() const
    {
        if (iMessageQueue == nullptr)
            throw no_message_queue();
        return *iMessageQueue;
    }

    i_message_queue& async_task::message_queue()
    {
        if (iMessageQueue == nullptr)
            throw no_message_queue();
        return *iMessageQueue;
    }

    bool async_task::pump_messages()
    {
        bool didWork = false;
        while (have_messages())
        {
            if (halted())
                return didWork;
            if (have_message_queue())
                message_queue().get_message();
            idle();
            didWork = true;
        }
        idle();
        return didWork;
    }

    bool async_task::halted() const
    {
        return iHalted;
    }

    void async_task::halt()
    {
        iHalted = true;
    }

    void async_task::set_destroying()
    {
        if (is_alive())
        {
            try
            {
                Destroying.trigger();
            }
            catch (event_queue_destroyed)
            {
            }
            lifetime::set_destroying();
        }
    }

    void async_task::set_destroyed()
    {
        if (!is_destroyed())
        {
            Destroyed.trigger();
            lifetime::set_destroyed();
        }
    }

    void async_task::run(yield_type aYieldType)
    {
        while (!thread().finished())
            do_work(aYieldType);
    }

    void async_task::idle()
    {
        if (have_message_queue())
            message_queue().idle();
    }
} // namespace neolib
