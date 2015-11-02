#pragma once

#include <cstddef>
#include <string>
#include <limits>
#include <shared_mutex>
#include <memory/Alloc.h>
#include "ResourcingLibraryDefine.h"

typedef size_t ResourceIdentifier;
#define RESOURCE_IDENTIFIER_INVALID std::numeric_limits<size_t>::max()

enum class ResourceLifetimeHint {
	Permanent,
	PerLevel,
	Temporary,
	Frame,
};

#define LookupResourceName( identifier ) ResourceNameLookup::GetInstance().Lookup( identifier )

struct ResourceNameLookup {
	static ResourceNameLookup& GetInstance() {
		static ResourceNameLookup rnl;
		return rnl;
	}

	void AddResource( const ResourceIdentifier identifier, const pString& name ) {
		std::unique_lock<std::shared_timed_mutex>( m_ResourceNamesMutex );
		m_ResourceNames.emplace( identifier, name );
	}

	pString Lookup( const ResourceIdentifier identifier ) {
		std::shared_lock<std::shared_timed_mutex>( m_ResourceNamesMutex );
#ifdef RESOURCE_REVERSE_LOOKUP
		auto it = m_ResourceNames.find( identifier );

		if ( it != m_ResourceNames.end() ) {
			return it->second;
		}
#endif
		return "";
	}

	pUnorderedMap<ResourceIdentifier, pString> m_ResourceNames;
	std::shared_timed_mutex m_ResourceNamesMutex;
};

static ResourceIdentifier HashResourceName( const std::string& name ) {
	ResourceIdentifier hashed = std::hash<std::string>()( name );

#ifdef RESOURCE_REVERSE_LOOKUP
	ResourceNameLookup::GetInstance().AddResource( hashed, name );
#endif
	return hashed;
}

