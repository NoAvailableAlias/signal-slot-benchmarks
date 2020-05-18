// vector.hpp - v1.0
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
#include "reference_counted.hpp"
#include "i_vector.hpp"
#include "container_iterator.hpp"

namespace neolib
{
    template <typename T>
    class vector : public reference_counted<i_vector<abstract_t<T>>>
    {
        typedef reference_counted<i_vector<T>> base_type;
        // types
    public:
        typedef i_vector<abstract_t<T>> abstract_type;
        typedef T value_type;
        typedef abstract_t<T> abstract_value_type;
        typedef std::vector<value_type> container_type;
        using typename abstract_type::size_type;
        using typename abstract_type::const_iterator;
        using typename abstract_type::iterator;
        using typename abstract_type::generic_container_type;
    protected:
        using typename abstract_type::abstract_const_iterator;
        using typename abstract_type::abstract_iterator;
    protected:
        typedef container::random_access_const_iterator<T, typename container_type::const_iterator> container_const_iterator;
        typedef container::random_access_iterator<T, typename container_type::iterator, typename container_type::const_iterator> container_iterator;
        // construction
    public:
        vector() {}
        vector(const vector& aOther) : iVector{ aOther.iVector } {}
        vector(vector&& aOther) : iVector{ std::move(aOther.iVector) } {}
        vector(const i_vector<abstract_value_type>& aOther) { assign(aOther); }
        vector(const container_type& aOtherContainer) : iVector{ aOtherContainer } {}
        template <typename InputIter>
        vector(InputIter aFirst, InputIter aLast) : iVector(aFirst, aLast) {}
        vector& operator=(const vector& aOther) { assign(aOther); return *this; }
        vector& operator=(vector&& aOther) { iVector = std::move(aOther.iVector); return *this; }
        vector& operator=(const i_vector<abstract_value_type>& aOther) { assign(aOther); return *this; }
        // operations
    public:
        container_type& container() { return iVector; }
        const container_type& container() const { return iVector; }
        template <typename... Args>
        iterator emplace(const_iterator aPos, Args&&... aArgs) { auto newPos = iVector.emplace(iVector.begin() + (aPos - abstract_type::cbegin()), std::forward<Args>(aArgs)...); return abstract_type::begin() + (newPos - iVector.begin()); }
        // implementation
        // from i_container
    public:
        size_type size() const override { return iVector.size(); }
        size_type max_size() const override { return iVector.max_size(); }
        void clear() override { iVector.clear(); }
        void assign(const generic_container_type& aOther) override { if (&aOther == this) return; clear(); reserve(aOther.size()); std::copy(aOther.begin(), aOther.end(), std::back_insert_iterator{ iVector }); }
        // from i_container
    private:
        abstract_const_iterator* do_begin(void* memory) const override { return new (memory) container_const_iterator(iVector.begin()); }
        abstract_const_iterator* do_end(void* memory) const override { return new (memory) container_const_iterator(iVector.end()); }
        abstract_iterator* do_begin(void* memory) override { return new (memory) container_iterator(iVector.begin()); }
        abstract_iterator* do_end(void* memory) override { return new (memory) container_iterator(iVector.end()); }
        abstract_iterator* do_erase(void* memory, const abstract_const_iterator& aPosition) override { return new (memory) container_iterator(iVector.erase(static_cast<const container_const_iterator&>(aPosition))); }
        abstract_iterator* do_erase(void* memory, const abstract_const_iterator& aFirst, const abstract_const_iterator& aLast) override { return new (memory) container_iterator(iVector.erase(static_cast<const container_const_iterator&>(aFirst), static_cast<const container_const_iterator&>(aLast))); }
        // from i_sequence_container
    public:
        size_type capacity() const override { return iVector.capacity(); }
        void reserve(size_type aCapacity) override { iVector.reserve(aCapacity); }
        void resize(size_type aSize, const abstract_value_type& aValue) override { iVector.resize(aSize, aValue); }
        void push_back(const abstract_value_type& aValue) override { iVector.push_back(aValue); }
        void pop_back() override { iVector.pop_back(); }
        const value_type& back() const override { return iVector.back(); }
        value_type& back() override { return iVector.back(); }
        // from i_random_access_container
    public:
        const value_type* cdata() const override { return iVector.data(); }
        const value_type* data() const override { return iVector.data(); }
        value_type* data() override { return iVector.data(); }
    public:
        const value_type& at(size_type aIndex) const override { return iVector.at(aIndex); }
        value_type& at(size_type aIndex) override { return iVector.at(aIndex); }
        const value_type& operator[](size_type aIndex) const override { return iVector[aIndex]; }
        value_type& operator[](size_type aIndex) override { return iVector[aIndex]; }
    private:
        std::ptrdiff_t iterator_offset() const override { return sizeof(value_type); }
        // from i_sequence_container
    private:
        abstract_iterator* do_insert(void* memory, const abstract_const_iterator& aPosition, const abstract_value_type& aValue) override { return new (memory) container_iterator(iVector.insert(static_cast<const container_const_iterator&>(aPosition), aValue)); }
        // attributes
    private:
        std::vector<value_type> iVector;
    };
}
