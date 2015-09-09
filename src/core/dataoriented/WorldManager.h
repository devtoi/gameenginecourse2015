#pragma once

#include <memory/Alloc.h>
#include "BaseCollection.h"
#include "World.h"

#define g_WorldManager WorldManager::GetInstance()

class WorldManager {
public:
	static WorldManager& GetInstance();

	World& CreateWorld( const pVector<BaseCollection*>& collections );

	World& GetSimWorld( );

private:
	World m_SimWorld;
};
