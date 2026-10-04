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
#include <optional>
#include <chrono>
#include <stdexcept>
#include <string>

#include <neolib/neolib_export.hpp>

#ifdef NDEBUG
constexpr bool ndebug = true;
#else
constexpr bool ndebug = false;
#endif

#define STRING2(x) #x
#define STRING(x) STRING2(x)

#define TODO_MSG __FILE__ "(" STRING(__LINE__) "): TODO"
#ifdef _MSC_VER
#define TODO \
    _Pragma("message (TODO_MSG)") \
    throw std::logic_error(std::string{ TODO_MSG });
#else
#define TODO \
    throw std::logic_error(std::string{ TODO_MSG });
#endif

#define rvalue_cast static_cast

#define GENERATE_HAS_MEMBER_TYPE(Type)                                            \
                                                                                  \
template <class T, bool OK = std::is_class_v<T>>                                  \
class HasMemberType_##Type                                                        \
{                                                                                 \
public:                                                                           \
    static constexpr bool RESULT = false;                                         \
};                                                                                \
                                                                                  \
template <class T>                                                                \
class HasMemberType_##Type<T, true>                                               \
{                                                                                 \
private:                                                                          \
    using Yes = char[2];                                                          \
    using  No = char[1];                                                          \
                                                                                  \
    struct Fallback { struct Type { }; };                                         \
    struct Derived : T, Fallback { };                                             \
                                                                                  \
    template < class U >                                                          \
    static No& test ( typename U::Type* );                                        \
    template < typename U >                                                       \
    static Yes& test ( U* );                                                      \
                                                                                  \
public:                                                                           \
    static constexpr bool RESULT = sizeof(test<Derived>(nullptr)) == sizeof(Yes); \
};                                                                                \
                                                                                  \
template < class T >                                                              \
struct has_member_type_##Type                                                     \
: public std::integral_constant<bool, HasMemberType_##Type<T>::RESULT>            \
{ };   

GENERATE_HAS_MEMBER_TYPE(abstract_type)

namespace neolib
{
    constexpr std::size_t MaxSize = static_cast<std::size_t>(-1);

    struct sfinae {};

    template <typename T>
    concept EnumClass = std::is_enum_v<T> && !std::is_convertible_v<T, std::underlying_type_t<T>>;

    template <typename T>
    using to_const_reference_t = const std::remove_reference_t<T>&;
    template <typename T>
    inline to_const_reference_t<T> to_const(T&& object)
    {
        return const_cast<to_const_reference_t<T>>(object);
    }

    template <typename T, typename... Ts> 
    struct variadic_index;

    template <typename T, typename... Ts>
    struct variadic_index<T, T, Ts...> : std::integral_constant<std::size_t, 0> {};

    template <typename T, typename Tail, typename... Ts>
    struct variadic_index<T, Tail, Ts...> : std::integral_constant<std::size_t, 1 + variadic_index<T, Ts...>::value> {};

    template <typename T, typename... Ts>
    constexpr std::size_t index_of_v = variadic_index<T, Ts...>::value;

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
        constexpr bool abstract_class_possible_v = std::is_class_v<T> && has_member_type_abstract_type<T>::value;

        template <typename T, typename AT, typename = sfinae>
        struct correct_const;
        template <typename T, typename AT>
        struct correct_const<T, AT, typename std::enable_if_t<!std::is_const_v<T>, sfinae>> { using type = AT; };
        template <typename T, typename AT>
        struct correct_const<T, AT, typename std::enable_if_t<std::is_const_v<T>, sfinae>> { using type = const AT ; };

        template <typename T, typename AT>
        using correct_const_t = typename correct_const<T, AT>::type;

        template <typename, typename = sfinae>
        struct abstract_type : std::false_type { using type = void; };
        template <typename T>
        struct abstract_type<T, typename std::enable_if_t<abstract_class_possible_v<T>, sfinae>> : std::true_type { using type = correct_const_t<T, typename T::abstract_type>; };
        template <typename T1, typename T2>
        struct abstract_type<std::pair<T1, pair<T1, T2>>> : std::false_type { using type = typename abstract_type<pair<T1, T2>>::type; };
        template <typename T1, typename T2>
        struct abstract_type<const std::pair<T1, pair<T1, T2>>> : std::false_type { using type = typename abstract_type<const pair<T1, T2>>::type; };

        template <typename T, bool HasAbstract = abstract_type<T>::value>
        struct maybe_abstract
        {
            using type = T;
        };

        template <typename T>
        struct maybe_abstract<T, true>
        {
            using type = typename abstract_type<T>::type;
        };
    }

    template <typename T>
    constexpr bool have_abstract_base_v = detail::abstract_class_possible_v<T>;

    template <typename T>
    using abstract_t = typename detail::abstract_type<T>::type;

    template <typename T>
    using maybe_abstract_t = typename detail::maybe_abstract<T>::type;

    template <typename T>
    inline const maybe_abstract_t<T>& to_abstract(const T& aArgument)
    {
        return static_cast<const maybe_abstract_t<T>&>(aArgument);
    }

    template <typename T>
    inline maybe_abstract_t<T>& to_abstract(T& aArgument)
    {
        return static_cast<maybe_abstract_t<T>&>(aArgument);
    }

    template <typename T1, typename T2>
    inline const maybe_abstract_t<pair<T1, T2>>& to_abstract(const std::pair<T1, pair<T1, T2>>& aArgument)
    {
        return static_cast<const maybe_abstract_t<pair<T1, T2>>&>(aArgument.second);
    }

    template <typename T1, typename T2>
    inline maybe_abstract_t<neolib::pair<T1, T2>>& to_abstract(std::pair<T1, pair<T1, T2>>& aArgument)
    {
        return static_cast<maybe_abstract_t<pair<T1, T2>>&>(aArgument.second);
    }

    namespace detail
    {
        template <typename T, typename = sfinae>
        struct abstract_return_type { using type = maybe_abstract_t<T>&; };
        template <typename T>
        struct abstract_return_type<T, std::enable_if_t<std::is_scalar_v<T>, sfinae>> { using type = std::remove_const_t<T>; };
    }

    template <typename T>
    using abstract_return_t = typename detail::abstract_return_type<T>::type;

    template <typename T>
    using cache = std::optional<T>;

    inline constexpr auto invalid = std::nullopt;

    template <typename T>
    inline void clear_cache(cache<T>& aCachedVariable)
    {
        aCachedVariable = invalid;
    }
}

#ifdef NEOLIB_HOSTED_ENVIRONMENT

// SIMD support
#ifndef NO_SIMD
#ifndef USE_AVX_DYNAMIC
#define USE_AVX
#endif
#ifndef USE_EMM_DYNAMIC
#define USE_EMM
#endif
#endif

#define USING_BOOST
#define BOOST_BIND_GLOBAL_PLACEHOLDERS
#define _SILENCE_CXX17_ALLOCATOR_VOID_DEPRECATION_WARNING

#ifdef _WIN32
#include <neolib/core/win32/win32.hpp>
#endif

#ifdef USING_BOOST
#ifndef API

#if defined(_MSC_VER) && !defined(__clang__) && !defined(__GNUC__)
#pragma warning ( push )
#pragma warning ( disable : 4456 )
#endif

#include <boost/dll.hpp>

#if defined(_MSC_VER) && !defined(__clang__) && !defined(__GNUC__)
#pragma warning ( pop )
#endif

#define API extern "C" BOOST_SYMBOL_EXPORT
#endif
#endif

#endif // NEOLIB_HOSTED_ENVIRONMENT
