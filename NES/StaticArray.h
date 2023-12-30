#pragma once

#include "NES/Common.h"
#include "NES/IArray.h"

namespace ninmuse
{
	template <typename ElementType, size_t NumElements>
	class StaticArray final : public IArray<ElementType>
	{
	public:
		constexpr StaticArray() = default;
		StaticArray( const StaticArray& other ) = delete;
		constexpr StaticArray( StaticArray&& other ) = default;
		constexpr StaticArray( std::initializer_list<ElementType> initializerList ) noexcept;
		~StaticArray() = default;
		
		StaticArray& operator=( const StaticArray & other ) = delete;
		constexpr StaticArray& operator=( StaticArray && other ) = default;
		constexpr StaticArray& operator=( std::initializer_list<ElementType> initializerList ) noexcept;

	public:
		// Element Access
		inline constexpr ElementType*		GetData() noexcept override { return mData; }
		inline constexpr const ElementType*	GetData() const noexcept override { return mData; }

		// Capacities
		inline constexpr size_t				GetSize() const noexcept override { return NumElements; }

	private:
		ElementType mData[NumElements];
	};
}