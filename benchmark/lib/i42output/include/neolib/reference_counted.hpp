// reference_counted.hpp - v1.0
/*
 *  Copyright (c) 2007 Leigh Johnston.
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
#include <vector>
#include <functional>
#include "i_discoverable.hpp"

namespace neolib
{
    template <typename, bool>
    class reference_counted;

    class ref_control_block : public i_ref_control_block
    {
        template <typename, bool>
        friend class reference_counted;
    public:
        ref_control_block(i_reference_counted& aObject) :
            iObject{ &aObject },
            iWeakUseCount{ 0 }
        {
        }
    public:
        i_reference_counted* ptr() const override
        {
            return iObject;
        }
        bool expired() const override
        {
            return iObject == nullptr;
        }
        int32_t weak_use_count() const override
        {
            return iWeakUseCount;
        }
        void add_ref() override
        {
            ++iWeakUseCount;
        }
        void release() override
        {
            if (--iWeakUseCount <= 0 && expired())
                delete this;
        }
    private:
        void set_expired()
        {
            iObject = nullptr;
            if (weak_use_count() <= 0)
                delete this;
        }
    private:
        i_reference_counted* iObject;
        int32_t iWeakUseCount;
    };

    template <typename Base, bool DeallocateOnRelease = true>
    class reference_counted : public Base
    {
        typedef Base base_type;
    public:
        using typename base_type::release_during_destruction;
        using typename base_type::too_many_references;
    public:
        reference_counted() : iDestroying{ false }, iReferenceCount{ 0 }, iPinned{ false }, iControlBlock{ nullptr }
        {
        }
        reference_counted(const reference_counted& aOther) : iDestroying{ false }, iReferenceCount{ 0 }, iPinned{ aOther.iPinned }, iControlBlock{ nullptr }
        {
        }
        ~reference_counted()
        {
            iDestroying = true;
            if (iControlBlock != nullptr)
                iControlBlock->set_expired();
        }
        reference_counted& operator=(const reference_counted&)
        {
            // do nothing
            return *this;
        }
    public:
        void add_ref() const override
        {
            ++iReferenceCount;
        }
        void release() const override
        {
            if (--iReferenceCount <= 0 && !iPinned)
            {
                if (!iDestroying)
                    destroy();
                else
                    throw release_during_destruction();
            }
        }
        int32_t reference_count() const override
        {
            return iReferenceCount;
        }
        const base_type* release_and_take_ownership() const override
        {
            if (iReferenceCount != 1)
                throw too_many_references();
            iReferenceCount = 0;
            return this;
        }
        base_type* release_and_take_ownership() override
        {
            return const_cast<base_type*>(to_const(*this).release_and_take_ownership());
        }
        void pin() const override
        {
            iPinned = true;
        }
        void unpin() const override
        {
            iPinned = false;
            if (iReferenceCount <= 0)
                destroy();
        }
    public:
        i_ref_control_block& control_block() override
        {
            if (iControlBlock == nullptr)
                iControlBlock = new ref_control_block{ *this };
            return *iControlBlock;
        }
    private:
        void destroy() const
        {
            if constexpr (DeallocateOnRelease)
                delete this;
            else
                (*this).~reference_counted();
        }
    private:
        bool iDestroying;
        mutable int32_t iReferenceCount;
        mutable bool iPinned;
        mutable ref_control_block* iControlBlock;
    };

    template <typename Interface>
    class ref_ptr : public i_ref_ptr<abstract_t<Interface>>
    {
    public:
        typedef i_ref_ptr<abstract_t<Interface>> abstract_type;
        typedef typename abstract_type::no_object no_object;
        typedef typename abstract_type::interface_not_found interface_not_found;
    public:
        ref_ptr(Interface* aObject = nullptr) :
            iObject{ aObject }, iReferenceCounted{ true }
        {
            if (valid())
                iObject->add_ref();
        }
        ref_ptr(Interface& aObject) :
            iObject{ &aObject }, iReferenceCounted{ aObject.reference_count() > 0 }
        {
            if (reference_counted())
                iObject->add_ref();
        }
        ref_ptr(const ref_ptr& aOther) :
            iObject{ aOther.ptr() }, iReferenceCounted{ aOther.reference_counted() }
        {
            if (valid() && iReferenceCounted)
                iObject->add_ref();
        }
        ref_ptr(const abstract_type& aOther) :
            iObject{ nullptr }, iReferenceCounted{ true }
        {
            reset(aOther.ptr(), aOther.reference_counted());
        }
        ref_ptr(i_discoverable& aDiscoverable) :
            iObject{ nullptr }, iReferenceCounted{ true }
        {
            if (!aDiscoverable.discover(*this))
                throw interface_not_found();
        }
        template <typename Interface2, typename = std::enable_if_t<std::is_base_of_v<Interface, Interface2>, sfinae>>
        ref_ptr(const ref_ptr<Interface2>& aOther) :
            iObject{ aOther.ptr() }, iReferenceCounted{ aOther.reference_counted() }
        {
            if (valid() && iReferenceCounted)
                iObject->add_ref();
        }
        template <typename Interface2, typename = std::enable_if_t<std::is_base_of_v<Interface, Interface2>, sfinae>>
        ref_ptr(const i_ref_ptr<Interface2>& aOther) :
            iObject{ aOther.ptr() }, iReferenceCounted{ aOther.reference_counted() }
        {
            if (valid() && iReferenceCounted)
                iObject->add_ref();
        }
        ~ref_ptr()
        {
            if (valid() && iReferenceCounted)
                iObject->release();
        }
        ref_ptr& operator=(const ref_ptr& aOther)
        {
            reset(aOther.ptr(), aOther.reference_counted());
            return *this;
        }
        ref_ptr& operator=(const abstract_type& aOther)
        {
            if (&aOther == this)
                return *this;
            reset(aOther.ptr(), aOther.reference_counted());
            return *this;
        }
        template <typename Interface2, typename = std::enable_if_t<std::is_base_of_v<Interface, Interface2>, sfinae>>
        ref_ptr& operator=(const ref_ptr<Interface2>& aOther)
        {
            if (&aOther == this)
                return *this;
            reset(aOther.ptr(), aOther.reference_counted());
            return *this;
        }
        template <typename Interface2, typename = std::enable_if_t<std::is_base_of_v<Interface, Interface2>, sfinae>>
        ref_ptr& operator=(const i_ref_ptr<Interface2>& aOther)
        {
            if (&aOther == this)
                return *this;
            reset(aOther.ptr(), aOther.reference_counted());
            return *this;
        }
        ref_ptr& operator=(nullptr_t)
        {
            reset();
            return *this;
        }
    public:
        template <typename Interface2>
        ref_ptr<Interface2> as()
        {
            return ref_ptr<Interface2>{ *this };
        }
    public:
        bool reference_counted() const override
        {
            return iReferenceCounted;
        }
        void reset(abstract_t<Interface>* aObject, bool aReferenceCounted = true) override
        {
            Interface* compatibleObject = dynamic_cast<Interface*>(aObject);
            if (aObject != nullptr && compatibleObject == nullptr)
                throw std::bad_cast();
            reset<Interface>(compatibleObject, aReferenceCounted);
        }
        Interface* release() override
        {
            if (iObject == nullptr)
                throw no_object();
            Interface* releasedObject = static_cast<Interface*>(iObject->release_and_take_ownership());
            iObject = nullptr;
            return releasedObject;
        }
        bool valid() const override
        {
            return iObject != nullptr;
        }
        Interface* ptr() const override
        {
            return iObject;
        }
        Interface* operator->() const override
        {
            if (iObject == nullptr)
                throw no_object();
            return iObject;
        }
        Interface& operator*() const override
        {
            if (iObject == nullptr)
                throw no_object();
            return *iObject;
        }
    public:
        template <typename Interface2 = Interface, typename = std::enable_if_t<std::is_base_of_v<Interface, Interface2>, sfinae>>
        void reset(Interface2* aObject = nullptr, bool aReferenceCounted = true)
        {
            ref_ptr copy(*this);
            if (valid() && iReferenceCounted)
                iObject->release();
            iObject = aObject;
            iReferenceCounted = aReferenceCounted;
            if (valid() && iReferenceCounted)
                iObject->add_ref();
        }
    private:
        Interface* iObject;
        bool iReferenceCounted;
    };

    template <typename Interface>
    class weak_ref_ptr : public i_weak_ref_ptr<abstract_t<Interface>>
    {
        typedef i_weak_ref_ptr<abstract_t<Interface>> base_type;
    public:
        typedef i_weak_ref_ptr<abstract_t<Interface>> abstract_type;
        typedef typename base_type::no_object no_object;
        typedef typename base_type::interface_not_found interface_not_found;
        typedef typename base_type::bad_release bad_release;
        typedef typename base_type::wrong_object wrong_object;
    public:
        weak_ref_ptr(Interface* aObject = nullptr) :
            iControlBlock{ nullptr }
        {
            update_control_block(aObject);
        }
        weak_ref_ptr(Interface& aObject) :
            iControlBlock{ nullptr }
        {
            update_control_block(&aObject);
        }
        weak_ref_ptr(const weak_ref_ptr& aOther) :
            iControlBlock{ nullptr }
        {
            update_control_block(aOther.ptr());
        }
        weak_ref_ptr(const i_ref_ptr<abstract_t<Interface>>& aOther) :
            iControlBlock{ nullptr }
        {
            update_control_block(aOther.ptr());
        }
        weak_ref_ptr(i_discoverable& aDiscoverable) :
            iControlBlock{ nullptr }
        {
            if (!aDiscoverable.discover(*this))
                throw interface_not_found();
        }
        ~weak_ref_ptr()
        {
            if (iControlBlock != nullptr)
                iControlBlock->release();
        }
        weak_ref_ptr& operator=(const weak_ref_ptr& aOther)
        {
            reset(aOther.ptr());
            return *this;
        }
        weak_ref_ptr& operator=(const i_ref_ptr<abstract_t<Interface>>& aOther)
        {
            reset(aOther.ptr());
            return *this;
        }
        weak_ref_ptr& operator=(nullptr_t)
        {
            reset();
            return *this;
        }
    public:
        bool reference_counted() const override
        {
            return false;
        }
        void reset(abstract_t<Interface>* aObject = nullptr, bool = false) override
        {
            weak_ref_ptr copy(*this);
            update_control_block(aObject);
        }
        Interface* release() override
        {
            if (expired())
                throw no_object();
            else
                throw bad_release();
        }
        bool expired() const override
        {
            return iControlBlock == nullptr || iControlBlock->expired();
        }
        Interface* ptr() const override
        {
            return iControlBlock != nullptr ? iControlBlock->ptr() : nullptr;
        }
        Interface* operator->() const override
        {
            if (expired())
                throw no_object();
            return ptr();
        }
        Interface& operator*() const override
        {
            if (expired())
                throw no_object();
            return *ptr();
        }
    private:
        void update_control_block(Interface* aObject)
        {
            auto controlBlock = aObject != nullptr ? &(*aObject).control_block() : nullptr;
            if (iControlBlock != controlBlock)
            {
                if (iControlBlock != nullptr)
                    iControlBlock->release();
                iControlBlock = controlBlock;
                if (iControlBlock != nullptr)
                    iControlBlock->add_ref();
            }
        }
    private:
        i_ref_control_block* iControlBlock;
    };

    template <typename Interface>
    inline bool operator<(const ref_ptr<Interface>& lhs, const ref_ptr<Interface>& rhs)
    {
        if (lhs == rhs)
            return false;
        else if (lhs == nullptr)
            return false;
        else if (rhs == nullptr)
            return true;
        else
            return *lhs < *rhs;
    }

    template <typename ConcreteType, typename... Args>
    inline ref_ptr<ConcreteType> make_ref(Args&&... args)
    {
        return ref_ptr<ConcreteType>{ new ConcreteType{ std::forward<Args>(args)... } };
    }
}
