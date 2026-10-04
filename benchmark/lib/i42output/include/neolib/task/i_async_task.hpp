// async_task.hpp
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

#pragma once

#include <neolib/neolib.hpp>
#include <neolib/core/i_reference_counted.hpp>
#include <neolib/core/service.hpp>
#include <neolib/app/module.hpp>
#include <neolib/task/i_thread.hpp>
#include <neolib/task/i_message_queue.hpp>
#include <neolib/task/i_task.hpp>
#include <neolib/task/i_event.hpp>

namespace neolib
{
    class i_timer_object;

    class i_async_service : public i_reference_counted
    {
        // types
    public:
        typedef i_async_service abstract_type;
        // constants
    public:
        static constexpr std::size_t kDefaultPollCount = 256;
        // construction
    public:
        virtual ~i_async_service() = default;
        // operations
    public:
        virtual bool poll(bool aProcessEvents = true, std::size_t aMaximumPollCount = kDefaultPollCount) = 0;
        virtual void* native_object() = 0;
        // helpers
    public:
        template <typename NativeObjectType>
        NativeObjectType& native_object()
        {
            return *static_cast<NativeObjectType*>(native_object());
        }
    };

    class i_timer_service : public i_async_service
    {
        // types
    public:
        typedef i_timer_service abstract_type;
        // exceptions
    public:
        struct task_destroying : std::logic_error { task_destroying() : std::logic_error("neolib::i_timer_service::task_destroying") {} };
        // operations
    public:
        virtual i_timer_object& create_timer_object() = 0;
        virtual void remove_timer_object(i_timer_object& aObject) = 0;
    };

    class i_async_task : public i_task, public i_service, public i_reference_counted
    {
        // events
    public:
        declare_event(idle_work)
        declare_event(destroying)
        declare_event(destroyed)
        // exceptions
    public:
        struct no_message_queue : std::logic_error { no_message_queue() : std::logic_error("i_async_task::no_message_queue") {} };
        // types
    public:
        typedef i_async_task abstract_type;
        // operations
    public:
        virtual i_thread& thread() const = 0;
        virtual bool joined() const = 0;
        virtual void join(i_thread& aThread) = 0;
        virtual void detach() = 0;
        virtual i_timer_service& timer_service() = 0;
        virtual i_async_service& io_context(i_module_services& aModuleServices = module_services()) = 0;
        virtual void cancel_io_context(i_module_services& aModuleServices = module_services()) = 0;
        virtual bool have_message_queue() const = 0;
        virtual bool have_messages() const = 0;
        virtual i_message_queue& create_message_queue(std::function<bool()> aIdleFunction = std::function<bool()>{}) = 0;
        virtual const i_message_queue& message_queue() const = 0;
        virtual i_message_queue& message_queue() = 0;
        virtual void register_event_queue(i_async_event_queue& aQueue) = 0;
        virtual void unregister_event_queue(i_async_event_queue& aQueue) = 0;
        virtual bool pump_events() = 0;
        virtual bool pump_messages() = 0;
        virtual bool running() const noexcept = 0;
        virtual bool halted() const noexcept = 0;
        virtual void halt() = 0;
        virtual bool finished() const noexcept = 0;
        virtual void wait() const noexcept = 0;
    public:
        virtual void idle() = 0;
    public:
        static uuid const& iid() { static uuid const sIid{ 0x5e572b8a, 0x272a, 0x40d1, 0xa788, { 0xd7, 0x32, 0xf7, 0x74, 0xfc, 0xe5 } }; return sIid; }
    };
}
