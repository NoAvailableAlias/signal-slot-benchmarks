// event.cpp
/*
 *  Copyright (c) 2021-2025 Leigh Johnston.
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
#include <unordered_map>
#include <neolib/task/event.hpp>

namespace neolib
{ 
    template<> i_event_system& services::start_service<i_event_system>()
    {
        static event_system sEventSystem;
        return sEventSystem;
    }

    namespace
    {
        struct instance_map_type : std::unordered_map<std::thread::id, async_event_queue*>
        {
            event_mutex<instance_map_type> mutex;
        };

        instance_map_type& instance_map()
        {
            static instance_map_type sMap;
            return sMap;
        }
    }

    async_event_queue& async_event_queue::instance()
    {
        thread_local async_event_queue tInstance;
        return tInstance;
    }

    async_event_queue& async_event_queue::instance(std::thread::id aThreadId)
    {
        std::scoped_lock lock{ instance_map().mutex };
        (void)instance();
        auto existing = instance_map().find(aThreadId);
        if (existing != instance_map().end())
            return *existing->second;
        throw std::logic_error("neolib::async_event_queue::instance: instance not found");
    }

    async_event_queue::async_event_queue()
    {
        std::scoped_lock lock{ instance_map().mutex };
        if (instance_map().find(std::this_thread::get_id()) == instance_map().end())
            instance_map()[std::this_thread::get_id()] = this;
    }

    async_event_queue::~async_event_queue()
    {
        {
            std::scoped_lock mapLock{ instance_map().mutex };
            auto existing = instance_map().find(std::this_thread::get_id());
            if (existing != instance_map().end() && existing->second == this)
                instance_map().erase(existing);
        }
        std::scoped_lock lock{ iMutex };
        if (iTask && !*iTaskDestroyed)
            iTask->unregister_event_queue(*this);
    }

    void async_event_queue::register_with_task(i_async_task& aTask)
    {
        iTask = &aTask;
        iTaskDestroyed.emplace(*iTask);
        iTask->register_event_queue(*this);
    }

    bool async_event_queue::pump_events()
    {
        std::unique_lock lock{ iMutex };
        thread_local std::size_t stack;
        scoped_counter<std::size_t> stackCounter{ stack };
        typedef std::vector<queue_entry> work_list;
        thread_local std::vector<std::unique_ptr<work_list>> workLists;
        if (workLists.size() < stack)
            workLists.push_back(std::make_unique<work_list>());
        auto& workList = *workLists[stack - 1];
        workList.swap(iQueue.multiple);
        workList.reserve(workList.size() + iQueue.single.size());
        for (auto& se : iQueue.single)
            workList.push_back(std::move(se.second));
        iQueue.single.clear();
        lock.unlock();
        bool didSome = false;
        for (auto& entry : workList)
        {
            if (entry.eventDestroyed || entry.slotDestroyed)
                continue;
            entry.callback();
            didSome = true;
        }
        workList.clear();
        return didSome;
    }
}