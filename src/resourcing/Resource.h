#pragma once
#include <cstddef>
#include <vector>
#include "ResourceTypes.h"

class Resource {
public:
	Resource( );
	~Resource( );
	void SetSize(size_t size);
	size_t GetSize() const;
	void SetReady();
	bool IsReady() const;
	void AddDependency( const ResourceIdentifier identifier );
	void ReleaseDependencies( );
private:
	size_t m_Size = 0;
	bool m_Ready = false;
	std::vector<ResourceIdentifier> m_Dependencies;
};
