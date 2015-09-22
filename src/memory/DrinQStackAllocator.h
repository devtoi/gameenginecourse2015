#pragma once
//#include <cstddef>
#include <stdint.h>
//#include <atomic>
#include <malloc.h>
#include <new>
#include "MemoryLibraryDefine.h"

#define DRINQ_STACKALLOCATOR_SIZE 16
#define DRINQ_STACKALLOCATOR_ALIGNMENT 8

#define DRINQ_STACKALLOCATOR_INITIAL 128
#define DRINQ_STACKALLOCATOR_PADDED 127
#define DRINQ_STACKALLOCATOR_RESET 126

#define g_ThreadStack DrinQStackAllocator::GetThreadStack()

#define permanentAlloc( Type, count )	g_ThreadStack.AllocateT<Type>( count, DRINQ_STACKALLOCATOR_ALIGNMENT )
#define permanentNew( Type, ... )		new ( permanentAlloc( Type, 1 ) ) Type( __VA_ARGS__ )



class DrinQStackAllocator {
public:

	MEMORY_API DrinQStackAllocator( size_t size );
	MEMORY_API size_t GetMarker();

	MEMORY_API uint8_t* GetBuffer();

	MEMORY_API size_t GetSize();


	MEMORY_API static DrinQStackAllocator& GetThreadStack();
	

	//void Initialize( );
	MEMORY_API void* Allocate( size_t dataSize, size_t alignment = DRINQ_STACKALLOCATOR_ALIGNMENT );

	//template< typename T>
	//T* AllocateT( size_t count, size_t alignment = DRINQ_STACKALLOCATOR_ALIGNMENT );

	template< typename T>
	T* AllocateT( size_t count, size_t alignment = DRINQ_STACKALLOCATOR_ALIGNMENT ) {
		T* data = (T*)Allocate( sizeof( T ), alignment );

		m_Marker += count * sizeof( T );
		return data;
	}

	MEMORY_API void Unwind( size_t marker );

private:
	uint8_t* m_Memory = nullptr;
	size_t m_Size;

	size_t m_Marker;

	//void* FrameAllocate


	//void Allocate( SIZE );
	


	

};
