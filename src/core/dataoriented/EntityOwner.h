#pragma once

#include <bitset>
#include "ComponentTypes.h"

class EntityManager;

class EntityOwner {
public:
	~EntityOwner();
	void Initialize( EntityManager* entityManager );
	void Deinitialize();
	Entity AddEntity( Entity entity );
	void KillAllOwnedEntities( );

private:
	void OnEntityRemoved( Entity entity );

	ComponentMask m_OwnedEntities;
	EntityDestructionCallbackRegister::Handle m_EntityDestructionCallbackHandle;

	EntityManager* m_EntityManager = nullptr;
};
