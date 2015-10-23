#pragma once

#include <cstddef>
#include <limits>

typedef size_t ResourceIdentifier;
#define RESOURCE_IDENTIFIER_INVALID std::numeric_limits<size_t>::max()

enum class ResourceLifetimeHint {
	Permanent,
	PerLevel,
	Temporary,
	Frame,
};

#define HashResourceName( aString ) std::hash<std::string>()( aString )
