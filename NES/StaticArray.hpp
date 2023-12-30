#pragma once

#include "StaticArray.h"

namespace ninmuse
{
	template<typename ElementType, size_t NumElements>
	inline constexpr StaticArray<ElementType, NumElements>::StaticArray( std::initializer_list<ElementType> initializerList ) noexcept
	{
		NM_ASSERT( NumElements > initializerList.size(), "Given initializer list has more elements than the static array!!" );

		size_t index = 0;
		for ( const ElementType& element : initializerList )
		{
			mData[index] = element;
			++index;
		}
	}

	template<typename ElementType, size_t NumElements>
	inline constexpr StaticArray<ElementType, NumElements>& StaticArray<ElementType, NumElements>::operator=( std::initializer_list<ElementType> initializerList ) noexcept
	{
		NM_ASSERT( NumElements > initializerList.size(), "Given initializer list has more elements than the static array!!" );

		size_t index = 0;
		for ( const ElementType& element : initializerList )
		{
			mData[index] = element;
			++index;
		}
	}
}