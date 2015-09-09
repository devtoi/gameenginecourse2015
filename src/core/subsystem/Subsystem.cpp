#include "Subsystem.h"

#include <utility/Logger.h>

Subsystem::Subsystem( const pString& name, int ID )
	: m_Name( name ), m_ID( ID ) {
}

void Subsystem::StartupFromCollection( SubsystemCollection* const subsystemCollection ) {
	// This will call start up for the specific system (virtual funtion call)
	Startup( subsystemCollection );
	Logger::Log( "Subsystem \"" + rString( GetName() ) + "\" started.", "Subsystem", LogSeverity::DEBUG_MSG );
}

void Subsystem::UpdateUserLayer( const float ) { }

void Subsystem::UpdateSimulationLayer( const float ) { }

void Subsystem::ShutdownFromCollection( SubsystemCollection* const subsystemCollection ) {	// This will call shut down for the specific system (virtual funtion call)
	Shutdown( subsystemCollection );
	Logger::Log( "Subsystem \"" + pString( GetName( ) ) + "\" shut down.", "Subsystem", LogSeverity::DEBUG_MSG );
}

Subsystem* Subsystem::Clone() const {
	Subsystem* subsystem = CreateNew();
	subsystem->SetUpdateOrderPriority( m_UpdateOrderPriority );
	subsystem->SetStartOrderPriority( m_StartOrderPriority );
	subsystem->SetShutdownOrderPriority( m_ShutdownOrderPriority );
	return subsystem;
}

void Subsystem::SetStartOrderPriority( int priority ) {
	m_StartOrderPriority = priority;
}

void Subsystem::SetUpdateOrderPriority( int priority ) {
	m_UpdateOrderPriority = priority;
}

void Subsystem::SetShutdownOrderPriority( int priority ) {
	m_ShutdownOrderPriority = priority;
}

int Subsystem::GetStartOrderPriority() const {
	return m_StartOrderPriority;
}

int Subsystem::GetUpdateOrderPriority() const {
	return m_UpdateOrderPriority;
}

int Subsystem::GetShutdownOrderPriority() const {
	return m_ShutdownOrderPriority;
}

const pString& Subsystem::GetName() const {
	return m_Name;
}

int Subsystem::GetID( ) const {
	return m_ID;
}

bool Subsystem::IsOnMainThread() const {
	return m_RunOnMainThread;
}

void Subsystem::Startup( SubsystemCollection* const subsystemCollection )  { }

void Subsystem::Shutdown( SubsystemCollection* const subsystemCollection ) { }

