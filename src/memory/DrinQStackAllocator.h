#pragma once
#include <stdint.h>
#include "MemoryLibraryDefine.h"

#define DRINQ_STACKALLOCATOR_SIZE 32
#define DRINQ_STACKALLOCATOR_ALIGNMENT 8

#define DRINQ_STACKALLOCATOR_INITIAL 128
#define DRINQ_STACKALLOCATOR_PADDED 127
#define DRINQ_STACKALLOCATOR_RESET 126

#define g_ThreadStack DrinQFrameAllocator::GetThreadStack()

//Per frame and thread allocation
#define frameAlloc( Type, count )				g_ThreadStack.AllocateT<Type>( count, DRINQ_STACKALLOCATOR_ALIGNMENT )
#define frameAllocAligned( Type, count, align )	g_ThreadStack.AllocateT<Type>( count, align )

//Per frame and thread allocation
#define frameNew( Type, ... )				new ( frameAlloc( Type, 1 ) ) Type( __VA_ARGS__ )
#define frameNewAligned( Type, align, ... )	new ( frameAllocAligned( Type, 1, align ) ) Type( __VA_ARGS__ )

#define frameNewArray( Type, count )		g_ThreadStack.Construct<Type>( count, DRINQ_STACKALLOCATOR_ALIGNMENT )
#define frameNewArrayAligned( Type, count )	g_ThreadStack.Construct<Type>( count, DRINQ_STACKALLOCATOR_ALIGNMENT )

//#define frameDealloc( ptr )				g_ThreadStack.Deallocate( ptr )
#define frameDelete( ptr )					g_ThreadStack.Destroy( ptr )
#define frameDeleteArray( ptr, count )		g_ThreadStack.Destroy( ptr, count )

//template< typename T>
class DrinQFrameAllocator {
public:

	MEMORY_API DrinQFrameAllocator( );

	MEMORY_API DrinQFrameAllocator( size_t size );
				~DrinQFrameAllocator();
	MEMORY_API size_t GetMarker();

	MEMORY_API uint8_t* GetBuffer();

	MEMORY_API size_t GetSize();

	MEMORY_API void Unwind( size_t marker );

	MEMORY_API void Reset();

	MEMORY_API static DrinQFrameAllocator& GetThreadStack();
	

	MEMORY_API void* Allocate( size_t dataSize, size_t alignment = DRINQ_STACKALLOCATOR_ALIGNMENT );

	template< typename T>
	T* AllocateT( size_t count, size_t alignment = DRINQ_STACKALLOCATOR_ALIGNMENT ) {
		T* data = (T*)Allocate( sizeof( T ), alignment );

		//m_Marker += count * sizeof( T );
		return data;
	}

	template< typename T>
	T* Construct( size_t count, size_t alignment = DRINQ_STACKALLOCATOR_ALIGNMENT ) {
		
		T* ptr = AllocateT<T>( count, alignment );

		for( size_t i = 0; i < count; ++i ) {
			new (&ptr[i]) T();
		}

		return ptr;

	}

	template< typename T>
	void Deallocate( T* ptr ) {
		
	}

	template< typename T>
	void Destroy( T*& ptr ) {
		ptr[0].~T();
	}

private:
	uint8_t* m_Memory = nullptr;
	size_t m_Size;

	size_t m_Marker;
};
