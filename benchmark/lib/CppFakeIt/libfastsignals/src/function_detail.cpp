#include "../include/function_detail.h"
#include <cstddef>
#include <functional>

namespace is::signals::detail
{

packed_function::packed_function(packed_function&& other)
{
	if (other.is_buffer_allocated())
	{
		m_proxy = other.m_proxy->clone(&m_buffer);
	}
	else
	{
		m_proxy = other.m_proxy;
		other.m_proxy = nullptr;
	}
}

packed_function::packed_function(const packed_function& other)
{
	m_proxy = other.m_proxy->clone(&m_buffer);
}

packed_function& packed_function::operator=(packed_function&& other)
{
	if (other.is_buffer_allocated())
	{
		// There are no strong exception safety since we cannot allocate
		//  new object on buffer and than reset same buffer.
		reset();
		m_proxy = other.m_proxy->clone(&m_buffer);
	}
	else
	{
		reset();
		std::swap(m_proxy, other.m_proxy);
	}
	return *this;
}

packed_function& packed_function::operator=(const packed_function& other)
{
	auto* proxy = other.m_proxy->clone(&m_buffer);
	reset();
	m_proxy = proxy;
	return *this;
}

packed_function::~packed_function() noexcept
{
	reset();
}

void packed_function::reset() noexcept
{
	if (m_proxy != nullptr)
	{
		if (is_buffer_allocated())
		{
			m_proxy->~base_function_proxy();
		}
		else
		{
			delete m_proxy;
		}
		m_proxy = nullptr;
	}
}

base_function_proxy& packed_function::unwrap() const
{
	if (m_proxy == nullptr)
	{
		throw std::bad_function_call();
	}
	return *m_proxy;
}

bool packed_function::is_buffer_allocated() const noexcept
{
	const std::byte* bufferStart = reinterpret_cast<const std::byte*>(&m_buffer);
	const std::byte* bufferEnd = reinterpret_cast<const std::byte*>(&m_buffer + 1);
	const std::byte* proxy = reinterpret_cast<const std::byte*>(m_proxy);
	return (proxy >= bufferStart && proxy < bufferEnd);
}

} // namespace is::signals::detail
