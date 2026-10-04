// optional.hpp - v1.2.2
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
#include <optional>
#include <stdexcept>
#include <neolib/core/i_reference_counted.hpp>

namespace neolib
{
    template<typename T>
    class i_optional;
        
    template <typename T>
    struct is_optional { static constexpr bool value = false; };
    template <typename T>
    struct is_optional<i_optional<T>> { static constexpr bool value = true; };

    template <typename T>
    constexpr bool is_optional_v = is_optional<T>::value;

    template<typename T>
    class i_optional : public i_reference_counted
    {
        // types
    public:
        typedef i_optional abstract_type;
        typedef T value_type;
        typedef value_type* pointer;
        typedef const value_type* const_pointer;
        typedef value_type& reference;
        typedef const value_type& const_reference;
        // state
    public:
        virtual bool has_value() const noexcept = 0;
        virtual explicit operator bool() const noexcept = 0;
        // element access
    public:
        virtual reference value() = 0;
        virtual const_reference value() const = 0;
        virtual const_reference value_or(const_reference aDefaultValue) const = 0;
        virtual const_reference value_or(reference aDefaultValue) const = 0;
        virtual reference value_or(reference aDefaultValue) = 0;
        virtual reference operator*() = 0;
        virtual const_reference operator*() const = 0;
        virtual pointer operator->() = 0;
        virtual const_pointer operator->() const = 0;
        // modifiers
    public:
        virtual void reset() = 0;
        virtual i_optional<T>& operator=(std::nullopt_t) noexcept = 0;
        virtual i_optional<T>& operator=(const i_optional<T>& rhs) = 0;
        virtual i_optional<T>& operator=(const T& value) = 0;
        // comparison
    public:
        bool operator==(std::nullopt_t) const
        {
            return !has_value();
        }
        bool operator!=(std::nullopt_t) const
        {
            return has_value();
        }
        bool operator==(const i_optional<T>& that) const
        {
            if (has_value() != that.has_value())
                return false;
            if (!has_value())
                return true;
            return value() == that.value();
        }
        std::partial_ordering operator<=>(const i_optional<T>& that) const
        {
            if (has_value() < that.has_value())
                return std::partial_ordering::less;
            else if (has_value() > that.has_value())
                return std::partial_ordering::greater;
            else if (!has_value())
                return std::partial_ordering::equivalent;
            return value() <=> that.value();
        }
    };

    template <typename T, typename U, typename = std::enable_if_t<!is_optional_v<U>, sfinae>>
    inline bool operator==(const i_optional<T>& lhs, const U& rhs)
    {
        if (!lhs.has_value())
            return false;
        return lhs.get() == rhs;
    }

    template <typename T, typename U, typename = std::enable_if_t<!is_optional_v<U>, sfinae>>
    inline bool operator==(const U& lhs, const i_optional<T>& rhs)
    {
        if (!rhs.has_value())
            return false;
        return lhs == rhs.get();
    }

    template <typename T, typename U, typename = std::enable_if_t<!is_optional_v<U>, sfinae>>
    inline std::partial_ordering operator<=>(const i_optional<T>& lhs, const U& rhs)
    {
        if (!lhs.has_value())
            return std::partial_ordering::less;
        return lhs.get() <=> rhs;
    }

    template <typename T, typename U, typename = std::enable_if_t<!is_optional_v<U>, sfinae>>
    inline std::partial_ordering operator<=>(const U& lhs, const i_optional<T>& rhs)
    {
        if (!rhs.has_value())
            return std::partial_ordering::greater;
        return lhs <=> rhs.get();
    }
}
