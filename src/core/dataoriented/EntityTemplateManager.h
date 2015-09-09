#pragma once

#include <memory/Alloc.h>
#include "EntityTypeDefinitions.h"

#define g_EntityTemplateManager EntityTemplateManager::GetInstance()

class EntityTemplateManager {
public:
	static EntityTemplateManager& GetInstance ();
	EntityTypeHandle			  RegisterType ( const pString& name );
	void						  UnregisterTypeWithHandle ( EntityTypeHandle handle );
	void						  UnregisterTypeWithName ( const pString& name );
	EntityTypeHandle			  GetHandleFromName ( const pString& name ) const;
	const pString&				  GetNameFromHandle ( EntityTypeHandle handle ) const;

private:
	EntityTemplateManager() { }

	~EntityTemplateManager() { }

	EntityTemplateManager( const EntityTemplateManager& rhs );
	EntityTemplateManager& operator = ( const EntityTemplateManager& rhs );

	pUnorderedMap<EntityTypeHandle, pString, EntityTypeHandleHasher> m_EntityTypeToName;
	rMap<pString, EntityTypeHandle> m_EntityNameToType;

	int m_HandleIncrementor = 0;

	const pString m_InvalidName = "";
};

