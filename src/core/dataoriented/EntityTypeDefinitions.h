#pragma once

#include <utility/CallbackRegister.h>
#include "ComponentTypes.h"

struct EntityTypeHandle_tag{ };
typedef Handle<EntityTypeHandle_tag, int, -1>		EntityTypeHandle;
typedef HandleHasher<EntityTypeHandle_tag, int, -1> EntityTypeHandleHasher;
