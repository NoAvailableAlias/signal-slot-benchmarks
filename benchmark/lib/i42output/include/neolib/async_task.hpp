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
#include <boost/asio.hpp>
#include <neolib/lifetime.hpp>
#include <neolib/i_thread.hpp>
#include <neolib/task.hpp>
#include <neolib/i_async_task.hpp>
#include <neolib/i_timer_object.hpp>
#include <neolib/dirty_list.hpp>
#include <neolib/plugin_event.hpp>

namespace neolib
{
    class async_task;

    class timer_service : public i_timer_service
    {
        // types
    public:
        // construction
    public:
        timer_service(async_task& aTask, bool aMultiThreaded = false);
        // operations
    public:
        bool poll(bool aProcessEvents = true, std::size_t aMaximumPollCount = kDefaultPollCount) override;
        i_timer_object& create_timer_object() override;
        void remove_timer_object(i_timer_object& aObject) override;
        // attributes
    private:
        async_task& iTask;
        std::vector<ref_ptr<i_timer_object>> iObjects;
        dirty_list iDirtyObjectList;
    };

    class io_service : public i_async_service
    {
        // types
    public:
        typedef boost::asio::io_service native_io_service_type;
        // construction
    public:
        io_service(async_task& aTask, bool aMultiThreaded = false);
        // operations
    public:
        bool poll(bool aProcessEvents = true, std::size_t aMaximumPollCount = kDefaultPollCount) override;
        native_io_service_type& native_object() { return iNativeIoService; }
        // attributes
    private:
        async_task& iTask;
        native_io_service_type iNativeIoService;
    };

    class async_task : public task<i_async_task>, public lifetime
    {
        friend class async_thread;
        // events
    public:
        define_declared_event(Destroying, destroying)
        define_declared_event(Destroyed, destroyed)
        // exceptions
    public:
        struct no_thread : std::logic_error { no_thread() : std::logic_error{ "neolib::async_task::no_thread" } {} };
        // types
    private:
        typedef std::unique_ptr<i_message_queue> message_queue_pointer;
        // construction
    public:
        async_task(const std::string& aName = std::string{});
        async_task(i_thread& aThread, const std::string& aName = std::string{});
        ~async_task();
        // operations
    public:
        i_thread& thread() const override;
        bool joined() const override;
        void join(i_thread& aThread) override;
        void detach() override;
        neolib::timer_service& timer_service() override;
        neolib::io_service& io_service() override;
        bool have_message_queue() const override;
        bool have_messages() const override;
        i_message_queue& create_message_queue(std::function<bool()> aIdleFunction = std::function<bool()>()) override;
        const i_message_queue& message_queue() const override;
        i_message_queue& message_queue() override;
        bool pump_messages() override;
        bool halted() const override;
        void halt() override;
        // implementation
    protected:
        // i_lifetime
        void set_destroying() override;
        void set_destroyed() override;
        // task
        void run(yield_type aYieldType = yield_type::NoYield) override;
        bool do_work(yield_type aYieldType = yield_type::NoYield) override;
        void idle() override;
        // attributes
    private:
        i_thread* iThread;
        std::optional<neolib::timer_service> iTimerService;
        std::optional<neolib::io_service> iIoService;
        message_queue_pointer iMessageQueue;
        bool iHalted;
    };
}
