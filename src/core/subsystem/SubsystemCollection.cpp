#include "SubsystemCollection.h"
#include <algorithm>
#include <cassert>
#include <utility/Logger.h>
#include "Subsystem.h"
#include "SubsystemBank.h"

SubsystemCollection::~SubsystemCollection() {
}

void SubsystemCollection::Deinitialize() {
	for ( auto& subsystem : m_ShutdownOrder ) {
		subsystem->ShutdownFromCollection( this );
		pDelete( subsystem );
	}
}

Subsystem* SubsystemCollection::RegisterDependencyWithID(const Subsystem * const requester, const int subsystemToRegisterID) {
	auto it = m_Subsystems.find( subsystemToRegisterID );
	if ( it == m_Subsystems.end() ) {
		Logger::Log( "Tried to register dependency for subsystem " + requester->GetName() + " to invalid subsystem (id= " + rToString( subsystemToRegisterID ) + ")", "SubsystemManager", LogSeverity::ERROR_MSG );
		return nullptr;
	}

	return RegisterDependency( requester, it->second );
}

Subsystem * SubsystemCollection::RegisterDependencyWithName( const Subsystem * const requester, const pString& subsystemToRegisterName ) {
	auto it = m_NameToSubsystem.find( subsystemToRegisterName );
	if ( it == m_NameToSubsystem.end() ) {
		Logger::Log( "Tried to register dependency for subsystem " + requester->GetName() + " to invalid subsystem " + subsystemToRegisterName, "SubsystemManager", LogSeverity::ERROR_MSG );
		return nullptr;
	}

	return RegisterDependency( requester, it->second );
}

void SubsystemCollection::UnregisterDependencyWithID( const Subsystem* const requester, const int subsystemToUnregisterID ) {
	auto it = m_Subsystems.find( subsystemToUnregisterID );
	if ( it == m_Subsystems.end() ) {
		Logger::Log( "Tried to unregister dependency for subsystem " + requester->GetName() + " to invalid subsystem (id= " + rToString( subsystemToUnregisterID ) + ")", "SubsystemManager", LogSeverity::ERROR_MSG );
		return;
	}
	UnregisterDependency( requester, it->second );
}

void SubsystemCollection::UnregisterDependencyWithName( const Subsystem* const requester, const pString& subsystemToUnregisterName ) {
	auto it = m_NameToSubsystem.find( subsystemToUnregisterName );
	if ( it == m_NameToSubsystem.end() ) {
		Logger::Log( "Tried to unregister dependency for subsystem " + requester->GetName() + " to invalid subsystem " + subsystemToUnregisterName, "SubsystemManager", LogSeverity::ERROR_MSG );
		return;
	}
	UnregisterDependency( requester, it->second );
}

void SubsystemCollection::UnregisterDependencies( const Subsystem* const requester ) {
	auto subsystemEntry = m_SubsystemDependencies.find( requester );
	if ( subsystemEntry == m_SubsystemDependencies.end() ) {
		Logger::Log( "The unregistered subsystem " + requester->GetName() + " attempted to unregister its dependencies", "SubsystemManager", LogSeverity::WARNING_MSG );
		return;
	}
	subsystemEntry->second.clear();
}

void SubsystemCollection::InitializeGameMode( const GameModeMask& gameModeMask ) {
	GameModeMask toShutdown = ( m_CurrentGameModeMask ^ gameModeMask ) & m_CurrentGameModeMask;
	GameModeMask toStartup = ( m_CurrentGameModeMask ^ gameModeMask ) & gameModeMask;

	for ( const auto& subsystem : m_ShutdownOrder ) {
		if ( toShutdown.test( subsystem->GetID() ) ) {
			subsystem->ShutdownFromCollection( this );
		}
	}

	for ( size_t i = 0; i < toShutdown.size( ); ++i ) {
		if ( toShutdown.test( i ) ) {
			RemoveSubsystem( static_cast<int>( i ) );
		}
	}

	for ( size_t i = 0; i < toStartup.size( ); ++i ) {
		if ( toStartup.test( i ) ) {
			AddSubsystem( g_SubsystemBank.CreateSubsystem( static_cast<int>( i ) ) );
		}
	}

	for ( const auto& subsystem : m_StartupOrder ) {
		if ( toStartup.test( subsystem->GetID() ) ) {
			subsystem->StartupFromCollection( this );
		}
	}
}

void SubsystemCollection::Startup() {
	for ( auto& subsystem : m_StartupOrder ) {
		subsystem->StartupFromCollection( this );
	}
}

void SubsystemCollection::Update( float deltaTime ) {
	for ( auto& subsystem : m_UpdateOrder ) {
		subsystem->UpdateUserLayer( deltaTime );
	}
}

void SubsystemCollection::Shutdown() {
	for ( auto& subsystem : m_ShutdownOrder ) {
		subsystem->ShutdownFromCollection( this );
	}
}

void SubsystemCollection::AddSubsystem( Subsystem* subsystem ) {
	m_StartupOrder.insert(
		std::lower_bound( m_StartupOrder.begin(), m_StartupOrder.end(), subsystem, [] ( const Subsystem* lhs, const Subsystem* rhs ) -> bool {
			return lhs->GetStartOrderPriority() < rhs->GetStartOrderPriority();
		} ), subsystem
	);

	m_UpdateOrder.insert(
		std::lower_bound( m_UpdateOrder.begin(), m_UpdateOrder.end(), subsystem, [] ( const Subsystem* lhs, const Subsystem* rhs ) -> bool {
			return lhs->GetUpdateOrderPriority() < rhs->GetUpdateOrderPriority();
		} ), subsystem
	);

	m_ShutdownOrder.insert(
		std::lower_bound( m_ShutdownOrder.begin(), m_ShutdownOrder.end(), subsystem, [] ( const Subsystem* lhs, const Subsystem* rhs ) -> bool {
			return lhs->GetShutdownOrderPriority() < rhs->GetShutdownOrderPriority();
		} ), subsystem
	);
	m_Subsystems.emplace( subsystem->GetID(), subsystem );
	m_NameToSubsystem.emplace( subsystem->GetName(), subsystem );
	m_SubsystemDependencies.emplace( std::piecewise_construct, std::make_tuple( subsystem ), std::make_tuple() );
}

void SubsystemCollection::RemoveSubsystem( int subsystemID ) {
	auto subsystemsIt = m_Subsystems.find( subsystemID );
	assert( subsystemsIt != m_Subsystems.end() );
	Subsystem* subsystem = subsystemsIt->second;
	m_Subsystems.erase( subsystemsIt );

	auto namesIt = m_NameToSubsystem.find( subsystem->GetName() );
	m_NameToSubsystem.erase( namesIt );

	auto dependencyIt = m_SubsystemDependencies.find( subsystem );
	m_SubsystemDependencies.erase( dependencyIt );

	m_StartupOrder.erase( std::remove_if( m_StartupOrder.begin(), m_StartupOrder.end(), [subsystem]( Subsystem* startSubsystem ) {
				return startSubsystem == subsystem;
			}
		), m_StartupOrder.end() );
	m_UpdateOrder.erase( std::remove_if( m_UpdateOrder.begin(), m_UpdateOrder.end(), [subsystem]( Subsystem* updateSubsystem ) {
				return updateSubsystem == subsystem;
			}
		), m_UpdateOrder.end() );
	m_ShutdownOrder.erase( std::remove_if( m_ShutdownOrder.begin(), m_ShutdownOrder.end(), [subsystem]( Subsystem* shutdownSubsystem ) {
				return shutdownSubsystem == subsystem;
			}
		), m_ShutdownOrder.end() );
	pDelete( subsystem );
}

Subsystem* SubsystemCollection::RegisterDependency( const Subsystem* const requester, Subsystem* subsystemToRegister ) {
	// Make sure the requester is a registered subsystem
	if ( m_SubsystemDependencies.find( requester ) == m_SubsystemDependencies.end() ) {
		Logger::Log( "The unregistered subsystem " + requester->GetName() + " attempted to register a dependency", "SubsystemManager", LogSeverity::ERROR_MSG );
		return nullptr;
	}

	auto current = m_SubsystemDependencies.find( requester );
	if ( current == m_SubsystemDependencies.end() ) {
		Logger::Log( "Failed to get dependency set for subsystem " + requester->GetName(), "SubsystemManager", LogSeverity::ERROR_MSG );
		return nullptr;
	}
	pUnorderedSet<const Subsystem*>& currentDependencies = current->second;
	if ( currentDependencies.find( subsystemToRegister ) != currentDependencies.end() ) {
		Logger::Log( "Tried to register already existing subsystem dependency for subsystem " + requester->GetName() + " to subsystem " + subsystemToRegister->GetName(),
			"SubsystemManager", LogSeverity::WARNING_MSG );
	}
	currentDependencies.emplace( subsystemToRegister );
	return subsystemToRegister;
}

void SubsystemCollection::UnregisterDependency( const Subsystem* const requester, const Subsystem* const subsystemToUnregister ) {
	auto subsystemEntry = m_SubsystemDependencies.find( requester );
	if ( subsystemEntry == m_SubsystemDependencies.end() ) {
		Logger::Log( "The unregistered subsystem " + requester->GetName() + " attempted to unregister a dependency", "SubsystemManager", LogSeverity::WARNING_MSG );
		return;
	}
	auto dependency = subsystemEntry->second.find( subsystemToUnregister );
	if ( dependency == subsystemEntry->second.end() ) {
		Logger::Log( "Tried to non existant dependency for " + requester->GetName() + " to " + subsystemToUnregister->GetName(), "SubsystemManager", LogSeverity::WARNING_MSG );
		return;
	}
	subsystemEntry->second.erase( dependency );
}
