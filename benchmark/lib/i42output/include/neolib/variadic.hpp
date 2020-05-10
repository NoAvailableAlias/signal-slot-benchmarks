/*
 *  variadic.hpp
 *
 *  PUBLIC DOMAIN
 *
 *  THIS SOURCE FILE IS PROVIDED BY THE CONTRIBUTORS "AS
 *  IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO,
 *  THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR
 *  PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE 
 *  CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL,
 *  EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO,
 *  PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR
 *  PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF
 *  LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING
 *  NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS
 *  SOURCE FILE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 */

#pragma once

#include <neolib/neolib.hpp>
#include <type_traits>

namespace neolib
{
    namespace variadic
    {
        template <typename...>
        struct index;

        // found it
        template <typename T, typename... R>
        struct index<T, T, R...> : std::integral_constant<size_t, 0u> {};

        // still looking
        template <typename T, typename F, typename... R>
        struct index<T, F, R...> : std::integral_constant<size_t, 1u + index<T, R...>::value> {};

        template <typename T, typename... R>
        constexpr size_t index_v = index<T, R...>::value;

        template <typename T, typename... R>
        constexpr size_t no_reference_index_v = index<std::remove_reference_t<T>, R...>::value;
    }
}
