#include "DrinQFrameAllocator.h"
#include <malloc.h>
#include <cstring>

std::atomic_int DrinQFrameAllocator::m_NrOfCreatedStacks(0);

void DrinQFrameAllocator::Unwind( size_t marker ) {
	size_t dist = m_Marker - marker;
	memset( m_Memory + marker, FRAME_ALLOCATOR_RESET, m_Marker - marker );
	m_Marker = marker;
}

void DrinQFrameAllocator::Reset() {
	memset( m_Memory, FRAME_ALLOCATOR_RESET, m_Marker );
	m_Marker = 0;
}

DrinQFrameAllocator::DrinQFrameAllocator( ) {
	DrinQFrameAllocator( FRAME_ALLOCATOR_SIZE );
}


DrinQFrameAllocator::DrinQFrameAllocator( size_t size ) {
	m_Memory = (uint8_t*)malloc( size );

	memset(  m_Memory, FRAME_ALLOCATOR_INITIAL, size );
	m_Size = size;
	m_NrOfCreatedStacks += 1;
}

DrinQFrameAllocator::~DrinQFrameAllocator( ) {
	free( m_Memory );
	m_NrOfCreatedStacks -= 1;
}

size_t DrinQFrameAllocator::GetMarker() {
	return m_Marker;
}

uint8_t* DrinQFrameAllocator::GetBuffer() {
	return m_Memory;
}

size_t DrinQFrameAllocator::GetSize() {
	return m_Size;
}

DrinQFrameAllocator& DrinQFrameAllocator::GetThreadStack() {
	static thread_local DrinQFrameAllocator stackAllocator( FRAME_ALLOCATOR_SIZE );
	return stackAllocator;
}

int DrinQFrameAllocator::GetNrOfStacks() {
	return m_NrOfCreatedStacks;
}

void* DrinQFrameAllocator::allocate( size_t size, size_t alignment ) {
	size_t unalignedMemory = reinterpret_cast<size_t>( m_Memory + m_Marker );

	size_t mask = alignment - 1;
	uintptr_t misalignment = unalignedMemory & mask;
	ptrdiff_t adjustment = misalignment > 0 ? alignment - misalignment : 0;
	size_t allocationSize = size + adjustment;
	uintptr_t alignedMemory = unalignedMemory + adjustment;

	//Out of memory
	assert( m_Marker + allocationSize < m_Size );

	memset( m_Memory + m_Marker, FRAME_ALLOCATOR_PADDED, allocationSize );
	m_Marker += allocationSize;

	return reinterpret_cast<void*>( alignedMemory );
}
