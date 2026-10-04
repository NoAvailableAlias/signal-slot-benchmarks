// i_service.hpp
/*
 *  Copyright (c) 2020-2025 Leigh Johnston.
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

#include <stdexcept>
#include <atomic>
#include <mutex>
#include <neolib/core/uuid.hpp>

namespace neolib
{
    namespace services
    {
        struct no_service_provider_instance : std::logic_error { no_service_provider_instance() : std::logic_error{ "neolib::services::no_service_provider_instance" } {} };
        struct service_provider_instance_exists : std::logic_error { service_provider_instance_exists() : std::logic_error{ "neolib::services::service_provider_instance_exists" } {} };
        struct service_not_found : std::logic_error { service_not_found() : std::logic_error{ "neolib::services::service_not_found" } {} };

        class i_service
        {
        public:
            virtual ~i_service() = default;
        };

        class i_service_provider
        {
        public:
            virtual ~i_service_provider() = default;
        public:
            virtual bool try_lock() = 0;
            virtual void lock() = 0;
            virtual void unlock() = 0;
        public:
            virtual bool service_registered(uuid aServiceIid) const = 0;
            virtual i_service& service(uuid aServiceIid) = 0;
            virtual void register_service(i_service& aService, uuid aServiceIid) = 0;
            virtual void unregister_service(uuid aServiceIid) = 0;
        public:
            virtual void migrate_to(i_service_provider& aOtherProvider) = 0;
        public:
            template <typename Service>
            Service& service()
            {
                return static_cast<Service&>(service(Service::iid()));
            }
        };
    }

    using namespace services;
}
