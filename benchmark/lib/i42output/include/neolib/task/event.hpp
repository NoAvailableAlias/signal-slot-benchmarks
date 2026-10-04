// event.hpp
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
#include <vector>
#include <boost/unordered/unordered_flat_map.hpp>
#include <neolib/core/lifetime.hpp>
#include <neolib/core/scoped.hpp>
#include <neolib/task/i_async_task.hpp>
#include <neolib/task/i_event.hpp>

namespace neolib
{
    class event_system : public i_event_system
    {
    public:
        event_system_locking_strategy locking_strategy() const noexcept final
        {
            return iStrategy;
        }
        void set_locking_strategy(event_system_locking_strategy aStrategy) noexcept final
        {
            iStrategy = aStrategy;
        }
    private:
        event_system_locking_strategy iStrategy = event_system_locking_strategy::MultiThreaded;
    };

    class async_event_queue : public lifetime<i_async_event_queue>
    {
    private:
        struct queue_entry
        {
            void const* event;
            destroyed_flag eventDestroyed;
            void const* slot;
            destroyed_flag slotDestroyed;
            std::function<void()> callback;
        };
        struct queue
        {
            std::vector<queue_entry> multiple;
            boost::unordered_flat_map<std::pair<void const*, void const*>, queue_entry> single;
        };
    public:
        static async_event_queue& instance();
        static async_event_queue& instance(std::thread::id aThreadId);
    public:
        async_event_queue();
        ~async_event_queue();
    public:
        template <typename... Args>
        void enqueue(i_slot<Args...>& aSlot, bool aNoDuplicates, Args... aArgs)
        {
            auto& event = aSlot.event();
            auto callback = [&aSlot, args = std::tuple<Args...>{ aArgs... }]()
            {
                std::apply([&](Args... aArgs) { aSlot.call(aArgs...); }, args);
            };
            bool const single = (aNoDuplicates || aSlot.stateless());
            std::scoped_lock lock{ iMutex };
            if (single)
            {
                typename decltype(iQueue.single)::key_type const key{ &event, &aSlot };
                auto existing = iQueue.single.find(key);
                if (existing != iQueue.single.end())
                {
                    existing->second.callback = std::move(callback);
                    return;
                }
                iQueue.single.emplace(key, queue_entry{ &event, event, &aSlot, aSlot, std::move(callback) });
            }
            else
                iQueue.multiple.emplace_back(&event, event, &aSlot, aSlot, std::move(callback));
        }
    public:
        void register_with_task(i_async_task& aTask) final;
        bool pump_events() final;
    private:
        mutable event_mutex<async_event_queue> iMutex;
        i_async_task* iTask = nullptr;
        std::optional<destroyed_flag> iTaskDestroyed;
        queue iQueue;
    };

    template <typename... Args>
    class event : public lifetime<i_event<Args...>>
    {
        typedef lifetime<i_event<Args...>> base_type;
    public:
        using typename base_type::abstract_type;
    private:
        typedef std::vector<ref_ptr<i_slot<Args...>>> slot_list;
        struct work_list
        {
            slot_list slots;
            bool accepted = false;
        };
    public:
        event()
        {
        }
        event(event const& aOther) : 
            iTriggerType{ aOther.iTriggerType }
        {
        }
        ~event()
        {
        }
    public:
        event& operator=(event const& aOther)
        {
            std::unique_lock lock{ iMutex };
            iTriggerType = aOther.iTriggerType;
            iSlots = {};
            iHasSlots = false;
            iActiveWorkList = {};
            return *this;
        }
    public:
        neolib::trigger_type trigger_type() const final
        {
            return iTriggerType;
        }
        void set_trigger_type(neolib::trigger_type aTriggerType) final
        {
            iTriggerType = aTriggerType;
        }
    public:
        trigger_result sync_trigger(Args... aArgs) const final
        {
            if (!has_slots())
                return trigger_result::Unaccepted;

            std::unique_lock lock{ iMutex };
            destroyed_flag destroyed{ *this };
            thread_local std::size_t stack;
            scoped_counter<std::size_t> stackCounter{ stack };
            thread_local std::vector<std::unique_ptr<work_list>> workLists;
            if (workLists.size() < stack)
                workLists.push_back(std::make_unique<work_list>());
            auto& workList = *workLists[stack - 1];
            scoped_pointer<work_list> activeWorkList{ iActiveWorkList, &workList };
            workList.slots = iSlots;
            lock.unlock();

            auto const thisThread = std::this_thread::get_id();
            std::thread::id lastQueueThread;
            async_event_queue* lastQueue = nullptr;
            for (auto const& slot : workList.slots)
            {
                if (slot->call_in_emitter_thread())
                    slot->call(aArgs...);
                else
                {
                    auto const callThread = slot->call_thread();
                    if (callThread == thisThread)
                        slot->call(aArgs...);
                    else
                    {
                        if (lastQueue == nullptr || callThread != lastQueueThread)
                        {
                            lastQueue = &async_event_queue::instance(callThread);
                            lastQueueThread = callThread;
                        }
                        async_trigger(*lastQueue, *slot, trigger_type() == neolib::trigger_type::SynchronousDontQueue, aArgs...);
                    }
                }
                if (destroyed)
                    return trigger_result::Unaccepted;
                if (workList.accepted)
                {
                    workList.accepted = false;
                    return trigger_result::Accepted;
                }
            }

            workList.slots.clear();
            workList.accepted = false;

            return trigger_result::Unaccepted;
        }
        void async_trigger(Args... aArgs) const final
        {
            std::unique_lock lock{ iMutex };
            std::thread::id lastQueueThread;
            async_event_queue* lastQueue = nullptr;
            for (auto const& slot : iSlots)
            {
                auto const callThread = slot->call_thread();
                if (lastQueue == nullptr || callThread != lastQueueThread)
                {
                    lastQueue = &async_event_queue::instance(callThread);
                    lastQueueThread = callThread;
                }
                async_trigger(*lastQueue, *slot, trigger_type() == neolib::trigger_type::AsynchronousDontQueue, aArgs...);
            }
        }
        void accept() const final
        {
            std::unique_lock lock{ iMutex };
            if (iActiveWorkList)
                iActiveWorkList->accepted = true;
        }
    public:
        bool has_slots() const final
        {
            return iHasSlots;
        }
        void add_slot(i_slot<Args...>& aSlot, bool aPriority = false) const final
        {
            std::scoped_lock lock{ iMutex };
            if (!aPriority)
                iSlots.push_back(&aSlot);
            else
                iSlots.insert(iSlots.begin(), &aSlot);
            iHasSlots = true;
        }
        void remove_slot(i_slot<Args...>& aSlot) const final
        {
            std::scoped_lock lock{ iMutex };
            auto existing = std::find_if(iSlots.begin(), iSlots.end(), [&](auto const& s) { return &aSlot == s.ptr(); });
            if (existing != iSlots.end())
                iSlots.erase(existing);
            iHasSlots = !iSlots.empty();
        }
    private:
        void async_trigger(async_event_queue& aQueue, i_slot<Args...>& aSlot, bool aNoDuplicates, Args... aArgs) const
        {
            aQueue.enqueue<Args...>(aSlot, aNoDuplicates, aArgs...);
        }
    private:
        mutable event_mutex<event> iMutex;
        neolib::trigger_type iTriggerType = neolib::trigger_type::Synchronous;
        mutable slot_list iSlots;
        mutable std::atomic<bool> iHasSlots = false;
        mutable work_list* iActiveWorkList = nullptr;
    };

    #define define_declared_event( name, declName, ... ) \
            neolib::event<__VA_ARGS__> name; \
            const neolib::i_event<__VA_ARGS__>& ev_##declName() const final { return name; };\
            neolib::i_event<__VA_ARGS__>& ev_##declName() final { return name; };

    #define define_event( name, declName, ... ) \
            neolib::event<__VA_ARGS__> name; \
            const neolib::i_event<__VA_ARGS__>& ev_##declName() const { return name; };\
            neolib::i_event<__VA_ARGS__>& ev_##declName() { return name; };\
            const neolib::i_event<__VA_ARGS__>& declName() const { return ev_##declName(); }\
            neolib::i_event<__VA_ARGS__>& declName() { return ev_##declName(); }\
            detail_event_subscribe(declName, __VA_ARGS__)
}
