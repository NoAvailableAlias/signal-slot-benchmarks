#pragma once

#include "signal_impl.h"

namespace is::signals
{

// Connection keeps link between signal and slot and can disconnect them.
// Disconnect operation is thread-safe: any thread can disconnect while
//  slots called on other thread.
// This class itself is not thread-safe: you can't use the same connection
//  object from different threads at the same time.
class connection
{
public:
	connection();
	explicit connection(detail::signal_impl_weak_ptr storage, uint64_t id);
	connection(const connection& other);
	connection& operator=(const connection& other);
	connection(connection&& other) noexcept;
	connection& operator=(connection&& other) noexcept;

	bool connected() const noexcept;
	void disconnect();

protected:
	detail::signal_impl_weak_ptr m_storage;
	uint64_t m_id = 0;
};

// Scoped connection keeps link between signal and slot and disconnects them in destructor.
// Scoped connection is movable, but not copyable.
class scoped_connection : public connection
{
public:
	scoped_connection();
	scoped_connection(const connection& conn);
	scoped_connection(connection&& conn) noexcept;
	scoped_connection(const scoped_connection&) = delete;
	scoped_connection& operator=(const scoped_connection&) = delete;
	scoped_connection(scoped_connection&& other) noexcept;
	scoped_connection& operator=(scoped_connection&& other) noexcept;
	~scoped_connection();

	connection release();
};

} // namespace is::signals
