#include "SSProfilerInOut.h"
#include <fstream>
#include <cstdio>
#include <utility/Logger.h>
#include <input/InputContext.h>
#include <profiler/Profiler.h>

const pString SSProfilerInOut::Name = "ProfileerInOut";
int SSProfilerInOut::ID = -1;

void SSProfilerInOut::Startup( SubsystemCollection* const ) {
	std::remove( m_AppendingProfileTimeFileName.c_str() );
}

void SSProfilerInOut::Shutdown( SubsystemCollection* const ) {
	// Perform Cleanup here (Don't forget to set shutdown order priority!)
}

void SSProfilerInOut::UpdateUserLayer( const float ) {
	if ( g_Input.KeyUpDown( SDL_SCANCODE_C ) ) {
		g_Profiler.PrintAveragesMilliSeconds();
	}
	if ( g_Input.KeyUpDown( SDL_SCANCODE_A ) ) {
		SaveAverageProfilerTimes();
	}
	if ( g_Input.KeyUpDown( SDL_SCANCODE_L ) ) {
		AppendLatestProfilerTimes();
	}
}

void SSProfilerInOut::UpdateSimulationLayer( const float ) {
	// Perform simulation update logic here (Don't forget to set update order priority!)
}

Subsystem* SSProfilerInOut::CreateNew( ) const {
	return pNew( SSProfilerInOut, SSProfilerInOut::ID );
}

int SSProfilerInOut::GetStaticID() {
	return SSProfilerInOut::ID;
}

void SSProfilerInOut::SaveAverageProfilerTimes() {
	std::ofstream file( m_AverageProfileTimeFileName );
	if ( file.is_open() ) {
		for ( const auto& entry : *g_Profiler.GetMapForCategory( Profiler::PROFILER_CATEGORY_STANDARD ) ) {
			file << entry.first << "," << entry.second.GetAverage() * g_Profiler.GetConversionFactorMilliSeconds() << std::endl;
		}
		file.close();
	} else {
		Logger::Log( "Failed to save profile data file: " + m_AverageProfileTimeFileName, SSProfilerInOut::Name, LogSeverity::WARNING_MSG );
	}
}

void SSProfilerInOut::AppendLatestProfilerTimes() {
	std::ofstream file( m_AppendingProfileTimeFileName, std::ios::app );
	if ( file.is_open() ) {
		for ( const auto& entry : *g_Profiler.GetMapForCategory( Profiler::PROFILER_CATEGORY_STANDARD ) ) {
			file << entry.first << "," << entry.second.GetLatest() * g_Profiler.GetConversionFactorMilliSeconds() << std::endl;
		}
		file.close();
	} else {
		Logger::Log( "Failed to save profile data file: " + m_AverageProfileTimeFileName, SSProfilerInOut::Name, LogSeverity::WARNING_MSG );
	}
}
