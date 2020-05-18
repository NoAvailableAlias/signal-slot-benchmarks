// iterator.hpp - v1.0
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
#include <neolib/reference_counted.hpp>
#include <neolib/i_iterator.hpp>

namespace neolib
{
    namespace container
    {
        struct pure_iterator : std::logic_error { pure_iterator() : std::logic_error{ "neolib iterator is pure" } {} };

        template <typename, typename, typename, typename>
        class iterator;

        template <typename T, typename ContainerIterator, typename AbstractIterator = i_const_iterator<abstract_t<T>>>
        class const_iterator : public reference_counted<AbstractIterator, false>
        {
            typedef const_iterator<T, ContainerIterator, AbstractIterator> self_type;
            typedef reference_counted<AbstractIterator, false> base_type;
        public:
            typedef AbstractIterator abstract_type;
        public:
            typedef const T value_type;
            typedef const abstract_t<T> abstract_value_type;
            typedef ContainerIterator container_iterator;
            using typename base_type::difference_type;
            using typename base_type::pointer;
            using typename base_type::reference;
            using typename base_type::abstract_iterator;
            using typename base_type::abstract_const_iterator;
            using typename base_type::abstract_base_iterator;
            using typename base_type::abstract_base_const_iterator;
        public:
            const_iterator() {}
            const_iterator(container_iterator aContainerIterator) : iContainerIterator(aContainerIterator) {}
            const_iterator(const self_type& aOther) : iContainerIterator(aOther.iContainerIterator) {}
            template <typename ContainerIterator2, typename AbstractIterator2>
            const_iterator(const const_iterator<T, ContainerIterator2, AbstractIterator2>& aOther) : iContainerIterator(aOther.iContainerIterator) {}
            template <typename ContainerIterator2, typename ContainerConstIterator, typename AbstractIterator2>
            const_iterator(const iterator<T, ContainerIterator2, ContainerConstIterator, AbstractIterator2>& aOther) : iContainerIterator(aOther.iContainerIterator) {}
        public:
            const_iterator& operator=(const self_type& aOther) { iContainerIterator = aOther.iContainerIterator; return *this; }
            template <typename ContainerIterator2, typename ContainerConstIterator, typename AbstractIterator2>
            const_iterator& operator=(const iterator<T, ContainerIterator2, ContainerConstIterator, AbstractIterator2>& aOther) { iContainerIterator = aOther.iContainerIterator; return *this; }
        public:
            operator container_iterator() const { return iContainerIterator; }
        public:
            abstract_iterator& operator++() override { ++iContainerIterator; return *this; }
            abstract_iterator& operator--() override { --iContainerIterator; return *this; }
            reference operator*() const override { return to_abstract(*iContainerIterator); }
            pointer operator->() const override { return &(**this); }
            bool operator==(const abstract_base_iterator& aOther) const override { return iContainerIterator == static_cast<const self_type&>(aOther).iContainerIterator; }
            bool operator!=(const abstract_base_iterator& aOther) const override { return iContainerIterator != static_cast<const self_type&>(aOther).iContainerIterator; }
        public:
            abstract_iterator* clone(void* memory) const override { if constexpr (!std::is_abstract_v<self_type>) return new (memory) self_type{ iContainerIterator }; else throw pure_iterator(); }
        private:
            abstract_iterator* do_post_increment(void* memory) override { if constexpr (!std::is_abstract_v<self_type>) return new (memory) self_type{ iContainerIterator++ }; else throw pure_iterator(); }
            abstract_iterator* do_post_decrement(void* memory) override { if constexpr (!std::is_abstract_v<self_type>) return new (memory) self_type{ iContainerIterator-- }; else throw pure_iterator(); }
        protected:
            container_iterator iContainerIterator;
        };

        template <typename, typename, typename>
        class random_access_iterator;

        template <typename T, typename ContainerIterator>
        class random_access_const_iterator : public const_iterator<T, ContainerIterator, i_random_access_const_iterator<abstract_t<T>>>
        {
            typedef random_access_const_iterator<T, ContainerIterator> self_type;
            typedef const_iterator<T, ContainerIterator, i_random_access_const_iterator<abstract_t<T>>> base_type;
        public:
            using typename base_type::abstract_type;
            using typename base_type::value_type;
            using typename base_type::abstract_value_type;
            using typename base_type::container_iterator;
            using typename base_type::difference_type;
            using typename base_type::pointer;
            using typename base_type::reference;
            using typename base_type::abstract_iterator;
            using typename base_type::abstract_const_iterator;
            using typename base_type::abstract_base_iterator;
            using typename base_type::abstract_base_const_iterator;
            using typename base_type::abstract_random_access_iterator;
            using typename base_type::abstract_random_access_const_iterator;
        public:
            random_access_const_iterator() {}
            random_access_const_iterator(container_iterator aContainerIterator) : base_type(aContainerIterator) {}
            random_access_const_iterator(const self_type& aOther) : base_type(aOther.base_type::iContainerIterator) {}
            template <typename ContainerIterator2>
            random_access_const_iterator(const random_access_const_iterator<T, ContainerIterator2>& aOther) : base_type(aOther.iContainerIterator) {}
            template <typename ContainerIterator2, typename ContainerConstIterator>
            random_access_const_iterator(const random_access_iterator<T, ContainerIterator2, ContainerConstIterator>& aOther) : base_type(aOther.iContainerIterator) {}
        public:
            random_access_const_iterator& operator=(const self_type& aOther) { base_type::operator=(aOther); return *this; }
            template <typename ContainerIterator2, typename ContainerConstIterator>
            random_access_const_iterator& operator=(const random_access_iterator<T, ContainerIterator2, ContainerConstIterator>& aOther) { base_type::operator=(aOther); return *this; }
        public:
            operator container_iterator() const { return base_type::iContainerIterator; }
        public:
            abstract_iterator& operator+=(difference_type aDifference) { base_type::iContainerIterator += aDifference; return *this; }
            abstract_iterator& operator-=(difference_type aDifference) { base_type::iContainerIterator -= aDifference; return *this; }
            reference operator[](difference_type aDifference) const override { return base_type::iContainerIterator[aDifference]; }
            difference_type operator-(const abstract_random_access_iterator& aOther) const override { return base_type::iContainerIterator - static_cast<const self_type&>(aOther).base_type::iContainerIterator; }
            bool operator<(const abstract_random_access_iterator& aOther) const override { return base_type::iContainerIterator < static_cast<const self_type&>(aOther).base_type::iContainerIterator; }
            bool operator<=(const abstract_random_access_iterator& aOther) const override { return base_type::iContainerIterator <= static_cast<const self_type&>(aOther).base_type::iContainerIterator; }
            bool operator>(const abstract_random_access_iterator& aOther) const override { return base_type::iContainerIterator > static_cast<const self_type&>(aOther).base_type::iContainerIterator; }
            bool operator>=(const abstract_random_access_iterator& aOther) const override { return base_type::iContainerIterator >= static_cast<const self_type&>(aOther).base_type::iContainerIterator; }
        public:
            abstract_iterator* clone(void* memory) const override { return new (memory) self_type{ base_type::iContainerIterator }; }
        private:
            abstract_iterator* do_post_increment(void* memory) override { return new (memory) self_type{ base_type::iContainerIterator++ }; }
            abstract_iterator* do_post_decrement(void* memory) override { return new (memory) self_type{ base_type::iContainerIterator-- }; }
            abstract_random_access_iterator* do_add(void* memory, difference_type aDifference) const override { return new (memory) self_type{ base_type::iContainerIterator + aDifference }; }
            abstract_random_access_iterator* do_subtract(void* memory, difference_type aDifference) const override { return new (memory) self_type{ base_type::iContainerIterator - aDifference }; }
        };

        template <typename T, typename ContainerIterator, typename ContainerConstIterator, typename AbstractIterator = i_iterator<abstract_t<T>>>
        class iterator : public reference_counted<AbstractIterator, false>
        {
            typedef iterator<T, ContainerIterator, ContainerConstIterator, AbstractIterator> self_type;
            typedef reference_counted<AbstractIterator, false> base_type;
            template <typename, typename, typename>
            friend class const_iterator;
        public:
            typedef AbstractIterator abstract_type;
            typedef T value_type;
            typedef abstract_t<T> abstract_value_type;
            typedef ContainerIterator container_iterator;
            typedef ContainerConstIterator container_const_iterator;
            using typename base_type::difference_type;
            using typename base_type::pointer;
            using typename base_type::reference;
            using typename base_type::abstract_iterator;
            using typename base_type::abstract_const_iterator;
            using typename base_type::abstract_base_iterator;
            using typename base_type::abstract_base_const_iterator;
        public:
            iterator() {}
            iterator(container_iterator aContainerIterator) : iContainerIterator(aContainerIterator) {}
            iterator(const iterator& aOther) : iContainerIterator(aOther.iContainerIterator) {}
            iterator& operator=(const iterator& aOther) { iContainerIterator = aOther.iContainerIterator; return *this; }
        public:
            operator container_iterator() const { return iContainerIterator; }
        public:
            abstract_iterator& operator++() override { ++iContainerIterator; return *this; }
            abstract_iterator& operator--() override { --iContainerIterator; return *this; }
            reference operator*() const override { return to_abstract(*iContainerIterator); }
            pointer operator->() const override { return &(**this); }
            bool operator==(const abstract_base_iterator& aOther) const override { return iContainerIterator == static_cast<const self_type&>(aOther).iContainerIterator; }
            bool operator!=(const abstract_base_iterator& aOther) const override { return iContainerIterator != static_cast<const self_type&>(aOther).iContainerIterator; }
        public:
            abstract_iterator* clone(void* memory) const override { if constexpr (!std::is_abstract_v<self_type>) return new (memory) self_type{ iContainerIterator }; else throw pure_iterator(); }
            abstract_const_iterator* const_clone(void* memory) const override { if constexpr (!std::is_abstract_v<self_type>) return new (memory) const_iterator<T, container_const_iterator, abstract_const_iterator>{ *this }; else throw pure_iterator(); }
        private:
            abstract_iterator* do_post_increment(void* memory) override { if constexpr (!std::is_abstract_v<self_type>) return new (memory) self_type{ iContainerIterator++ }; else throw pure_iterator(); }
            abstract_iterator* do_post_decrement(void* memory) override { if constexpr (!std::is_abstract_v<self_type>) return new (memory) self_type{ iContainerIterator-- }; else throw pure_iterator(); }
        protected:
            container_iterator iContainerIterator;
        };

        template <typename T, typename ContainerIterator, typename ContainerConstIterator>
        class random_access_iterator : public iterator<T, ContainerIterator, ContainerConstIterator, i_random_access_iterator<abstract_t<T>>>
        {
            typedef random_access_iterator<T, ContainerIterator, ContainerConstIterator> self_type;
            typedef iterator<T, ContainerIterator, ContainerConstIterator, i_random_access_iterator<abstract_t<T>>> base_type;
            template <typename, typename>
            friend class random_access_const_iterator;
        public:
            using typename base_type::abstract_type;
            using typename base_type::value_type;
            using typename base_type::abstract_value_type;
            using typename base_type::container_iterator;
            using typename base_type::container_const_iterator;
            using typename base_type::difference_type;
            using typename base_type::pointer;
            using typename base_type::reference;
            using typename base_type::abstract_iterator;
            using typename base_type::abstract_const_iterator;
            using typename base_type::abstract_base_iterator;
            using typename base_type::abstract_base_const_iterator;
            using typename base_type::abstract_random_access_iterator;
            using typename base_type::abstract_random_access_const_iterator;
        public:
            random_access_iterator() : base_type{} {}
            random_access_iterator(container_iterator aContainerIterator) : base_type{ aContainerIterator } {}
            random_access_iterator(const random_access_iterator& aOther) : base_type{ aOther } {}
            random_access_iterator& operator=(const random_access_iterator& aOther) { base_type::operator=(aOther); return *this; }
        public:
            abstract_iterator& operator+=(difference_type aDifference) override { base_type::iContainerIterator += aDifference; return *this; }
            abstract_iterator& operator-=(difference_type aDifference) override { base_type::iContainerIterator -= aDifference; return *this; }
            reference operator[](difference_type aDifference) const override { return base_type::iContainerIterator[aDifference]; }
            difference_type operator-(const abstract_random_access_iterator& aOther) const override { return base_type::iContainerIterator - static_cast<const self_type&>(aOther).base_type::iContainerIterator; }
            bool operator<(const abstract_random_access_iterator& aOther) const override { return base_type::iContainerIterator < static_cast<const self_type&>(aOther).base_type::iContainerIterator; }
            bool operator<=(const abstract_random_access_iterator& aOther) const override { return base_type::iContainerIterator <= static_cast<const self_type&>(aOther).base_type::iContainerIterator; }
            bool operator>(const abstract_random_access_iterator& aOther) const override { return base_type::iContainerIterator > static_cast<const self_type&>(aOther).base_type::iContainerIterator; }
            bool operator>=(const abstract_random_access_iterator& aOther) const override { return base_type::iContainerIterator >= static_cast<const self_type&>(aOther).base_type::iContainerIterator; }
        public:
            abstract_random_access_iterator* clone(void* memory) const override { return new (memory) self_type{ base_type::iContainerIterator }; }
            abstract_random_access_const_iterator* const_clone(void* memory) const override { return new (memory) random_access_const_iterator<T, ContainerConstIterator>{ *this }; }
        private:
            abstract_random_access_iterator* do_add(void* memory, difference_type aDifference) const override { return new (memory) self_type{ base_type::iContainerIterator + aDifference }; }
            abstract_random_access_iterator* do_subtract(void* memory, difference_type aDifference) const override { return new (memory) self_type{ base_type::iContainerIterator - aDifference }; }
        };
    }
}
