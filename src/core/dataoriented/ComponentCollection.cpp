#include "ComponentCollection.h"

template <typename Component>
ComponentCollection<Component>::ComponentCollection()
	: BaseCollection( Component::ComponentName ) {
}

template <typename Component>
void ComponentCollection<Component>::ConstructComponent( const Entity entity ) {
	GetWriteLock().lock();
	assert( entity < MAX_NR_OF_ENTITIES ); // Make sure we don't create more entities that what is supported
	assert( !m_EntitiesWithComponentMask.test( entity ) ); // Make sure we don't recreate a component

	m_Backend.ConstructComponent( entity );

	BaseCollection::ConstructComponent( entity );

	GetWriteLock().unlock();
}

template <typename Component>
void ComponentCollection<Component>::DestructComponent( const Entity entity ) {
	GetWriteLock().lock();
	assert( entity < MAX_NR_OF_ENTITIES );
	assert( m_EntitiesWithComponentMask.test( entity ) ); // Make sure we don't try to destruct a "non existant" a component
	m_Backend.DestructComponent( entity );
	BaseCollection::DestructComponent( entity );
	GetWriteLock().unlock();
}

template <typename Component>
Component& ComponentCollection<Component>::GetComponent( const Entity entity ) {
	assert( entity < MAX_NR_OF_ENTITIES );
	assert( m_EntitiesWithComponentMask.test( entity ) ); // Make sure we don't try to get a destructed component
	return m_Backend.GetComponent( entity );
}

template <typename Component>
const Component& ComponentCollection<Component>::GetComponent( const Entity entity ) const {
	assert( entity < MAX_NR_OF_ENTITIES );
	assert( m_EntitiesWithComponentMask.test( entity ) ); // Make sure we don't try to get a destructed component
	return m_Backend.GetComponent( entity );
}

template <typename Component>
typename ComponentCollection<Component>::StorageBackend& ComponentCollection<Component>::GetStorageBackend() {
	return m_Backend.GetStorageBackend();
}

template <typename Component>
const typename ComponentCollection<Component>::StorageBackend& ComponentCollection<Component>::GetStorageBackend() const {
	return m_Backend.GetStorageBackend();
}

template <typename Component>
typename ComponentCollection<Component>::StorageBackend& ComponentCollection<Component>::GetComponents() {
	return m_Backend.GetStorageBackend();
}

template <typename Component>
void ComponentCollection<Component>::SetComponentTypeIndex( ComponentTypeIndex componentTypeIndex ) {
	m_ComponentTypeIndex = componentTypeIndex;
	Component::ComponentID = componentTypeIndex;
}
