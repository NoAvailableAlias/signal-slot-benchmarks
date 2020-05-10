// jar.hpp
/*
 *  Copyright (c) 2018 Leigh Johnston.
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
#include <set>
#include <mutex>
#include <atomic>
#include <boost/stacktrace.hpp>
#include <neolib/vector.hpp>
#include <neolib/mutex.hpp>

namespace neolib
{
    typedef uint32_t cookie;
    typedef uint16_t small_cookie;

    template <typename CookieType>
    class i_basic_cookie_consumer
    {
    public:
        typedef CookieType cookie_type;
    public:
        struct invalid_release : std::logic_error { invalid_release() : std::logic_error("neolib::i_basic_cookie_consumer::invalid_release") {} };
    public:
        virtual ~i_basic_cookie_consumer() = default;
    public:
        virtual void add_ref(cookie_type aCookie) = 0;
        virtual void release(cookie_type aCookie) = 0;
        virtual long use_count(cookie_type aCookie) const = 0;
    };

    template <typename CookieType>
    class basic_cookie_ref_ptr
    {
    public:
        typedef CookieType cookie_type;
        static constexpr cookie_type no_cookie = cookie_type{};
    public:
        basic_cookie_ref_ptr() :
            iConsumer{ nullptr },
            iCookie{ no_cookie }
        {
        }
        basic_cookie_ref_ptr(i_basic_cookie_consumer<cookie_type>& aConsumer, cookie_type aCookie) :
            iConsumer{ &aConsumer },
            iCookie{ aCookie }
        {
            add_ref();
        }
        ~basic_cookie_ref_ptr()
        {
            release();
        }
        basic_cookie_ref_ptr(const basic_cookie_ref_ptr& aOther) :
            iConsumer{ aOther.iConsumer },
            iCookie{ aOther.iCookie }
        {
            add_ref();
        }
        basic_cookie_ref_ptr(basic_cookie_ref_ptr&& aOther) :
            iConsumer{ aOther.iConsumer },
            iCookie{ aOther.iCookie }
        {
            add_ref();
            aOther.release();
        }
    public:
        basic_cookie_ref_ptr& operator=(const basic_cookie_ref_ptr& aOther)
        {
            if (&aOther == this)
                return *this;
            basic_cookie_ref_ptr temp{ std::move(*this) };
            iConsumer = aOther.iConsumer;
            iCookie = aOther.iCookie;
            add_ref();
            return *this;
        }
        basic_cookie_ref_ptr& operator=(basic_cookie_ref_ptr&& aOther)
        {
            if (&aOther == this)
                return *this;
            basic_cookie_ref_ptr temp{ std::move(*this) };
            iConsumer = aOther.iConsumer;
            iCookie = aOther.iCookie;
            add_ref();
            aOther.release();
            return *this;
        }
    public:
        bool operator==(const basic_cookie_ref_ptr& aRhs) const
        {
            return iConsumer == aRhs.iConsumer && iCookie == aRhs.iCookie;
        }
        bool operator!=(const basic_cookie_ref_ptr& aRhs) const
        {
            return !(*this == aRhs);
        }
        bool operator<(const basic_cookie_ref_ptr& aRhs) const
        {
            return std::tie(iConsumer, iCookie) < std::tie(aRhs.iConsumer, aRhs.iCookie);
        }
    public:
        bool valid() const
        {
            return have_consumer() && have_cookie();
        }
        bool expired() const
        {
            return !valid();
        }
        cookie_type cookie() const
        {
            return iCookie;
        }
        void reset() const
        {
            iConsumer = nullptr;
            iCookie = no_cookie;
        }
    private:
        void add_ref() const
        {
            if (!valid())
                return;
            consumer().add_ref(cookie());
        }
        void release() const
        {
            if (!valid())
                return;
            consumer().release(cookie());
            reset();
        }
        bool have_consumer() const
        {
            return iConsumer != nullptr;
        }
        i_basic_cookie_consumer<cookie_type>& consumer() const
        {
            return *iConsumer;
        }
        bool have_cookie() const
        {
            return iCookie != no_cookie;
        }
    private:
        mutable i_basic_cookie_consumer<cookie_type>* iConsumer;
        mutable cookie_type iCookie;
    };

    template<class CookieType>
    constexpr CookieType invalid_cookie = static_cast<CookieType>(~CookieType{});

    struct cookie_invalid : std::logic_error { cookie_invalid() : std::logic_error("neolib::cookie_invalid") {} };
    struct cookie_already_added : std::logic_error { cookie_already_added() : std::logic_error("neolib::cookie_already_added") {} };
    struct cookies_exhausted : std::logic_error { cookies_exhausted() : std::logic_error("neolib::cookies_exhausted") {} };
    struct no_pointer_value_type_cookie_lookup : std::logic_error { no_pointer_value_type_cookie_lookup() : std::logic_error("neolib::no_pointer_value_type_cookie_lookup") {} };

    namespace detail
    {
        template<typename T> struct is_smart_ptr : std::false_type {};
        template<typename T> struct is_smart_ptr<std::shared_ptr<T>> : std::true_type {};
        template<typename T> struct is_smart_ptr<std::unique_ptr<T>> : std::true_type {};
        template<typename T>
        inline constexpr bool is_smart_ptr_v = is_smart_ptr<T>::value;
    }

    template <typename T, typename Container = std::vector<T>, typename CookieType = cookie, typename MutexType = null_mutex>
    class basic_jar
    {
    public:
        typedef CookieType cookie_type;
    public:
        typedef T value_type;
        typedef Container container_type;
        typedef typename container_type::const_iterator const_iterator;
        typedef typename container_type::iterator iterator;
        typedef MutexType mutex_type;
    private:
        typedef typename container_type::size_type reverse_index_t;
        typedef std::vector<reverse_index_t> reverse_indices_t;
        typedef std::vector<cookie_type> cookies_t;
    private:
        static constexpr cookie_type INVALID_COOKIE = invalid_cookie<cookie_type>;
        static constexpr reverse_index_t INVALID_REVERSE_INDEX = static_cast<reverse_index_t>(~reverse_index_t{});
    public:
        basic_jar() : iNextAvailableCookie{}
        {
        }
    public:
        bool empty() const
        {
            return items().empty();
        }
        std::size_t size() const
        {
            return items().size();
        }
        bool contains(cookie_type aCookie) const
        {
            std::scoped_lock<mutex_type> lock{ mutex() };
            return aCookie < reverse_indices().size() && reverse_indices()[aCookie] != INVALID_REVERSE_INDEX;
        }
        const value_type& operator[](cookie_type aCookie) const
        {
            std::scoped_lock<mutex_type> lock{ mutex() };
            if (aCookie >= reverse_indices().size())
                throw cookie_invalid();
            auto reverseIndex = reverse_indices()[aCookie];
            if (reverseIndex == INVALID_REVERSE_INDEX)
                throw cookie_invalid();
            return items()[reverseIndex];
        }
        value_type& operator[](cookie_type aCookie)
        {
            return const_cast<value_type&>(to_const(*this)[aCookie]);
        }
        const value_type& at_index(std::size_t aIndex) const
        {
            return items().at(aIndex);
        }
        value_type& at_index(std::size_t aIndex)
        {
            return items().at(aIndex);
        }
        cookie_type insert(const value_type& aItem)
        {
            auto cookie = next_cookie();
            try
            {
                add(cookie, aItem);
            }
            catch (...)
            {
                return_cookie(cookie);
                throw;
            }
            return cookie;
        }
        template <typename... Args>
        cookie_type emplace(Args&&... aArgs)
        {
            auto cookie = next_cookie();
            try
            {
                add(cookie, std::forward<Args>(aArgs)...);
            }
            catch (...)
            {
                return_cookie(cookie);
                throw;
            }
            return cookie;
        }
        iterator erase(const_iterator aItem)
        {
            return remove(*aItem);
        }
        template <typename... Args>
        iterator add(cookie_type aCookie, Args&&... aArgs)
        {
            std::scoped_lock<mutex_type> lock{ mutex() };
            if (reverse_indices().size() <= aCookie)
                reverse_indices().insert(reverse_indices().end(), (aCookie + 1) - reverse_indices().size(), INVALID_REVERSE_INDEX);
            if (reverse_indices()[aCookie] != INVALID_REVERSE_INDEX)
                throw cookie_already_added();
            std::optional<iterator> result;
            if constexpr (!detail::is_smart_ptr_v<value_type>)
                result = items().emplace(items().end(), std::forward<Args>(aArgs)...);
            else if constexpr (std::is_abstract_v<typename value_type::element_type>)
                result = items().emplace(items().end(), std::forward<Args>(aArgs)...);
            else
                result = items().insert(items().end(), value_type{ new typename value_type::element_type{std::forward<Args>(aArgs)...} });
            try
            {
                allocated_cookies().insert(allocated_cookies().end(), aCookie);
            }
            catch (...)
            {
                items().pop_back();
                throw;
            }
            reverse_indices()[aCookie] = items().size() - 1;
            return *result;
        }
        iterator remove(const value_type& aItem)
        {
            std::scoped_lock<mutex_type> lock{ mutex() };
            return remove(item_cookie(aItem));
        }
        iterator remove(cookie_type aCookie)
        {
            std::scoped_lock<mutex_type> lock{ mutex() };
            if (aCookie >= reverse_indices().size())
                throw cookie_invalid();
            auto& reverseIndex = reverse_indices()[aCookie];
            if (reverseIndex == INVALID_REVERSE_INDEX)
                throw cookie_invalid();
            if (reverseIndex < items().size() - 1)
            {
                auto& item = items()[reverseIndex];
                std::swap(item, items().back());
                auto& cookie = allocated_cookies()[reverseIndex];
                std::swap(cookie, allocated_cookies().back());
                reverse_indices()[cookie] = reverseIndex;
            }
            items().pop_back();
            allocated_cookies().pop_back();
            iterator result = std::next(items().begin(), reverseIndex);
            reverseIndex = INVALID_REVERSE_INDEX;
            return_cookie(aCookie);
            return result;
        }
    public:
        cookie_type item_cookie(const value_type& aItem) const
        {
            if constexpr (!std::is_pointer_v<value_type>)
                return allocated_cookies()[std::distance(&*items().begin(), &aItem)];
            else
                throw no_pointer_value_type_cookie_lookup();
        }
        cookie_type next_cookie()
        {
            std::scoped_lock<mutex_type> lock{ mutex() };
            if (!free_cookies().empty())
            {
                auto nextCookie = free_cookies().back();
                free_cookies().pop_back();
                return nextCookie;
            }
            auto nextCookie = ++iNextAvailableCookie;
            if (nextCookie == INVALID_COOKIE)
                throw cookies_exhausted();
            return nextCookie;
        }
        void return_cookie(cookie_type aCookie)
        {
            std::scoped_lock<mutex_type> lock{ mutex() };
            free_cookies().push_back(aCookie);
        }
    public:
        mutex_type& mutex() const
        {
            return iMutex;
        }
        const_iterator cbegin() const
        {
            return items().begin();
        }
        const_iterator begin() const
        {
            return cbegin();
        }
        iterator begin()
        {
            return items().begin();
        }
        const_iterator cend() const
        {
            return items().end();
        }
        const_iterator end() const
        {
            return cend();
        }
        iterator end()
        {
            return items().end();
        }
    public:
        void clear()
        {
            std::scoped_lock<mutex_type> lock{ mutex() };
            iNextAvailableCookie = 0ul;
            free_cookies().clear();
            items().clear();
            reverse_indices().clear();
        }
        const container_type& items() const
        {
            return iItems;
        }
        container_type& items()
        {
            return iItems;
        }
    private:
        const cookies_t& allocated_cookies() const
        {
            return iAllocatedCookies;
        }
        cookies_t& allocated_cookies()
        {
            return iAllocatedCookies;
        }
        const cookies_t& free_cookies() const
        {
            return iFreeCookies;
        }
        cookies_t& free_cookies()
        {
            return iFreeCookies;
        }
        const reverse_indices_t& reverse_indices() const
        {
            return iReverseIndices;
        }
        reverse_indices_t& reverse_indices()
        {
            return iReverseIndices;
        }
    private:
        mutable mutex_type iMutex;
        mutable std::atomic<cookie_type> iNextAvailableCookie;
        cookies_t iAllocatedCookies;
        container_type iItems;
        mutable cookies_t iFreeCookies;
        reverse_indices_t iReverseIndices;
    };

    typedef i_basic_cookie_consumer<cookie> i_cookie_consumer;
    typedef i_basic_cookie_consumer<small_cookie> i_small_cookie_consumer;
    typedef basic_cookie_ref_ptr<cookie> cookie_ref_ptr;
    typedef basic_cookie_ref_ptr<small_cookie> small_cookie_ref_ptr;

    template <typename T, typename MutexType = null_mutex>
    using jar = basic_jar<T, std::vector<T>, cookie, MutexType>;
    template <typename T, typename MutexType = null_mutex>
    using small_jar = basic_jar<T, std::vector<T>, small_cookie, MutexType>;

    template <typename T, typename MutexType = null_mutex>
    using polymorphic_jar = basic_jar<T, vector<T>, cookie, MutexType>;
    template <typename T, typename MutexType = null_mutex>
    using polymorphic_small_jar = basic_jar<T, vector<T>, small_cookie, MutexType>;
}
