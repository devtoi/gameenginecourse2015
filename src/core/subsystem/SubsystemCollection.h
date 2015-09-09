#pragma once

#include <memory/Alloc.h>
#include "SubsystemTypes.h"

class Subsystem;

class SubsystemCollection {
public:
	~SubsystemCollection();
	void InitializeGameMode( const GameModeMask& gameModeMask );
	void Deinitialize();

	Subsystem* RegisterDependencyWithID( const Subsystem* const requester, const int subsystemToRegisterID );
	Subsystem* RegisterDependencyWithName( const Subsystem* const requester, const pString& subsystemToRegisterName );
	void UnregisterDependencyWithID( const Subsystem* const requester, const int subsystemToUnregisterID );
	void UnregisterDependencyWithName( const Subsystem* const requester, const pString& subsystemToUnregisterName );
	void UnregisterDependencies( const Subsystem* const requester );

	void Startup();
	void Update( float deltaTime );
	void Shutdown();

private:
	void AddSubsystem( Subsystem* subsystem );
	void RemoveSubsystem( int subsystemID );
	Subsystem* RegisterDependency( const Subsystem* const requester, Subsystem* subsystemToRegister );
	void UnregisterDependency( const Subsystem* const requester, const Subsystem* const subsystemToUnregister );

	pVector<Subsystem*> m_StartupOrder;
	pVector<Subsystem*> m_UpdateOrder;
	pVector<Subsystem*> m_ShutdownOrder;
	pUnorderedMap<int, Subsystem*> m_Subsystems;
	pMap<pString, Subsystem*> m_NameToSubsystem;
	GameModeMask m_CurrentGameModeMask;
	pUnorderedMap<const Subsystem*, pUnorderedSet<const Subsystem*>> m_SubsystemDependencies;
};
