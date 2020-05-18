/*
 *  neolib.hpp
 *
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

#include <type_traits>
#include <cstdint>
#include <utility>
#include <variant>

#ifdef NDEBUG
constexpr bool ndebug = true;
#else
constexpr bool ndebug = false;
#endif

#define rvalue_cast static_cast

namespace neolib
{
    struct sfinae {};

    template <typename T>
    using to_const_reference_t = const std::remove_reference_t<T>&;
    template <typename T>
    inline to_const_reference_t<T> to_const(T&& object)
    {
        return const_cast<to_const_reference_t<T>>(object);
    }

    template <typename T1, typename T2>
    class pair;

    namespace detail
    {
        template <typename T>
        struct is_pair { static constexpr bool value = false; };
        template <typename T1, typename T2>
        struct is_pair<std::pair<T1, T2>> { static constexpr bool value = true; };
        template <typename T1, typename T2>
        struct is_pair<const std::pair<T1, T2>> { static constexpr bool value = true; };
        template <typename T>
        constexpr bool is_pair_v = is_pair<T>::value;

        template <typename T>
        constexpr bool abstract_class_possible_v = std::is_class_v<T> && !is_pair_v<T>;

        template <typename T, typename AT, typename = sfinae>
        struct correct_const;
        template <typename T, typename AT>
        struct correct_const<T, AT, typename std::enable_if<!std::is_const_v<T>, sfinae>::type> { typedef AT type; };
        template <typename T, typename AT>
        struct correct_const<T, AT, typename std::enable_if<std::is_const_v<T>, sfinae>::type> { typedef const AT type; };

        template <typename T, typename AT>
        using correct_const_t = typename correct_const<T, AT>::type;

        template <typename, typename = sfinae>
        struct abstract_type : std::false_type {};
        template <typename T>
        struct abstract_type<T, typename std::enable_if<abstract_class_possible_v<T>, sfinae>::type> : std::true_type { typedef correct_const_t<T, typename T::abstract_type> type; };
        template <typename T>
        struct abstract_type<T, typename std::enable_if<std::is_arithmetic_v<T>, sfinae>::type> : std::true_type { typedef correct_const_t<T, T> type; };
        template <typename T>
        struct abstract_type<T, typename std::enable_if<std::is_enum_v<T>, sfinae>::type> : std::true_type { typedef correct_const_t<T, T> type; };
        template <typename T>
        struct abstract_type<T, typename std::enable_if<std::is_pointer_v<T>, sfinae>::type> : std::true_type { typedef correct_const_t<T, T> type; };
        template <typename T1, typename T2>
        struct abstract_type<std::pair<T1, pair<T1, T2>>> : std::false_type { typedef typename abstract_type<pair<T1, T2>>::type type; };
        template <typename T1, typename T2>
        struct abstract_type<const std::pair<T1, pair<T1, T2>>> : std::false_type { typedef typename abstract_type<const pair<T1, T2>>::type type; };
        template <>
        struct abstract_type<std::monostate> : std::true_type { typedef std::monostate type; };
    }

    template <typename T>
    using abstract_t = typename detail::abstract_type<T>::type;

    template <typename T, typename = std::enable_if_t<detail::abstract_type<T>::value, sfinae>>
    inline const abstract_t<T>& to_abstract(const T& aArgument)
    {
        return static_cast<const abstract_t<T>&>(aArgument);
    }

    template <typename T, typename = std::enable_if_t<detail::abstract_type<T>::value, sfinae>>
    inline abstract_t<T>& to_abstract(T& aArgument)
    {
        return static_cast<abstract_t<T>&>(aArgument);
    }

    template <typename T1, typename T2>
    inline const abstract_t<pair<T1, T2>>& to_abstract(const std::pair<T1, pair<T1, T2>>& aArgument)
    {
        return static_cast<const abstract_t<pair<T1, T2>>&>(aArgument.second);
    }

    template <typename T1, typename T2>
    inline abstract_t<neolib::pair<T1, T2>>& to_abstract(std::pair<T1, pair<T1, T2>>& aArgument)
    {
        return static_cast<abstract_t<pair<T1, T2>>&>(aArgument.second);
    }
}

#ifdef NEOLIB_HOSTED_ENVIRONMENT

#define USING_BOOST
#define BOOST_ASIO_DISABLE_SMALL_BLOCK_RECYCLING
#define _SILENCE_CXX17_ALLOCATOR_VOID_DEPRECATION_WARNING

#ifdef _WIN32
#include "win32.hpp"
#endif

#ifdef USING_BOOST
#ifndef API
#include <boost/dll.hpp>
#define API extern "C" BOOST_SYMBOL_EXPORT
#endif
#endif

#endif // NEOLIB_HOSTED_ENVIRONMENT
