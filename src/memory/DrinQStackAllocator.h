#pragma once
#include <stdint.h>
#include <assert.h>
#include <mutex>
#include "MemoryLibraryDefine.h"

#define STACK_ALLOCATOR_SIZE 64
#define STACK_ALLOCATOR_ALIGNMENT 8

#define STACK_ALLOCATOR_INITIAL 128
#define STACK_ALLOCATOR_PADDED 127
#define STACK_ALLOCATOR_RESET 126


//#define DISABLE_FRAME_ALLOCATOR

//#define g_ThreadStack DrinQFrameAllocator::GetThreadStack()

//#ifndef DISABLE_FRAME_ALLOCATOR
//
////Per frame and thread allocation
//#define frameAlloc( Type, count )				g_ThreadStack.Allocate<Type>( count, FRAME_ALLOCATOR_ALIGNMENT )
//#define frameAllocAligned( Type, count, align )	g_ThreadStack.Allocate<Type>( count, align )
//#define frameNew( Type, ... )					new ( frameAlloc( Type, 1 ) ) Type( __VA_ARGS__ )
//#define frameNewAligned( Type, align, ... )		new ( frameAllocAligned( Type, 1, align ) ) Type( __VA_ARGS__ )
//#define frameNewArray( Type, count )			g_ThreadStack.Construct<Type>( count, STACK_ALLOCATOR_ALIGNMENT )
//#define frameNewArrayAligned( Type, count )		g_ThreadStack.Construct<Type>( count, STACK_ALLOCATOR_ALIGNMENT )
//#define frameDelete( ptr )						g_ThreadStack.Destroy( ptr )
//#define frameDeleteArray( ptr, count )			g_ThreadStack.Destroy( ptr, count )
//
//#else
//
//#define frameAlloc( Type, count )				(Type*) malloc( sizeof(Type) * (count) )
//#define frameAllocAligned( Type, count, align )	g_ThreadStack.AllocateT<Type>( count, align )
//#define frameNew( Type, ... )					new Type(__VA_ARGS__)
//#define frameNewAligned( Type, align, ... )		new Type(__VA_ARGS__)
//#define frameNewArray( Type, count )			new Type[(count)]
//#define frameNewArrayAligned( Type, count )		new Type[(count)]
//#define frameDelete( ptr )						delete ptr
//#define frameDeleteArray( ptr, count )			delete [] ptr
//
//#endif

class DrinQStackAllocator {
public:

	MEMORY_API DrinQStackAllocator( );

	MEMORY_API DrinQStackAllocator( size_t size );
				~DrinQStackAllocator();
	MEMORY_API size_t GetMarker();

	MEMORY_API uint8_t* GetBuffer();

	MEMORY_API size_t GetSize();

	MEMORY_API void Unwind( size_t marker );

	//MEMORY_API void Reset();

	//MEMORY_API static DrinQStackAllocator& GetThreadStack();
	
	template< typename T>
	T* Allocate( size_t count, size_t alignment = FRAME_ALLOCATOR_ALIGNMENT ) {
		m_Mutex.lock();
		size_t unalignedMemory = reinterpret_cast<size_t>( m_Memory + m_Marker );

		size_t mask = alignment - 1;
		uintptr_t misalignment = unalignedMemory & mask;
		ptrdiff_t adjustment = misalignment > 0 ? alignment - misalignment : 0;
		size_t allocationSize = count*sizeof(T) + adjustment;
		uintptr_t alignedMemory = unalignedMemory + adjustment;

		//Out of memory
		assert( m_Marker + allocationSize < m_Size );

		memset( m_Memory + m_Marker, FRAME_ALLOCATOR_PADDED, allocationSize );
		m_Marker += allocationSize;

		m_Mutex.unlock();
		return reinterpret_cast<T*>( alignedMemory );
	}

	template< typename T>
	T* Construct( size_t count, size_t alignment = FRAME_ALLOCATOR_ALIGNMENT ) {
		
		T* ptr = Allocate<T>( count, alignment );

		for( size_t i = 0; i < count; ++i ) {
			new (&ptr[i]) T();
		}

		return ptr;

	}

	template< typename T>
	void Destroy( T*& ptr ) {
		ptr[0].~T();
	}

private:

	std::mutex m_Mutex;

	uint8_t* m_Memory = nullptr;
	size_t m_Size;

	size_t m_Marker;
};
