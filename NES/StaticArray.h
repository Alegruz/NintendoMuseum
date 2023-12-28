#pragma once

#include "NES/Common.h"
#include "NES/IArray.h"

namespace ninmuse
{
	template <typename ElementType, size_t NumElements>
	class StaticArray final : IArray<ElementType>
	{
	public:
		StaticArray() = default;
		StaticArray( const StaticArray& other ) noexcept;
		StaticArray( StaticArray&& other ) noexcept;
		~StaticArray() = default;
		
		StaticArray& operator=( const StaticArray & other ) noexcept;
		StaticArray& operator=( StaticArray && other ) noexcept;
	private:
		ElementType mData[NumElements];
	};
}