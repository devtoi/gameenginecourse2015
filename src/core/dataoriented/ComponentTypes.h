#pragma once

#include <cstdint>
#include <limits>
#include <bitset>
#include <functional>
#include <utility/CallbackRegister.h>

#define ENTITY_INVALID -1

typedef int Entity;
typedef uint64_t EntityMask;
typedef short ComponentTypeIndex;

#define MAX_NR_OF_ENTITIES 2048

// Flags for a component type what entity has a component of this type. Also used for marking active entities.
typedef std::bitset<MAX_NR_OF_ENTITIES> ComponentMask;

// Types for callback when a component is created
struct ComponentCreationCallbackHandle_tag {};
typedef CallbackRegister<ComponentCreationCallbackHandle_tag, void, Entity> ComponentCreationCallbackRegister;

// Types for callback when a component is destroyed
struct ComponentDestructionCallbackHandle_tag {};
typedef CallbackRegister<ComponentDestructionCallbackHandle_tag, void, Entity> ComponentDestructionCallbackRegister;

// Types for callback when an entity that has a specific component is created
struct EntityWithComponentCreationCallbackHandle_tag {};
typedef CallbackRegister<EntityWithComponentCreationCallbackHandle_tag, void, Entity> EntityWithComponentCreationCallbackRegister;

// Types for callback when a specific entity is created
struct SpecificEntityCreatedCallbackRegister_tag { };
typedef CallbackRegister<SpecificEntityCreatedCallbackRegister_tag, void, Entity, const void* const> SpecificEntityCreatedCallbackRegister;

// Types for callback when an entity is created
struct EntityCreationCallbackRegister_tag { };
typedef CallbackRegister<EntityCreationCallbackRegister_tag, void, Entity> EntityCreationCallbackRegister;

// Types for callback when an entity is destroyed
struct EntityDestructionCallbackRegister_tag { };
typedef CallbackRegister<EntityDestructionCallbackRegister_tag, void, Entity> EntityDestructionCallbackRegister;
