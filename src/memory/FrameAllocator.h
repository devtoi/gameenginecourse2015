#pragma once
#include <stdint.h>
#include <assert.h>
#include <string.h>
#include <atomic>
#include <cstddef>
#include "MemoryLibraryDefine.h"

#define FRAME_ALLOCATOR_SIZE 64
#define FRAME_ALLOCATOR_ALIGNMENT 8

#define FRAME_ALLOCATOR_INITIAL 122
#define FRAME_ALLOCATOR_PADDED 121
#define FRAME_ALLOCATOR_RESET 120

#define RESET_FRAME_MEMORY


//#define DISABLE_FRAME_ALLOCATOR

#define g_ThreadStack FrameAllocator::GetThreadStack()

#ifndef DISABLE_FRAME_ALLOCATOR

//Per frame and thread allocation
#define frameAlloc( size )						g_ThreadStack.allocate( size, FRAME_ALLOCATOR_ALIGNMENT )
#define frameAllocAligned( size, align )		g_ThreadStack.allocate( size, align )
#define frameNew( Type, ... )					reinterpret_cast<Type*>( new ( frameAlloc( sizeof(Type) ) ) Type( __VA_ARGS__ ) )
#define frameNewAligned( Type, align, ... )		reinterpret_cast<Type*>( new ( frameAllocAligned( sizeof(Type), align ) ) Type( __VA_ARGS__ ) )

#define frameNewArray( Type, count, ... )					g_ThreadStack.construct<Type>( count, __VA_ARGS__  )
//#define frameNewArrayAligned( Type, count, align, ... )		g_ThreadStack.construct<Type>( count, __VA_ARGS__ )
#define frameDelete( ptr )									ptr.~Type()
#define frameDeleteArray( ptr, count )						g_ThreadStack.Destroy( ptr, count )

#else

#include <memory>


#define frameAlloc( size )								malloc( size )
#define frameAllocAligned( size, align )				malloc( size )
#define frameNew( Type, ... )							new Type(__VA_ARGS__)
#define frameNewAligned( Type, align, ... )				new Type(__VA_ARGS__)
#define frameNewArray( Type, count, ... )				new Type[(count)]
#define frameNewArrayAligned( Type, count, align )		new Type[(count)]
#define frameDelete( ptr )								delete ptr
#define frameDeleteArray( ptr, count )					delete [] ptr

#endif

class FrameAllocator {
public:

	MEMORY_API FrameAllocator( );

	MEMORY_API FrameAllocator( size_t size );
				~FrameAllocator();
	MEMORY_API size_t GetMarker();

	MEMORY_API uint8_t* GetBuffer();

	MEMORY_API size_t GetSize();

	MEMORY_API void Unwind( size_t marker );

	MEMORY_API void Reset();

	MEMORY_API static FrameAllocator& GetThreadStack();
	MEMORY_API static int GetNrOfStacks();
	
	MEMORY_API void* allocate( size_t size, size_t alignment = FRAME_ALLOCATOR_ALIGNMENT );

	template<typename Type, typename... Args>
	Type* construct( size_t count, Args&&... args ) {
		Type* data = static_cast<Type*>( allocate( count * sizeof( Type ), alignof( Type ) ) );
		for( size_t i = 0; i < count; ++i ) {
			new (&data[i]) Type(std::forward<Args>( args )...);
		}
		return data;
	}

	template< typename T>
	void Destroy( T*& ptr, size_t count = 1 ) {

		for ( size_t i = 0; i < count; ++i ) {
			if ( ptr[i] ) {
				ptr[i].~T();
			}
		}
	}



private:
	static std::atomic_int m_NrOfCreatedStacks;
	uint8_t* m_Memory = nullptr;
	size_t m_Size;

	size_t m_Marker;
};
