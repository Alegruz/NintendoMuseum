#pragma once

#include "NES/IArray.h"

namespace ninmuse
{
	template <typename ElementType>
	class DynamicArray final : public IArray<ElementType>
	{
	public:
		constexpr DynamicArray() noexcept;
		DynamicArray( size_t capacity ) noexcept;
		DynamicArray( const DynamicArray& other ) noexcept;
		constexpr DynamicArray( DynamicArray&& other ) noexcept;
		~DynamicArray() noexcept;

		DynamicArray& operator=( const DynamicArray& other ) noexcept;
		constexpr DynamicArray& operator=( DynamicArray&& other ) noexcept;

		// Element Access
		constexpr ElementType*			GetData() noexcept override { return mData; }
		constexpr const ElementType*	GetData() const noexcept override { return mData; }

		// Capacities
		void				SetSize( const size_t zSize ) noexcept;
		inline constexpr size_t	GetSize() const noexcept override { return mSize; }
		void				SetCapacity( size_t zCapacity ) noexcept;
		inline constexpr size_t	GetCapacity() const noexcept { return mCapacity; }

		// Modifiers
		constexpr void		Clear() noexcept;
		constexpr void		PushBack( const ElementType& value ) noexcept;
		constexpr void		PushBack( ElementType&& value ) noexcept;
		constexpr void		PopBack() noexcept;

	private:
		static constexpr const size_t DEFAULT_CAPACITY = 8;

	private:
		size_t			mCapacity;
		size_t			mSize;
		ElementType*	mData;
	};
}