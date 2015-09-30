#include "SSProfilerInOut.h"
#include <fstream>
#include <input/InputContext.h>
#include <profiler/Profiler.h>

const pString SSProfilerInOut::Name = "ProfileerInOut";
int SSProfilerInOut::ID = -1;

void SSProfilerInOut::Startup( SubsystemCollection* const subsystemCollection ) {
	// Perform Initialization here (Don't forget to set startup order priority!)
}

void SSProfilerInOut::Shutdown( SubsystemCollection* const subsystemCollection ) {
	// Perform Cleanup here (Don't forget to set shutdown order priority!)
}

void SSProfilerInOut::UpdateUserLayer( const float deltaTime ) {
	if ( g_Input.KeyUpDown( SDL_SCANCODE_C ) ) {
		g_Profiler.PrintAveragesMilliSeconds();
	}
	if ( g_Input.KeyUpDown( SDL_SCANCODE_S ) ) {
		SaveProfilerTimes();
	}
}

void SSProfilerInOut::UpdateSimulationLayer( const float timeStep ) {
	// Perform simulation update logic here (Don't forget to set update order priority!)
}

Subsystem* SSProfilerInOut::CreateNew( ) const {
	return pNew( SSProfilerInOut, SSProfilerInOut::ID );
}

int SSProfilerInOut::GetStaticID() {
	return SSProfilerInOut::ID;
}

void SSProfilerInOut::SaveProfilerTimes() {
	std::ofstream file( "profiledata.txt" );
	if ( file.is_open() ) {
		for ( const auto& entry : *g_Profiler.GetMapForCategory( Profiler::PROFILER_CATEGORY_STANDARD ) ) {
			file << entry.first << "," << entry.second.GetAverage() * g_Profiler.GetConversionFactorMilliSeconds() << std::endl;
		}
		file.close();
	} else {
		std::cout << "Failed to save file" << std::endl;
	}
}
