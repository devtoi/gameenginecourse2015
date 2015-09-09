#pragma once

#include "BaseCollection.h"
#include "ComponentCollection.h"

namespace CollectionHelper {
	template <typename... Components>
	static rVector<BaseCollection*> CreateComponentTypeIndices( ) {
		pVector<BaseCollection*> collections;
		collections = { pNew( ComponentCollection<Components> )... };
		for ( short i = 0; i < static_cast<short>( collections.size( ) ); ++i ) {
			collections.at( i )->SetComponentTypeIndex( i );
		}
		return collections;
	}
}
