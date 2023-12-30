#pragma once

#include "DynamicArray.h"
#include "IArray.h"

namespace ninmuse
{
	template<typename ElementType>
	inline constexpr DynamicArray<ElementType>::DynamicArray() noexcept
		: DynamicArray( DEFAULT_CAPACITY )
	{
	}

	template<typename ElementType>
	inline DynamicArray<ElementType>::DynamicArray( size_t capacity ) noexcept
		: IArray<ElementType>()
		, mCapacity( capacity )
		, mSize( 0 )
		, mData( nullptr )
	{
		SetCapacity( mCapacity );
	}

	template<typename ElementType>
	inline DynamicArray<ElementType>::DynamicArray( const DynamicArray& other ) noexcept
		: IArray<ElementType>( other )
		, mCapacity( other.mCapacity )
		, mSize( other.mSize )
	{
		if ( this != &other )
		{
			SetCapacity( mCapacity );
		}
	}

	template<typename ElementType>
	inline constexpr DynamicArray<ElementType>::DynamicArray( DynamicArray&& other ) noexcept
		: IArray<ElementType>( std::move( other ) )
		, mCapacity( other.mCapacity )
		, mSize( other.mSize )
		, mData( other.mData )
	{
		if ( this != &other )
		{
			other.mCapacity = 0;
			other.mSize = 0;
			other.mData = nullptr;
		}

		return *this;
	}

	template<typename ElementType>
	inline DynamicArray<ElementType>::~DynamicArray() noexcept
	{
		if ( mData != nullptr )
		{
			free( mData );
		}
	}

	template<typename ElementType>
	inline DynamicArray<ElementType>& DynamicArray<ElementType>::operator=( const DynamicArray& other ) noexcept
	{
		IArray::operator=( other );

		if ( this != &other )
		{
			mSize = other.mSize;
			SetCapacity( other.mCapacity );
		}

		return *this;
	}

	template<typename ElementType>
	inline constexpr DynamicArray<ElementType>& DynamicArray<ElementType>::operator=( DynamicArray&& other ) noexcept
	{
		IArray::operator=( std::move( other ) );

		if ( this != &other )
		{
			mCapacity = other.mCapacity;
			mSize = other.mSize;
			mData = other.mData;

			other.mCapacity = 0;
			other.mSize = 0;
			other.mData = nullptr;
		}

		return *this;
	}

	template<typename ElementType>
	inline void DynamicArray<ElementType>::SetSize( const size_t zSize ) noexcept
	{
		const bool needsReallocation = mData == nullptr || mCapacity < zSize;

		if ( needsReallocation == true )
		{
			ElementType* paData = new ElementType[zSize];
			if ( mData != nullptr )
			{
				memcpy( paData, mData, sizeof( ElementType ) * mSize );
				free( mData );
			}

			mCapacity = zSize;
			mSize = zSize;
			mData = paData;
		}
	}

	template<typename ElementType>
	inline void DynamicArray<ElementType>::SetCapacity( size_t capacity ) noexcept
	{
		const bool needsReallocation = mData == nullptr || mCapacity < capacity;

		if ( needsReallocation == true )
		{
			ElementType* paData = static_cast<ElementType*>( malloc( sizeof( ElementType ) * capacity ) );
			if ( paData == nullptr )
			{
				NM_ASSERT( false, "Failed to allocate an array!!" );
				return;
			}

			if ( mData != nullptr )
			{
				memcpy( paData, mData, sizeof( ElementType ) * mSize );
				free( mData );
			}

			mCapacity = capacity;
			mData = paData;
		}
	}

	template<typename ElementType>
	inline constexpr void DynamicArray<ElementType>::Clear() noexcept
	{
		for ( size_t zDataIndex = 0; zDataIndex < mSize; ++zDataIndex )
		{
			mData[zDataIndex].~ElementType();
		}

		mSize = 0;
	}

	template<typename ElementType>
	inline constexpr void DynamicArray<ElementType>::PushBack( const ElementType& value ) noexcept
	{
		if ( mSize >= mCapacity )
		{
			SetCapacity( mCapacity * 2 );
		}

		mData[mSize++] = value;
	}

	template<typename ElementType>
	inline constexpr void DynamicArray<ElementType>::PushBack( ElementType&& value ) noexcept
	{
		if ( mSize >= mCapacity )
		{
			SetCapacity( mCapacity * 2 );
		}

		mData[mSize++] = std::move( value );
	}

	template<typename ElementType>
	inline constexpr void DynamicArray<ElementType>::PopBack() noexcept
	{
		mData[--mSize].~ElementType();
	}
}