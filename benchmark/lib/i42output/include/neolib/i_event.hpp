// event.hpp
/*
Transplanted from neogfx C++ App/Game Engine
Copyright (c) 2015, 2018, 2020 Leigh Johnston.  All Rights Reserved.

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
#include <neolib/mutex.hpp>
#include <neolib/i_reference_counted.hpp>
#include <neolib/jar.hpp>
#include <neolib/i_map.hpp>

namespace neolib
{
    struct event_destroyed : std::logic_error { event_destroyed() : std::logic_error{ "neolib::event_destroyed" } {} };
    struct event_queue_destroyed : std::logic_error { event_queue_destroyed() : std::logic_error{ "neolib::event_queue_destroyed" } {} };
    struct event_handler_not_found : std::logic_error { event_handler_not_found() : std::logic_error{ "neolib::event_handler_not_found" } {} };

    inline switchable_mutex& event_mutex()
    {
        static switchable_mutex sMutex;
        return sMutex;
    }

    namespace event_system
    {
        inline void set_single_threaded()
        {
            event_mutex().set_single_threaded();
        }

        inline void set_multi_threaded()
        {
            event_mutex().set_multi_threaded_spinlock();
        }
    }

    class i_event : public i_cookie_consumer
    {
    public:
        virtual ~i_event() = default;
    public:
        virtual void release_control() = 0;
        virtual void handle_in_same_thread_as_emitter(cookie aHandleId) = 0;
        virtual void handler_is_stateless(cookie aHandleId) = 0;
    public:
        virtual void pre_trigger() const = 0;
    public:
        virtual void push_context() const = 0;
        virtual void pop_context() const = 0;
    public:
        virtual bool accepted() const = 0;
        virtual void accept() const = 0;
        virtual void ignore() const = 0;
    public:
        virtual bool filtered() const = 0;
        virtual void filter_added() const = 0;
        virtual void filter_removed() const = 0;
        virtual void filters_removed() const = 0;
    };

    class i_event_control
    {
    public:
        virtual ~i_event_control() = default;
    public:
        virtual void add_ref() = 0;
        virtual void release() = 0;
        virtual bool valid() const = 0;
        virtual i_event& get() const = 0;
    public:
        virtual void reset() = 0;
    };

    class i_event_callback : public i_reference_counted
    {
    public:
        typedef i_event_callback abstract_type;
    public:
        virtual ~i_event_callback() = default;
    public:
        virtual const i_event& event() const = 0;
        virtual const void* identity() const = 0;
        virtual void call() const = 0;
    };

    class i_event_filter
    {
    public:
        virtual ~i_event_filter() = default;
    public:
        virtual void pre_filter_event(const i_event& aEvent) = 0;
        virtual void filter_event(const i_event& aEvent) = 0;
    };

    class i_event_filter_registry
    {
    public:
        virtual void install_event_filter(i_event_filter& aFilter, const i_event& aEvent) = 0;
        virtual void uninstall_event_filter(i_event_filter& aFilter, const i_event& aEvent) = 0;
        virtual void uninstall_event_filter(const i_event& aEvent) = 0;
    public:
        virtual void pre_filter_event(const i_event& aEvent) const = 0;
        virtual void filter_event(const i_event& aEvent) const = 0;
    };
}
