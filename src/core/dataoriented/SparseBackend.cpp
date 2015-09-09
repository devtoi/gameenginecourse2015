#include "SparseBackend.h"
#include <cassert>

template<typename Component>
SparseBackend<Component>::SparseBackend() {
}

template<typename Component>
void SparseBackend<Component>::ConstructComponent( const Entity entity ) {
	m_Components.emplace( entity, Component() );
}

template<typename Component>
void SparseBackend<Component>::DestructComponent( const Entity entity ) {
	assert( m_Components.find( entity ) != m_Components.end() );
	auto it = m_Components.find( entity );
	m_Components.erase( it );
}

template<typename Component>
Component& SparseBackend<Component>::GetComponent( const Entity entity ) {
	return m_Components.at( entity );
}

template<typename Component>
const Component& SparseBackend<Component>::GetComponent( const Entity entity ) const {
	return m_Components.at( entity );
}

template<typename Component>
typename SparseBackend<Component>::StorageBackend& SparseBackend<Component>::GetStorageBackend( ) {
	return m_Components;
}

template<typename Component>
const typename SparseBackend<Component>::StorageBackend& SparseBackend<Component>::GetStorageBackend( ) const {
	return m_Components;
}
