#pragma once

#include <memory/Alloc.h>
#include "EntityManager.h"

class BaseCollection;
class Subsystem;

class World {
public:
	enum class UsageHint;

	World() = default;
	World( const World& ) = delete;
	World& operator=( const World& ) = delete;

	~World();

	void Initialize( const pVector<BaseCollection*>& collections );

	const BaseCollection* RegisterReadDependency( const Subsystem* subsystem, const pString& componentName );
	BaseCollection* RegisterWriteDependency( const Subsystem* subsystem, const pString& componentName );
	void 			UnregisterDependency( const Subsystem* subsystem, const pString& componentName );
	BaseCollection* GetCollectionWithID( const int id );

	const EntityManager& GetEntityManager( ) const;
	EntityManager& GetEditableEntityManager( );

	enum class UsageHint {
		Read,
		Write
	};

	struct CollectionDependencyInfo {
		World::UsageHint UsageHint;
	};

private:
	const BaseCollection* RegisterDependency( const Subsystem* subsystem, const pString& componentName, UsageHint usageHint );

	pVector<BaseCollection*>	   m_ComponentCollections;
	pMap<pString, BaseCollection*> m_NameToCollectionMap;
	pMap<const Subsystem*, pUnorderedMap<const BaseCollection*, CollectionDependencyInfo>> m_CollectionDependencies;
	EntityManager m_EntityManager;
};
