#pragma once
#include <memory/Alloc.h>
#include "ComponentTypes.h"

template<typename Component>
class DenseBackend {
public:
	typedef pVector<Component> StorageBackend;
	DenseBackend();

	void ConstructComponent( const Entity entity );
	void DestructComponent( const Entity entity );

	Component& GetComponent( const Entity entity );
	const Component& GetComponent( const Entity entity ) const;
	StorageBackend& GetStorageBackend( );
	const StorageBackend& GetStorageBackend( ) const;
private:
	StorageBackend m_Components;
};
