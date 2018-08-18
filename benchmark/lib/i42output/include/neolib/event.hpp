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

#include "neolib.hpp"
#include <list>
#include <deque>
#include <unordered_map>
#include <optional>
#include <mutex>
#include "allocator.hpp"
#include "mutex.hpp"
#include "lifetime.hpp"
#include "async_task.hpp"
#include "timer.hpp"

namespace neolib
{
	class event_system
	{
	public:
		static bool single_threaded()
		{
			return !instance().iMultiThreaded;
		}
		static bool multi_threaded()
		{
			return instance().iMultiThreaded;
		}
		static void set_single_threaded()
		{
			instance().iMultiThreaded = false;
		}
		static void set_multi_threaded()
		{
			instance().iMultiThreaded = true;
		}
	private:
		static event_system& instance()
		{
			static event_system sInstance;
			return sInstance;
		}
	private:
		bool iMultiThreaded = true;
	};

	class event_mutex : public lifetime
	{
	public:
		event_mutex() :
			iLockCount(0)
		{
		}
		~event_mutex()
		{
			while (iLockCount)
				unlock();
		}
	public:
		void lock()
		{
			if (event_system::multi_threaded())
			{
				++iLockCount;
				iRealMutex.lock();
			}
		}
		void unlock() noexcept
		{
			if (iLockCount > 0)
			{
				--iLockCount;
				iRealMutex.unlock();
			}
		}
		bool try_lock()
		{
			if (event_system::multi_threaded())
			{
				bool locked = iRealMutex.try_lock();
				if (locked)
					++iLockCount;
				return locked;
			}
			else
				return true;
		}
	private:
		std::atomic<uint32_t> iLockCount;
		std::recursive_mutex iRealMutex;
	};

	class sink;

	template <typename... Arguments>
	class event;
		
	template <typename... Arguments>
	class event_handle
	{
	public:
		typedef const event<Arguments...>* event_ptr;
		typedef std::shared_ptr<event_ptr> event_instance_ptr;
		typedef std::weak_ptr<event_ptr> event_instance_weak_ptr;
		typedef const void* unique_id_type;
		typedef std::function<void(Arguments...)> handler_callback;
		typedef uint32_t sink_reference_count;
		struct handler_list_item { std::optional<std::thread::id> iThreadId; unique_id_type iUniqueId; handler_callback iHandlerCallback; sink_reference_count iSinkReferenceCount; };
		typedef std::list<handler_list_item, thread_safe_fast_pool_allocator<handler_list_item>> handler_list;
	public:
		event_instance_weak_ptr iEvent;
		typename handler_list::iterator iHandler;
	public:
		event_handle& operator~()
		{
			iHandler->iThreadId = std::nullopt;
			return *this;
		}
	};

	class async_event_queue
	{
	public:
		typedef std::function<void()> callback;
	public:
		struct no_instance : std::logic_error { no_instance() : std::logic_error("neogfx::async_event_queue::no_instance") {} };
		struct instance_exists : std::logic_error { instance_exists() : std::logic_error("neogfx::async_event_queue::instance_exists") {} };
		struct event_not_found : std::logic_error { event_not_found() : std::logic_error("neogfx::async_event_queue::event_not_found") {} };
	private:
		struct instance_pointers
		{
			async_event_queue* aliased;
			std::weak_ptr<async_event_queue> counted;
		};
		typedef std::unordered_multimap<
			const void*, 
			std::pair<callback, neolib::lifetime::destroyed_flag>, 
			std::hash<const void*>, 
			std::equal_to<const void*>,
			thread_safe_fast_pool_allocator<std::pair<const void* const, std::pair<callback, neolib::lifetime::destroyed_flag>>>> event_list;
		typedef std::vector<callback> callback_list;
		typedef std::unordered_map<
			std::thread::id, 
			callback_list, 
			std::hash<std::thread::id>, 
			std::equal_to<std::thread::id>, 
			thread_safe_fast_pool_allocator<std::pair<const std::thread::id, callback_list>>> threaded_callbacks;
	public:
		async_event_queue();
		async_event_queue(neolib::async_task& aTask);
		~async_event_queue();
		static std::shared_ptr<async_event_queue> instance();
	public:
		template<typename... Arguments>
		void add(const event<Arguments...>& aEvent, callback aCallback)
		{
			add(static_cast<const void*>(&aEvent), aCallback, neolib::lifetime::destroyed_flag(aEvent));
		}
		template<typename... Arguments>
		void remove(const event<Arguments...>& aEvent)
		{
			remove(static_cast<const void*>(&aEvent));
		}
		template<typename... Arguments>
		bool has(const event<Arguments...>& aEvent) const
		{
			return has(static_cast<const void*>(&aEvent));
		}
		bool exec();
		void enqueue_to_thread(std::thread::id aThreadId, callback aCallback);
		void terminate();
		void persist(std::shared_ptr<async_event_queue> aPtr, uint32_t aDuration_ms = 1000u);
	private:
		async_event_queue(std::shared_ptr<async_task> aTask);
		static std::recursive_mutex& instance_mutex();
		static instance_pointers& instance_ptrs();
		void add(const void* aEvent, callback aCallback, neolib::lifetime::destroyed_flag aDestroyedFlag);
		void remove(const void* aEvent);
		bool has(const void* aEvent) const;
		void publish_events();
	private:
		std::shared_ptr<async_task> iTask;
		neolib::callback_timer iTimer;
		mutable event_mutex iEventsMutex;
		event_list iEvents;
		event_mutex iThreadedCallbacksMutex;
		std::atomic<bool> iHaveThreadedCallbacks;
		threaded_callbacks iThreadedCallbacks;
		std::atomic<bool> iTerminated;
		std::pair<std::shared_ptr<async_event_queue>, std::chrono::time_point<std::chrono::steady_clock>> iCache;
	};

	enum class event_trigger_type
	{
		Default,
		Synchronous,
		Asynchronous
	};

	template <typename... Arguments>
	class event : protected lifetime
	{
		friend class sink;
		friend class async_event_queue;
	private:
		typedef event_handle<Arguments...> handle;
		typedef typename handle::event_ptr ptr;
		typedef typename handle::event_instance_ptr instance_ptr;
		typedef typename handle::event_instance_weak_ptr instance_weak_ptr;
		typedef typename handle::unique_id_type unique_id_type;
		typedef typename handle::handler_callback handler_callback;
		typedef typename handle::sink_reference_count sink_reference_count;
		typedef typename handle::handler_list_item handler_list_item;
		typedef typename handle::handler_list handler_list;
		typedef std::map<unique_id_type, typename handler_list::iterator> unique_id_map;
		typedef std::deque<typename handler_list::const_iterator> notification_list;
		struct instance_data
		{
			instance_ptr instancePtr;
			std::shared_ptr<async_event_queue> asyncEventQueue;
			handler_list handlers;
			unique_id_map uniqueIdMap;
			event_trigger_type triggerType;
			bool accepted;
			notification_list notifications;
			event_mutex mutex;
		};
		typedef thread_safe_fast_pool_allocator<instance_data> instance_allocator;
	public:
		event()
		{
		}
		event(const event&)
		{
			// do nothing.
		}
		~event()
		{
			clear();
		}
	public:
		event& operator=(const event&)
		{
			clear();
			return *this;
		}
	public:
		event_trigger_type trigger_type() const
		{
			return instance().triggerType;
		}
		void set_trigger_type(event_trigger_type aTriggerType)
		{
			instance().triggerType = aTriggerType;
		}
		template<class... Ts>
		bool trigger(Ts&&... aArguments) const
		{
			if (!has_instance()) // no instance means no subscribers so no point triggering.
				return true;
			destroyable_mutex_lock_guard<event_mutex> guard{ instance().mutex };
			switch (instance().triggerType)
			{
			case event_trigger_type::Default:
			case event_trigger_type::Synchronous:
			default:
				return sync_trigger(std::forward<Ts>(aArguments)...);
			case event_trigger_type::Asynchronous:
				async_trigger(std::forward<Ts>(aArguments)...);
				return true;
			}
		}
		template<class... Ts>
		bool sync_trigger(Ts&&... aArguments) const
		{
			if (!has_instance()) // no instance means no subscribers so no point triggering.
				return true;
			destroyable_mutex_lock_guard<event_mutex> guard{ instance().mutex };
			destroyed_flag destroyed{ *this };
			for (auto i = instance().handlers.begin(); i != instance().handlers.end(); ++i)
				instance().notifications.push_back(i);
			while (!instance().notifications.empty())
			{
				auto i = instance().notifications.front();
				instance().notifications.pop_front();
				if (i->iThreadId == std::nullopt || *i->iThreadId == std::this_thread::get_id())
					i->iHandlerCallback(std::forward<Ts>(aArguments)...);
				else
					enqueue_to_thread(*i, std::forward<Ts>(aArguments)...);
				if (destroyed)
					return false;
				if (instance().accepted)
				{
					instance().notifications.clear();
					instance().accepted = false;
					return false;
				}
			}
			return true;
		}
		template<class... Ts>
		void async_trigger(Ts&&... aArguments) const
		{
			if (!has_instance()) // no instance means no subscribers so no point triggering.
				return;
			destroyable_mutex_lock_guard<event_mutex> guard{ instance().mutex };
			instance().asyncEventQueue->add(*this, [this, &aArguments...]() { sync_trigger(std::forward<Ts>(aArguments)...); });
		}
		void accept() const
		{
			instance().accepted = true;
		}
		void ignore() const
		{
			instance().accepted = false;
		}
	public:
		handle subscribe(const handler_callback& aHandlerCallback, const void* aUniqueId = 0) const
		{
			destroyable_mutex_lock_guard<event_mutex> guard{ instance().mutex };
			if (aUniqueId == 0)
				return handle{ instance().instancePtr, instance().handlers.insert(instance().handlers.end(), handler_list_item{ std::this_thread::get_id(), aUniqueId, aHandlerCallback, 0 }) };
			auto existing = instance().uniqueIdMap.find(aUniqueId);
			if (existing == instance().uniqueIdMap.end())
				existing = instance().uniqueIdMap.insert(std::make_pair(aUniqueId, instance().handlers.insert(instance().handlers.end(), handler_list_item{ std::this_thread::get_id(), aUniqueId, aHandlerCallback, 0 }))).first;
			else
				existing->second->iHandlerCallback = aHandlerCallback;
			return handle{ instance().instancePtr, existing->second };
		}
		handle operator()(const handler_callback& aHandlerCallback, const void* aUniqueId = 0) const
		{
			return subscribe(aHandlerCallback, aUniqueId);
		}
		template <typename T>
		handle subscribe(const handler_callback& aHandlerCallback, const T* aUniqueIdObject) const
		{
			return subscribe(aHandlerCallback, static_cast<const void*>(aUniqueIdObject));
		}
		template <typename T>
		handle operator()(const handler_callback& aHandlerCallback, const T* aUniqueIdObject) const
		{
			return subscribe(aHandlerCallback, static_cast<const void*>(aUniqueIdObject));
		}
		template <typename T>
		handle subscribe(const handler_callback& aHandlerCallback, const T& aUniqueIdObject) const
		{
			return subscribe(aHandlerCallback, static_cast<const void*>(&aUniqueIdObject));
		}
		template <typename T>
		handle operator()(const handler_callback& aHandlerCallback, const T& aUniqueIdObject) const
		{
			return subscribe(aHandlerCallback, static_cast<const void*>(&aUniqueIdObject));
		}
		void unsubscribe(const void* aUniqueId) const
		{
			destroyable_mutex_lock_guard<event_mutex> guard{ instance().mutex };
			auto existing = instance().uniqueIdMap.find(aUniqueId);
			if (existing != instance().uniqueIdMap.end())
				unsubscribe(handle{ instance().instancePtr, existing->second });
		}
		template <typename T>
		void unsubscribe(const T* aUniqueIdObject) const
		{
			return unsubscribe(static_cast<const void*>(aUniqueIdObject));
		}
		template <typename T>
		void unsubscribe(const T& aUniqueIdObject) const
		{
			return unsubscribe(static_cast<const void*>(&aUniqueIdObject));
		}
	private:
		template<class... Ts>
		void enqueue_to_thread(const handler_list_item& aItem, Ts&&... aArguments) const
		{
			auto& callback = aItem.iHandlerCallback;
			instance().asyncEventQueue->enqueue_to_thread(*aItem.iThreadId, [callback, &aArguments...](){ callback(std::forward<Ts>(aArguments)...); });
		}
		void clear()
		{
			if (instance().asyncEventQueue->has(*this))
				instance().asyncEventQueue->remove(*this);
			decltype(iInstanceData) temp;
			{
				destroyable_mutex_lock_guard<event_mutex> guard{ instance().mutex };
				temp = std::move(iInstanceData);
				auto queue = temp->asyncEventQueue;
				if (queue.use_count() == 2)
					queue->persist(queue); // keeps event queue around (cached) for a second 
			}
		}
		void unsubscribe(handle aHandle) const
		{
			destroyable_mutex_lock_guard<event_mutex> guard{ instance().mutex };
			instance().notifications.erase(std::remove(instance().notifications.begin(), instance().notifications.end(), aHandle.iHandler), instance().notifications.end());
			if (aHandle.iHandler->iUniqueId != 0)
			{
				auto existing = instance().uniqueIdMap.find(aHandle.iHandler->iUniqueId);
				if (existing != instance().uniqueIdMap.end())
					instance().uniqueIdMap.erase(existing);
			}
			instance().handlers.erase(aHandle.iHandler);
		}
		bool has_instance() const
		{
			return iInstanceData != nullptr;
		}
		instance_data& instance() const
		{
			if (iInstanceData == nullptr)
			{
				auto newInstance = allocator().allocate();
				try
				{
					allocator().construct(newInstance);
					newInstance->instancePtr = std::make_shared<ptr>(this);
					newInstance->asyncEventQueue = async_event_queue::instance();
				}
				catch (...)
				{
					allocator().deallocate(newInstance);
					throw;
				}
				iInstanceData = decltype(iInstanceData){
					newInstance,
					[](instance_data* aInstance)
					{
						allocator().destroy(aInstance);
						allocator().deallocate(aInstance);
					}};
			}
			return *iInstanceData;
		}
		static instance_allocator& allocator()
		{
			static instance_allocator sAllocator;
			return sAllocator;
		}
	private:
		mutable std::unique_ptr<instance_data, std::function<void(instance_data*)>> iInstanceData;
	};

	class sink
	{
	private:
		enum controller_op_e
		{
			AddRef,
			Release
		};
	public:
		sink()
		{
		}
		template <typename... Arguments>
		sink(event_handle<Arguments...> aHandle) :
			iControllers{[aHandle](controller_op_e aOperation)
				{ 
					if (!aHandle.iEvent.expired())
					{
						switch (aOperation)
						{
						case AddRef:
							++aHandle.iHandler->iSinkReferenceCount;
							break;
						case Release:
							if (--aHandle.iHandler->iSinkReferenceCount == 0 && !aHandle.iEvent.expired())
								(**aHandle.iEvent.lock()).unsubscribe(aHandle);
							break;
						}
					}
				}}
		{
			add_ref();
		}
		sink(const sink& aSink) : 
			iControllers{aSink.iControllers}
		{
			add_ref();
		}
		sink& operator=(const sink& aSink)
		{
			if (this == &aSink)
				return *this;
			release();
			iControllers = aSink.iControllers;
			add_ref();
			return *this;
		}
		template <typename... Arguments>
		sink& operator=(event_handle<Arguments...> aHandle)
		{
			return *this = sink{aHandle};
		}
		template <typename... Arguments>
		sink& operator+=(event_handle<Arguments...> aHandle)
		{
			sink s{aHandle};
			s.add_ref();
			iControllers.insert(iControllers.end(), s.iControllers.begin(), s.iControllers.end());
			return *this;
		}
		~sink()
		{
			release();
		}
	private:
		void add_ref() const
		{
			for (auto c : iControllers)
				c(AddRef);
		}
		void release() const
		{
			for (auto c : iControllers)
				c(Release);
		}
	private:
		std::vector<std::function<void(controller_op_e)>> iControllers;
	};
}