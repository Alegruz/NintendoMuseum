#include "stdafx.h"

#include "NES/Memory.hpp"

namespace ninmuse
{
	namespace nes
	{
		MemoryMap::MemoryMap() noexcept
			: ConsecutiveMemory<data_t>()
			, mRam( *this, 0, 0x07ff - 0x0000 )
			, mRom( *this, 0x4020, 0xFFFF - 0x4020 )
		{
		}
	}
}
