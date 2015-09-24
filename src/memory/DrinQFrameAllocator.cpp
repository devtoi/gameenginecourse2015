#include "DrinQFrameAllocator.h"
#include <malloc.h>
#include <cstring>

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
	static thread_local DrinQFrameAllocator stackAllocator( FRAME_ALLOCATOR_SIZE );
	return stackAllocator;
}
