#include "stdafx.h"

#include "NES/Memory.hpp"

namespace ninmuse
{
	namespace nes
	{
		NesRam::NesRam() noexcept
			: IRam<data_t>( std::numeric_limits<address_t>::max() )
			, mRam( mMemory, RAM_ADDRESS, RAM_SIZE )
			, mRamMirrors()
			, mPpuRegisters( mMemory, PPU_REGISTERS_ADDRESS, PPU_REGISTERS_SIZE )
			, mPpuRegistersMirrors()
			, mApuAndIoRegisters( mMemory, APU_AND_IO_REGISTERS_ADDRESS, APU_AND_IO_REGISTERS_SIZE )
			, mDisabledApuAndIo( mMemory, DISABLED_APU_AND_IO_ADDRESS, DISABLED_APU_AND_IO_SIZE )
			, mCartridge( mMemory, CARTRIDGE_ADDRESS, CARTRIDGE_SIZE )
		{
			mRamMirrors.SetCapacity( NUM_RAM_MIRRORS );
			for ( size_t i = 0; i < NUM_RAM_MIRRORS; ++i )
			{
				ConsecutiveMemory8BitView ramMirror( mMemory, RAM_MIRROR_ADDRESSES[i], RAM_MIRROR_SIZE );
				mRamMirrors.PushBack( std::move( ramMirror ) );
			}

			mPpuRegistersMirrors.SetCapacity( NUM_PPU_REGISTERS_MIRRORS );
			address_t ppuMirrorAddress = PPU_REGISTERS_MIRRORS_ADDRESS;
			for ( size_t i = 0; i < NUM_PPU_REGISTERS_MIRRORS && ppuMirrorAddress < APU_AND_IO_REGISTERS_ADDRESS; ++i, ppuMirrorAddress += PPU_REGISTERS_MIRROR_SIZE )
			{
				ConsecutiveMemory8BitView ppuRegisterMirror( mMemory, ppuMirrorAddress, PPU_REGISTERS_MIRROR_SIZE );
				mPpuRegistersMirrors.PushBack( std::move( ppuRegisterMirror ) );
			}
		}
	}
}
