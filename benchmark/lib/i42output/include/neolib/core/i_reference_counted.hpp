// i_reference_counted.hpp
/*
 *  Copyright (c) 2019, 2020 Leigh Johnston.
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
#include <type_traits>

namespace neolib
{
    class i_reference_counted;

    class i_ref_control_block
    {
    public:
        virtual ~i_ref_control_block() = default;
    public:
        virtual i_reference_counted* ptr() const noexcept = 0;
        virtual bool expired() const noexcept = 0;
        virtual long weak_use_count() const noexcept = 0;
        virtual void add_ref(long aCount = 1) noexcept = 0;
        virtual void release(long aCount = 1) = 0;
    };

    class i_reference_counted
    {
    public:
        struct too_many_references : std::logic_error { too_many_references() : std::logic_error("i_reference_counted::too_many_references") {} };
        struct release_during_destruction : std::logic_error { release_during_destruction() : std::logic_error("i_reference_counted::release_during_destruction") {} };
    public:
        virtual ~i_reference_counted() = default;
    public:
        virtual void add_ref(long aCount = 1) const noexcept = 0;
        virtual void release(long aCount = 1) const = 0;
        virtual long use_count() const noexcept = 0;
        virtual const i_reference_counted* release_and_take_ownership() const = 0;
        virtual i_reference_counted* release_and_take_ownership() = 0;
        virtual bool pinned() const noexcept = 0;
        virtual void pin() const noexcept = 0;
        virtual void unpin() const = 0;
    public:
        virtual i_ref_control_block& control_block() = 0;
    };

    template <typename Interface>
    class i_ref_ptr
    {
    public:
        struct no_object : std::logic_error { no_object() : std::logic_error("i_ref_ptr::no_object") {} };
        struct no_managed_object : std::logic_error { no_managed_object() : std::logic_error("i_ref_ptr::no_managed_object") {} };
        struct interface_not_found : std::logic_error { interface_not_found() : std::logic_error("i_ref_ptr::interface_not_found") {} };
    public:
        typedef i_ref_ptr abstract_type;
    public:
        virtual ~i_ref_ptr() = default;
    public:
        virtual bool reference_counted() const noexcept = 0;
        virtual long use_count() const noexcept = 0;
        virtual bool unique() const noexcept = 0;
        virtual void reset() = 0;
        virtual void reset(Interface* aPtr) = 0;
        virtual void reset(Interface* aPtr, Interface* aManagedPtr) = 0;
        virtual void reset(Interface* aPtr, Interface* aManagedPtr, bool aReferenceCounted, bool aAddRef) = 0;
        virtual Interface* release() = 0;
        virtual Interface* detach() = 0;
        virtual bool valid() const noexcept = 0;
        virtual bool managing() const noexcept = 0;
        virtual Interface* ptr() const noexcept = 0;
        virtual Interface* managed_ptr() const noexcept = 0;
        virtual Interface* operator->() const = 0;
        virtual Interface& operator*() const = 0;
        i_ref_ptr& operator=(const i_ref_ptr& aOther) { reset(aOther.ptr(), aOther.managed_ptr(), aOther.reference_counted(), true); return *this; }
        template <typename Interface2>
        i_ref_ptr& operator=(const i_ref_ptr<Interface2>& aOther) { reset(aOther.ptr(), aOther.managed_ptr(), aOther.reference_counted(), true); return *this; }
        explicit operator bool() const noexcept { return valid(); }
        bool operator==(std::nullptr_t) const noexcept { return !valid(); }
        bool operator!=(std::nullptr_t) const noexcept { return valid(); }
        template <typename Interface2>
        bool operator==(const i_ref_ptr<Interface2>& aOther) const noexcept { return ptr() == aOther.ptr(); }
        template <typename Interface2>
        bool operator!=(const i_ref_ptr<Interface2>& aOther) const noexcept { return ptr() != aOther.ptr(); }
        template <typename Interface2>
        bool operator<(const i_ref_ptr<Interface2>& aOther) const noexcept { return ptr() < aOther.ptr(); }
    };

    template <typename Interface1, typename Interface2>
    inline bool operator==(const i_ref_ptr<Interface1>& lhs, const Interface2* rhs) noexcept
    {
        return static_cast<const maybe_abstract_t<Interface1>*>(lhs.ptr()) == static_cast<const maybe_abstract_t<Interface1>*>(rhs);
    }

    template <typename Interface1, typename Interface2>
    inline bool operator==(const Interface2* lhs, const i_ref_ptr<Interface1>& rhs) noexcept
    {
        return static_cast<const maybe_abstract_t<Interface1>*>(lhs) == static_cast<const maybe_abstract_t<Interface1>*>(rhs.ptr());
    }

    template <typename Interface1, typename Interface2>
    inline bool operator!=(const i_ref_ptr<Interface1>& lhs, const Interface2* rhs) noexcept
    {
        return !(lhs == rhs);
    }

    template <typename Interface1, typename Interface2>
    inline bool operator!=(const Interface2* lhs, const i_ref_ptr<Interface1>& rhs) noexcept
    {
        return !(lhs == rhs);
    }

    template <typename Interface1, typename Interface2>
    inline bool operator<(const i_ref_ptr<Interface1>& lhs, const Interface2* rhs) noexcept
    {
        return static_cast<const maybe_abstract_t<Interface1>*>(lhs.ptr()) < static_cast<const maybe_abstract_t<Interface1>*>(rhs);
    }

    template <typename Interface1, typename Interface2>
    inline bool operator<(const Interface2* lhs, const i_ref_ptr<Interface1>& rhs) noexcept
    {
        return static_cast<const maybe_abstract_t<Interface1>*>(lhs) < static_cast<const maybe_abstract_t<Interface1>*>(rhs.ptr());
    }

    template <typename Interface>
    class i_weak_ref_ptr : public i_ref_ptr<Interface>
    {
    public:
        struct bad_release : std::logic_error { bad_release() : std::logic_error("i_weak_ref_ptr::bad_release") {} };
        struct wrong_object : std::logic_error { wrong_object() : std::logic_error("i_weak_ref_ptr::wrong_object") {} };
    public:
        virtual bool expired() const noexcept = 0;
    };
}
