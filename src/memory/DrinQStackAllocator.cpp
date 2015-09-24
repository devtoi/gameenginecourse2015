#include "DrinQStackAllocator.h"
#include <malloc.h>
#include <assert.h>
#include <cstring>

//#include <atomic>

void* DrinQFrameAllocator::Allocate( size_t size, size_t alignment ) {

	size_t unalignedMemory = reinterpret_cast<size_t>( m_Memory + m_Marker );
	
	size_t mask = alignment - 1;
	uintptr_t misalignment = unalignedMemory & mask;
	ptrdiff_t adjustment = misalignment > 0 ? alignment - misalignment : 0;
	size_t allocationSize = size + adjustment;
	uintptr_t alignedMemory = unalignedMemory + adjustment;

	assert( m_Marker + allocationSize < DRINQ_STACKALLOCATOR_SIZE );

	memset( m_Memory + m_Marker, DRINQ_STACKALLOCATOR_PADDED, allocationSize );

	m_Marker += allocationSize;

	return reinterpret_cast<void*>( alignedMemory );
}

void DrinQFrameAllocator::Unwind( size_t marker ) {
	size_t dist = m_Marker - marker;
	memset( m_Memory + marker, DRINQ_STACKALLOCATOR_RESET, m_Marker - marker );
	m_Marker = marker;
}

void DrinQFrameAllocator::Reset() {
	memset( m_Memory, DRINQ_STACKALLOCATOR_RESET, m_Marker );
	m_Marker = 0;
}

DrinQFrameAllocator::DrinQFrameAllocator( ) {
	DrinQFrameAllocator( DRINQ_STACKALLOCATOR_SIZE );
}


DrinQFrameAllocator::DrinQFrameAllocator( size_t size ) {
	m_Memory = (uint8_t*)malloc( size );

	memset(  m_Memory, DRINQ_STACKALLOCATOR_INITIAL, size );
	m_Size = size;
}

DrinQFrameAllocator::~DrinQFrameAllocator( ) {
	free( m_Memory );
}

size_t DrinQFrameAllocator::GetMarker() {
	return m_Marker;
}

uint8_t* DrinQFrameAllocator::GetBuffer() {
	return m_Memory;
}

MEMORY_API size_t DrinQFrameAllocator::GetSize() {
	return m_Size;
}

DrinQFrameAllocator& DrinQFrameAllocator::GetThreadStack() {
	static thread_local DrinQFrameAllocator stackAllocator( DRINQ_STACKALLOCATOR_SIZE );
	return stackAllocator;
}
