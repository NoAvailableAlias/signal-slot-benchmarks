#pragma once

#include "combiners.h"
#include "lfs_connection.h"
#include "function.h"
#include "signal_impl.h"
#include "type_traits.h"
#include <type_traits>

//#if defined(_MSC_VER)
//#	include "msvc_autolink.h"
//#endif

namespace is::signals
{
template <class Signature, template <class T> class Combiner = optional_last_value>
class signal;

// Signal allows to fire events to many subscribers (slots).
// In other words, it implements one-to-many relation between event and listeners.
// Signal implements observable object from Observable pattern.
template <class Return, class... Arguments, template <class T> class Combiner>
class signal<Return(Arguments...), Combiner>
{
public:
	using signature_type = Return(signal_arg_t<Arguments>...);
	using slot_type = function<signature_type>;
	using combiner_type = Combiner<Return>;
	using result_type = typename combiner_type::result_type;

	signal()
		: m_slots(std::make_shared<detail::signal_impl>())
	{
	}

	/**
	 * connect(slot) method subscribes slot to signal emission event.
	 * Each time you call signal as functor, all slots are also called with given arguments.
	 * @returns connection - object which manages signal-slot connection lifetime
	 */
	connection connect(slot_type slot)
	{
		const uint64_t id = m_slots->add(slot.pack());
		return connection(m_slots, id);
	}

	/**
	 * disconnect_all_slots() method disconnects all slots from signal emission event.
	 */
	void disconnect_all_slots()
	{
		m_slots->remove_all();
	}

	/**
	 * operator(args...) calls all slots connected to this signal.
	 * Logically, it fires signal emission event.
	 */
	result_type operator()(signal_arg_t<Arguments>... args) const
	{
		return m_slots->invoke<combiner_type, result_type, signature_type, signal_arg_t<Arguments>...>(args...);
	}

private:
	detail::signal_impl_ptr m_slots;
};
} // namespace is::signals
