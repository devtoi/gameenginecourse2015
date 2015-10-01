#include "ToiStackAllocator.h"
#include <cstdlib>
#ifdef TOI_STACK_ALLOCATOR_SET_MEMORY
#include <cstring>
#endif
#include <iostream>// TODOJM: Remove

std::atomic_int ToiStackAllocator::m_NrOfCreatedStacks( 0 );

ToiStackAllocator& ToiStackAllocator::GetThreadStack() {
	static thread_local ToiStackAllocator stackAllocator( TOI_STACK_ALLOCATOR_SIZE );
	return stackAllocator;
}

ToiStackAllocator::ToiStackAllocator( size_t stackSize )
: m_StackSize( stackSize ) {
	m_Memory = static_cast<uint8_t*>( malloc( stackSize ) );

#ifdef TOI_STACK_ALLOCATOR_COUT_CONSTRUCTION
	std::cout << "created stack with size: " << stackSize << std::endl;
#endif

#ifdef TOI_STACK_ALLOCATOR_TRACK_NR_OF
	m_NrOfCreatedStacks += 1;
#endif

#ifdef TOI_STACK_ALLOCATOR_SET_MEMORY
	memset( m_Memory, TOI_STACK_ALLOCATOR_MEMORY_INITIALIZE_VALUE, stackSize );
#endif
}

ToiStackAllocator::~ToiStackAllocator() {
	free( m_Memory );

#ifdef TOI_STACK_ALLOCATOR_TRACK_NR_OF
	m_NrOfCreatedStacks -= 1;
#endif

#ifdef TOI_STACK_ALLOCATOR_COUT_DESTRUCTION
	std::cout << "destructed stack" << std::endl;
#endif
}

void* ToiStackAllocator::Allocate( size_t allocationSize, size_t alignment ) {
	size_t unAlignedMemory = reinterpret_cast<size_t>( m_Memory + m_Marker );
	size_t mask = unAlignedMemory & ( alignment - 1 );
	size_t adjust = mask > 0 ? alignment - mask : 0;
	size_t alignedMemory = unAlignedMemory + adjust;
	size_t alignedAllocationSize = allocationSize + adjust;

#ifdef STACK_ALLOCATOR_MEMORY_ASSERT_ON_OVERFLOW
	assert( m_Marker + alignedAllocationSize <= m_StackSize );
#endif

#ifdef TOISTACK_ALLOCATOR_SET_MEMORY
	memset( m_Memory + m_Marker, TOISTACK_ALLOCATOR_MEMORY_ALLOCATION_VALUE, alignedAllocationSize );
#endif
	m_Marker += alignedAllocationSize;


	return reinterpret_cast<void*>( alignedMemory );
}

ToiStackAllocator::Marker ToiStackAllocator::GetMarker() const {
	return m_Marker;
}

void ToiStackAllocator::Unwind( Marker marker ) {
#ifdef TOI_STACK_ALLOCATOR_SET_MEMORY
	memset( m_Memory + marker, TOI_STACK_ALLOCATOR_MEMORY_FREE_VALUE, m_Marker - marker );
#endif
	m_Marker = marker;
}

uint8_t* ToiStackAllocator::GetBuffer() {
	return m_Memory;
}

size_t ToiStackAllocator::GetSize() const {
	return m_StackSize;
}

int ToiStackAllocator::GetNrOfStacks() {
	return m_NrOfCreatedStacks;
}
