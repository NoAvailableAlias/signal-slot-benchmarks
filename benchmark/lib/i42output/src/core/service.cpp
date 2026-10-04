// service.cpp
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

#include <neolib/neolib.hpp>
#include <unordered_map>
#include <memory>
#include <mutex>
#include <neolib/core/service.hpp>

namespace neolib::services
{
    struct service_provider : public i_service_provider
    {
        mutable std::recursive_mutex mutex;
        std::unordered_map<uuid, i_service*> services;

        bool try_lock() final
        {
            return mutex.try_lock();
        }

        void lock() final
        {
            mutex.lock();
        }

        void unlock() final
        {
            mutex.unlock();
        }

        bool service_registered(uuid aServiceIid) const final
        {
            std::unique_lock lock{ mutex };
            return services.find(aServiceIid) != services.end();
        }

        i_service& service(uuid aServiceIid) final
        {
            std::unique_lock lock{ mutex };
            auto existing = services.find(aServiceIid);
            if (existing != services.end())
                return *existing->second;
            throw service_not_found();
        }

        void register_service(i_service& aService, uuid aServiceIid) final
        {
            std::unique_lock lock{ mutex };
            services[aServiceIid] = &aService;
        }

        void unregister_service(uuid aServiceIid) final
        {
            std::unique_lock lock{ mutex };
            auto existing = services.find(aServiceIid);
            if (existing != services.end())
            {
                services.erase(existing);
                return;
            }
            throw service_not_found();
        }

        void migrate_to(i_service_provider& aOtherProvider) final
        {
            std::scoped_lock lock{ mutex, aOtherProvider };
            for (auto& service : services)
                aOtherProvider.register_service(*service.second, service.first);
            services.clear();
        }
    };

    struct service_provider_instance
    {
        std::recursive_mutex mutex;
        std::shared_ptr<i_service_provider> instance;
    };

    service_provider_instance& get_service_provider_instance()
    {
        static service_provider_instance sInstance;
        return sInstance;
    }

    bool service_provider_allocated()
    {
        auto& instance = get_service_provider_instance();
        std::unique_lock lock{ instance.mutex };
        return instance.instance != nullptr;
    }

    i_service_provider& allocate_service_provider()
    {
        auto& instance = get_service_provider_instance();
        std::unique_lock lock{ instance.mutex };
        if (instance.instance != nullptr)
            throw service_provider_instance_exists();
        instance.instance = std::make_shared<service_provider>();
        return *instance.instance;
    }

    i_service_provider& get_service_provider()
    {
        auto& instance = get_service_provider_instance();
        std::unique_lock lock{ instance.mutex };
        if (instance.instance == nullptr)
            throw no_service_provider_instance();
        return *instance.instance;
    }

    void set_service_provider(i_service_provider& aServiceProvider)
    {
        auto& instance = get_service_provider_instance();
        std::unique_lock lock{ instance.mutex };
        auto previous = instance.instance;
        instance.instance = std::shared_ptr<i_service_provider>{ std::shared_ptr<i_service_provider>{}, &aServiceProvider };
        if (previous != nullptr)
            previous->migrate_to(aServiceProvider);
    }
}
