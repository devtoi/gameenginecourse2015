#pragma once
#include <cstddef>

class Resource {
public:
	Resource( );
	void SetSize(size_t size);
	size_t GetSize() const;
	void SetReady();
	bool IsReady() const;
private:
	size_t m_Size = 0;
	bool m_Ready = false;
};
