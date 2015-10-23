#pragma once
#include <cstddef>

class Resource {
public:
	Resource( size_t size );

	size_t GetSize() const;

private:
	size_t m_Size = 0;
};
