#ifdef _WIN32
	#ifdef RESOURCING_DLL_EXPORT
		#define RESOURCING_API __declspec(dllexport)
	#else
		#define RESOURCING_API __declspec(dllimport)
	#endif
#else
	#define RESOURCING_API
#endif

#ifndef RESOURCING_ALLOCATION_HEADER
#include "Alloc.h"
#else
#include RESOURCING_ALLOCATION_HEADER
#endif
