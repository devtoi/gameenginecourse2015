#pragma once

#include <memory/Alloc.h>
#include <array>
#include <queue>
#include <bitset>
#include <functional>

#include "BaseCollection.h"
#include "EntityTypeDefinitions.h"
#include "ComponentTypes.h"

#define ENTITY_MANAGER_NR_OF_BITS_TO_PRINT		   24

class EntityManager {
public:
	EntityManager();
	~EntityManager();
	EntityManager( const EntityManager& ) = delete;
	EntityManager& operator=( const EntityManager& ) = delete;

	void	Reset( );

	void 	SetCollectionVector ( rVector<BaseCollection*>& collections );

	void 	RemoveComponent		( Entity entity, ComponentTypeIndex componentTypeIndex );
	void 	AddComponent		( Entity entity, ComponentTypeIndex componentTypeIndex );
	Entity 	StartCreateEntity	( );
	void 	FinishCreateEntity	( Entity entity, EntityTypeHandle entityTypeHandle = EntityTypeHandle::invalid(), const void* const data = nullptr );
	bool 	RemoveEntity 		( Entity entity );

	void 	PrintMasks			( );
	
	SpecificEntityCreatedCallbackRegister::Handle 	RegisterSpecificEntityCreatedCallback( EntityTypeHandle entityTypeHandle, const SpecificEntityCreatedCallbackRegister::CallbackFunction callbackFunction );
	bool 											UnregisterSpecificEntityCreatedCallback( EntityTypeHandle entityTypeHandle, SpecificEntityCreatedCallbackRegister::Handle& handle );

	EntityCreationCallbackRegister::Handle 	RegisterEntityCreationCallback( const EntityCreationCallbackRegister::CallbackFunction callbackFunction );
	bool 									UnregisterEntityCreationCallback( EntityCreationCallbackRegister::Handle& handle );

	EntityDestructionCallbackRegister::Handle 	RegisterEntityDestructionCallback( const EntityDestructionCallbackRegister::CallbackFunction callbackFunction );
	bool 										UnregisterEntityDestructionCallback( EntityDestructionCallbackRegister::Handle& handle );

	bool EntityMatchesMask( const Entity entity, const EntityMask mask ) const;

	const rVector<EntityMask>& GetEntityMasks() const;
	EntityMask GetEntityMask( Entity entity ) const;
	const ComponentMask& GetActiveEntitySlotsMask( ) const;
	Entity GetLastActiveEntityPlusOne( ) const;

	const rVector<BaseCollection*>* GetComponentCollections( ) const;

private:
	Entity	FindLastActiveEntity( ) const;
	Entity	GetFirstFree( ) const;
	void	CallEntityDestructionCallbacks( Entity entity, const void* const data = nullptr );

	rVector<BaseCollection*>* 	m_Collections = nullptr;
	rVector<EntityMask> 		m_EntityMasks;
	ComponentMask				m_ActiveEntitySlotsMask;
	Entity m_MaxEntityValue = -1;
	pUnorderedMap<
		EntityTypeHandle,
		SpecificEntityCreatedCallbackRegister,
		EntityTypeHandleHasher
	> m_SpecificEntityCreatedCallbacks;
	EntityCreationCallbackRegister m_EntityCreationCallbacks;
	EntityDestructionCallbackRegister m_EntityDestructionCallbacks;
};
