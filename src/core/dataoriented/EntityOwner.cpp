#include "EntityOwner.h"
#include <utility/Logger.h>
#include "EntityManager.h"

EntityOwner::~EntityOwner() {
	if ( m_OwnedEntities.any() ) {
		Logger::Log( "Entity owner is being destructed while still owning entities. Please kill these entities or migrate ownership.", "EntityOwner", LogSeverity::WARNING_MSG );
	}
}

void EntityOwner::Initialize( EntityManager* entityManager ) {
	m_EntityManager = entityManager;
	m_EntityDestructionCallbackHandle = m_EntityManager->RegisterEntityDestructionCallback( 
			std::bind( &EntityOwner::OnEntityRemoved, this, std::placeholders::_1 ) );
}

void EntityOwner::Deinitialize() {
	m_EntityManager->UnregisterEntityDestructionCallback( m_EntityDestructionCallbackHandle );
}

Entity EntityOwner::AddEntity( Entity entity ) {
	if ( m_OwnedEntities.test( entity ) ) {
		Logger::Log( "Tried to add entity that is already owned by the entity owner", "EntityOwner", LogSeverity::WARNING_MSG );
	} else {
		m_OwnedEntities.set( entity, true );
	}
	return entity;
}

void EntityOwner::KillAllOwnedEntities() {
	for ( Entity entity = 0; entity < m_EntityManager->GetLastActiveEntityPlusOne(); ++entity ) {
		if ( m_OwnedEntities.test( entity ) ) {
			m_EntityManager->RemoveEntity( entity );
		}
	}
}

void EntityOwner::OnEntityRemoved( Entity entity ) {
	m_OwnedEntities.set( entity, false );
}
