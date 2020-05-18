// i_timer_object.hpp
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
#include <neolib/reference_counted.hpp>

namespace neolib
{
    class i_timer_object;

    class i_timer_subscriber : public i_reference_counted
    {
    public:
        typedef i_timer_subscriber abstract_type;
    public:
        virtual ~i_timer_subscriber() = default;
    public:
        virtual void timer_expired(i_timer_object& aTimerObject) = 0;
    public:
        friend bool operator<(const i_timer_subscriber& aLeft, const i_timer_subscriber& aRight)
        {
            return &aLeft < &aRight;
        }
    };

    class i_timer_object : public i_reference_counted
    {
    public:
        typedef i_timer_object abstract_type;
    public:
        struct subscriber_not_found : std::logic_error { subscriber_not_found() : std::logic_error{ "neolib::i_timer_object::subscriber_not_found" } {} };
    private:
        class subscriber_wrapper : public reference_counted<i_timer_subscriber>
        {
        public:
            subscriber_wrapper(std::function<void()> aCallback) :
                iCallback{ aCallback }
            {
            }
        protected:
            void timer_expired(i_timer_object&) override
            {
                iCallback();
            }
        private:
            std::function<void()> iCallback;
        };
    public:
        virtual ~i_timer_object() = default;
    public:
        virtual void expires_at(const std::chrono::steady_clock::time_point& aDeadline) = 0;
        virtual void async_wait(i_timer_subscriber& aSubscriber) = 0;
        virtual void unsubscribe(i_timer_subscriber& aSubscriber) = 0;
        virtual void cancel() = 0;
    public:
        virtual bool poll() = 0;
    public:
        template <typename Duration>
        void expires_from_now(const Duration& aDuration)
        {
            expires_at(std::chrono::steady_clock::now() + aDuration);
        }
        i_timer_subscriber& async_wait(const std::function<void()>& aSubscriber)
        {
            auto subscriber = make_ref<subscriber_wrapper>([aSubscriber]() { aSubscriber(); });
            async_wait(*subscriber);
            return *subscriber;
        }
        i_timer_subscriber& async_wait(const std::function<void(i_timer_object&)>& aSubscriber)
        {
            auto subscriber = make_ref<subscriber_wrapper>([this, aSubscriber]() { aSubscriber(*this); });
            async_wait(*subscriber);
            return *subscriber;
        }
    };
}
