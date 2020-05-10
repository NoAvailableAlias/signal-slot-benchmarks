// i_container.hpp - v1.0
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
#include <algorithm>
#include "reference_counted.hpp"
#include "i_iterator.hpp"

namespace neolib
{
    template <typename T, typename ConstIteratorType, typename IteratorType, bool DefaultComparisonOperators = true>
    class i_container : public i_reference_counted
    {
    protected:
        typedef i_container<T, ConstIteratorType, IteratorType, DefaultComparisonOperators> generic_container_type;
    public:
        typedef T value_type;
        typedef size_t size_type;
        typedef ConstIteratorType abstract_const_iterator;
        typedef IteratorType abstract_iterator;
    public:
        typedef typename abstract_const_iterator::iterator_wrapper const_iterator;
        typedef typename abstract_iterator::iterator_wrapper iterator;
        typedef std::reverse_iterator<const_iterator> const_reverse_iterator;
        typedef std::reverse_iterator<iterator> reverse_iterator;
    public:
        virtual size_type size() const = 0;
        virtual size_type max_size() const = 0;
        bool empty() const { return size() == 0; }
        const_iterator cbegin() const { const_iterator result; return do_begin(result.storage()); }
        const_iterator begin() const { return cbegin(); }
        iterator begin() { iterator result; return do_begin(result.storage()); }
        const_iterator cend() const { const_iterator result; return do_end(result.storage()); }
        const_iterator end() const { return cend(); }
        iterator end() { iterator result; return do_end(result.storage()); }
        const_reverse_iterator crbegin() const { return const_reverse_iterator{ end() }; }
        const_reverse_iterator rbegin() const { return crbegin(); }
        reverse_iterator rbegin() { return reverse_iterator{ end() }; }
        const_reverse_iterator crend() const { return const_reverse_iterator{ begin() }; }
        const_reverse_iterator rend() const { return crend(); }
        reverse_iterator rend() { return reverse_iterator{ begin() }; }
        iterator erase(const abstract_iterator& aPosition) { iterator result; return do_erase(result.storage(), const_iterator(aPosition)); }
        iterator erase(const abstract_const_iterator& aPosition) { iterator result; return do_erase(result.storage(), aPosition); }
        iterator erase(const abstract_iterator& aFirst, const abstract_iterator& aLast) { iterator result; return do_erase(result.storage(), const_iterator(aFirst), const_iterator(aLast)); }
        iterator erase(const abstract_const_iterator& aFirst, const abstract_const_iterator& aLast) { iterator result; return do_erase(result.storage(), aFirst, aLast); }
        virtual void clear() = 0;
        virtual void assign(const i_container& aRhs) = 0;
    public:
        i_container& operator=(const i_container& aRhs)
        {
            assign(aRhs);
            return *this;
        }
        template <bool SFINAE = DefaultComparisonOperators>
        typename std::enable_if_t<SFINAE, bool>::type operator==(const i_container& aRhs) const
        {
            return size() == aRhs.size() && std::equal(begin(), end(), aRhs.begin());
        }
        template <bool SFINAE = DefaultComparisonOperators>
        typename std::enable_if_t<SFINAE, bool>::type operator!=(const i_container& aRhs) const
        {
            return size() != aRhs.size() && !std::equal(begin(), end(), aRhs.begin());
        }
        template <bool SFINAE = DefaultComparisonOperators>
        typename std::enable_if_t<SFINAE, bool>::type operator<(const i_container& aRhs) const
        {
            return std::lexicographical_compare(begin(), end(), aRhs.begin(), aRhs.end());
        }
    private:
        virtual abstract_const_iterator* do_begin(void* memory) const = 0;
        virtual abstract_const_iterator* do_end(void* memory) const = 0;
        virtual abstract_iterator* do_begin(void* memory) = 0;
        virtual abstract_iterator* do_end(void* memory) = 0;
        virtual abstract_iterator* do_erase(void* memory, const abstract_const_iterator& aPosition) = 0;
        virtual abstract_iterator* do_erase(void* memory, const abstract_const_iterator& aFirst, const abstract_const_iterator& aLast) = 0;
    };
}
