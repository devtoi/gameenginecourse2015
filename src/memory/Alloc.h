#pragma once

#include <vector>
#include <map>
#include <set>
#include <unordered_set>
#include <unordered_map>
#include <queue>
#include "rString.h"

typedef std::basic_string < char, std::char_traits<char>, std::allocator<char> > rString;
typedef std::basic_stringstream< char, std::char_traits<char>, std::allocator<char> > rStringStream;
typedef std::basic_istringstream< char, std::char_traits<char>, std::allocator<char> > rIStringStream;
typedef std::basic_ostringstream< char, std::char_traits<char>, std::allocator<char> > rOStringStream;

// String
typedef std::basic_string < char, std::char_traits<char>, std::allocator<char> >		pString;
typedef std::basic_string < char, std::char_traits<char>, std::allocator<char> >		tString;
typedef std::basic_string < char, std::char_traits<char>, std::allocator<char> >		fString;

// StringStream
typedef std::basic_stringstream< char, std::char_traits<char>, std::allocator<char> >	pStringStream;
typedef std::basic_stringstream< char, std::char_traits<char>, std::allocator<char> >	tStringStream;
typedef std::basic_stringstream< char, std::char_traits<char>, std::allocator<char> >	fStringStream;

// IStringStream
typedef std::basic_istringstream< char, std::char_traits<char>, std::allocator<char> >	pIStringStream;
typedef std::basic_istringstream< char, std::char_traits<char>, std::allocator<char> >	tIStringStream;
typedef std::basic_istringstream< char, std::char_traits<char>, std::allocator<char> >	fIStringStream;

// OStringStream
typedef std::basic_ostringstream< char, std::char_traits<char>, std::allocator<char> >	pOStringStream;
typedef std::basic_ostringstream< char, std::char_traits<char>, std::allocator<char> >	tOStringStream;
typedef std::basic_ostringstream< char, std::char_traits<char>, std::allocator<char> >	fOStringStream;

// STL
template<typename T> using rVector = class std::vector < T, std::allocator<T> >;
template<typename T> using pVector = class std::vector < T, std::allocator<T> >;
template<typename T> using tVector = class std::vector < T, std::allocator<T> >;
template<typename T> using fVector = class std::vector < T, std::allocator<T> >;

template<typename T> using pDeque = class std::deque < T, std::allocator<T> >;
template<typename T> using tDeque = class std::deque < T, std::allocator<T> >;
template<typename T> using fDeque = class std::deque < T, std::allocator<T> >;

template<typename T, typename U, typename V = std::less<T>> using rMap = std::map < T, U, V, std::allocator< std::pair< const T, U > > >;
template<typename T, typename U, typename V = std::less<T>> using pMap = std::map < T, U, V, std::allocator< std::pair< const T, U > > >;
template<typename T, typename U, typename V = std::less<T>> using tMap = std::map < T, U, V, std::allocator< std::pair< const T, U > > >;
template<typename T, typename U, typename V = std::less<T>> using fMap = std::map < T, U, V, std::allocator< std::pair< const T, U > > >;

template<typename T, typename V = std::less<T>> using pSet = std::set < T, V, std::allocator< T > >;
template<typename T, typename V = std::less<T>> using tSet = std::set < T, V, std::allocator< T > >;
template<typename T, typename V = std::less<T>> using fSet = std::set < T, V, std::allocator< T > >;

template<typename Key, typename Value, typename H = std::hash<Key>> using pUnorderedMap = std::unordered_map < Key, Value, H, std::equal_to<Key>, std::allocator< std::pair< const Key, Value > > >;
template<typename Key, typename Value, typename H = std::hash<Key>> using tUnorderedMap = std::unordered_map < Key, Value, H, std::equal_to<Key>, std::allocator< std::pair< const Key, Value > > >;
template<typename Key, typename Value, typename H = std::hash<Key>> using fUnorderedMap = std::unordered_map < Key, Value, H, std::equal_to<Key>, std::allocator< std::pair< const Key, Value > > >;

template<typename Key, typename Hash = std::hash<Key>, typename Pred = std::equal_to<Key>> using pUnorderedSet = std::unordered_set < Key, Hash, Pred, std::allocator< Key > >;
template<typename Key, typename Hash = std::hash<Key>, typename Pred = std::equal_to<Key>> using tUnorderedSet = std::unordered_set < Key, Hash, Pred, std::allocator< Key > >;
template<typename Key, typename Hash = std::hash<Key>, typename Pred = std::equal_to<Key>> using fUnorderedSet = std::unordered_set < Key, Hash, Pred, std::allocator< Key > >;

#define rAlloc( Type, count )		(Type*) malloc( sizeof(Type) * (count) )
#define rNew( Type, ... )			new Type(__VA_ARGS__)
#define rNewArray( Type, count )	new Type[(count)]
#define rMalloc( count )			malloc( (count) )
#define rFree( ptr )				free( (void*) ptr )
#define rDelete( ptr )				delete ptr
#define rDeleteArray( ptr )			delete [] ptr

#define pAlloc( Type, count )		rAlloc( Type, (count) )
#define pNew( Type, ... )			rNew( Type, __VA_ARGS__ )
#define pNewArray( Type, count )	rNewArray( Type, (count) )
#define pMalloc( count )			rMalloc( (count) )
#define pFree( ptr )				rFree( (ptr) )
#define pDelete( ptr )				rDelete( (ptr) )
#define pDeleteArray( ptr )			rDeleteArray( (ptr) )

#define tAlloc( Type, count )		rAlloc( Type, (count) )
#define tNew( Type, ... )			rNew( Type, __VA_ARGS__ )
#define tNewArray( Type, count )	rNewArray( Type, (count) )
#define tMalloc( count )			rMalloc( (count) )
#define tFree( ptr )				rFree( (ptr) )
#define tDelete( ptr )				rDelete( (ptr) )
#define tDeleteArray( ptr )			rDeleteArray( (ptr) )

#define fAlloc( Type, count )		rAlloc( Type, (count) )
#define fNew( Type, ... )			rNew( Type, __VA_ARGS__ )
#define fNewArray( Type, count )	rNewArray( Type, (count) )
#define fMalloc( count )			rMalloc( (count) )
#define fFree( ptr )				rFree( (ptr) )
#define fDelete( ptr )				rDelete( (ptr) )
#define fDeleteArray( ptr )			rDeleteArray( (ptr) )
