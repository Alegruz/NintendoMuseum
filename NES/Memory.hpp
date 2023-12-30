#pragma once

#include "NES/Memory.h"
#include "NES/DynamicArray.hpp"
#include "NES/StaticArray.hpp"

namespace ninmuse
{
	template<Data TData>
	inline IRam<TData>::IRam( const size_t capacity ) noexcept
		: mMemory()
	{
		mMemory.GetData().SetSize( capacity );
	}

	template<Array<nes::data_t> TArray>
	inline ConsecutiveMemory8BitView::ConsecutiveMemory8BitView( ConsecutiveMemory<nes::data_t, TArray>& memory, const nes::address_t startAddress, const size_t size ) noexcept
		: IConsecutiveMemory()
		, mData( memory.GetData(), startAddress, size )
	{
	}
}