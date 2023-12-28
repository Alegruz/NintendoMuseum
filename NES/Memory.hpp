#pragma once

#include "NES/Memory.h"
#include "NES/StaticArray.hpp"

namespace ninmuse
{
	inline ConsecutiveMemory8BitView::ConsecutiveMemory8BitView( const ConsecutiveMemory<nes::data_t>& memory, const nes::address_t startAddress, const size_t size ) noexcept
		: mData( *memory.GetData(), startAddress, size )
	{
	}
}