#pragma once

#include "SDL_timer.h"
#include "ProfileMacro.h"
#include <memory/Alloc.h>

#define FRAMECOUNTER_NR_OF_SAMPLES 100u
#define FRAMECOUNTER_STRING_FPS_PRECISION 0
#define FRAMECOUNTER_STRING_MS_PRECISION 2
#define FRAMECOUNTER_STRING_MAX_FRAMETIME_PRECISION 5

class FrameCounter {
public:
	MEMORY_API FrameCounter();

	MEMORY_API void TickFrame( );

	MEMORY_API float GetFPS( ) const;
	MEMORY_API const rString GetFPSString( ) const;

	MEMORY_API float GetAverageFPS( ) const;
	MEMORY_API const rString GetAverageFPSString( ) const;

	MEMORY_API float GetMS( ) const;
	MEMORY_API const rString GetMSString( ) const;

	MEMORY_API float GetMaxFrameTime( ) const;
	MEMORY_API const rString GetMaxFrameTimeString( ) const;
	MEMORY_API void ResetMaxFrameTime( );

	MEMORY_API float GetAverageMS( ) const;
	MEMORY_API const rString GetAverageMSString( ) const;

private:

	uint64_t		m_frameTimes[FRAMECOUNTER_NR_OF_SAMPLES];
	uint64_t 		m_maxFrameTime = 0;
	unsigned int 	m_nrOfFrames = 0;
	unsigned int	m_overwritePos = 0;
	uint64_t 		m_startTime = 0;
};
