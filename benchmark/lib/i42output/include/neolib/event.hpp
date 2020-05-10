// event.hpp
/*
Transplanted from neogfx C++ GUI Library
Copyright (c) 2015-2018 Leigh Johnston.  All Rights Reserved.

This program is free software: you can redistribute it and / or modify
it under the terms of the GNU General Public License as published by
the Free Software Foundation, either version 3 of the License, or
(at your option) any later version.

This program is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU General Public License for more details.

You should have received a copy of the GNU General Public License
along with this program.  If not, see <http://www.gnu.org/licenses/>.
*/

#pragma once

#include <neolib/neolib.hpp>
#include <vector>
#include <deque>
#include <unordered_set>
#include <optional>
#include <mutex>
#include <atomic>

#include <neolib/scoped.hpp>
#include <neolib/lifetime.hpp>
#include <neolib/jar.hpp>
#include <neolib/i_event.hpp>

namespace neolib
{
    class sink;

    class event_handle
    {
    public:
        struct no_control : std::logic_error { no_control() : std::logic_error{ "neolib::event_handle::no_control" } {} };
    public:
        event_handle(i_event_control& aControl, cookie aId) : 
            iControl{ &aControl }, iRef{ aControl.get(), aId }, iPrimary{ true }
        {
            if (have_control())
                control().add_ref();
        }
        event_handle(const event_handle& aOther) :
            iControl{ aOther.iControl }, iRef{ aOther.iRef }, iPrimary{ false }
        {
            if (have_control())
                control().add_ref();
        }
        event_handle(event_handle&& aOther) :
            iControl{ aOther.iControl }, iRef{ aOther.iRef }, iPrimary{ false }
        {
            aOther.iPrimary = false;
            if (have_control())
                control().add_ref();
        }
        ~event_handle()
        {
            if (have_control())
            {
                if (!control().valid() || primary())
                    iRef.reset();
                control().release();
            }
        }
    public:
        event_handle& operator=(const event_handle& aRhs)
        {
            if (&aRhs == this)
                return *this;
            auto oldControl = iControl;
            iControl = aRhs.iControl;
            if (have_control())
                control().add_ref();
            if (oldControl != nullptr)
                oldControl->release();
            iRef = aRhs.iRef;
            return *this;
        }
    public:
        bool have_control() const
        {
            return iControl != nullptr;
        }
        i_event_control& control() const
        {
            if (have_control())
                return *iControl;
            throw no_control();
        }
        cookie id() const
        {
            return iRef.cookie();
        }
        bool primary() const
        {
            return iPrimary;
        }
        event_handle& operator~()
        {
            if (control().valid())
                control().get().handle_in_same_thread_as_emitter(id());
            return *this;
        }
    private:
        i_event_control* iControl;
        cookie_ref_ptr iRef;
        bool iPrimary;
    };

    class event_control : public i_event_control
    {
    public:
        struct no_event : std::logic_error { no_event() : std::logic_error{ "neolib::event_control::no_event" } {} };
    public:
        event_control(i_event& aEvent) :
            iEvent{ &aEvent }, iRefCount{ 0u }
        {
        }
        ~event_control()
        {
            if (valid())
                get().release_control();
        }
    public:
        void add_ref() override
        {
            ++iRefCount;
        }
        void release() override
        {
            if (--iRefCount == 0u)
                delete this;
        }
        bool valid() const override
        {
            return iEvent != nullptr;
        }
        i_event& get() const override
        {
            if (valid())
                return *iEvent;
            throw no_event();
        }
    public:
        void reset() override
        {
            iEvent = nullptr;
        }
    private:
        std::atomic<i_event*> iEvent;
        std::atomic<uint32_t> iRefCount;
    };

    template <typename... Args>
    class i_event_callable : public i_reference_counted
    {
        typedef i_event_callable<Args...> self_type;
    public:
        typedef self_type abstract_type;
    };

    template <typename... Args>
    class event_callable : public std::function<void(Args...)>, public reference_counted<i_event_callable<Args...>>
    {
    public:
        typedef std::function<void(Args...)> concrete_callable;
    public:
        template <typename Callable>
        event_callable(const Callable& aCallable) :
            concrete_callable{ aCallable }
        {
        }
        event_callable(const concrete_callable& aCallable) :
            concrete_callable{ aCallable }
        {
        }
    };

    template <typename... Args>
    class event_callback : public reference_counted<i_event_callback>
    {
        template <typename...>
        friend class event;
        friend class async_event_queue;
    private:
        typedef event_callable<Args...> callable;
        typedef typename callable::concrete_callable concrete_callable;
        typedef std::tuple<Args...> argument_pack;
    public:
        event_callback(const i_event& aEvent, const ref_ptr<callable>& aCallable, Args... aArguments) :
            iEvent{ &aEvent }, iCallable{ aCallable }, iArguments{ aArguments... }
        {
        }
    public:
        const i_event& event() const override
        {
            return *iEvent;
        }
        void call() const override
        {
            std::apply(*iCallable, iArguments);
        }
    public:
        ref_ptr<const callable>& get_callable()
        {
            return iCallable;
        }
    private:
        const i_event* iEvent;
        ref_ptr<callable> iCallable;
        argument_pack iArguments;
    };

    class async_task;
    class callback_timer;

    class async_event_queue : public lifetime
    {
        template <typename...>
        friend class event;
    public:
        struct async_event_queue_needs_a_task : std::logic_error { async_event_queue_needs_a_task() : std::logic_error("neogfx::async_event_queue::async_event_queue_needs_a_task") {} };
        struct async_event_queue_already_instantiated : std::logic_error { async_event_queue_already_instantiated() : std::logic_error("neogfx::async_event_queue::async_event_queue_already_instantiated") {} };
        struct event_not_found : std::logic_error { event_not_found() : std::logic_error("neogfx::async_event_queue::event_not_found") {} };
    private:
        typedef uint64_t transaction;
        typedef std::optional<transaction> optional_transaction;
        typedef ref_ptr<i_event_callback> callback_ptr;
        struct event_list_entry
        {
            async_event_queue::transaction transaction;
            destroyed_flag destroyed;
            callback_ptr callback;
        };
        typedef std::deque<event_list_entry> event_list_t;
    public:
        static async_event_queue& instance();
        static async_event_queue& instance(async_task& aTask);
        ~async_event_queue();
    private:
        async_event_queue(async_task& aTask);
        static async_event_queue& get_instance(async_task* aTask);
    public:
        bool exec();
        transaction enqueue(callback_ptr aCallback, const optional_transaction& aTransaction = {})
        {
            return add(std::move(aCallback), aTransaction);
        }
        void unqueue(const i_event& aEvent);
        void terminate();
    public:
        i_event_filter_registry& filter_registry();
    private:
        bool terminated() const;
        transaction add(callback_ptr aCallback, const optional_transaction& aTransaction);
        void remove(const i_event& aEvent);
        bool has(const i_event& aEvent) const;
        bool publish_events();
    private:
        async_task& iTask;
        std::unique_ptr<callback_timer> iTimer;
        event_list_t iEvents;
        std::atomic<bool> iTerminated;
        destroyed_flag iTaskDestroyed;
        std::atomic<uint32_t> iPublishNestingLevel;
        std::vector<std::unique_ptr<event_list_t>> iPublishCache;
        transaction iNextTransaction;
    };

    enum class event_trigger_type
    {
        Default,
        Synchronous,
        SynchronousDontQueue,
        Asynchronous,
        AsynchronousDontQueue
    };

    template <typename... Args>
    class event : public i_event, public lifetime
    {
        typedef event<Args...> self_type;
        friend class sink;
        friend class async_event_queue;
    private:
        typedef event_callback<Args...> callback;
        typedef typename callback::callable callback_callable;
        typedef typename callback_callable::concrete_callable concrete_callable;
        typedef async_event_queue::optional_transaction optional_async_transaction;
        struct handler
        {
            async_event_queue* queue;
            destroyed_flag queueDestroyed;
            uint32_t referenceCount;
            const void* clientId;
            ref_ptr<callback_callable> callable;
            bool handleInSameThreadAsEmitter;
            uint64_t triggerId = 0ull;

            handler(
                async_event_queue& queue, 
                const void* clientId, 
                const ref_ptr<callback_callable>& callable,
                bool handleInSameThreadAsEmitter = false) : 
                queue{ &queue },
                queueDestroyed{ queue },
                referenceCount{ 0u },
                clientId{ clientId },
                callable{ callable },
                handleInSameThreadAsEmitter{ handleInSameThreadAsEmitter }
            {}
        };
        typedef neolib::jar<handler> handler_list_t;
        struct context
        {
            bool accepted;
            std::atomic<bool> handlersChanged;
            context() : 
                accepted{ false },
                handlersChanged{ false }
            {
            }
            context(const context& aOther) : 
                accepted{ aOther.accepted },
                handlersChanged{ aOther.handlersChanged.load() }
            {
            }
            context& operator=(const context& aOther)
            {
                accepted = aOther.accepted;
                handlersChanged = aOther.handlersChanged.load();
                return *this;
            }
        };
        typedef std::vector<context> context_list_t;
        struct instance_data
        {
            bool ignoreErrors = false;
            event_trigger_type triggerType = event_trigger_type::Default;
            handler_list_t handlers;
            context_list_t contexts;
            bool triggering = false;
            uint64_t triggerId = 0ull;
            std::atomic<bool> handlersChanged = false;
            std::atomic<uint32_t> filterCount;
        };
    public:
        event() : iAlias{ *this }, iControl{ nullptr }, iInstanceDataPtr{ nullptr }
        {
        }
        event(const event&) = delete;
        virtual ~event()
        {
            if (filtered())
                async_event_queue::instance().filter_registry().uninstall_event_filter(*this);
            std::scoped_lock<switchable_mutex> lock{ event_mutex() };
            if (is_controlled())
            {
                control().reset();
                control().release();
            }
            set_destroying();
            clear();
        }
    public:
        self_type& operator=(const self_type&) = delete;
    public:
        void release_control() override
        {
            if (iControl != nullptr)
            {
                iControl.load()->reset();
                iControl.store(nullptr);
            }
        }
        void handle_in_same_thread_as_emitter(cookie aHandleId) override
        {
            std::scoped_lock<switchable_mutex> lock{ event_mutex() };
            instance().handlers[aHandleId].handleInSameThreadAsEmitter = true;
        }
    public:
        void push_context() const override
        {
            std::scoped_lock<switchable_mutex> lock{ event_mutex() };
            instance().contexts.emplace_back();
        }
        void pop_context() const override
        {
            std::scoped_lock<switchable_mutex> lock{ event_mutex() };
            instance().contexts.pop_back();
        }
    public:
        void pre_trigger() const override
        {
            if (filtered())
                async_event_queue::instance().filter_registry().pre_filter_event(*this);
        }
    public:
        void ignore_errors()
        {
            instance().ignoreErrors = true;
        }
        event_trigger_type trigger_type() const
        {
            return instance().triggerType;
        }
        void set_trigger_type(event_trigger_type aTriggerType)
        {
            instance().triggerType = aTriggerType;
        }
        bool trigger(Args... aArguments) const
        {
            if (!has_instance()) // no instance date means no subscribers so no point triggering.
                return true;
            switch (trigger_type())
            {
            case event_trigger_type::Default:
            case event_trigger_type::Synchronous:
            case event_trigger_type::SynchronousDontQueue:
            default:
                return sync_trigger(aArguments...);
            case event_trigger_type::Asynchronous:
            case event_trigger_type::AsynchronousDontQueue:
                async_trigger(aArguments...);
                return true;
            }
        }
        bool sync_trigger(Args... aArguments) const
        {
            if (!has_instance()) // no instance means no subscribers so no point triggering.
                return true;
            if (trigger_type() == event_trigger_type::SynchronousDontQueue)
                unqueue();
            std::scoped_lock<switchable_mutex> lock{ event_mutex() };
            if (instance().handlers.empty() && !filtered())
                return true;
            destroyed_flag destroyed{ *this };
            push_context();
            if (filtered())
            {
                async_event_queue::instance().filter_registry().filter_event(*this);
                if (destroyed)
                    return true;
                if (instance().contexts.back().accepted)
                {
                    pop_context();
                    return false;
                }
            }
            if (instance().handlers.empty())
                return true;
            scoped_flag sf{ instance().triggering };
            if (!instance().triggering)
            {
                instance().triggering = true;
                instance().triggerId = 0ull;
                for (auto& handler : instance().handlers)
                    handler.triggerId = 0ull;
            }
            auto triggerId = ++instance().triggerId;
            optional_async_transaction transaction;
            for (std::size_t handlerIndex = {}; handlerIndex < instance().handlers.size();)
            {
                auto& handler = instance().handlers.at_index(handlerIndex++);
                if (handler.triggerId < triggerId)
                    handler.triggerId = triggerId;
                else if (handler.triggerId == triggerId)
                    continue;
                try
                {
                    transaction = enqueue(handler, false, transaction, aArguments...);
                    if (destroyed)
                        return true;
                }
                catch (...)
                {
                    pop_context();
                    throw;
                }
                if (destroyed)
                    return true;
                if (instance().contexts.back().accepted)
                {
                    pop_context();
                    return false;
                }
                if (instance().handlersChanged.exchange(false))
                    handlerIndex = {};
            }
            pop_context();
            return true;
        }
        void async_trigger(Args... aArguments) const
        {
            if (!has_instance()) // no instance means no subscribers so no point triggering.
                return;
            if (trigger_type() == event_trigger_type::AsynchronousDontQueue)
                unqueue();
            std::scoped_lock<switchable_mutex> lock{ event_mutex() };
            if (instance().handlers.empty())
                return;
            destroyed_flag destroyed{ *this };
            scoped_flag sf{ instance().triggering };
            if (!instance().triggering)
            {
                instance().triggering = true;
                instance().triggerId = 0ull;
                for (auto& handler : instance().handlers)
                    handler.triggerId = 0ull;
            }
            auto triggerId = ++instance().triggerId;
            optional_async_transaction transaction;
            for (std::size_t handlerIndex = {}; handlerIndex < instance().handlers.size();)
            {
                auto& handler = instance().handlers.at_index(handlerIndex++);
                if (handler.triggerId < triggerId)
                    handler.triggerId = triggerId;
                else if (handler.triggerId == triggerId)
                    continue;
                transaction = enqueue(handler, true, transaction, aArguments...);
                if (destroyed)
                    return;
                if (instance().handlersChanged.exchange(false))
                    handlerIndex = {};
            }
        }
        bool accepted() const override
        {
            std::scoped_lock<switchable_mutex> lock{ event_mutex() };
            return !instance().contexts.empty() ? instance().contexts.back().accepted : false;
        }
        void accept() const override
        {
            std::scoped_lock<switchable_mutex> lock{ event_mutex() };
            instance().contexts.back().accepted = true;
        }
        void ignore() const override
        {
            std::scoped_lock<switchable_mutex> lock{ event_mutex() };
            instance().contexts.back().accepted = false;
        }
    public:
        bool filtered() const override
        {
            return instance().filterCount > 0u;
        }
        void filter_added() const override
        {
            ++instance().filterCount;
        }
        void filter_removed() const override
        {
            --instance().filterCount;
        }
        void filters_removed() const override
        {
            instance().filterCount = 0u;
        }
    public:
        event_handle subscribe(const concrete_callable& aCallable, const void* aUniqueId = nullptr) const
        {
            std::scoped_lock<switchable_mutex> lock{ event_mutex() };
            invalidate_handler_list();
            return event_handle{ control(), instance().handlers.emplace(async_event_queue::instance(), aUniqueId, make_ref<callback_callable>(aCallable)) };
        }
        event_handle operator()(const concrete_callable& aCallable, const void* aUniqueId = nullptr) const
        {
            return subscribe(aCallable, aUniqueId);
        }
        template <typename T>
        event_handle subscribe(const concrete_callable& aCallable, const T* aClientId) const
        {
            return subscribe(aCallable, static_cast<const void*>(aClientId));
        }
        template <typename T>
        event_handle operator()(const concrete_callable& aCallable, const T* aClientId) const
        {
            return subscribe(aCallable, static_cast<const void*>(aClientId));
        }
        template <typename T>
        event_handle subscribe(const concrete_callable& aCallable, const T& aClientId) const
        {
            return subscribe(aCallable, static_cast<const void*>(&aClientId));
        }
        template <typename T>
        event_handle operator()(const concrete_callable& aCallable, const T& aClientId) const
        {
            return subscribe(aCallable, static_cast<const void*>(&aClientId));
        }
        void unsubscribe(event_handle aHandle) const
        {
            std::scoped_lock<switchable_mutex> lock{ event_mutex() };
            invalidate_handler_list();
            instance().handlers.remove(aHandle.id());
        }
        void unsubscribe(const void* aClientId) const
        {
            std::scoped_lock<switchable_mutex> lock{ event_mutex() };
            invalidate_handler_list();
            for (auto h = instance().handlers.begin(); h != instance().handlers.end();)
                if ((*h).clientId == aClientId)
                    h = instance().handlers.erase(h);
                else
                    ++h;
        }
        template <typename T>
        void unsubscribe(const T* aClientId) const
        {
            return unsubscribe(static_cast<const void*>(aClientId));
        }
        template <typename T>
        void unsubscribe(const T& aClientId) const
        {
            return unsubscribe(static_cast<const void*>(&aClientId));
        }
    private:
        void add_ref(cookie aCookie) override
        {
            std::scoped_lock<switchable_mutex> lock{ event_mutex() };
            ++instance().handlers[aCookie].referenceCount;
        }
        void release(cookie aCookie) override
        {
            std::scoped_lock<switchable_mutex> lock{ event_mutex() };
            if (--instance().handlers[aCookie].referenceCount == 0u)
                instance().handlers.remove(aCookie);
        }
        long use_count(cookie aCookie) const override
        {
            std::scoped_lock<switchable_mutex> lock{ event_mutex() };
            return instance().handlers[aCookie].referenceCount;
        }
    private:
        void invalidate_handler_list() const
        {
            std::scoped_lock<switchable_mutex> lock{ event_mutex() };
            instance().handlersChanged = true;
            for (auto& context : instance().contexts)
                context.handlersChanged = true;
        }
        optional_async_transaction enqueue(handler& aHandler, bool aAsync, const optional_async_transaction& aAsyncTransaction, Args... aArguments) const
        {
            optional_async_transaction transaction;
            auto& emitterQueue = async_event_queue::instance();
            if (!aAsync && !aHandler.queueDestroyed && aHandler.queue == &emitterQueue)
                (*aHandler.callable)(aArguments...);
            else
            {
                auto ecb = make_ref<callback>(*this, aHandler.callable, aArguments...);
                if (aHandler.handleInSameThreadAsEmitter)
                    transaction = emitterQueue.enqueue(ecb, aAsyncTransaction);
                else
                {
                    if (!aHandler.queueDestroyed)
                        transaction = aHandler.queue->enqueue(ecb, aAsyncTransaction);
                    else if (!instance().ignoreErrors)
                        throw event_queue_destroyed();
                }
            }
            return transaction;
        }
        void unqueue() const
        {
            std::scoped_lock<switchable_mutex> lock{ event_mutex() };
            std::unordered_set<async_event_queue*> queues;
            for (auto const& h : instance().handlers)
                if (!h.queueDestroyed)
                    queues.insert(h.queue);
            for (auto const& q : queues)
                q->unqueue(*this);
        }
        void clear()
        {
            std::scoped_lock<switchable_mutex> lock{ event_mutex() };
            for (auto& h : instance().handlers)
                if (!h.queueDestroyed)
                    h.queue->remove(*this);
            iInstanceDataPtr = nullptr;
            iInstanceData = std::nullopt;
        }
        bool is_controlled() const
        {
            return iControl != nullptr;
        }
        i_event_control& control() const
        {
            std::scoped_lock<switchable_mutex> lock{ event_mutex() };
            if (iControl == nullptr)
            {
                iControl = new event_control{ iAlias };
                iControl.load()->add_ref();
            }
            return *iControl;
        }
        bool has_instance() const
        {
            return iInstanceDataPtr != nullptr;
        }
        instance_data& instance() const
        {
            if (iInstanceDataPtr != nullptr)
                return *iInstanceDataPtr;
            std::scoped_lock<switchable_mutex> lock{ event_mutex() };
            iInstanceData.emplace();
            iInstanceDataPtr = &*iInstanceData;
            return *iInstanceDataPtr;
        }
    private:
        self_type& iAlias; // bit of a hack: most event operations are logically const as we want to be able to trigger events from const methods of the containing object
        mutable std::atomic<i_event_control*> iControl;
        mutable std::optional<instance_data> iInstanceData;
        mutable std::atomic<instance_data*> iInstanceDataPtr;
    };

    class sink
    {
    public:
        sink()
        {
        }
        sink(const event_handle& aHandle)
        {
            iHandles.push_back(aHandle);
        }
        sink(event_handle&& aHandle)
        {
            iHandles.push_back(std::move(aHandle));
        }
        sink(const sink& aSink) :
            iHandles{ aSink.iHandles }
        {
        }
        virtual ~sink()
        {
            clear();
        }
    public:
        sink& operator=(const sink& aSink)
        {
            if (this == &aSink)
                return *this;
            iHandles = aSink.iHandles;
            return *this;
        }
    public:
        sink& operator=(const event_handle& aHandle)
        {
            return *this = sink{ aHandle };
        }
        sink& operator=(event_handle&& aHandle)
        {
            return *this = sink{ std::move(aHandle) };
        }
        sink& operator+=(const event_handle& aHandle)
        {
            sink s{ aHandle };
            iHandles.insert(iHandles.end(), s.iHandles.begin(), s.iHandles.end());
            return *this;
        }
        sink& operator+=(event_handle&& aHandle)
        {
            sink s{ std::move(aHandle) };
            iHandles.insert(iHandles.end(), s.iHandles.begin(), s.iHandles.end());
            return *this;
        }
    public:
        void clear()
        {
            iHandles.clear();
        }
    private:
        mutable std::vector<event_handle> iHandles;
    };
}
