// i_random_access_container.hpp
/*
 *  Copyright (c) 2019 Leigh Johnston.
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
#include <neolib/i_sequence_container.hpp>

namespace neolib
{
    template <typename T, typename = sfinae>
    struct offset_pointer;
    template <typename T>
    struct offset_pointer<T, typename std::enable_if_t<std::is_const_v<T>, sfinae>> { typedef char const* pointer_type; };
    template <typename T>
    struct offset_pointer<T, typename std::enable_if_t<!std::is_const_v<T>, sfinae>> { typedef char* pointer_type; };
    template <typename T>
    using offset_pointer_t = typename offset_pointer<T>::pointer_type;

    template <typename T>
    class offset_iterator
    {
        template <typename T2>
        friend class offset_iterator;
        typedef offset_iterator<T> self_type;
    public:
        using iterator_category = std::random_access_iterator_tag;
        using value_type = T;
        using pointer = T*;
        using reference = T&;
        using difference_type = std::ptrdiff_t;
    public:
        offset_iterator(T* aElement, std::ptrdiff_t aOffset) : 
            iElement{ aElement }, iOffset{ aOffset }
        {
        }
        offset_iterator(const self_type& aOther) :
            iElement{ aOther.iElement }, iOffset{ aOther.iOffset }
        {
        }
        template <typename Iterator>
        offset_iterator(const Iterator& aOther, std::enable_if_t<!std::is_same_v<Iterator, self_type> && std::is_same_v<Iterator, offset_iterator<std::remove_const_t<value_type>>>, sfinae> = {}) :
            iElement{ aOther.iElement }, iOffset{ aOther.iOffset }
        {
        }
    public:
        offset_iterator& operator++() { return *this += 1; }
        offset_iterator& operator--() { return *this -= 1; }
        offset_iterator operator++(int) { offset_iterator result = *this; ++(*this); return result; }
        offset_iterator operator--(int) { offset_iterator result = *this; --(*this); return result; }
        offset_iterator& operator+=(difference_type aDifference) { iElement = reinterpret_cast<T*>(ptr() + aDifference * iOffset); return *this; }
        offset_iterator& operator-=(difference_type aDifference) { iElement = reinterpret_cast<T*>(ptr() - aDifference * iOffset); return *this; }
        offset_iterator operator+(difference_type aDifference) const { offset_iterator result = *this; result += aDifference; return result; }
        offset_iterator operator-(difference_type aDifference) const { offset_iterator result = *this; result -= aDifference; return result; }
        reference operator[](difference_type aDifference) const { return *(*this + aDifference); }
        difference_type operator-(const offset_iterator& aOther) const { return (ptr() - aOther.ptr()) / iOffset; }
        reference operator*() const { return *iElement; }
        pointer operator->() const { return &(**this); }
        bool operator==(const offset_iterator& aOther) const { return iElement == aOther.iElement; }
        bool operator!=(const offset_iterator& aOther) const { return iElement != aOther.iElement; }
        bool operator<(const offset_iterator& aOther) const { return iElement < aOther.iElement; }
        bool operator<=(const offset_iterator& aOther) const { return iElement <= aOther.iElement; }
        bool operator>(const offset_iterator& aOther) const { return iElement > aOther.iElement; }
        bool operator>=(const offset_iterator& aOther) const { return iElement >= aOther.iElement; }
    private:
        offset_pointer_t<T> ptr() const { return reinterpret_cast<offset_pointer_t<T>>(iElement); }
    private:
        T* iElement;
        std::ptrdiff_t iOffset;
    };

    template <typename T, bool DefaultComparisonOperators = true>
    class i_random_access_container : public i_sequence_container<T, i_random_access_const_iterator<T>, i_random_access_iterator<T>, DefaultComparisonOperators>
    {
        typedef i_random_access_container<T, DefaultComparisonOperators> self_type;
        typedef i_sequence_container<T, i_random_access_const_iterator<T>, i_random_access_iterator<T>, DefaultComparisonOperators> base_type;
    public:
        typedef self_type abstract_type;
    public:
        using typename base_type::value_type;
        using typename base_type::size_type;
        typedef offset_iterator<const value_type> const_iterator;
        typedef offset_iterator<value_type> iterator;
        typedef std::reverse_iterator<const_iterator> const_reverse_iterator;
        typedef std::reverse_iterator<iterator> reverse_iterator;
    public:
        using base_type::size;
    public:
        virtual const value_type* cdata() const = 0;
        virtual const value_type* data() const = 0;
        virtual value_type* data() = 0;
    public:
        virtual const value_type& at(size_type aIndex) const = 0;
        virtual value_type& at(size_type aIndex) = 0;
        virtual const value_type& operator[](size_type aIndex) const = 0;
        virtual value_type& operator[](size_type aIndex) = 0;
    public:
        const_iterator cbegin() const { return const_iterator{ cdata(), iterator_offset() }; }
        const_iterator begin() const { return cbegin(); }
        iterator begin() { return iterator{ data(), iterator_offset() }; }
        const_iterator cend() const { return cbegin() + size(); }
        const_iterator end() const { return cend(); }
        iterator end() { return begin() + size(); }
        const_reverse_iterator crbegin() const { return const_reverse_iterator{ end() }; }
        const_reverse_iterator rbegin() const { return crbegin(); }
        reverse_iterator rbegin() { return reverse_iterator{ end() }; }
        const_reverse_iterator crend() const { return const_reverse_iterator{ begin() }; }
        const_reverse_iterator rend() const { return crend(); }
        reverse_iterator rend() { return reverse_iterator{ begin() }; }
        iterator insert(const const_iterator& aPosition, const value_type& aValue) { return to_iterator(base_type::insert(from_iterator(aPosition), aValue)); }
        iterator erase(const const_iterator& aPosition) { return to_iterator(base_type::erase(from_iterator(aPosition))); }
    private:
        virtual std::ptrdiff_t iterator_offset() const = 0;
        typename base_type::const_iterator from_iterator(const_iterator aPosition) const
        {
            return std::next(base_type::cbegin(), std::distance(cbegin(), aPosition));
        }
        typename base_type::iterator from_iterator(iterator aPosition)
        {
            return std::next(base_type::begin(), std::distance(begin(), aPosition));
        }
        const_iterator to_iterator(typename base_type::const_iterator aPosition) const
        {
            return std::next(cbegin(), std::distance(base_type::cbegin(), aPosition));
        }
        iterator to_iterator(typename base_type::iterator aPosition)
        {
            return std::next(begin(), std::distance(base_type::begin(), aPosition));
        }
    };
}
