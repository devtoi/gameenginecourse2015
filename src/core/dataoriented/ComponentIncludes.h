#pragma once
#include "DenseBackend.h"
#include "SparseBackend.h"
#include "ComponentCollection.h"
#include "ComponentTypes.h"
#include "EntityManager.h"

#include "WorldManager.h"

#define g_EntityManager g_WorldManager.GetSimWorld().GetEditableEntityManager() // TODOJM: Remove when all calls are gone
