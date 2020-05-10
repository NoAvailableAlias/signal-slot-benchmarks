// i_map.hpp - v1.0
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
#include "i_container.hpp"
#include "i_pair.hpp"

namespace neolib
{
    template <typename Key, typename T>
    class i_map : public i_container<i_pair<const Key, T>, i_const_iterator<i_pair<const Key, T>>, i_iterator<i_pair<const Key, T>>>
    {
        typedef i_map<Key, T> self_type;
        typedef i_container<i_pair<const Key, T>, i_const_iterator<i_pair<const Key, T> >, i_iterator<i_pair<const Key, T>>> base_type;
    public:
        typedef self_type abstract_type;
        typedef Key abstract_key_type;
        typedef T abstract_mapped_type;
        typedef i_pair<const abstract_key_type, abstract_mapped_type> abstract_value_type;
    protected:
        typedef typename base_type::abstract_const_iterator abstract_const_iterator;
        typedef typename base_type::abstract_iterator abstract_iterator;
    public:
        typedef typename base_type::const_iterator const_iterator;
        typedef typename base_type::iterator iterator;
    public:
        virtual abstract_mapped_type& operator[](const abstract_key_type& aKey) = 0;
        virtual const abstract_mapped_type& at(const abstract_key_type& aKey) const = 0;
        virtual abstract_mapped_type& at(const abstract_key_type& aKey) = 0;
    public:
        iterator insert(const abstract_value_type& aValue) { iterator result; return do_insert(result.storage(), aValue.first(), aValue.second()); }
        iterator insert(const abstract_key_type& aKey, const abstract_mapped_type& aMapped) { iterator result; return do_insert(result.storage(), aKey, aMapped); }
        const_iterator find(const abstract_key_type& aKey) const { const_iterator result; return do_find(result.storage(), aKey); }
        iterator find(const abstract_key_type& aKey) { iterator result; return do_find(result.storage(), aKey); }
        const_iterator lower_bound(const abstract_key_type& aKey) const { const_iterator result; return do_lower_bound(result.storage(), aKey); }
        iterator lower_bound(const abstract_key_type& aKey) { iterator result; return do_lower_bound(result.storage(), aKey); }
        const_iterator upper_bound(const abstract_key_type& aKey) const { const_iterator result; return do_upper_bound(result.storage(), aKey); }
        iterator upper_bound(const abstract_key_type& aKey) { iterator result; return do_upper_bound(result.storage(), aKey); }
    private:
        virtual abstract_iterator* do_insert(void* memory, const abstract_key_type& aKey, const abstract_mapped_type& aMapped) = 0;
        virtual abstract_const_iterator* do_find(void* memory, const abstract_key_type& aKey) const = 0;
        virtual abstract_iterator* do_find(void* memory, const abstract_key_type& aKey) = 0;
        virtual abstract_const_iterator* do_lower_bound(void* memory, const abstract_key_type& aKey) const = 0;
        virtual abstract_iterator* do_lower_bound(void* memory, const abstract_key_type& aKey) = 0;
        virtual abstract_const_iterator* do_upper_bound(void* memory, const abstract_key_type& aKey) const = 0;
        virtual abstract_iterator* do_upper_bound(void* memory, const abstract_key_type& aKey) = 0;
    };

    template <typename Key, typename T>
    class i_multimap : public i_container<i_pair<const Key, T>, i_const_iterator<i_pair<const Key, T>>, i_iterator<i_pair<const Key, T>>>
    {
        typedef i_multimap<Key, T> self_type;
        typedef i_container<i_pair<const Key, T>, i_const_iterator<i_pair<const Key, T> >, i_iterator<i_pair<const Key, T>>> base_type;
    public:
        typedef self_type abstract_type;
        typedef Key abstract_key_type;
        typedef T abstract_mapped_type;
        typedef i_pair<const abstract_key_type, abstract_mapped_type> abstract_value_type;
    protected:
        typedef typename base_type::abstract_const_iterator abstract_const_iterator;
        typedef typename base_type::abstract_iterator abstract_iterator;
    public:
        typedef typename base_type::const_iterator const_iterator;
        typedef typename base_type::iterator iterator;
    public:
        iterator insert(const abstract_value_type& aValue) { iterator result; return do_insert(result.storage(), aValue.first(), aValue.second()); }
        iterator insert(const abstract_key_type& aKey, const abstract_mapped_type& aMapped) { iterator result; return do_insert(result.storage(), aKey, aMapped); }
        const_iterator find(const abstract_key_type& aKey) const { const_iterator result; return do_find(result.storage(), aKey); }
        iterator find(const abstract_key_type& aKey) { iterator result; return do_find(result.storage(), aKey); }
        const_iterator lower_bound(const abstract_key_type& aKey) const { const_iterator result; return do_lower_bound(result.storage(), aKey); }
        iterator lower_bound(const abstract_key_type& aKey) { iterator result; return do_lower_bound(result.storage(), aKey); }
        const_iterator upper_bound(const abstract_key_type& aKey) const { const_iterator result; return do_upper_bound(result.storage(), aKey); }
        iterator upper_bound(const abstract_key_type& aKey) { iterator result; return do_upper_bound(result.storage(), aKey); }
    private:
        virtual abstract_iterator* do_insert(void* memory, const abstract_key_type& aKey, const abstract_mapped_type& aMapped) = 0;
        virtual abstract_const_iterator* do_find(void* memory, const abstract_key_type& aKey) const = 0;
        virtual abstract_iterator* do_find(void* memory, const abstract_key_type& aKey) = 0;
        virtual abstract_const_iterator* do_lower_bound(void* memory, const abstract_key_type& aKey) const = 0;
        virtual abstract_iterator* do_lower_bound(void* memory, const abstract_key_type& aKey) = 0;
        virtual abstract_const_iterator* do_upper_bound(void* memory, const abstract_key_type& aKey) const = 0;
        virtual abstract_iterator* do_upper_bound(void* memory, const abstract_key_type& aKey) = 0;
    };
}
