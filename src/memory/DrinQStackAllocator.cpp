#include "DrinQStackAllocator.h"
#include <malloc.h>
#include <cstring>

void* DrinQStackAllocator::Allocate( size_t size, size_t alignment ) {

	size_t unalignedMemory = reinterpret_cast<size_t>( m_Memory + m_Marker );
	
	size_t mask = alignment - 1;
	uintptr_t misalignment = unalignedMemory & mask;
	ptrdiff_t adjustment = misalignment > 0 ? alignment - misalignment : 0;
	size_t allocationSize = size + adjustment;

	uintptr_t alignedMemory = unalignedMemory + adjustment;

	//uintptr_t

	memset( m_Memory + m_Marker, DRINQ_STACKALLOCATOR_PADDED, allocationSize );

	m_Marker += allocationSize;

	return reinterpret_cast<void*>( alignedMemory );
}

template<typename T>
T* DrinQStackAllocator::AllocateT( size_t count, size_t alignment ) {
	
	T* data = (T*);

	FrameMarker += count * sizeof( T );
	return data;
}



void DrinQStackAllocator::Unwind( size_t marker ) {

	size_t dist = m_Marker - marker;

	memset( m_Memory + marker, DRINQ_STACKALLOCATOR_RESET, m_Marker - marker );

	m_Marker = marker;
}

DrinQStackAllocator::DrinQStackAllocator( size_t size ) {
	m_Memory = (uint8_t*)malloc( size );

	memset(  m_Memory, DRINQ_STACKALLOCATOR_INITIAL, size );
	m_Size = size;
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

DrinQStackAllocator& DrinQStackAllocator::GetThreadStack() {
	static thread_local DrinQStackAllocator stackAllocator( DRINQ_STACKALLOCATOR_SIZE );
	return stackAllocator;
}
