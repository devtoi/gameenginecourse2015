#include "BaseCollection.h"
#include <utility/Logger.h>

BaseCollection::BaseCollection( const char* componentName )
	: m_ComponentName( componentName ),
	m_WriteLock( m_AccessMutex, std::defer_lock ),
	m_ReadLock( m_AccessMutex, std::defer_lock ) {
}

void BaseCollection::ConstructComponent( const Entity entity ) {
	m_EntitiesWithComponentMask.set( static_cast<size_t>( entity ), true );

	// Call any registered creation callbacks
	m_CreationCallbacks.TriggerCallbacks( entity );
}

void BaseCollection::DestructComponent( const Entity entity ) {
	m_EntitiesWithComponentMask.set( static_cast<size_t>( entity ), false );
}

void BaseCollection::TriggerDestructionCallbacks( const Entity entity ) {
	// Call any registered destruction callback
	m_DestructionCallbacks.TriggerCallbacks( entity );
}

ComponentCreationCallbackRegister::Handle BaseCollection::RegisterCreationCallback( const ComponentCreationCallbackRegister::CallbackFunction callbackFunction ) {
	return m_CreationCallbacks.RegisterCallback( callbackFunction );
}

ComponentDestructionCallbackRegister::Handle BaseCollection::RegisterDestructionCallback( const ComponentDestructionCallbackRegister::CallbackFunction callbackFunction ) {
	return m_DestructionCallbacks.RegisterCallback( callbackFunction );
}

EntityWithComponentCreationCallbackRegister::Handle BaseCollection::RegisterEntityWithComponentCreationCallback( const EntityWithComponentCreationCallbackRegister::CallbackFunction callbackFunction ) {
	return m_EntityCreationCallbacks.RegisterCallback( callbackFunction );
}

bool BaseCollection::UnregisterCreationCallback( ComponentCreationCallbackRegister::Handle& callbackHandle ) {
	if ( m_CreationCallbacks.UnregisterCallback( callbackHandle, true ) ) {
		return true;
	} else {
		Logger::Log( "Failed to unregister component creation callback for " + m_ComponentName + " since the inputed handle not found (Handle value = " +
			rToString( static_cast<int>( callbackHandle ) ) + " )", "BaseCollection", LogSeverity::WARNING_MSG );
		return false;
	}
}

bool BaseCollection::UnregisterDestructionCallback( ComponentDestructionCallbackRegister::Handle& callbackHandle ) {
	if ( m_DestructionCallbacks.UnregisterCallback( callbackHandle, true ) ) {
		return true;
	} else {
		Logger::Log( "Failed to unregister component destruction callback for " + m_ComponentName + " since the inputed handle not found (Handle value = " +
			rToString( static_cast<int>( callbackHandle ) ) + " )", "BaseCollection", LogSeverity::WARNING_MSG );
		return false;
	}
}

bool BaseCollection::UnregisterEntityWithComponentCreationCallback( EntityWithComponentCreationCallbackRegister::Handle& callbackHandle ) {
	if ( m_EntityCreationCallbacks.UnregisterCallback( callbackHandle, true ) ) {
		return true;
	} else {
		Logger::Log( "Failed to unregister entity with component creation callback for " + m_ComponentName + " since the inputed handle not found (Handle value = " +
			rToString( static_cast<int>( callbackHandle ) ) + " )", "BaseCollection", LogSeverity::WARNING_MSG );
		return false;
	}
}

void BaseCollection::TriggerEntityCreationCallback( const Entity entity ) {
	m_EntityCreationCallbacks.TriggerCallbacks( entity );
}

ComponentTypeIndex BaseCollection::GetComponentTypeIndex( ) const {
	assert( m_ComponentTypeIndex != -1 );
	return m_ComponentTypeIndex;
}

EntityMask BaseCollection::GetComponentTypeFlag( ) const {
	return ( 1ULL << GetComponentTypeIndex( ) );
}

const ComponentMask& BaseCollection::GetEntitiesWithComponentMask( ) const {
	return m_EntitiesWithComponentMask;
}

const pString& BaseCollection::GetComponentName( ) const {
	return m_ComponentName;
}

std::shared_lock<std::shared_timed_mutex>& BaseCollection::GetReadLock( ) const {
	return m_ReadLock;
}

std::unique_lock<std::shared_timed_mutex>& BaseCollection::GetWriteLock( ) {
	return m_WriteLock;
}
