// optional.hpp
/*
 *  Copyright (c) 2007, 2021 Leigh Johnston.
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
#include <optional>
#include <istream>
#include <ostream>
#include <neolib/core/reference_counted.hpp>
#include <neolib/core/i_optional.hpp>

namespace neolib
{
    template<typename T>
    class optional;

    template <typename T>
    struct is_optional<optional<T>> { static constexpr bool value = true; };

    template<typename T>
    class optional : public reference_counted<i_optional<maybe_abstract_t<T>>>
    {
        // types
    public:
        typedef i_optional<maybe_abstract_t<T>> abstract_type;
        typedef std::optional<T> std_type;
        typedef T value_type;
        typedef value_type* pointer;
        typedef const value_type* const_pointer;
        typedef value_type& reference;
        typedef const value_type& const_reference;
        typedef typename abstract_type::reference abstract_reference;
        typedef typename abstract_type::const_reference abstract_const_reference;
        // construction
    public:
        optional() :
            iData{}
        {
        }
        optional(std::nullopt_t) :
            iData{ std::nullopt }
        {
        }
        optional(value_type const& other) :
            iData{ other }
        {
        }
        optional(abstract_type const& other) :
            iData{}
        {
            *this = other;
        }
        explicit optional(std_type const& other) :
            iData{ other }
        {
        }
        template <typename U, typename = std::enable_if_t<std::is_constructible_v<T, U>, sfinae>>
        optional(U const& value) :
            iData{ value }
        {
        }
        // state
    public:
        bool has_value() const noexcept final
        {
            return iData.has_value();
        }
        explicit operator bool() const noexcept final
        {
            return has_value();
        }
        // std
    public:
        const std_type& as_std_optional() const
        {
            return iData;
        }
        std_type& as_std_optional()
        {
            return iData;
        }
        std_type to_std_optional() const
        {
            return iData;
        }
        // element access
    public:
        reference value() final
        {
            return iData.value();
        }
        const_reference value() const final
        {
            return iData.value();
        }
        abstract_const_reference value_or(abstract_const_reference aDefaultValue) const final
        {
            if (iData.has_value())
                return iData.value();
            else
                return aDefaultValue;
        }
        abstract_const_reference value_or(abstract_reference aDefaultValue) const final
        {
            if (iData.has_value())
                return iData.value();
            else
                return aDefaultValue;
        }
        abstract_reference value_or(abstract_reference aDefaultValue) final
        {
            if (iData.has_value())
                return iData.value();
            else 
                return aDefaultValue;
        }
        reference operator*() final
        {
            return iData.operator*();
        }
        const_reference operator*() const final
        {
            return iData.operator*();
        }
        pointer operator->() final
        {
            return iData.operator->();
        }
        const_pointer operator->() const final
        {
            return iData.operator->();
        }
        // modifiers
    public:
        template <typename... Args>
        reference& emplace(Args&&... args)
        {
            iData.emplace(std::forward<Args>(args)...);
            return value();
        }
        void reset() final
        {
            iData.reset();
        }
        optional& operator=(std::nullopt_t) noexcept final
        {
            iData = std::nullopt;
            return *this;
        }
        optional& operator=(const optional& rhs)
        {
            if (rhs.has_value())
                iData = rhs.value();
            else
                iData = std::nullopt;
            return *this;
        }
        optional& operator=(const abstract_type& rhs) final
        {
            if (rhs.has_value())
                iData = T{ rhs.value() };
            else
                iData = std::nullopt;
            return *this;
        }
        optional& operator=(const maybe_abstract_t<T>& value) final
        {
            iData = T{ value };
            return *this;
        }
    public:
        bool operator==(std::nullopt_t) const
        {
            return !has_value();
        }
        bool operator!=(std::nullopt_t) const
        {
            return has_value();
        }
        bool operator==(const optional<T>& that) const
        {
            if (has_value() != that.has_value())
                return false;
            if (!has_value())
                return true;
            return value() == that.value();
        }
        std::partial_ordering operator<=>(const optional<T>& that) const
        {
            if (has_value() < that.has_value())
                return std::partial_ordering::less;
            else if (has_value() > that.has_value())
                return std::partial_ordering::greater;
            else if (!has_value())
                return std::partial_ordering::equivalent;
            return value() <=> that.value();
        }
    private:
        std::optional<T> iData;
    };

    template <typename T>
    inline constexpr optional<std::decay_t<T>> make_optional(T&& value)
    {
        return optional<std::decay_t<T>>(std::forward<T>(value));
    }

    template <typename T>
    inline bool operator==(const optional<T>& lhs, const maybe_abstract_t<optional<T>>& rhs)
    {
        if (lhs.has_value() != rhs.has_value())
            return false;
        if (!lhs.has_value())
            return true;
        return lhs.value() == rhs.value();
    }

    template <typename T>
    inline bool operator==(const maybe_abstract_t<optional<T>>& lhs, const optional<T>& rhs)
    {
        if (lhs.has_value() != rhs.has_value())
            return false;
        if (!lhs.has_value())
            return true;
        return lhs.value() == rhs.value();
    }

    template <typename T>
    inline std::partial_ordering operator<=>(const optional<T>& lhs, const maybe_abstract_t<optional<T>>& rhs)
    {
        if (lhs.has_value() < rhs.has_value())
            return std::partial_ordering::less;
        else if (lhs.has_value() > rhs.has_value())
            return std::partial_ordering::greater;
        else if (!lhs.has_value())
            return std::partial_ordering::equivalent;
        return lhs.value() <=> rhs.value();
    }

    template <typename T>
    inline std::partial_ordering operator<=>(const maybe_abstract_t<optional<T>>& lhs, const optional<T>& rhs)
    {
        if (lhs.has_value() < rhs.has_value())
            return std::partial_ordering::less;
        else if (lhs.has_value() > rhs.has_value())
            return std::partial_ordering::greater;
        else if (!lhs.has_value())
            return std::partial_ordering::equivalent;
        return lhs.value() <=> rhs.value();
    }

    template <typename T, typename U, typename = std::enable_if_t<!is_optional_v<U>, sfinae>>
    inline bool operator==(const optional<T>& lhs, const U& rhs)
    {
        if (!lhs.has_value())
            return false;
        return lhs.value() == rhs;
    }

    template <typename T, typename U, typename = std::enable_if_t<!is_optional_v<U>, sfinae>>
    inline bool operator==(const U& lhs, const optional<T>& rhs)
    {
        if (!rhs.has_value())
            return false;
        return lhs == rhs.value();
    }

    template <typename T, typename U, typename = std::enable_if_t<!is_optional_v<U>, sfinae>>
    inline std::partial_ordering operator<=>(const optional<T>& lhs, const U& rhs)
    {
        if (!lhs.has_value())
            return std::partial_ordering::less;
        else
            return lhs.value() <=> rhs;
    }

    template <typename T, typename U, typename = std::enable_if_t<!is_optional_v<U>, sfinae>>
    inline std::partial_ordering operator<=>(const U& lhs, const optional<T>& rhs)
    {
        if (!rhs.has_value())
            return std::partial_ordering::greater;
        else
            return lhs <=> rhs.value();
    }

    template <typename T, std::enable_if_t<!std::is_same_v<T, maybe_abstract_t<T>>, int> = 0>
    inline std::partial_ordering operator<=>(const optional<T>& lhs, const maybe_abstract_t<T>& rhs)
    {
        if (!lhs.has_value())
            return std::partial_ordering::less;
        else
            return lhs.value() <=> rhs;
    }

    template <typename T, std::enable_if_t<!std::is_same_v<T, maybe_abstract_t<T>>, int> = 0>
    inline std::partial_ordering operator<=>(const maybe_abstract_t<T>& lhs, const optional<T>& rhs)
    {
        if (!rhs.has_value())
            return std::partial_ordering::greater;
        else
            return lhs <=> rhs.value();
    }

    template <typename T>
    struct optional_type { typedef T type; };
    template <typename T>
    struct optional_type<std::optional<T>> { typedef T type; };
    template <typename T>
    struct optional_type<optional<T>> { typedef T type; };

    template <typename T>
    using optional_t = typename optional_type<T>::type;

    template <typename Elem, typename Traits, typename T>
    inline std::basic_ostream<Elem, Traits>& operator<<(std::basic_ostream<Elem, Traits>& aStream, optional<T> const& aOptional)
    {
        if (aOptional.has_value())
            aStream << aOptional.value();
        else
            aStream << '?';
        return aStream;
    }

    template <typename Elem, typename Traits, typename T>
    inline std::basic_istream<Elem, Traits>& operator>>(std::basic_istream<Elem, Traits>& aStream, optional<T>& aOptional)
    {
        char temp;
        aStream >> temp;
        if (temp != '?')
        {
            aStream.unget();
            aOptional.emplace();
            aStream >> aOptional.value();
        }
        return aStream;
    }
}
