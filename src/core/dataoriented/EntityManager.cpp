#include "EntityManager.h"

#include <cassert>
#include <bitset>
#include <utility/Logger.h>
#include "EntityTemplateManager.h"

EntityManager::EntityManager() {
	m_EntityMasks.resize( MAX_NR_OF_ENTITIES );
}

EntityManager::~EntityManager() {
}

void EntityManager::Reset( ) {
	m_MaxEntityValue = -1;
	m_ActiveEntitySlotsMask = ComponentMask( );
}

void EntityManager::SetCollectionVector( rVector<BaseCollection*>& collections ) {
	m_Collections = &collections;
}

void EntityManager::RemoveComponent( Entity entity, ComponentTypeIndex componentTypeIndex ) {
	assert(componentTypeIndex < m_Collections->size());
	assert(entity < MAX_NR_OF_ENTITIES);
	// Remove component from the component types collection
	m_Collections->at( componentTypeIndex )->DestructComponent( entity );
	// Turn flag off that marks this component type
	m_EntityMasks.at( entity ) &= ~(1 << componentTypeIndex);
}

void EntityManager::AddComponent( Entity entity, ComponentTypeIndex componentTypeIndex ) {
	assert(componentTypeIndex < m_Collections->size());
	assert(entity < MAX_NR_OF_ENTITIES);
	// Add component to the component types collection
	m_Collections->at( componentTypeIndex )->ConstructComponent( entity );
	// Turn flag on that marks this component type
	m_EntityMasks.at( entity ) |= 1ULL << componentTypeIndex;
}

Entity EntityManager::StartCreateEntity() {
	// Get first element from the queue
	Entity newEntity = GetFirstFree( );
	m_MaxEntityValue = std::max( m_MaxEntityValue, newEntity );
	// Mark entity to have no components as default
	m_EntityMasks.at( newEntity ) = 0;
	// Entity slot is now active
	m_ActiveEntitySlotsMask.set( static_cast<size_t>( newEntity ), true );

	return newEntity;
}

void EntityManager::FinishCreateEntity( Entity entity, EntityTypeHandle entityTypeHandle, const void* const data ) {
	EntityMask mask = m_EntityMasks.at( entity );
	for ( unsigned long i = 0; i < sizeof(EntityMask) * 8; ++i ) {
		if ( mask & (1ULL << i) ) {
			m_Collections->at( i )->TriggerEntityCreationCallback( entity );
		}
	}
	auto specificCallback = m_SpecificEntityCreatedCallbacks.find( entityTypeHandle );
	if ( specificCallback != m_SpecificEntityCreatedCallbacks.end( ) ) {
		specificCallback->second.TriggerCallbacks( entity, data );
	}
	m_EntityCreationCallbacks.TriggerCallbacks( entity );
}

bool EntityManager::RemoveEntity( Entity entity ) {
	assert( entity < MAX_NR_OF_ENTITIES );
	if ( m_ActiveEntitySlotsMask.test( entity ) ) {
		m_EntityDestructionCallbacks.TriggerCallbacks( entity );
		EntityMask mask = m_EntityMasks.at(entity);
		// Loop through each flag in the component mask
		for ( unsigned long i = 0; i < sizeof(EntityMask) * 8; ++i ) {
			// Only remove component if flag is set
			if ( mask & (1ULL << i) ) {
				m_Collections->at( i )->TriggerDestructionCallbacks( entity );
			}
		}
		for ( unsigned long i = 0; i < sizeof(EntityMask) * 8; ++i ) {
			// Only remove component if flag is set
			if ( mask & (1ULL << i) ) {
				m_Collections->at( i )->DestructComponent( entity );
			}
		}
		m_EntityMasks.at(entity) = 0;
		// Entity slot is not longer active
		m_ActiveEntitySlotsMask.set( static_cast<size_t>( entity ), false );
		// Special case if last entity is removed
		if ( m_MaxEntityValue == entity ) {
			m_MaxEntityValue = FindLastActiveEntity( );
		}
		return true;
	} else {
		Logger::Log( "Tried to remove already removed entity " + rToString( entity ), "EntityManager", LogSeverity::WARNING_MSG );
		return false;
	}
}

void EntityManager::PrintMasks() {
	Logger::GetStream() << "Entity masks: " << std::endl;
	for ( Entity i = 0; i < m_MaxEntityValue; ++i ) {
		Logger::GetStream() << i << ": " << std::bitset<ENTITY_MANAGER_NR_OF_BITS_TO_PRINT>(m_EntityMasks[i]) << std::endl;
	}
}

SpecificEntityCreatedCallbackRegister::Handle EntityManager::RegisterSpecificEntityCreatedCallback( EntityTypeHandle entityTypeHandle, const SpecificEntityCreatedCallbackRegister::CallbackFunction callbackFunction ) {
	// Make sure that the map entry exists
	if( m_SpecificEntityCreatedCallbacks.find( entityTypeHandle ) == m_SpecificEntityCreatedCallbacks.end() ) {
		m_SpecificEntityCreatedCallbacks.emplace( std::piecewise_construct, std::forward_as_tuple( entityTypeHandle ), std::forward_as_tuple() );
	}
	
	SpecificEntityCreatedCallbackRegister::Handle callbackHandle = m_SpecificEntityCreatedCallbacks.at( entityTypeHandle ).RegisterCallback( callbackFunction );
	return callbackHandle;
}

bool EntityManager::UnregisterSpecificEntityCreatedCallback( EntityTypeHandle entityTypeHandle, SpecificEntityCreatedCallbackRegister::Handle& callbackHandle ) {
	auto entityTypeExisting = m_SpecificEntityCreatedCallbacks.find( entityTypeHandle );
	if ( entityTypeExisting == m_SpecificEntityCreatedCallbacks.end( ) ) {
		Logger::Log( "Tried to unregister from specific entity type creation when no callbacks exist for the \"" + g_EntityTemplateManager.GetNameFromHandle( entityTypeHandle ) + "\" entity type", "EntityManager", LogSeverity::WARNING_MSG );
		return false;
	}
	if ( entityTypeExisting->second.UnregisterCallback( callbackHandle ) ) {
		return true;
	} else {
		Logger::Log( "Tried to unregister from specific entity type creation with invalid callback handle " + rToString( static_cast<int>( callbackHandle ) ), "EntityManager", LogSeverity::WARNING_MSG );
		return false;
	}
}

EntityCreationCallbackRegister::Handle EntityManager::RegisterEntityCreationCallback( const EntityCreationCallbackRegister::CallbackFunction callbackFunction ) {
	return m_EntityCreationCallbacks.RegisterCallback( callbackFunction );
}

bool EntityManager::UnregisterEntityCreationCallback( EntityCreationCallbackRegister::Handle& handle ) {
	if ( m_EntityCreationCallbacks.UnregisterCallback( handle ) ) {
		return true;
	} else {
		Logger::Log( "Tried to unregister from entity creation with invalid callback handle " + rToString( static_cast<int>( handle ) ), "EntityManager", LogSeverity::WARNING_MSG );
		return false;
	}
}

EntityDestructionCallbackRegister::Handle EntityManager::RegisterEntityDestructionCallback( const EntityDestructionCallbackRegister::CallbackFunction callbackFunction ) {
	return m_EntityDestructionCallbacks.RegisterCallback( callbackFunction );
}

bool EntityManager::UnregisterEntityDestructionCallback( EntityDestructionCallbackRegister::Handle& handle ) {
	if ( m_EntityDestructionCallbacks.UnregisterCallback( handle ) ) {
		return true;
	} else {
		Logger::Log( "Tried to unregister from entity destruction with invalid callback handle " + rToString( static_cast<int>( handle ) ), "EntityManager", LogSeverity::WARNING_MSG );
		return false;
	}
}

bool EntityManager::EntityMatchesMask( const Entity entity, const EntityMask mask ) const {
	return (GetEntityMask( entity ) & mask) != 0;
}

const rVector<EntityMask>& EntityManager::GetEntityMasks() const {
	return m_EntityMasks;
}

EntityMask EntityManager::GetEntityMask( Entity entity ) const {
	assert( entity < m_EntityMasks.size() );
	return m_EntityMasks.at( entity );
}

const ComponentMask& EntityManager::GetActiveEntitySlotsMask( ) const {
	return m_ActiveEntitySlotsMask;
}

Entity EntityManager::GetLastActiveEntityPlusOne( ) const {
	return m_MaxEntityValue + 1;
}

const rVector<BaseCollection*>* EntityManager::GetComponentCollections( ) const {
	return m_Collections;
}

Entity EntityManager::FindLastActiveEntity( ) const {
	for ( Entity i = m_MaxEntityValue; i > 0; --i ) {
		if ( m_ActiveEntitySlotsMask.test( i ) ) {
			return i;
		}
	}
	return ENTITY_INVALID;
}

Entity EntityManager::GetFirstFree( ) const {
	for ( Entity i = 0; i < MAX_NR_OF_ENTITIES; ++i ) {
		if ( !m_ActiveEntitySlotsMask.test( i ) ) {
			return i;
		}
	}
	return ENTITY_INVALID;
}

