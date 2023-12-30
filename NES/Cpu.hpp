#pragma once

#include "NES/Cpu.h"
#include "NES/DynamicArray.hpp"

namespace ninmuse
{
	template<Data TData, Address TAddress>
	inline constexpr const TData& ICpu<TData, TAddress>::Read( const TAddress& address ) const noexcept
	{
		NM_ASSERT( address < mRam.GetMemory().GetData().GetSize(), "Invalid address!!");
		const TData& data = mRam.GetMemory().GetData()[address];
		return data;
	}

	template<Data TData, Address TAddress>
	inline constexpr void ICpu<TData, TAddress>::Write( const TAddress& address, const TData& data ) noexcept
	{
		NM_ASSERT( address < mRam.GetMemory().GetData().GetSize(), "Invalid address!!" );
		mRam.GetMemory().GetData()[address] = data;
	}
}