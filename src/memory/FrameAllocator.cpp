#include "FrameAllocator.h"
#include <malloc.h>
#include <cstring>

std::atomic_int FrameAllocator::m_NrOfCreatedStacks(0);

void FrameAllocator::Unwind( size_t marker ) {
	size_t dist = m_Marker - marker;
#ifdef RESET_FRAME_MEMORY
	memset( m_Memory + marker, FRAME_ALLOCATOR_RESET, m_Marker - marker );
#endif
	m_Marker = marker;
}

void FrameAllocator::Reset() {
#ifdef RESET_FRAME_MEMORY
	memset( m_Memory, FRAME_ALLOCATOR_RESET, m_Marker );
#endif
	m_Marker = 0;
}

FrameAllocator::FrameAllocator( ) {
	FrameAllocator( FRAME_ALLOCATOR_SIZE );
}


FrameAllocator::FrameAllocator( size_t size ) {
	m_Memory = (uint8_t*)malloc( size );
#ifdef RESET_FRAME_MEMORY
	memset(  m_Memory, FRAME_ALLOCATOR_INITIAL, size );
#endif
	m_Size = size;
	m_NrOfCreatedStacks += 1;
}

FrameAllocator::~FrameAllocator( ) {
	free( m_Memory );
	m_NrOfCreatedStacks -= 1;
}

size_t FrameAllocator::GetMarker() {
	return m_Marker;
}

uint8_t* FrameAllocator::GetBuffer() {
	return m_Memory;
}

size_t FrameAllocator::GetSize() {
	return m_Size;
}

FrameAllocator& FrameAllocator::GetThreadStack() {
	static thread_local FrameAllocator stackAllocator( FRAME_ALLOCATOR_SIZE );
	return stackAllocator;
}

int FrameAllocator::GetNrOfStacks() {
	return m_NrOfCreatedStacks;
}

void* FrameAllocator::allocate( size_t size, size_t alignment ) {
	size_t unalignedMemory = reinterpret_cast<size_t>( m_Memory + m_Marker );

	size_t mask = alignment - 1;
	uintptr_t misalignment = unalignedMemory & mask;
	ptrdiff_t adjustment = misalignment > 0 ? alignment - misalignment : 0;
	size_t allocationSize = size + adjustment;
	uintptr_t alignedMemory = unalignedMemory + adjustment;

	//Out of memory
	assert( m_Marker + allocationSize < m_Size );

#ifdef RESET_FRAME_MEMORY
	memset( m_Memory + m_Marker, FRAME_ALLOCATOR_PADDED, allocationSize );
#endif

	m_Marker += allocationSize;

	return reinterpret_cast<void*>( alignedMemory );
}
