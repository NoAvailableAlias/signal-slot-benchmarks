#pragma once

#include "function_detail.h"

namespace is::signals
{

template <class Signature>
class function;

// Compact function class - causes minimal code bloat when compiled.
// Replaces std::function in this library.
template <class Return, class... Arguments>
class function<Return(Arguments...)>
{
public:
	function() = default;

	template <class Fn>
	function(Fn&& function)
	{
		m_packed.init<Fn, Return, Arguments...>(std::forward<Fn>(function));
	}

	Return operator()(Arguments&&... args) const
	{
		auto& proxy = m_packed.get<Return(Arguments...)>();
		return proxy(std::forward<Arguments>(args)...);
	}

	detail::packed_function pack()
	{
		return std::move(m_packed);
	}

private:
	detail::packed_function m_packed;
};

} // namespace is::signals
