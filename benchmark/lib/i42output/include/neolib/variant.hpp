// variant.hpp - v1.0
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
#include <neolib/variadic.hpp>

#include <type_traits>
#include <optional>
#include <variant>

namespace neolib
{
    typedef std::monostate none_t;
    const none_t none;

    template <typename... Types>
    class variant : public std::variant<none_t, Types...>
    {
    public:
        typedef std::variant<none_t, Types...> value_type;
        typedef std::size_t index_type;
    public:
        variant() :
            value_type()
        {
        }
        variant(const variant& aOther) :
            value_type{ static_cast<const value_type&>(aOther) }
        {
        }
        variant(variant&& aOther) noexcept :
            value_type{ static_cast<value_type&&>(std::move(aOther)) }
        {
        }
        template <typename T>
        variant(T&& aArgument, std::enable_if_t<!std::is_base_of_v<variant, std::decay_t<T>>, sfinae> = sfinae{}) :
            value_type{ std::forward<T>(aArgument) }
        {
        }
    public:
        variant& operator=(const variant& aOther)
        {
            value_type::operator=(static_cast<const value_type&>(aOther));
            return *this;
        }
        variant& operator=(variant&& aOther) noexcept
        {
            value_type::operator=(static_cast<value_type&&>(std::move(aOther)));
            return *this;
        }
        template <typename T>
        std::enable_if_t<!std::is_base_of_v<variant, std::decay_t<T>>, variant>& operator=(T&& aArgument)
        {
            value_type::operator=(std::forward<T>(aArgument));
            return *this;
        }
    public:
        const value_type& for_visitor() const
        {
            return *this;
        }
        value_type& for_visitor()
        {
            return *this;
        }
    public:
        bool operator==(const none_t) const
        {
            return std::holds_alternative<std::monostate>(*this);
        }
        bool operator!=(const none_t) const
        {
            return !std::holds_alternative<std::monostate>(*this);
        }
    };

    namespace variant_visitors
    {
        template <typename Visitor, typename... Types>
        auto visit(Visitor&& vis, const neolib::variant<Types...>& var)
        {
            return std::visit(std::forward<Visitor>(vis), var.for_visitor());
        }

        template <typename Visitor, typename... Types>
        auto visit(Visitor&& vis, neolib::variant<Types...>& var)
        {
            return std::visit(std::forward<Visitor>(vis), var.for_visitor());
        }
    }

    using namespace variant_visitors;

    // Deprecated, use std::get.
    template <typename T, typename Variant>
    inline auto& static_variant_cast(const Variant& aVariant)
    {
        return std::get<std::decay_t<T>>(aVariant);
    }

    // Deprecated, use std::get.
    template <typename T, typename Variant>
    inline auto& static_variant_cast(Variant& aVariant)
    { 
        return std::get<std::decay_t<T>>(aVariant);
    }

    template <typename VariantRef>
    struct auto_variant_caster
    {
        VariantRef variant;
        auto_variant_caster(VariantRef variant) :
            variant{ variant }
        {}
        template <typename T2>
        operator T2 () const
        {
            return static_variant_cast<T2&&>(variant);
        }
    };

    template <typename Variant>
    auto make_auto_variant_caster(const Variant& aVariant)
    {
        return auto_variant_caster<const Variant&>{aVariant};
    }

    template <typename Variant>
    auto make_auto_variant_caster(Variant& aVariant)
    {
        return auto_variant_caster<Variant&>{aVariant};
    }


    struct bad_numeric_variant_cast : std::logic_error { bad_numeric_variant_cast() : std::logic_error{ "neolib::bad_numeric_variant_cast" } {} };

    template <typename T, typename Variant>
    inline T static_numeric_variant_cast(const Variant& aVariant)
    {
        typedef T result_type;
        std::optional<result_type> result;
        visit([&result](auto&& source) 
        { 
            typedef std::remove_cv_t<std::remove_reference_t<decltype(source)>> source_type;
            if constexpr (std::is_arithmetic_v<source_type>)
                result = static_cast<result_type>(source); 
        }, aVariant);
        if (result != std::nullopt)
            return *result;
        throw bad_numeric_variant_cast();
    }

    template <typename T, typename Variant>
    inline T static_numeric_variant_cast(Variant& aVariant)
    {
        typedef T result_type;
        typedef std::remove_cv_t<std::remove_reference_t<result_type>> alternative_type;
        visit([&aVariant](auto&& source)
        { 
            typedef std::remove_cv_t<std::remove_reference_t<decltype(source)>> source_type;
            if constexpr (std::is_arithmetic_v<source_type> && !std::is_same_v<alternative_type, source_type>)
                aVariant = static_cast<alternative_type>(source);
        }, aVariant);
        return static_variant_cast<T>(aVariant);
    }
}

namespace std
{
    template <typename... Types>
    struct variant_size<neolib::variant<Types...>>
        : std::integral_constant<std::size_t, sizeof...(Types)> { };
    
    template <size_t I, class... Types>
    struct variant_alternative<I, neolib::variant<Types...>> 
        { typedef typename std::variant_alternative<I, typename neolib::variant<Types...>::value_type>::type type; };

    using neolib::variant_visitors::visit;
}

namespace neolib
{
    template<typename T, typename Variant, std::size_t index = 0>
    constexpr std::size_t index_of()
    {
        if constexpr (index == std::variant_size_v<Variant>)
            return index - 1;
        else if constexpr (std::is_same_v<std::variant_alternative_t<index, Variant>, T>)
            return index - 1;
        else
            return index_of<T, Variant, index + 1>();
    }
}

// Deprecated, use std::get.
using neolib::static_variant_cast;