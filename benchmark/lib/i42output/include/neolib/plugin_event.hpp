// plugin_event.hpp
/*
  Transplanted from neogfx C++ App/Game Engine
  Copyright (c) 2015, 2020 Leigh Johnston.  All Rights Reserved.
  
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
#include <neolib/event.hpp>
#include <neolib/i_plugin_event.hpp>

namespace neolib
{
    namespace plugin_events
    {
        using neolib::async_event_queue;
        using neolib::event_trigger_type;

        template <typename... Args>
        class event : public i_event<Args...>, public neolib::event<Args...>
        {
            typedef neolib::event<Args...> base_type;
        public:
            using typename i_event<Args...>::abstract_callback;
        public:
            typedef base_type event_type;
            typedef i_event<Args...> abstract_plugin_event_type;
        public:
            using event_type::event_type;
        public:
            using event_type::subscribe;
            using event_type::operator();
            using event_type::unsubscribe;
        public:
            const neolib::i_event& raw_event() const override
            {
                return *this;
            }
            neolib::i_event& raw_event() override
            {
                return *this;
            }
        public:
            void pre_trigger() const override
            {
                event_type::pre_trigger();
            }
        public:
            bool trigger(Args... aArguments) const override
            {
                return event_type::trigger(aArguments...);
            }
            bool sync_trigger(Args... aArguments) const override
            {
                return event_type::sync_trigger(aArguments...);
            }
            void async_trigger(Args... aArguments) const override
            {
                event_type::async_trigger(rvalue_cast<Args>(aArguments)...);
            }
            bool accepted() const override
            {
                return event_type::accepted();
            }
            void accept() const override
            {
                event_type::accept();
            }
            void ignore() const override
            {
                event_type::ignore();
            }
        private:
            event_handle do_subscribe(const abstract_callback& aCallback, const void* aUniqueId = nullptr) const override
            {
                auto callback = aCallback.clone();
                return event_type::subscribe(
                        [callback](Args&& ... aArguments)
                        {
                            (*callback)(std::forward<Args>(aArguments)...);
                        },
                        aUniqueId);
            }
            void do_unsubscribe(event_handle aHandle) const override
            {
                return event_type::unsubscribe(aHandle);
            }
            void do_unsubscribe(const void* aUniqueId) const override
            {
                return event_type::unsubscribe(aUniqueId);
            }
        };
    }
}