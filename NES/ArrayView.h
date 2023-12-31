#pragma once

#include "NES/IArray.h"

namespace ninmuse
{
	template <typename ElementType>
	class ArrayView final : IArray<ElementType>
	{
	public:
		ArrayView() = delete;
		inline constexpr ArrayView( IArray<ElementType>& originalArray, const size_t startIndex, const size_t size ) noexcept
			: mSize( size )
			, mData( originalArray.GetData() + startIndex )
		{}
		ArrayView( const ArrayView& ) = delete;
		explicit ArrayView( ArrayView&& ) noexcept = default;
		~ArrayView() = default;

		ArrayView& operator=( const ArrayView& ) = delete;
		ArrayView& operator=( ArrayView&& ) noexcept = default;

	public:
		inline constexpr ElementType*		GetData() noexcept override { return mData; }
		inline constexpr const ElementType* GetData() const noexcept override { return mData; }

		// Capacities
		constexpr size_t					GetSize() const noexcept override { return mSize; }

	private:
		size_t				mSize;
		ElementType*		mData;
	};
}