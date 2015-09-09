#include "EntityTemplateManager.h"
#include <utility/Logger.h>
#include <cassert>

EntityTemplateManager& EntityTemplateManager::GetInstance() {
	static EntityTemplateManager etm;

	return etm;
}

EntityTypeHandle EntityTemplateManager::RegisterType( const pString& name ) {
	auto existingName = m_EntityNameToType.find( name );

	if ( existingName != m_EntityNameToType.end() ) {
		Logger::Log( "Tried to create already existing entity type with name \"" + name + "\" and current handle is " + rToString(
				static_cast<int>( existingName->second ) ), "EntityTemplateManager", LogSeverity::WARNING_MSG );
		return EntityTypeHandle::invalid();
	}
	EntityTypeHandle handle = static_cast<EntityTypeHandle>( m_HandleIncrementor++ );
	assert( m_EntityTypeToName.find( handle ) == m_EntityTypeToName.end() );
	m_EntityTypeToName.emplace( handle, name );
	m_EntityNameToType.emplace( name, handle );

	return handle;
}

void EntityTemplateManager::UnregisterTypeWithHandle( EntityTypeHandle handle ) {
	auto existingType = m_EntityTypeToName.find( handle );

	if ( existingType == m_EntityTypeToName.end() ) {
		Logger::Log( "Tried to unregister entity type with non existant handle " + rToString( static_cast<int>( handle ) ), "EntityTemplateManager", LogSeverity::WARNING_MSG );
		return;
	}
	auto existingName = m_EntityNameToType.find( existingType->second );
	assert( existingName == m_EntityNameToType.end() );
	m_EntityTypeToName.erase( existingType );
	m_EntityNameToType.erase( existingName );
}

void EntityTemplateManager::UnregisterTypeWithName( const pString& name ) {
	auto existingName = m_EntityNameToType.find( name );

	if ( existingName == m_EntityNameToType.end() ) {
		Logger::Log( "Tried to unregister entity with non existant name \"" + name + "\"", "EntityTemplateManager", LogSeverity::WARNING_MSG );
	}
	auto existingType = m_EntityTypeToName.find( existingName->second );
	assert( existingType == m_EntityTypeToName.end() );
	m_EntityNameToType.erase( existingName );
	m_EntityTypeToName.erase( existingType );
}

EntityTypeHandle EntityTemplateManager::GetHandleFromName( const pString& name ) const {
	auto existingName = m_EntityNameToType.find( name );

	if ( existingName == m_EntityNameToType.end() ) {
		Logger::Log( "Tried to get handle for non existant entity type name \"" + name + "\"", "EntityTemplateManager", LogSeverity::WARNING_MSG );
		return EntityTypeHandle::invalid();
	}
	return existingName->second;
}

const pString& EntityTemplateManager::GetNameFromHandle( EntityTypeHandle handle ) const {
	auto existingType = m_EntityTypeToName.find( handle );

	if ( existingType == m_EntityTypeToName.end() ) {
		Logger::Log( "Tried to get entity type name with invalid handle " + rToString( static_cast<int>( handle ) ), "EntityTemplateManager", LogSeverity::WARNING_MSG );
		return m_InvalidName;
	}
	return existingType->second;
}

