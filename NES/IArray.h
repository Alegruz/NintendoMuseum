#pragma once

#include "Common.h"

namespace ninmuse
{
	template <typename ElementType>
	class IArray
	{
	public:
		IArray() = default;
		IArray( const IArray& ) = default;
		IArray( IArray&& ) = default;
		virtual ~IArray() = default;

		IArray& operator=( const IArray& ) = default;
		IArray& operator=( IArray&& ) = default;

	public:
		// Element Access
		constexpr ElementType&					GetElementAt( const size_t index ) noexcept { return GetData()[index]; }
		constexpr const ElementType&			GetElementAt( const size_t index ) const noexcept { return GetData()[index]; }
		constexpr ElementType&					operator[]( const size_t index ) noexcept { assert( index < GetSize() ); return GetElementAt( index ); }
		constexpr const ElementType&			operator[]( const size_t index ) const noexcept { assert( index < GetSize() ); return GetElementAt( index ); }

		virtual constexpr ElementType*			GetData() noexcept = 0;
		virtual constexpr const ElementType*	GetData() const noexcept = 0;

		// Iterators
		inline constexpr ElementType*			begin() noexcept { return GetData(); }
		inline constexpr const ElementType*		begin() const noexcept { return GetData(); }
		inline constexpr ElementType*			end() noexcept { return GetData() + GetSize(); }
		inline constexpr const ElementType*		end() const noexcept { return GetData() + GetSize(); }

		// Capacities
		[[nodiscard]] inline constexpr bool		IsEmpty() const noexcept { return GetSize() == 0; }
		virtual constexpr size_t				GetSize() const noexcept = 0;
	};
}