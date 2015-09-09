#pragma once
#include <memory/Alloc.h>
#include "ComponentTypes.h"

template<typename Component>
using SparseComponentMap = typename Component::Backend::StorageBackend;

template<typename Component>
class SparseBackend {
public:
	typedef pUnorderedMap<Entity, Component> StorageBackend;
	SparseBackend();

	void ConstructComponent( const Entity entity );
	void DestructComponent( const Entity entity );

	Component& GetComponent( const Entity entity );
	const Component& GetComponent( const Entity entity ) const;
	StorageBackend& GetStorageBackend( );
	const StorageBackend& GetStorageBackend( ) const;
private:
	StorageBackend m_Components;
};
