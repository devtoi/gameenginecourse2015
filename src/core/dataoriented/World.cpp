#include "World.h"
#include "BaseCollection.h"
#include "../subsystem/Subsystem.h"
#include <utility/Logger.h>
#include "EntityManager.h"

World::~World() {
	for ( const auto& subsystem : m_CollectionDependencies ) {
		for ( const auto& collection : subsystem.second ) {
			//Logger::Log( "Dependency for subsystem " + subsystem.first->GetName() + " to collection " + collection.first->GetComponentName() + " still registered. Please unregister it.", "World", LogSeverity::WARNING_MSG );
			//std::cout << "Dependency for subsystem " + subsystem.first->GetName() + " to collection " + collection.first->GetComponentName() + " still registered. Please unregister it." << std::endl; // TODOJM: Replace with above line when world is not singleton
			std::cout << "Dependency for subsystem to collection " + collection.first->GetComponentName() + " still registered. Please unregister it." << std::endl; // TODOJM: Replace with above line when world is not singleton
		}
	}
	for ( const auto& collection : m_ComponentCollections ) {
		pDelete( collection );
	}
}

void World::Initialize( const pVector<BaseCollection*>& collections ) {
	m_ComponentCollections = collections;
	m_NameToCollectionMap.clear();
	for ( auto& collection : m_ComponentCollections ) {
		m_NameToCollectionMap.emplace( collection->GetComponentName(), collection );
	}
	m_EntityManager.SetCollectionVector( m_ComponentCollections );
}

const BaseCollection* World::RegisterReadDependency( const Subsystem* subsystem, const pString& componentName ) {
	return RegisterDependency( subsystem, componentName, UsageHint::Read );
}

BaseCollection* World::RegisterWriteDependency( const Subsystem* subsystem, const pString& componentName ) {
	return const_cast<BaseCollection*>( RegisterDependency( subsystem, componentName, UsageHint::Write ) );
}

void World::UnregisterDependency( const Subsystem* subsystem, const pString& componentName ) {
	auto it = m_NameToCollectionMap.find( componentName );
	if ( it == m_NameToCollectionMap.end() ) {
		Logger::Log( "Tried to unregister dependency for subsystem " + subsystem->GetName() + " to component collection with invalid name: " + componentName, "World", LogSeverity::ERROR_MSG );
	} else {
		const BaseCollection* collection = it->second;
		auto existingSubsystem = m_CollectionDependencies.find( subsystem );
		if ( existingSubsystem == m_CollectionDependencies.end() ) {
			Logger::Log( "Tried to unregister dependecy for subsystem " + subsystem->GetName() + " to component " + componentName + " but the subsystem has no dependencies", "World", LogSeverity::WARNING_MSG );
		} else {
			auto existingCollection = existingSubsystem->second.find( collection  );
			if ( existingCollection == existingSubsystem->second.end() ) {
				Logger::Log( "Tried to unregister dependecy for subsystem " + subsystem->GetName() + " to component " + componentName + " but the dependency did not exist", "World", LogSeverity::WARNING_MSG );
			} else {
				existingSubsystem->second.erase( existingCollection );
				if ( existingSubsystem->second.size() == 0 ) {
					m_CollectionDependencies.erase( existingSubsystem );
				}
			}
		}
	}
}

BaseCollection* World::GetCollectionWithID( const int id ) {
	return m_ComponentCollections.at( id );
}

const EntityManager& World::GetEntityManager() const {
	return m_EntityManager;
}

EntityManager& World::GetEditableEntityManager() {
	return m_EntityManager;
}

const BaseCollection* World::RegisterDependency( const Subsystem* subsystem, const pString& componentName, UsageHint usageHint ) {
	auto it = m_NameToCollectionMap.find( componentName );
	if ( it == m_NameToCollectionMap.end() ) {
		Logger::Log( "Tried to register dependency for subsystem " + subsystem->GetName() + " to component collection with invalid name: " + componentName, "World", LogSeverity::ERROR_MSG );
		return nullptr;
	} else {
		const BaseCollection* collectionToReturn = it->second;
		auto existingSubsystem = m_CollectionDependencies.find( subsystem );
		if ( existingSubsystem == m_CollectionDependencies.end() ) {
			// Create entry if it does not exist
			existingSubsystem = m_CollectionDependencies.emplace( std::piecewise_construct, std::forward_as_tuple( subsystem ), std::forward_as_tuple() ).first;
		}
		// Check if dependency already exists
		auto existingCollection = existingSubsystem->second.find( collectionToReturn );
		if ( existingCollection != existingSubsystem->second.end() ) {
			Logger::Log( "Attempted to register dependency for subsystem " + subsystem->GetName() + " to the collection " + componentName + " when it was already registered", "World", LogSeverity::WARNING_MSG );
		} else {
			// Add dependency to set
			existingSubsystem->second.emplace( collectionToReturn, CollectionDependencyInfo{ usageHint } );
		}
		return it->second;
	}
}

