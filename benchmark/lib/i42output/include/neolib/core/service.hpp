// service.hpp
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
#include <neolib/core/i_service.hpp>

namespace neolib
{
    namespace services
    {
        template <typename Service>
        inline std::atomic<Service*>& service_ptr()
        {
            static std::atomic<Service*> sService;
            return sService;
        }

        bool service_provider_allocated();
        i_service_provider& allocate_service_provider();
        i_service_provider& get_service_provider();
        void set_service_provider(i_service_provider& aServiceProvider);

        template <typename Service>
        inline bool service_registered()
        {
            return get_service_provider().service_registered(Service::iid());
        }

        template <typename Service>
        inline void register_service(Service& aService)
        {
            get_service_provider().register_service(aService, Service::iid());
            service_ptr<Service>() = &aService;
        }

        template <typename Service>
        inline void unregister_service(Service& aService)
        {
            get_service_provider().unregister_service(Service::iid());
            service_ptr<Service>() = nullptr;
        }

        template <typename Service>
        Service& start_service();

        template <typename Service>
        void teardown_service();

        template <typename Service>
        inline Service& service()
        {
            if (service_ptr<Service>() != nullptr)
                return *service_ptr<Service>();
            std::unique_lock lock{ get_service_provider() };
            if (!service_registered<Service>())
                register_service(start_service<Service>());
            else
                service_ptr<Service>() = &get_service_provider().service<Service>();
            return *service_ptr<Service>();
        }
    }

    using namespace services;
}
