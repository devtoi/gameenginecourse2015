#include "WorldManager.h"

WorldManager& WorldManager::GetInstance() {
	static WorldManager worldManager;
	return worldManager;
}

World& WorldManager::CreateWorld( const pVector<BaseCollection*>& collections ) {
	m_SimWorld.Initialize( collections );
	return m_SimWorld;
}

World& WorldManager::GetSimWorld() {
	return m_SimWorld;
}
