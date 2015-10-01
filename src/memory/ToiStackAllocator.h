#pragma once
#include <cstddef>
#include <stdint.h>
#include <atomic>
#include "MemoryLibraryDefine.h"

#define TOI_STACK_ALLOCATOR_SIZE 64 * 1024 * 1024

#define TOI_STACK_ALLOCATOR_SET_MEMORY
#define TOI_STACK_ALLOCATOR_MEMORY_INITIALIZE_VALUE 254
#define TOI_STACK_ALLOCATOR_MEMORY_ALLOCATION_VALUE 253
#define TOI_STACK_ALLOCATOR_MEMORY_FREE_VALUE 252
		
#define TOI_STACK_ALLOCATOR_ASSERT_ON_OVERFLOW
#define TOI_STACK_ALLOCATOR_TRACK_NR_OF
//#define TOI_STACK_ALLOCATOR_COUT_CONSTRUCTION
//#define TOI_STACK_ALLOCATOR_COUT_DESTRUCTION

//typedef ToiStackAllocator StackAllocator;

#define g_ThreadStack ToiStackAllocator::GetThreadStack()

class ToiStackAllocator {
public:
	MEMORY_API static ToiStackAllocator& GetThreadStack();
	typedef size_t Marker;
	
	MEMORY_API explicit ToiStackAllocator( size_t stackSize );
	MEMORY_API ~ToiStackAllocator();
	MEMORY_API bool operator==( ToiStackAllocator& ) = delete;
	MEMORY_API bool operator=( ToiStackAllocator& ) = delete;

	MEMORY_API void* Allocate( size_t allocationSize, size_t alignment = 8 );
	MEMORY_API void Deallocate();

	MEMORY_API void Unwind( Marker marker );

	MEMORY_API Marker GetMarker() const;

	MEMORY_API uint8_t* GetBuffer();
	MEMORY_API size_t GetSize() const;

	MEMORY_API static int GetNrOfStacks();

private:
#ifdef TOI_STACK_ALLOCATOR_TRACK_NR_OF
	static std::atomic_int m_NrOfCreatedStacks;
#endif
	uint8_t* m_Memory = nullptr;
	Marker m_Marker = 0;
	size_t m_StackSize = 0;
};
