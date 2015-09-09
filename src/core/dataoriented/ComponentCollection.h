#pragma once

#include "BaseCollection.h"
#include "ComponentTypes.h"
#include "WorldManager.h"

template<typename Component>
class ComponentCollection : public BaseCollection {
public:
	typedef typename Component::Backend::StorageBackend StorageBackend;

	ComponentCollection();

	#pragma warning( disable : 4373 )
	void ConstructComponent( Entity entity ) override;

	void DestructComponent( Entity entity ) override;

	Component& GetComponent( Entity entity );
	const Component& GetComponent( Entity entity ) const;

	StorageBackend& GetStorageBackend();
	const StorageBackend& GetStorageBackend() const;
	StorageBackend& GetComponents(); // TODOJM: Remove when no one calls this

	// Sets the component type index for this system. Don't call this!
	void SetComponentTypeIndex( ComponentTypeIndex componentTypeIndex ) override;
private:
	typename Component::Backend m_Backend;
};

