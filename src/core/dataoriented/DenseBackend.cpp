#include "DenseBackend.h"
#include <cassert>

template<typename Component>
DenseBackend<Component>::DenseBackend() {
	m_Components.resize( MAX_NR_OF_ENTITIES );
}

template<typename Component>
void DenseBackend<Component>::ConstructComponent( const Entity entity ) {
	m_Components.at( entity ) = Component();
}

template<typename Component>
void DenseBackend<Component>::DestructComponent( const Entity entity ) {
	m_Components.at( entity ) = Component();
}

template<typename Component>
Component& DenseBackend<Component>::GetComponent( const Entity entity ) {
	return m_Components.at( entity );
}

template<typename Component>
const Component& DenseBackend<Component>::GetComponent( const Entity entity ) const {
	return m_Components.at( entity );
}

template<typename Component>
typename DenseBackend<Component>::StorageBackend& DenseBackend<Component>::GetStorageBackend( ) {
	return m_Components;
}

template<typename Component>
const typename DenseBackend<Component>::StorageBackend& DenseBackend<Component>::GetStorageBackend( ) const {
	return m_Components;
}
