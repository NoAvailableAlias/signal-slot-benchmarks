// async_task.hpp v1.0
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

#pragma once

#include <neolib/neolib.hpp>
#include <neolib/i_thread.hpp>
#include <neolib/i_plugin_event.hpp>

namespace neolib
{
    class i_io_service
    {
        // constants
    public:
        static constexpr std::size_t kDefaultPollCount = 256;
        // construction
    public:
        virtual ~i_io_service() = default;
        // operations
    public:
        virtual bool do_io(bool aProcessEvents = true, std::size_t aMaximumPollCount = kDefaultPollCount) = 0;
    };

    class i_async_task : public i_task
    {
        // events
    public:
        declare_event(destroying)
        declare_event(destroyed)
        // exceptions
    public:
        struct no_message_queue : std::logic_error { no_message_queue() : std::logic_error("neolib::i_async_task::no_message_queue") {} };
        // operations
    public:
        virtual i_thread& thread() const = 0;
        virtual bool do_io(yield_type aYieldIfNoWork = yield_type::NoYield) = 0;
        virtual i_io_service& timer_io_service() = 0;
        virtual i_io_service& networking_io_service() = 0;
        virtual bool have_message_queue() const = 0;
        virtual bool have_messages() const = 0;
        virtual bool pump_messages() = 0;
        virtual bool halted() const = 0;
        virtual void halt() = 0;
        virtual void idle() = 0;
    };
}
