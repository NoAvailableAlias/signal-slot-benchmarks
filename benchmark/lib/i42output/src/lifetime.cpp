// lifetime.cpp
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
#include <neolib/lifetime.hpp>

namespace neolib
{
    template <lifetime_state RequiredState>
    lifetime_flag<RequiredState>::lifetime_flag(const i_lifetime& aSubject) : 
        iState{ aSubject.object_state_ptr() }, iDebug{ false }
    {
    }

    template <lifetime_state RequiredState>
    lifetime_flag<RequiredState>::lifetime_flag(const lifetime_flag& aOther) : 
        iState { aOther.iState }, iDebug{ false }
    {
    }

    template <lifetime_state RequiredState>
    lifetime_flag<RequiredState>::~lifetime_flag()
    {
    }

    template <lifetime_state RequiredState>
    bool lifetime_flag<RequiredState>::is_creating() const
    {
        return *iState == lifetime_state::Creating;
    }

    template <lifetime_state RequiredState>
    bool lifetime_flag<RequiredState>::is_alive() const
    {
        return *iState == lifetime_state::Alive;
    }

    template <lifetime_state RequiredState>
    bool lifetime_flag<RequiredState>::is_destroying() const
    {
        return *iState == lifetime_state::Destroying;
    }

    template <lifetime_state RequiredState>
    bool lifetime_flag<RequiredState>::is_destroyed() const
    {
        return *iState == lifetime_state::Destroyed;
    }

    template <lifetime_state RequiredState>
    lifetime_flag<RequiredState>::operator bool() const
    {
        return *iState == RequiredState;
    }

    template <lifetime_state RequiredState>
    bool lifetime_flag<RequiredState>::debug() const
    {
        return iDebug;
    }
    
    template <lifetime_state RequiredState>
    void lifetime_flag<RequiredState>::set_debug(bool aDebug)
    {
        iDebug = aDebug;
    }

    template class lifetime_flag<lifetime_state::Creating>;
    template class lifetime_flag<lifetime_state::Alive>;
    template class lifetime_flag<lifetime_state::Destroying>;
    template class lifetime_flag<lifetime_state::Destroyed>;

    lifetime::lifetime(lifetime_state aState) : iState{ std::make_shared<std::atomic<lifetime_state>>(aState) }
    {
    }

    lifetime::~lifetime()
    {
        if (!is_destroyed())
        {
            set_destroying();
            set_destroyed();
        }
    }

    lifetime_state lifetime::object_state() const
    {
        return *iState;
    }

    std::shared_ptr<std::atomic<lifetime_state>> lifetime::object_state_ptr() const
    {
        return iState;
    }

    bool lifetime::is_creating() const
    {
        return *iState == lifetime_state::Creating;
    }

    bool lifetime::is_alive() const
    {
        return *iState == lifetime_state::Alive;
    }

    bool lifetime::is_destroying() const
    {
        return *iState == lifetime_state::Destroying;
    }

    bool lifetime::is_destroyed() const
    {
        return *iState == lifetime_state::Destroyed;
    }

    void lifetime::set_alive()
    {
        if (!is_creating())
            throw not_creating();
        *iState = lifetime_state::Alive;
    }

    void lifetime::set_destroying()
    {
        if (!is_destroying())
        {
            if (is_destroyed())
                throw already_destroyed();
            *iState = lifetime_state::Destroying;
        }
    }

    void lifetime::set_destroyed()
    {
        if (!is_destroyed())
        {
            if (*iState == lifetime_state::Creating || *iState == lifetime_state::Alive)
                set_destroying();
            *iState = lifetime_state::Destroyed;
        }
    }
}
