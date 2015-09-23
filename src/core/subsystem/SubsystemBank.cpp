#include "SubsystemBank.h"
#include <cassert>
#include "graphics/SSWindow.h"
#include "graphics/SSGraphicsSwap.h"
#include "graphics/SSGraphicsInitialize.h"
#include "graphics/SSParticles.h"
#include "graphics/SSEqualizer.h"
#include "testing/SSDeranesPoolTest.h"
#include "testing/SSStackAllocatorBasicTest.h"
#include "testing/SSToiPoolTest.h"
#include "testing/SSPoolThreadingTest.h"

SubsystemBank& SubsystemBank::GetInstance() {
	static SubsystemBank subsystemBank;
	return subsystemBank;
}

void SubsystemBank::Initialize() {
	CreateSubsystemTemplate<SSDeranesPoolTest>();
	CreateSubsystemTemplate<SSGraphicsSwap>();
	CreateSubsystemTemplate<SSGraphicsInitialize>();
	CreateSubsystemTemplate<SSParticles>();
	CreateSubsystemTemplate<SSStackAllocatorBasicTest>();
	CreateSubsystemTemplate<SSToiPoolTest>();
	CreateSubsystemTemplate<SSWindow>();
	CreateSubsystemTemplate<SSEqualizer>();
	CreateSubsystemTemplate<SSPoolThreadingTest>();

	// Startup priorities
	auto setStartPrio = [this] ( int id, int prio ) {
		m_SubsystemTemplates.at( id )->SetStartOrderPriority( prio );
	};
	setStartPrio( SSWindow::GetStaticID(),          	-500 ); // Before Input, graphics etc.
	setStartPrio( SSGraphicsInitialize::GetStaticID(), 	-400 ); //                                    | After window

	// Update priorities
	auto setUpdatePrio = [this] ( int id, int prio ) {
		m_SubsystemTemplates.at( id )->SetUpdateOrderPriority( prio );
	};
	// Defaulted
	setUpdatePrio( SSDeranesPoolTest::GetStaticID(), 0 );
	setUpdatePrio( SSGraphicsInitialize::GetStaticID(), 0 );
	setUpdatePrio( SSParticles::GetStaticID(), 0);
	setUpdatePrio( SSStackAllocatorBasicTest::GetStaticID(), 0 );
	setUpdatePrio( SSToiPoolTest::GetStaticID(), 0 );
	setUpdatePrio( SSWindow::GetStaticID(), 0 );
	setUpdatePrio( SSEqualizer::GetStaticID(), 100); //after particles
	setUpdatePrio( SSGraphicsSwap::GetStaticID(), 500 ); // Before frame reset stuff 			| After all rendering
	
	// Shutdown priorities
	auto setShutdownPrio = [this] ( int id, int prio ) {
		m_SubsystemTemplates.at( id )->SetShutdownOrderPriority( prio );
	};
	setShutdownPrio( SSWindow::GetStaticID(), 900 ); //									| After context destruction
}

void SubsystemBank::Deinitialize() {
	for ( auto& subsystem : m_SubsystemTemplates ) {
		pDelete( subsystem );
	}
}

Subsystem* SubsystemBank::CreateSubsystem( int subsystemID ) {
	assert( subsystemID >= 0 && subsystemID < m_SubsystemTemplates.size() );
	return m_SubsystemTemplates.at( subsystemID )->Clone();
}

size_t SubsystemBank::GetNrOfSubsystems() const {
	return m_SubsystemTemplates.size();
}
