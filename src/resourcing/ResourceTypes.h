#pragma once

#include <stdint.h>

typedef uint64_t ResourceIdentifier;

enum class ResourceLifetimeHint {
	Permanent,
	PerLevel,
	Temporary,
	Frame,
};
