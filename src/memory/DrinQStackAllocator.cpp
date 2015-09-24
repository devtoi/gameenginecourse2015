#include "DrinQStackAllocator.h"
#include <malloc.h>
#include <cstring>

void DrinQStackAllocator::Unwind( size_t marker ) {
	m_Mutex.lock();
	size_t dist = m_Marker - marker;
	memset( m_Memory + marker, STACK_ALLOCATOR_RESET, m_Marker - marker );
	m_Marker = marker;
	m_Mutex.unlock();
}

DrinQStackAllocator::DrinQStackAllocator( ) {
	DrinQStackAllocator( STACK_ALLOCATOR_SIZE );
}

DrinQStackAllocator::DrinQStackAllocator( size_t size ) {
	m_Memory = (uint8_t*)malloc( size );

	memset(  m_Memory, STACK_ALLOCATOR_INITIAL, size );
	m_Size = size;
}

DrinQStackAllocator::~DrinQStackAllocator( ) {
	free( m_Memory );
}

size_t DrinQStackAllocator::GetMarker() {
	return m_Marker;
}

uint8_t* DrinQStackAllocator::GetBuffer() {
	return m_Memory;
}

MEMORY_API size_t DrinQStackAllocator::GetSize() {
	return m_Size;
}