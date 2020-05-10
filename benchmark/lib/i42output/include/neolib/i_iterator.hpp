// i_iterator.hpp - v1.0
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
#include <iterator>
#include <neolib/i_reference_counted.hpp>

namespace neolib
{
    struct singular_iterator : std::logic_error { singular_iterator() : std::logic_error("neolib::singular_iterator") {} };

    template <typename, typename, typename, typename, typename>
    class const_iterator;

    template <typename T, typename Category = std::bidirectional_iterator_tag, typename Difference = std::ptrdiff_t, typename Pointer = const T*, typename Reference = const T&>
    class i_const_iterator : public i_reference_counted
    {
        typedef i_const_iterator<T, Category, Difference, Pointer, Reference> self_type;
    public:
        typedef self_type abstract_type;
        typedef T value_type;
        typedef Difference difference_type;
        typedef Pointer pointer;
        typedef Reference reference;
        typedef Category iterator_category;
    public:
        typedef self_type abstract_iterator;
        typedef self_type abstract_const_iterator;
        typedef abstract_iterator abstract_base_iterator;
        typedef abstract_const_iterator abstract_base_const_iterator;
        typedef const_iterator<T, Category, Difference, Pointer, Reference> iterator_wrapper;
    public:
        virtual self_type& operator++() = 0;
        virtual self_type& operator--() = 0;
        iterator_wrapper operator++(int);
        iterator_wrapper operator--(int);
        virtual reference operator*() const = 0;
        virtual pointer operator->() const = 0;
        virtual bool operator==(const self_type& aOther) const = 0;
        virtual bool operator!=(const self_type& aOther) const = 0;
    public:
        virtual self_type* clone(void* memory) const = 0;
    private:
        virtual self_type* do_post_increment(void* memory) = 0;
        virtual self_type* do_post_decrement(void* memory) = 0;
    };

    template <typename, typename, typename, typename, typename>
    class random_access_const_iterator;

    template <typename T, typename Category = std::random_access_iterator_tag, typename Difference = std::ptrdiff_t, typename Pointer = const T*, typename Reference = const T&>
    class i_random_access_const_iterator : public i_const_iterator<T, Category, Difference, Pointer, Reference>
    {
        typedef i_random_access_const_iterator<T, Category, Difference, Pointer, Reference> self_type;
    public:
        typedef self_type abstract_type;
        typedef i_const_iterator<T, Category, Difference, Pointer, Reference> base_type;
    public:
        using typename base_type::value_type;
        using typename base_type::difference_type;
        using typename base_type::pointer;
        using typename base_type::reference;
        using typename base_type::iterator_category;
    public:
        typedef self_type abstract_iterator;
        typedef self_type abstract_const_iterator;
        typedef abstract_iterator abstract_random_access_iterator;
        typedef abstract_const_iterator abstract_random_access_const_iterator;
        typedef random_access_const_iterator<T, Category, Difference, Pointer, Reference> iterator_wrapper;
    public:
        virtual self_type& operator+=(difference_type aDifference) = 0;
        virtual self_type& operator-=(difference_type aDifference) = 0;
        iterator_wrapper operator+(difference_type aDifference) const;
        iterator_wrapper operator-(difference_type aDifference) const;
        virtual reference operator[](difference_type aDifference) const = 0;
        virtual difference_type operator-(const self_type& aOther) const = 0;
        virtual bool operator<(const self_type& aOther) const = 0;
        virtual bool operator<=(const self_type& aOther) const = 0;
        virtual bool operator>(const self_type& aOther) const = 0;
        virtual bool operator>=(const self_type& aOther) const = 0;
    private:
        virtual self_type* do_add(void* memory, difference_type aDifference) const = 0;
        virtual self_type* do_subtract(void* memory, difference_type aDifference) const = 0;
    };

    template <typename, typename, typename, typename, typename>
    class i_iterator;
    template <typename, typename, typename, typename, typename>
    class iterator;

    template <typename T, typename Category = std::bidirectional_iterator_tag, typename Difference = std::ptrdiff_t, typename Pointer = const T*, typename Reference = const T&>
    class const_iterator
    {
        typedef const_iterator<T, Category, Difference, Pointer, Reference> self_type;
    public:
        typedef T value_type;
        typedef Difference difference_type;
        typedef Pointer pointer;
        typedef Reference reference;
        typedef Category iterator_category;
    public:
        typedef i_const_iterator<T, Category, Difference, Pointer, Reference> abstract_iterator;
        typedef i_const_iterator<T, Category, Difference, Pointer, Reference> abstract_const_iterator;
    public:
        const_iterator()
        {
        }
        const_iterator(abstract_iterator* aWrappedIterator)
        {
            aWrappedIterator->clone(storage());
            wrapped_iterator().add_ref();
        }
        const_iterator(const self_type& aOther)
        {
            aOther.clone(storage());
            wrapped_iterator().add_ref();
        }
        const_iterator(const i_iterator<T, Category, Difference, T*, T&>& aOther)
        {
            aOther.const_clone(storage());
            wrapped_iterator().add_ref();
        }
        const_iterator(const iterator<T, Category, Difference, T*, T&>& aOther)
        {
            aOther.const_clone(storage());
            wrapped_iterator().add_ref();
        }
        ~const_iterator()
        {
            if (!is_singular())
                wrapped_iterator().release();
        }
        self_type& operator=(const self_type& aOther)
        {
            const_iterator temp(aOther);
            if (!is_singular())
                wrapped_iterator().release();
            temp.clone(storage());
            wrapped_iterator().add_ref();
            return *this;
        }
        self_type& operator=(const iterator<T, Category, Difference, T*, T&>& aOther)
        {
            iterator temp(aOther);
            if (!is_singular())
                wrapped_iterator().release();
            temp.const_clone(storage());
            wrapped_iterator().add_ref();
            return *this;
        }
        operator abstract_iterator& ()
        {
            return wrapped_iterator();
        }
    public:
        abstract_iterator& operator++() { return ++wrapped_iterator(); }
        abstract_iterator& operator--() { return --wrapped_iterator(); }
        self_type operator++(int) { return wrapped_iterator()++; }
        self_type operator--(int) { return wrapped_iterator()--; }
        reference operator*() const { return wrapped_iterator().operator*(); }
        pointer operator->() const { return wrapped_iterator().operator->(); }
        bool operator==(const self_type& aOther) const { return wrapped_iterator() == aOther.wrapped_iterator(); }
        bool operator!=(const self_type& aOther) const { return !(*this == aOther); }
    public:
        bool is_singular() const
        {
            return iSingular;
        }
        abstract_iterator& wrapped_iterator() const
        {
            if (is_singular())
                throw singular_iterator();
            return *static_cast<abstract_iterator*>(storage());
        }
        abstract_iterator* clone(void* memory) const
        {
            if (is_singular())
                throw singular_iterator();
            return wrapped_iterator().clone(memory);
        }
        void* storage() const
        {
            iSingular = false;
            return &iStorage;
        }
    protected:
        mutable bool iSingular = true;
        mutable std::aligned_storage<sizeof(void*) * 10>::type iStorage;
    };

    template <typename, typename, typename, typename, typename>
    class i_random_access_iterator;

    template <typename T, typename Category = std::random_access_iterator_tag, typename Difference = std::ptrdiff_t, typename Pointer = const T*, typename Reference = const T&>
    class random_access_const_iterator : public const_iterator<T, Category, Difference, Pointer, Reference>
    {
        typedef random_access_const_iterator<T, Category, Difference, Pointer, Reference> self_type;
        typedef const_iterator<T, Category, Difference, Pointer, Reference> base_type;
    public:
        typedef i_random_access_const_iterator<T, Category, Difference, Pointer, Reference> abstract_type;
        using typename base_type::value_type;
        using typename base_type::difference_type;
        using typename base_type::pointer;
        using typename base_type::reference;
        using typename base_type::iterator_category;
    public:
        using abstract_iterator = abstract_type;
        using abstract_const_iterator = abstract_type;
    public:
        random_access_const_iterator() :
            base_type()
        {
        }
        random_access_const_iterator(abstract_iterator* aWrappedIterator) :
            base_type(aWrappedIterator)
        {
        }
        random_access_const_iterator(const self_type& aOther) :
            base_type(aOther)
        {
        }
        random_access_const_iterator(const i_random_access_iterator<T, Category, Difference, T*, T&>& aOther) :
            base_type(aOther)
        {
        }
        random_access_const_iterator(const iterator<T, Category, Difference, T*, T&>& aOther) :
            base_type(aOther)
        {
        }
        ~random_access_const_iterator()
        {
        }
        self_type& operator=(const self_type& aOther)
        {
            base_type::operator=(aOther);
            return *this;
        }
        self_type& operator=(const i_random_access_iterator<T, Category, Difference, T*, T&>& aOther)
        {
            base_type::operator=(aOther);
            return *this;
        }
        self_type& operator=(const iterator<T, Category, Difference, T*, T&>& aOther)
        {
            base_type::operator=(aOther);
            return *this;
        }
        operator abstract_iterator& ()
        {
            return wrapped_iterator();
        }
    public:
        abstract_iterator& operator+=(difference_type aDifference) { return wrapped_iterator() += aDifference; }
        abstract_iterator& operator-=(difference_type aDifference) { return wrapped_iterator() += aDifference; }
        self_type operator+(difference_type aDifference) const { return wrapped_iterator() + aDifference; }
        self_type operator-(difference_type aDifference) const { return wrapped_iterator() - aDifference; }
        reference operator[](difference_type aDifference) const { return wrapped_iterator()[aDifference]; }
        difference_type operator-(const self_type& aOther) const { return wrapped_iterator() - (aOther.wrapped_iterator()); }
        bool operator<(const self_type& aOther) const { return wrapped_iterator() < aOther.wrapped_iterator(); }
        bool operator<=(const self_type& aOther) const { return wrapped_iterator() <= aOther.wrapped_iterator(); }
        bool operator>(const self_type& aOther) const { return wrapped_iterator() > aOther.wrapped_iterator(); }
        bool operator>=(const self_type& aOther) const { return wrapped_iterator() >= aOther.wrapped_iterator(); }
    public:
        abstract_iterator& wrapped_iterator() const { return static_cast<abstract_iterator&>(base_type::wrapped_iterator()); }
    };

    template <typename T, typename Category, typename Difference, typename Pointer, typename Reference>
    inline const_iterator<T, Category, Difference, Pointer, Reference> i_const_iterator<T, Category, Difference, Pointer, Reference>::operator++(int) { return do_post_increment(); }
    template <typename T, typename Category, typename Difference, typename Pointer, typename Reference>
    inline const_iterator<T, Category, Difference, Pointer, Reference> i_const_iterator<T, Category, Difference, Pointer, Reference>::operator--(int) { return do_post_decrement(); }
    template <typename T, typename Category, typename Difference, typename Pointer, typename Reference>
    inline random_access_const_iterator<T, Category, Difference, Pointer, Reference> i_random_access_const_iterator<T, Category, Difference, Pointer, Reference>::operator+(difference_type aDifference) const
    {
        random_access_const_iterator<T, Category, Difference, Pointer, Reference> result;
        do_add(result.storage(), aDifference);
        return result;
    }
    template <typename T, typename Category, typename Difference, typename Pointer, typename Reference>
    inline random_access_const_iterator<T, Category, Difference, Pointer, Reference> i_random_access_const_iterator<T, Category, Difference, Pointer, Reference>::operator-(difference_type aDifference) const
    {
        random_access_const_iterator<T, Category, Difference, Pointer, Reference> result;
        do_subtract(result.storage(), aDifference);
        return result;
    }

    template <typename, typename, typename, typename, typename>
    class iterator;

    template <typename T, typename Category = std::bidirectional_iterator_tag, typename Difference = std::ptrdiff_t, typename Pointer = T*, typename Reference = T&>
    class i_iterator : public i_reference_counted
    {
        typedef i_iterator<T, Category, Difference, Pointer, Reference> self_type;
    public:
        typedef self_type abstract_type;
    public:
        typedef T value_type;
        typedef Difference difference_type;
        typedef Pointer pointer;
        typedef Reference reference;
        typedef Category iterator_category;
    public:
        typedef self_type abstract_iterator;
        typedef i_const_iterator<T, Category, Difference, const T*, const T&> abstract_const_iterator;
        typedef abstract_iterator abstract_base_iterator;
        typedef abstract_const_iterator abstract_base_const_iterator;
        typedef iterator<T, Category, Difference, Pointer, Reference> iterator_wrapper;
    public:
        virtual self_type& operator++() = 0;
        virtual self_type& operator--() = 0;
        iterator_wrapper operator++(int);
        iterator_wrapper operator--(int);
        virtual reference operator*() const = 0;
        virtual pointer operator->() const = 0;
        virtual bool operator==(const self_type& aOther) const = 0;
        virtual bool operator!=(const self_type& aOther) const = 0;
    public:
        virtual self_type* clone(void* memory) const = 0;
        virtual abstract_const_iterator* const_clone(void* memory) const = 0;
    private:
        virtual self_type* do_post_increment(void* memory) = 0;
        virtual self_type* do_post_decrement(void* memory) = 0;
    };

    template <typename, typename, typename, typename, typename>
    class random_access_iterator;

    template <typename T, typename Category = std::random_access_iterator_tag, typename Difference = std::ptrdiff_t, typename Pointer = T*, typename Reference = T&>
    class i_random_access_iterator : public i_iterator<T, Category, Difference, Pointer, Reference>
    {
        typedef i_random_access_iterator<T, Category, Difference, Pointer, Reference> self_type;
        typedef i_iterator<T, Category, Difference, Pointer, Reference> base_type;
    public:
        typedef self_type abstract_type;
    public:
        using typename base_type::value_type;
        using typename base_type::difference_type;
        using typename base_type::pointer;
        using typename base_type::reference;
        using typename base_type::iterator_category;
    public:
        typedef self_type abstract_iterator;
        typedef i_random_access_const_iterator<T, Category, Difference, const T*, const T&> abstract_const_iterator;
        typedef abstract_iterator abstract_random_access_iterator;
        typedef abstract_const_iterator abstract_random_access_const_iterator;
        typedef random_access_iterator<T, Category, Difference, Pointer, Reference> iterator_wrapper;
    public:
        virtual self_type& operator+=(difference_type aDifference) = 0;
        virtual self_type& operator-=(difference_type aDifference) = 0;
        iterator_wrapper operator+(difference_type aDifference) const;
        iterator_wrapper operator-(difference_type aDifference) const;
        virtual reference operator[](difference_type aDifference) const = 0;
        virtual difference_type operator-(const self_type& aOther) const = 0;
        virtual bool operator<(const self_type& aOther) const = 0;
        virtual bool operator<=(const self_type& aOther) const = 0;
        virtual bool operator>(const self_type& aOther) const = 0;
        virtual bool operator>=(const self_type& aOther) const = 0;
    private:
        virtual self_type* do_add(void* memory, difference_type aDifference) const = 0;
        virtual self_type* do_subtract(void* memory, difference_type aDifference) const = 0;
    };

    template <typename T, typename Category = std::bidirectional_iterator_tag, typename Difference = std::ptrdiff_t, typename Pointer = T*, typename Reference = T&>
    class iterator
    {
        typedef iterator<T, Category, Difference, Pointer, Reference> self_type;
    public:
        typedef i_iterator<T, Category, Difference, Pointer, Reference> abstract_type;
        typedef T value_type;
        typedef Difference difference_type;
        typedef Pointer pointer;
        typedef Reference reference;
        typedef Category iterator_category;
    public:
        typedef abstract_type abstract_iterator;
        typedef i_const_iterator<T, Category, Difference, const T*, const T&> abstract_const_iterator;
    public:
        iterator()
        {
        }
        iterator(abstract_iterator* aWrappedIterator)
        {
            aWrappedIterator->clone(storage());
            wrapped_iterator().add_ref();
        }
        iterator(const self_type& aOther)
        {
            aOther.clone(storage());
            wrapped_iterator().add_ref();
        }
        ~iterator()
        {
            if (!is_singular())
                wrapped_iterator().release();
        }
        self_type& operator=(const self_type& aOther)
        {
            self_type temp(aOther);
            if (!is_singular())
                wrapped_iterator().release();
            temp.clone(storage());
            wrapped_iterator().add_ref();
            return *this;
        }
        operator abstract_iterator&()
        {
            return wrapped_iterator();
        }
    public:
        abstract_iterator& operator++() { return ++wrapped_iterator(); }
        abstract_iterator& operator--() { return --wrapped_iterator(); }
        self_type operator++(int) { return wrapped_iterator()++; }
        self_type operator--(int) { return wrapped_iterator()--; }
        reference operator*() const { return wrapped_iterator().operator*(); }
        pointer operator->() const { return wrapped_iterator().operator->(); }
        bool operator==(const self_type& aOther) const { return wrapped_iterator() == aOther.wrapped_iterator(); }
        bool operator!=(const self_type& aOther) const { return !(*this == aOther); }
    public:
        bool is_singular() const 
        { 
            return iSingular; 
        }
        abstract_iterator& wrapped_iterator() const 
        { 
            if (is_singular())
                throw singular_iterator();
            return *static_cast<abstract_iterator*>(storage());
        }
        abstract_iterator* clone(void* memory) const
        {
            if (is_singular())
                throw singular_iterator();
            return wrapped_iterator().clone(memory);
        }
        abstract_const_iterator* const_clone(void* memory) const
        {
            if (is_singular())
                throw singular_iterator();
            return wrapped_iterator().const_clone(memory);
        }
        void* storage() const
        {
            iSingular = false;
            return &iStorage;
        }
    protected:
        mutable bool iSingular = true;
        mutable std::aligned_storage<sizeof(void*) * 10>::type iStorage;
    };

    template <typename T, typename Category = std::random_access_iterator_tag, typename Difference = std::ptrdiff_t, typename Pointer = T*, typename Reference = T&>
    class random_access_iterator : public iterator<T, Category, Difference, Pointer, Reference>
    {
        typedef random_access_iterator<T, Category, Difference, Pointer, Reference> self_type;
        typedef iterator<T, Category, Difference, Pointer, Reference> base_type;
    public:
        typedef i_random_access_iterator<T, Category, Difference, Pointer, Reference> abstract_type;
        using typename base_type::value_type;
        using typename base_type::difference_type;
        using typename base_type::pointer;
        using typename base_type::reference;
        using typename base_type::iterator_category;
    public:
        using abstract_iterator = abstract_type;
        using abstract_const_iterator = i_random_access_const_iterator<T, Category, Difference, const T*, const T&>;
    public:
        random_access_iterator() :
            base_type()
        {
        }
        random_access_iterator(abstract_iterator* aWrappedIterator) :
            base_type(aWrappedIterator)
        {
        }
        random_access_iterator(const self_type& aOther) :
            base_type(aOther)
        {
        }
        ~random_access_iterator()
        {
        }
        self_type& operator=(const self_type& aOther)
        {
            base_type::operator=(aOther);
            return *this;
        }
        operator abstract_iterator&()
        {
            return wrapped_iterator();
        }
    public:
        abstract_iterator& operator+=(difference_type aDifference) { return wrapped_iterator() += aDifference; }
        abstract_iterator& operator-=(difference_type aDifference) { return wrapped_iterator() += aDifference; }
        self_type operator+(difference_type aDifference) const { return wrapped_iterator() + aDifference; }
        self_type operator-(difference_type aDifference) const { return wrapped_iterator() - aDifference; }
        reference operator[](difference_type aDifference) const { return wrapped_iterator()[aDifference]; }
        difference_type operator-(const self_type& aOther) const { return wrapped_iterator() - (aOther.wrapped_iterator()); }
        bool operator<(const self_type& aOther) const { return wrapped_iterator() < aOther.wrapped_iterator(); }
        bool operator<=(const self_type& aOther) const { return wrapped_iterator() <= aOther.wrapped_iterator(); }
        bool operator>(const self_type& aOther) const { return wrapped_iterator() > aOther.wrapped_iterator(); }
        bool operator>=(const self_type& aOther) const { return wrapped_iterator() >= aOther.wrapped_iterator(); }
    public:
        abstract_iterator& wrapped_iterator() const { return static_cast<abstract_iterator&>(base_type::wrapped_iterator()); }
    };

    template <typename T, typename Category, typename Difference, typename Pointer, typename Reference>
    inline iterator<T, Category, Difference, Pointer, Reference> i_iterator<T, Category, Difference, Pointer, Reference>::operator++(int) { return do_post_increment(); }
    template <typename T, typename Category, typename Difference, typename Pointer, typename Reference>
    inline iterator<T, Category, Difference, Pointer, Reference> i_iterator<T, Category, Difference, Pointer, Reference>::operator--(int) { return do_post_decrement(); }
    template <typename T, typename Category, typename Difference, typename Pointer, typename Reference>
    inline random_access_iterator<T, Category, Difference, Pointer, Reference> i_random_access_iterator<T, Category, Difference, Pointer, Reference>::operator+(difference_type aDifference) const 
    { 
        random_access_iterator<T, Category, Difference, Pointer, Reference> result;
        do_add(result.storage(), aDifference);
        return result;
    }
    template <typename T, typename Category, typename Difference, typename Pointer, typename Reference>
    inline random_access_iterator<T, Category, Difference, Pointer, Reference> i_random_access_iterator<T, Category, Difference, Pointer, Reference>::operator-(difference_type aDifference) const 
    { 
        random_access_iterator<T, Category, Difference, Pointer, Reference> result;
        do_subtract(result.storage(), aDifference);
        return result;
    }
}
