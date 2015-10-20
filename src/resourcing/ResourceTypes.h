#pragma once

#include <cstddef>

typedef size_t ResourceIdentifier;

enum class ResourceLifetimeHint {
	Permanent,
	PerLevel,
	Temporary,
	Frame,
};

#define HashResourceName( aString ) std::hash<std::string>()( aString )
