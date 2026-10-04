// i_event.hpp
/*
 *  Copyright (c) 2021 Leigh Johnston.
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
#include <functional>
#include <neolib/core/mutex.hpp>
#include <neolib/core/lifetime.hpp>
#include <neolib/core/reference_counted.hpp>

namespace neolib
{
    enum class event_system_locking_strategy
    {
        SingleThreaded,
        MultiThreaded,
        MultiThreadedProfiled,
        MultiThreadedSpinlock
    };

    class i_event_system : public i_service
    {
    public:
        virtual event_system_locking_strategy locking_strategy() const noexcept = 0;
        virtual void set_locking_strategy(event_system_locking_strategy aStrategy) noexcept = 0;
    public:
        static uuid const& iid() { static uuid const sIid{ 0x9f84fbad, 0xc980, 0x4d71, 0xb4b0, { 0x89, 0xb5, 0x7b, 0x94, 0xdb, 0xfe } }; return sIid; }
    };

    template <typename ProfilerTag = void>
    class event_mutex : public switchable_mutex<ProfilerTag>
    {
    public:
        event_mutex()
        {
            switch (services::service<i_event_system>().locking_strategy())
            {
            case event_system_locking_strategy::SingleThreaded:
                this->set_single_threaded();
                break;
            case event_system_locking_strategy::MultiThreaded:
                this->set_multi_threaded();
                break;
            case event_system_locking_strategy::MultiThreadedProfiled:
                this->set_multi_threaded_profiled();
                break;
            case event_system_locking_strategy::MultiThreadedSpinlock:
                this->set_multi_threaded_spinlock();
                break;
            }
        }
    };

    class i_async_task;

    class i_async_event_queue : public i_lifetime
    {
    public:
        virtual void register_with_task(i_async_task& aTask) = 0;
        virtual bool pump_events() = 0;
    };

    template <typename... Args>
    class i_event;
        
    class i_slot_base : public i_reference_counted, public i_lifetime
    {
    public:
        typedef i_slot_base abstract_type;
    public:
        virtual void remove() = 0;
    };

    template <typename... Args>
    class i_slot : public i_slot_base
    {
    public:
        typedef i_slot abstract_type;
    public:
        virtual i_event<Args...> const& event() const = 0;
        virtual void call(Args... aArgs) const = 0;
        virtual std::thread::id call_thread() const = 0;
        virtual bool call_in_emitter_thread() const = 0;
        virtual void set_call_in_emitter_thread(bool aCallInEmitterThread) = 0;
        virtual bool stateless() const = 0;
        virtual void set_stateless(bool aStateless) = 0;
    };

    enum class trigger_type
    {
        Synchronous,
        SynchronousDontQueue,
        Asynchronous,
        AsynchronousDontQueue
    };

    enum class trigger_result
    {
        Unknown,
        Unaccepted,
        Accepted
    };

    inline bool event_consumed(trigger_result aTriggerResult)
    {
        switch (aTriggerResult)
        {
        case trigger_result::Unknown:
        case trigger_result::Unaccepted:
        default:
            return false;
        case trigger_result::Accepted:
            return true;
        }
    }

    template <typename... Args>
    class slot;

    template <typename... Args>
    struct slot_proxy
    {
        ref_ptr<neolib::slot<Args...>> slot;

        slot_proxy&& operator~()
        {
            slot->set_call_in_emitter_thread(true);
            return std::move(*this);
        }

        slot_proxy&& operator!()
        {
            slot->set_stateless(true);
            return std::move(*this);
        }
    };

    template <typename... Args>
    class i_event : public i_lifetime
    {
    public:
        typedef i_event abstract_type;
    public:
        virtual ~i_event() = default;
    public:
        virtual neolib::trigger_type trigger_type() const = 0;
        virtual void set_trigger_type(neolib::trigger_type aTriggerType) = 0;
    public:
        virtual trigger_result sync_trigger(Args... aArgs) const = 0;
        virtual void async_trigger(Args... aArgs) const = 0;
        virtual void accept() const = 0;
    public:
        virtual bool has_slots() const = 0;
        virtual void add_slot(i_slot<Args...>& aSlot, bool aPriority = false) const = 0;
        virtual void remove_slot(i_slot<Args...>& aSlot) const = 0;
    public:
        trigger_result trigger(Args... aArgs) const
        {
            switch (trigger_type())
            {
            case neolib::trigger_type::Synchronous:
            case neolib::trigger_type::SynchronousDontQueue:
                return sync_trigger(aArgs...);
            case neolib::trigger_type::Asynchronous:
            case neolib::trigger_type::AsynchronousDontQueue:
                async_trigger(aArgs...);
                return trigger_result::Unknown;
            default:
                return trigger_result::Unaccepted;
            }
        }
        trigger_result operator()(Args... aArgs) const
        {
            return trigger(aArgs...);
        }
        slot_proxy<Args...> operator()(std::function<void(Args...)> const& aCallback, bool aPriority = false) const
        {
            return slot_proxy<Args...>{ make_ref<slot<Args...>>(*this, aCallback, aPriority) };
        }
    };

    template <typename... Args>
    class slot : public reference_counted<lifetime<i_slot<Args...>>>
    {
    public:
        slot(i_event<Args...> const& aEvent, std::function<void(Args...)> const& aCallable, bool aPriority = false) :
            iEvent{ aEvent },
            iEventDestroyed{ aEvent },
            iCallable{ aCallable },
            iCallThread{ std::this_thread::get_id() }
        {
            event().add_slot(*this, aPriority);
        }
        ~slot()
        {
            remove();
        }
    public:
        void remove() final
        {
            if (!iEventDestroyed)
                event().remove_slot(*this);
        }
        i_event<Args...> const& event() const final
        {
            return iEvent;
        }
        void call(Args... aArgs) const final
        {
            iCallable(aArgs...);
        }
        std::thread::id call_thread() const final
        {
            if (call_in_emitter_thread())
                return std::this_thread::get_id();
            return *iCallThread;
        }
        bool call_in_emitter_thread() const final
        {
            return iCallThread == std::nullopt;
        }
        void set_call_in_emitter_thread(bool aCallInEmitterThread) final
        {
            if (aCallInEmitterThread)
                iCallThread = std::nullopt;
            else
                iCallThread = std::this_thread::get_id();
        }
        bool stateless() const final
        {
            return iStateless;
        }
        void set_stateless(bool aStateless) final
        {
            iStateless = aStateless;
        }
    private:
        i_event<Args...> const& iEvent;
        destroyed_flag iEventDestroyed;
        std::function<void(Args...)> iCallable;
        std::optional<std::thread::id> iCallThread;
        bool iStateless = false;
    };

    class sink
    {
    public:
        sink()
        {
        }
        sink(sink const& aOther)
        {
            std::scoped_lock lock{ iMutex, aOther.iMutex };
            iSlots = aOther.iSlots;
        }
        template <typename... Args>
        sink(slot_proxy<Args...>&& aSlot)
        {
            *this = std::move(aSlot);
        }
        ~sink()
        {
            clear();
        }
    public:
        sink& operator=(sink const& aOther)
        {
            std::scoped_lock lock{ iMutex, aOther.iMutex };
            clear();
            iSlots = aOther.iSlots;
            return *this;
        }
    public:
        bool empty() const
        {
            return iSlots.empty();
        }
    public:
        template <typename... Args>
        slot_proxy<Args...>&& operator=(slot_proxy<Args...>&& aSlot)
        {
            std::unique_lock lock{ iMutex };
            clear();
            iSlots.push_back(aSlot.slot);
            return std::move(aSlot);
        }
        template <typename... Args>
        slot_proxy<Args...>&& operator+=(slot_proxy<Args...>&& aSlot)
        {
            std::unique_lock lock{ iMutex };
            iSlots.push_back(aSlot.slot);
            return std::move(aSlot);
        }
        void clear()
        {
            std::unique_lock lock{ iMutex };
            for (auto& slot : iSlots)
                slot->remove();
            iSlots.clear();
        }
    private:
        mutable event_mutex<sink> iMutex;
        std::vector<ref_ptr<i_slot_base>> iSlots;
    };

    #define detail_event_subscribe( declName, ... ) \
            neolib::slot_proxy<__VA_ARGS__> declName(const std::function<void(__VA_ARGS__)>& aCallback, bool aPriority = false) const { return declName()(aCallback, aPriority); }\
            neolib::slot_proxy<__VA_ARGS__> declName(const std::function<void(__VA_ARGS__)>& aCallback, bool aPriority = false) { return declName()(aCallback, aPriority); }

    #define declare_event( declName, ... ) \
            virtual const neolib::i_event<__VA_ARGS__>& ev_##declName() const = 0;\
            virtual neolib::i_event<__VA_ARGS__>& ev_##declName() = 0;\
            const neolib::i_event<__VA_ARGS__>& declName() const { return ev_##declName(); }\
            neolib::i_event<__VA_ARGS__>& declName() { return ev_##declName(); }\
            detail_event_subscribe(declName, __VA_ARGS__)
}
