#include "DrinQStackAllocator.h"
#include <malloc.h>
#include <cstring>

void DrinQStackAllocator::Unwind( size_t marker ) {
#ifdef THREAD_SAFE
	m_Mutex.lock();
#endif

#ifdef RESET_MEMORY
	size_t dist = m_Marker - marker;
	memset( m_Memory + marker, STACK_ALLOCATOR_RESET, m_Marker - marker );
#endif

	m_Marker = marker;

#ifdef THREAD_SAFE
	m_Mutex.unlock();
#endif
	
}

DrinQStackAllocator::DrinQStackAllocator( ) {
	DrinQStackAllocator( STACK_ALLOCATOR_SIZE );
}

DrinQStackAllocator::DrinQStackAllocator( size_t size ) {
	m_Memory = (uint8_t*)malloc( size );
#ifdef RESET_MEMORY
	memset(  m_Memory, STACK_ALLOCATOR_INITIAL, size );
#endif
	m_Size = size;
	m_Marker = 0;
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

size_t DrinQStackAllocator::GetSize() {
	return m_Size;
}

void* DrinQStackAllocator::Allocate( size_t size, size_t alignment ) {
#ifdef THREAD_SAFE
	m_Mutex.lock();
#endif

	size_t unalignedMemory = reinterpret_cast<size_t>( m_Memory + m_Marker );

	size_t mask = alignment - 1;
	uintptr_t misalignment = unalignedMemory & mask;
	ptrdiff_t adjustment = misalignment > 0 ? alignment - misalignment : 0;
	size_t allocationSize = size + adjustment;
	uintptr_t alignedMemory = unalignedMemory + adjustment;

	//Out of memory
	assert( m_Marker + allocationSize < m_Size );
#ifdef RESET_MEMORY
	memset( m_Memory + m_Marker, STACK_ALLOCATOR_PADDED, allocationSize );
#endif
	m_Marker += allocationSize;

#ifdef THREAD_SAFE
	m_Mutex.unlock();
#endif
	return reinterpret_cast<void*>( alignedMemory );
}