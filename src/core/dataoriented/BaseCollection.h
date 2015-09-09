#pragma once

#include <cassert>
#include <mutex>
#include <shared_mutex>
#include <functional>
#include <memory/Alloc.h>
#include "ComponentTypes.h"
#include <utility/ThreadingUtility.h>

class BaseCollection {
public:
	BaseCollection( const char* componentName );
	virtual ~BaseCollection() = default;
	// This will construct a component. It will not mark it as active for the entity manager.
	virtual void ConstructComponent( const Entity entity );
	// This will destruct a component. It will not mark it as deactivated for the entity manager.
	virtual void DestructComponent( const Entity entity );
	void TriggerDestructionCallbacks( const Entity entity );

	// Registers a callback function that will be called when a component is created or destroyed
	ComponentCreationCallbackRegister::Handle RegisterCreationCallback( const ComponentCreationCallbackRegister::CallbackFunction callbackFunction );
	ComponentDestructionCallbackRegister::Handle RegisterDestructionCallback( const ComponentDestructionCallbackRegister::CallbackFunction callbackFunction );
	// Registers a callback function for when an entity with this component has been created.
	EntityWithComponentCreationCallbackRegister::Handle RegisterEntityWithComponentCreationCallback( const EntityWithComponentCreationCallbackRegister::CallbackFunction callbackFunction );
	// Unregisters a creation callback, also invalidates the input handle
	bool UnregisterCreationCallback( ComponentCreationCallbackRegister::Handle& callbackHandle );
	// Unregisters a destruction callback, also invalidates the input handle
	bool UnregisterDestructionCallback( ComponentDestructionCallbackRegister::Handle& callbackHandle );
	// Unregisters a entity with component creation callback, also invalidates the input handle
	bool UnregisterEntityWithComponentCreationCallback( EntityWithComponentCreationCallbackRegister::Handle& callbackHandle );

	void TriggerEntityCreationCallback( const Entity entity );
	
	// Sets the component type index for this system. Don't call this!
	virtual void SetComponentTypeIndex( ComponentTypeIndex componentTypeIndex ) = 0;
	// Gets the unique index assigned to this collection
	ComponentTypeIndex GetComponentTypeIndex( ) const;

	// Gets a flag marking this systems index, used for bitmasks.
	EntityMask GetComponentTypeFlag( ) const;

	const ComponentMask& GetEntitiesWithComponentMask( ) const;

	virtual const pString& GetComponentName( ) const;

	std::shared_lock<std::shared_timed_mutex>& GetReadLock( ) const;
	std::unique_lock<std::shared_timed_mutex>& GetWriteLock( );

protected:
	ComponentMask m_EntitiesWithComponentMask;
	ComponentTypeIndex m_ComponentTypeIndex = -1;

private:
	pString m_ComponentName					= "Unnamed";
	ComponentCreationCallbackRegister			m_CreationCallbacks;
	ComponentDestructionCallbackRegister		m_DestructionCallbacks;
	EntityWithComponentCreationCallbackRegister m_EntityCreationCallbacks;

	std::shared_timed_mutex m_AccessMutex;
	std::unique_lock<std::shared_timed_mutex> m_WriteLock;
	mutable std::shared_lock<std::shared_timed_mutex> m_ReadLock;
};
