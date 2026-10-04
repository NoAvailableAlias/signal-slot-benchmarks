// lifetime.hpp
/*
 *  Copyright (c) 2020 Leigh Johnston.
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
#include <atomic>
#include <neolib/core/i_lifetime.hpp>
#include <neolib/core/reference_counted.hpp>

namespace neolib
{
    class lifetime_control_block : public reference_counted<i_lifetime_control_block>
    {
    public:
        using abstract_type = i_lifetime_control_block;
    public:
        lifetime_control_block(lifetime_state aState) :
            iState{ aState }
        {
        }
    public:
        lifetime_state state() const
        {
            return iState.load();
        }
        void set_state(lifetime_state aState)
        {
            iState.store(aState);
        };
    private:
        std::atomic<lifetime_state> iState;
    };

    template <lifetime_state RequiredState>
    class lifetime_flag : public i_lifetime_flag
    {
    public:
        lifetime_flag(const i_lifetime& aSubject);
        template <typename Subject>
        lifetime_flag(const Subject& aSubject, std::enable_if_t<std::is_base_of_v<i_lifetime, Subject>, sfinae> = {}) :
            lifetime_flag{ static_cast<const i_lifetime&>(aSubject), } {}
        template <typename Subject>
        lifetime_flag(const Subject& aSubject, std::enable_if_t<!std::is_base_of_v<i_lifetime, Subject>, sfinae> = {}) :
            lifetime_flag{ dynamic_cast<const i_lifetime&>(aSubject), } {}
        lifetime_flag(const lifetime_flag& aOther);
        lifetime_flag(lifetime_flag&& aOther);
        ~lifetime_flag();
    public:
        lifetime_flag& operator=(const lifetime_flag& aOther);
        lifetime_flag& operator=(lifetime_flag&& aOther);
    public:
        bool is_creating() const final;
        bool is_alive() const final;
        bool is_destroying() const final;
        bool is_destroyed() const final;
        operator bool() const final;
    public:
        bool debug() const override;
        void set_debug(bool aDebug = true) override;
    private:
        ref_ptr<i_lifetime_control_block> iState;
        bool iDebug;
    };

    typedef lifetime_flag<lifetime_state::Destroying> destroying_flag;
    typedef std::optional<destroying_flag> optional_destroying_flag;
    typedef lifetime_flag<lifetime_state::Destroyed> destroyed_flag;
    typedef std::optional<destroyed_flag> optional_destroyed_flag;

    template <typename Base = i_lifetime>
    class lifetime : public Base
    {
    public:
        using typename Base::not_creating;
        using typename Base::already_destroyed;
    public:
        typedef neolib::destroyed_flag destroyed_flag;
    public:
        lifetime(lifetime_state aState = lifetime_state::Alive);
        virtual ~lifetime();
    public:
        lifetime_state object_state() const final;
        i_ref_ptr<i_lifetime_control_block>& object_state_ptr() const final;
        bool is_creating() const final;
        bool is_alive() const final;
        bool is_destroying() const final;
        bool is_destroyed() const final;
        void set_alive() override;
        void set_destroying() override;
        void set_destroyed() override;
    private:
        mutable ref_ptr<i_lifetime_control_block> iState;
    };

    template <typename Base>
    inline lifetime<Base>::lifetime(lifetime_state aState) : iState{ make_ref<lifetime_control_block>(aState) }
    {
    }

    template <typename Base>
    inline lifetime<Base>::~lifetime()
    {
        if (!is_destroyed())
        {
            set_destroying();
            set_destroyed();
        }
    }

    template <typename Base>
    inline lifetime_state lifetime<Base>::object_state() const
    {
        return *iState;
    }

    template <typename Base>
    i_ref_ptr<i_lifetime_control_block>& lifetime<Base>::object_state_ptr() const
    {
        return iState;
    }

    template <typename Base>
    inline bool lifetime<Base>::is_creating() const
    {
        return *iState == lifetime_state::Creating;
    }

    template <typename Base>
    inline bool lifetime<Base>::is_alive() const
    {
        return *iState == lifetime_state::Alive;
    }

    template <typename Base>
    inline bool lifetime<Base>::is_destroying() const
    {
        return *iState == lifetime_state::Destroying;
    }

    template <typename Base>
    inline bool lifetime<Base>::is_destroyed() const
    {
        return *iState == lifetime_state::Destroyed;
    }

    template <typename Base>
    inline void lifetime<Base>::set_alive()
    {
        if (!is_creating())
            throw not_creating();
        *iState = lifetime_state::Alive;
    }

    template <typename Base>
    inline void lifetime<Base>::set_destroying()
    {
        if (!is_destroying())
        {
            if (is_destroyed())
                throw already_destroyed();
            *iState = lifetime_state::Destroying;
        }
    }

    template <typename Base>
    inline void lifetime<Base>::set_destroyed()
    {
        if (!is_destroyed())
        {
            if (*iState == lifetime_state::Creating || *iState == lifetime_state::Alive)
                set_destroying();
            *iState = lifetime_state::Destroyed;
        }
    }
}

