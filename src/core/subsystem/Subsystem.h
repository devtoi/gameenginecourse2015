#pragma once
#include <memory/Alloc.h>

class SubsystemCollection;

class Subsystem {
public:
	Subsystem( const pString& name, int ID );
	virtual ~Subsystem() = default;

	void 					StartupFromCollection		( SubsystemCollection* const subsystemCollection );
	virtual void 			UpdateUserLayer				( const float deltaTime );
	virtual void 			UpdateSimulationLayer		( const float timeStep );
	void 					ShutdownFromCollection		( SubsystemCollection* const subsystemCollection );

	Subsystem* 				Clone 						( ) const;

	// Subsystems are started in ascending order. Default value is 1000.
	void 					SetStartOrderPriority		( int priority );
	// Subsystems are updated in ascending order. Default value is 1000.
	void 					SetUpdateOrderPriority		( int priority );
	// Subsystems are shut down in ascending order. Default value is 1000.
	void 					SetShutdownOrderPriority	( int priority );

	int 					GetStartOrderPriority		( ) const;
	int 					GetUpdateOrderPriority		( ) const;
	int 					GetShutdownOrderPriority	( ) const;
	const pString& 			GetName 					( ) const;
	int 					GetID 						( ) const;
	bool 					IsOnMainThread				( ) const;

protected:
	virtual void 			Startup						( SubsystemCollection* const subsystemCollection );
	virtual void 			Shutdown					( SubsystemCollection* const subsystemCollection );

	virtual Subsystem* 		CreateNew 					( ) const = 0;

    bool 					m_RunOnMainThread 			= false;

private:
	int 					m_StartOrderPriority		= 0;
	int 					m_UpdateOrderPriority		= 0;
	int 					m_ShutdownOrderPriority		= 0;
	pString 				m_Name;
	int 					m_ID 						= -1;
};
