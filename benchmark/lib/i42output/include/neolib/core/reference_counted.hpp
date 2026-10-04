// reference_counted.hpp
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

#include <cassert>
#include <neolib/neolib.hpp>
#include <atomic>
#include <vector>
#include <functional>
#include <neolib/core/i_discoverable.hpp>

namespace neolib
{
    bool reference_count_checked();
    void check_reference_count(bool aCheck);

    template <typename, bool>
    class reference_counted;

    class ref_control_block : public i_ref_control_block
    {
        template <typename, bool>
        friend class reference_counted;
    public:
        ref_control_block(i_reference_counted& aManagedPtr) :
            iManagedPtr{ &aManagedPtr },
            iWeakUseCount{ 0 }
        {
        }
    public:
        i_reference_counted* ptr() const noexcept final
        {
            return iManagedPtr;
        }
        bool expired() const noexcept final
        {
            return iManagedPtr == nullptr;
        }
        long weak_use_count() const noexcept final
        {
            return iWeakUseCount;
        }
        void add_ref(long aCount = 1) noexcept final
        {
            iWeakUseCount += aCount;
        }
        void release(long aCount = 1) final
        {
            if ((iWeakUseCount -= aCount) <= 0 && expired())
                delete this;
        }
    private:
        void set_expired()
        {
            iManagedPtr = nullptr;
            if (weak_use_count() <= 0)
                delete this;
        }
    private:
        i_reference_counted* iManagedPtr;
        long iWeakUseCount;
    };

    template <typename Base, bool DeallocateOnRelease = true>
    class reference_counted : public Base
    {
        typedef Base base_type;
    public:
        using typename base_type::release_during_destruction;
        using typename base_type::too_many_references;
    public:
        reference_counted() noexcept : iDestroying{ false }, iReferenceCount{ 0 }, iPinned{ false }, iControlBlock{ nullptr }
        {
        }
        reference_counted(const reference_counted& aOther) noexcept : iDestroying{ false }, iReferenceCount{ 0 }, iPinned{ aOther.iPinned.load() }, iControlBlock{ nullptr }
        {
        }
        ~reference_counted()
        {
            assert(!(reference_count_checked() && use_count() && !pinned()) && "Destroying in-use reference counted object!");            
            iDestroying = true;
            bool expected = true;
            if (iControlled.compare_exchange_strong(expected, false))
                iControlBlock.load()->set_expired();
        }
        reference_counted& operator=(const reference_counted&)
        {
            // do nothing
            return *this;
        }
    public:
        void add_ref(long aCount = 1) const noexcept override
        {
            iReferenceCount += aCount;
        }
        void release(long aCount = 1) const override
        {
            if ((iReferenceCount -= aCount) <= 0 && !iPinned)
            {
                if (!iDestroying)
                    destroy();
                else
                    throw release_during_destruction();
            }
        }
        long use_count() const noexcept override
        {
            return iReferenceCount;
        }
        const base_type* release_and_take_ownership() const final
        {
            if (iReferenceCount != 1)
                throw too_many_references();
            iReferenceCount = 0;
            return this;
        }
        base_type* release_and_take_ownership() final
        {
            return const_cast<base_type*>(to_const(*this).release_and_take_ownership());
        }
        bool pinned() const noexcept final
        {
            return iPinned;
        }
        void pin() const noexcept final
        {
            iPinned = true;
        }
        void unpin() const final
        {
            iPinned = false;
            if (iReferenceCount <= 0)
                destroy();
        }
    public:
        i_ref_control_block& control_block() final
        {
            bool expected = false;
            if (iControlled.compare_exchange_strong(expected, true))
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
        std::atomic<bool> iDestroying;
        mutable std::atomic<long> iReferenceCount;
        mutable std::atomic<bool> iPinned;
        mutable std::atomic<bool> iControlled;
        mutable std::atomic<ref_control_block*> iControlBlock;
    };

    template <typename Interface>
    class ref_ptr : public i_ref_ptr<maybe_abstract_t<Interface>>
    {
    public:
        typedef Interface element_type;
        typedef i_ref_ptr<maybe_abstract_t<Interface>> abstract_type;
        typedef typename abstract_type::no_object no_object;
        typedef typename abstract_type::no_managed_object no_managed_object;
        typedef typename abstract_type::interface_not_found interface_not_found;
    public:
        ref_ptr(Interface* aManagedPtr = nullptr) noexcept :
            iPtr{ aManagedPtr }, iManagedPtr{ aManagedPtr }, iReferenceCounted{ aManagedPtr != nullptr }
        {
            if (iManagedPtr)
                iManagedPtr->add_ref();
        }
        ref_ptr(Interface& aManagedPtr) noexcept :
            iPtr{ &aManagedPtr }, iManagedPtr{ &aManagedPtr }, iReferenceCounted{ aManagedPtr.use_count() > 0 }
        {
            if (iReferenceCounted)
                iManagedPtr->add_ref();
        }
        ref_ptr(ref_ptr const& aOther) noexcept :
            iPtr{ aOther.ptr() }, iManagedPtr{ aOther.managed_ptr() }, iReferenceCounted{ aOther.reference_counted() }
        {
            if (iManagedPtr && iReferenceCounted)
                iManagedPtr->add_ref();
        }
        ref_ptr(ref_ptr&& aOther) noexcept :
            iPtr{ aOther.ptr() }, iManagedPtr { aOther.managed_ptr() }, iReferenceCounted{ aOther.reference_counted() }
        {
            aOther.detach();
        }
        ref_ptr(ref_ptr const& aOther, Interface* aPtr) noexcept :
            iPtr{ aPtr }, iManagedPtr{ aOther.managed_ptr() }, iReferenceCounted{ aOther.reference_counted() }
        {
            if (iManagedPtr && iReferenceCounted)
                iManagedPtr->add_ref();
        }
        ref_ptr(ref_ptr&& aOther, Interface* aPtr) noexcept :
            iPtr{ aPtr }, iManagedPtr{ aOther.managed_ptr() }, iReferenceCounted{ aOther.reference_counted() }
        {
            aOther.detach();
        }
        ref_ptr(abstract_type const& aOther, Interface* aPtr) noexcept :
            iPtr{ aPtr }, iManagedPtr{ aOther.managed_ptr() }, iReferenceCounted{ aOther.reference_counted() }
        {
            if (iManagedPtr && iReferenceCounted)
                iManagedPtr->add_ref();
        }
        ref_ptr(abstract_type const& aOther) noexcept :
            iPtr{ static_cast<Interface*>(aOther.ptr()) }, iManagedPtr{ static_cast<Interface*>(aOther.managed_ptr()) }, iReferenceCounted{ aOther.reference_counted() }
        {
            if (iManagedPtr && iReferenceCounted)
                iManagedPtr->add_ref();
        }
        ref_ptr(i_discoverable& aDiscoverable) :
            iPtr{ nullptr }, iManagedPtr { nullptr }, iReferenceCounted{ true }
        {
            if (!aDiscoverable.discover(*this))
                throw interface_not_found();
        }
        template <typename Interface2, typename = std::enable_if_t<std::is_base_of_v<Interface, Interface2>, sfinae>>
        ref_ptr(ref_ptr<Interface2> const& aOther) noexcept :
            iPtr{ static_cast<Interface*>(aOther.ptr()) }, iManagedPtr{ static_cast<Interface*>(aOther.managed_ptr()) }, iReferenceCounted{ aOther.reference_counted() }
        {
            if (iManagedPtr && iReferenceCounted)
                iManagedPtr->add_ref();
        }
        template <typename Interface2, typename = std::enable_if_t<std::is_base_of_v<Interface, Interface2>, sfinae>>
        ref_ptr(ref_ptr<Interface2>&& aOther) noexcept :
            iPtr{ static_cast<Interface*>(aOther.ptr()) }, iManagedPtr { static_cast<Interface*>(aOther.managed_ptr()) }, iReferenceCounted{ aOther.reference_counted() }
        {
            aOther.detach();
        }
        template <typename Interface2, typename = std::enable_if_t<std::is_base_of_v<Interface, Interface2>, sfinae>>
        ref_ptr(i_ref_ptr<Interface2> const& aOther) noexcept :
            iPtr{ static_cast<Interface*>(aOther.ptr()) }, iManagedPtr{ static_cast<Interface*>(aOther.managed_ptr()) }, iReferenceCounted{ aOther.reference_counted() }
        {
            if (iManagedPtr && iReferenceCounted)
                iManagedPtr->add_ref();
        }
        template <typename Interface2>
        ref_ptr(ref_ptr<Interface2> const& aOther, Interface* aPtr) noexcept :
            iPtr{ aPtr }, iManagedPtr{ static_cast<Interface*>(aOther.managed_ptr()) }, iReferenceCounted{ aOther.reference_counted() }
        {
            if (iManagedPtr && iReferenceCounted)
                iManagedPtr->add_ref();
        }
        template <typename Interface2>
        ref_ptr(ref_ptr<Interface2>&& aOther, Interface* aPtr) noexcept :
            iPtr{ aPtr }, iManagedPtr{ static_cast<Interface*>(aOther.managed_ptr()) }, iReferenceCounted{ aOther.reference_counted() }
        {
            aOther.detach();
        }
        template <typename Interface2>
        ref_ptr(i_ref_ptr<Interface2> const& aOther, Interface* aPtr) noexcept :
            iPtr{ aPtr }, iManagedPtr{ static_cast<Interface*>(aOther.managed_ptr()) }, iReferenceCounted{ aOther.reference_counted() }
        {
            if (iManagedPtr && iReferenceCounted)
                iManagedPtr->add_ref();
        }
        ~ref_ptr()
        {
            if (iManagedPtr && iReferenceCounted)
            {
                auto releasingObject = iManagedPtr;
                iManagedPtr = nullptr;
                releasingObject->release();
            }
        }
        ref_ptr& operator=(ref_ptr const& aOther)
        {
            if (&aOther == this)
                return *this;
            reset(aOther.ptr(), aOther.managed_ptr(), aOther.reference_counted(), true);
            return *this;
        }
        ref_ptr& operator=(ref_ptr&& aOther)
        {
            if (&aOther == this)
                return *this;
            reset(aOther.ptr(), aOther.managed_ptr(), aOther.reference_counted(), false);
            aOther.detach();
            return *this;
        }
        ref_ptr& operator=(abstract_type const& aOther)
        {
            if (&aOther == this)
                return *this;
            reset(aOther.ptr(), aOther.managed_ptr(), aOther.reference_counted(), true);
            return *this;
        }
        template <typename Interface2, typename = std::enable_if_t<std::is_base_of_v<Interface, Interface2>, sfinae>>
        ref_ptr& operator=(ref_ptr<Interface2> const& aOther)
        {
            reset(aOther.ptr(), aOther.managed_ptr(), aOther.reference_counted(), true);
            return *this;
        }
        template <typename Interface2, typename = std::enable_if_t<std::is_base_of_v<Interface, Interface2>, sfinae>>
        ref_ptr& operator=(ref_ptr<Interface2>&& aOther)
        {
            reset(aOther.ptr(), aOther.managed_ptr(), aOther.reference_counted(), false);
            aOther.detach();
            return *this;
        }
        template <typename Interface2, typename = std::enable_if_t<std::is_base_of_v<Interface, Interface2>, sfinae>>
        ref_ptr& operator=(i_ref_ptr<Interface2> const& aOther)
        {
            reset(aOther.ptr(), aOther.managed_ptr(), aOther.reference_counted(), true);
            return *this;
        }
        ref_ptr& operator=(std::nullptr_t)
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
        bool reference_counted() const noexcept final
        {
            return iReferenceCounted;
        }
        long use_count() const noexcept final
        {
            if (managing() && reference_counted())
                return managed_ptr()->use_count();
            return 0;
        }
        bool unique() const noexcept final
        {
            if (reference_counted())
                return use_count() == 1;
            else
                return valid();
        }
        void reset() final
        {
            reset<Interface>(nullptr, nullptr, false, false);
        }
        void reset(maybe_abstract_t<Interface>* aPtr) final
        {
            reset<maybe_abstract_t<Interface>>(aPtr, aPtr, aPtr != nullptr, true);
        }
        void reset(maybe_abstract_t<Interface>* aPtr, maybe_abstract_t<Interface>* aManagedPtr) final
        {
            reset<maybe_abstract_t<Interface>>(aPtr, aManagedPtr, aManagedPtr != nullptr, true);
        }
        void reset(maybe_abstract_t<Interface>* aPtr, maybe_abstract_t<Interface>* aManagedPtr, bool aReferenceCounted, bool aAddRef) final
        {
            reset<maybe_abstract_t<Interface>>(aPtr, aManagedPtr, aReferenceCounted, aAddRef);
        }
        Interface* release() final
        {
            if (iManagedPtr == nullptr)
                throw no_managed_object();
            Interface* releasedObject = dynamic_cast<Interface*>(iManagedPtr->release_and_take_ownership());
            iPtr = nullptr;
            iManagedPtr = nullptr;
            iReferenceCounted = false;
            return releasedObject;
        }
        Interface* detach() noexcept final
        {
            auto detached = iManagedPtr;
            iPtr = nullptr;
            iManagedPtr = nullptr;
            iReferenceCounted = false;
            return detached;
        }
        bool valid() const noexcept final
        {
            return iPtr != nullptr;
        }
        bool managing() const noexcept final
        {
            return iManagedPtr != nullptr;
        }
        Interface* ptr() const noexcept final
        {
            return iPtr;
        }
        Interface* managed_ptr() const noexcept final
        {
            return iManagedPtr;
        }
        Interface* operator->() const final
        {
            if (iPtr == nullptr)
                throw no_object();
            return iPtr;
        }
        Interface& operator*() const final
        {
            if (iPtr == nullptr)
                throw no_object();
            return *iPtr;
        }
    public:
        template <typename Interface2 = Interface, typename = std::enable_if_t<std::is_base_of_v<Interface2, Interface>, sfinae>>
        void reset(Interface2* aPtr, Interface2* aManagedPtr, bool aReferenceCounted, bool aAddRef)
        {
            if (iPtr == aPtr && iManagedPtr == aManagedPtr)
                return;
            Interface* compatiblePtr = dynamic_cast<Interface*>(aPtr);
            if (aPtr != nullptr && compatiblePtr == nullptr)
                throw std::bad_cast();
            Interface* compatibleManagedPtr = dynamic_cast<Interface*>(aManagedPtr);
            if (aManagedPtr != nullptr && compatibleManagedPtr == nullptr)
                throw std::bad_cast();
            ref_ptr copy{ *this };
            iPtr = nullptr;
            if (iManagedPtr && iReferenceCounted)
            {
                auto releasingObject = iManagedPtr;
                iManagedPtr = nullptr;
                releasingObject->release();
            }
            iPtr = compatiblePtr;
            iManagedPtr = compatibleManagedPtr;
            iReferenceCounted = aReferenceCounted;
            if (iManagedPtr && iReferenceCounted && aAddRef)
                iManagedPtr->add_ref();
        }
    private:
        Interface* iPtr;
        Interface* iManagedPtr;
        bool iReferenceCounted;
    };

    template <typename Interface>
    class weak_ref_ptr : public i_weak_ref_ptr<maybe_abstract_t<Interface>>
    {
        typedef i_weak_ref_ptr<maybe_abstract_t<Interface>> base_type;
    public:
        typedef i_weak_ref_ptr<maybe_abstract_t<Interface>> abstract_type;
        typedef typename base_type::no_object no_object;
        typedef typename base_type::interface_not_found interface_not_found;
        typedef typename base_type::bad_release bad_release;
        typedef typename base_type::wrong_object wrong_object;
    public:
        weak_ref_ptr(Interface* aManagedPtr = nullptr) noexcept :
            iControlBlock{ nullptr }
        {
            update_control_block(aManagedPtr);
        }
        weak_ref_ptr(Interface& aManagedPtr) noexcept :
            iControlBlock{ nullptr }
        {
            update_control_block(&aManagedPtr);
        }
        weak_ref_ptr(const weak_ref_ptr& aOther) noexcept :
            iControlBlock{ nullptr }
        {
            update_control_block(aOther.ptr());
        }
        weak_ref_ptr(const i_ref_ptr<maybe_abstract_t<Interface>>& aOther) noexcept :
            iControlBlock{ nullptr }
        {
            update_control_block(aOther.managed_ptr());
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
        weak_ref_ptr& operator=(weak_ref_ptr const& aOther)
        {
            reset(aOther.ptr());
            return *this;
        }
        weak_ref_ptr& operator=(i_ref_ptr<maybe_abstract_t<Interface>> const& aOther)
        {
            reset(aOther.managed_ptr());
            return *this;
        }
        weak_ref_ptr& operator=(std::nullptr_t)
        {
            reset();
            return *this;
        }
    public:
        bool reference_counted() const noexcept final
        {
            return false;
        }
        long use_count() const noexcept final
        {
            return 0;
        }
        bool unique() const noexcept final
        {
            return false;
        }
        void reset() final
        {
            weak_ref_ptr copy(*this);
            update_control_block(nullptr);
        }
        void reset(maybe_abstract_t<Interface>* aPtr) final
        {
            weak_ref_ptr copy(*this);
            update_control_block(aPtr);
        }
        void reset(maybe_abstract_t<Interface>*, maybe_abstract_t<Interface>* aManagedPtr) final
        {
            weak_ref_ptr copy(*this);
            update_control_block(aManagedPtr);
        }
        void reset(maybe_abstract_t<Interface>*, maybe_abstract_t<Interface>* aManagedPtr, bool, bool) final
        {
            weak_ref_ptr copy(*this);
            update_control_block(aManagedPtr);
        }
        Interface* release() final
        {
            if (expired())
                throw no_object();
            else
                throw bad_release();
        }
        Interface* detach() final
        {
            weak_ref_ptr copy(*this);
            update_control_block(nullptr);
            return copy.ptr();
        }
        bool valid() const noexcept final
        {
            return ptr() != nullptr;
        }
        bool managing() const noexcept final
        {
            return valid();
        }
        bool expired() const noexcept final
        {
            return iControlBlock == nullptr || iControlBlock->expired();
        }
        Interface* ptr() const noexcept final
        {
            return static_cast<Interface*>(iControlBlock != nullptr ? iControlBlock->ptr() : nullptr);
        }
        Interface* managed_ptr() const noexcept final
        {
            return ptr();
        }
        Interface* operator->() const final
        {
            if (expired())
                throw no_object();
            return ptr();
        }
        Interface& operator*() const final
        {
            if (expired())
                throw no_object();
            return *ptr();
        }
    private:
        void update_control_block(maybe_abstract_t<Interface>* aManagedPtr)
        {
            auto controlBlock = aManagedPtr != nullptr ? &(*aManagedPtr).control_block() : nullptr;
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
    inline bool operator<(ref_ptr<Interface> const& lhs, ref_ptr<Interface> const& rhs) noexcept
    {
        return lhs.ptr() < rhs.ptr();
    }

    template <typename ConcreteType, typename... Args>
    inline ref_ptr<ConcreteType> make_ref(Args&&... args)
    {
        return ref_ptr<ConcreteType>{ new ConcreteType{ std::forward<Args>(args)... } };
    }

    template <typename ConcreteType, typename AbstractType, typename... Args>
    inline ref_ptr<AbstractType> make_ref(Args&&... args)
    {
        return ref_ptr<AbstractType>{ new ConcreteType{ std::forward<Args>(args)... } };
    }

    template <typename ConcreteType, typename... Args>
    inline ref_ptr<maybe_abstract_t<ConcreteType>> make_abstract_ref(Args&&... args)
    {
        return ref_ptr<maybe_abstract_t<ConcreteType>>{ new ConcreteType{ std::forward<Args>(args)... } };
    }

    template <class T, class U>
    ref_ptr<T> static_pointer_cast(ref_ptr<U> const& aOther) noexcept 
    {
        auto const ptr = static_cast<typename ref_ptr<T>::element_type*>(aOther.ptr());
        return ref_ptr<T>{ aOther, ptr };
    }

    template <class T, class U>
    ref_ptr<T> static_pointer_cast(ref_ptr<U>&& aOther) noexcept 
    {
        auto const ptr = static_cast<typename ref_ptr<T>::element_type*>(aOther.ptr());
        return ref_ptr<T>{ std::move(aOther), ptr };
    }

    template <class T, class U>
    ref_ptr<T> const_pointer_cast(ref_ptr<U> const& aOther) noexcept 
    {
        auto const ptr = const_cast<typename ref_ptr<T>::element_type*>(aOther.ptr());
        return ref_ptr<T>{ aOther, ptr };
    }

    template <class T, class U>
    ref_ptr<T> const_pointer_cast(ref_ptr<U>&& aOther) noexcept 
    {
        auto const ptr = const_cast<typename ref_ptr<T>::element_type*>(aOther.ptr());
        return ref_ptr<T>{ std::move(aOther), ptr };
    }

    template <class T, class U>
    ref_ptr<T> reinterpret_pointer_cast(ref_ptr<U> const& aOther) noexcept 
    {
        auto const ptr = reinterpret_cast<typename ref_ptr<T>::element_type*>(aOther.ptr());
        return ref_ptr<T>{ aOther, ptr };
    }

    template <class T, class U>
    ref_ptr<T> reinterpret_pointer_cast(ref_ptr<U>&& aOther) noexcept 
    {
        auto const ptr = reinterpret_cast<typename ref_ptr<T>::element_type*>(aOther.ptr());
        return ref_ptr<T>{ std::move(aOther), ptr };
    }

    template <class T, class U>
    ref_ptr<T> dynamic_pointer_cast(ref_ptr<U> const& aOther) noexcept
    {
        auto const ptr = dynamic_cast<typename ref_ptr<T>::element_type*>(aOther.ptr());
        if (ptr)
            return ref_ptr<T>{ aOther, ptr };
        return ref_ptr<T>{};
    }

    template <class T, class U>
    ref_ptr<T> dynamic_pointer_cast(ref_ptr<U>&& aOther) noexcept 
    {
        auto const ptr = dynamic_cast<typename ref_ptr<T>::element_type*>(aOther.ptr());
        if (ptr)
            return ref_ptr<T>{ std::move(aOther), ptr };
        return ref_ptr<T>{};
    }

    template <class T, class U>
    ref_ptr<T> static_pointer_cast(i_ref_ptr<U> const& aOther) noexcept
    {
        auto const ptr = static_cast<typename ref_ptr<T>::element_type*>(aOther.ptr());
        return ref_ptr<T>{ aOther, ptr };
    }

    template <class T, class U>
    ref_ptr<T> static_pointer_cast(i_ref_ptr<U>&& aOther) noexcept
    {
        auto const ptr = static_cast<typename ref_ptr<T>::element_type*>(aOther.ptr());
        return ref_ptr<T>{ std::move(aOther), ptr };
    }

    template <class T, class U>
    ref_ptr<T> const_pointer_cast(i_ref_ptr<U> const& aOther) noexcept
    {
        auto const ptr = const_cast<typename ref_ptr<T>::element_type*>(aOther.ptr());
        return ref_ptr<T>{ aOther, ptr };
    }

    template <class T, class U>
    ref_ptr<T> const_pointer_cast(i_ref_ptr<U>&& aOther) noexcept
    {
        auto const ptr = const_cast<typename ref_ptr<T>::element_type*>(aOther.ptr());
        return ref_ptr<T>{ std::move(aOther), ptr };
    }

    template <class T, class U>
    ref_ptr<T> reinterpret_pointer_cast(i_ref_ptr<U> const& aOther) noexcept
    {
        auto const ptr = reinterpret_cast<typename ref_ptr<T>::element_type*>(aOther.ptr());
        return ref_ptr<T>{ aOther, ptr };
    }

    template <class T, class U>
    ref_ptr<T> reinterpret_pointer_cast(i_ref_ptr<U>&& aOther) noexcept
    {
        auto const ptr = reinterpret_cast<typename ref_ptr<T>::element_type*>(aOther.ptr());
        return ref_ptr<T>{ std::move(aOther), ptr };
    }

    template <class T, class U>
    ref_ptr<T> dynamic_pointer_cast(i_ref_ptr<U> const& aOther) noexcept
    {
        auto const ptr = dynamic_cast<typename ref_ptr<T>::element_type*>(aOther.ptr());
        if (ptr)
            return ref_ptr<T>{ aOther, ptr };
        return ref_ptr<T>{};
    }

    template <class T, class U>
    ref_ptr<T> dynamic_pointer_cast(i_ref_ptr<U>&& aOther) noexcept
    {
        auto const ptr = dynamic_cast<typename ref_ptr<T>::element_type*>(aOther.ptr());
        if (ptr)
            return ref_ptr<T>{ std::move(aOther), ptr };
        return ref_ptr<T>{};
    }
}
