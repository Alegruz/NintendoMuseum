#include "stdafx.h"

#include "NES/Cartridge.h"
#include "NES/Nes.h"

namespace ninmuse
{
    namespace nes
    {
		Nes::Nes()
			: mCartridgeOrNull()
			, mMemoryMap()
			, mCpu( mMemoryMap, nullptr )
        {
        }

        void Nes::InsertCartridge( std::unique_ptr<Cartridge>&& cartridge ) noexcept
        {
            if ( mCartridgeOrNull != nullptr )
            {
                mCartridgeOrNull.release();
            }

            mCartridgeOrNull = std::move( cartridge );
        }

		void Nes::TurnOn() noexcept
		{
			readCartridge();
            mCpu.SetRom( *mCartridgeOrNull );

            mCpu.Run();
        }

        void Nes::TurnOff() noexcept
        {
        }

        bool Nes::readCartridge() noexcept
        {
            if ( mCartridgeOrNull == nullptr )
            {
                return false;
            }
            
            mCartridgeOrNull->Read();

            return true;
        }

        void Nes::loadProgramRom() noexcept
        {
            if ( mCartridgeOrNull == nullptr )
            {
                return;
            }

            //const Cartridge::ProgramRom& prgRom = mCartridgeOrNull->GetProgramRom();
        }
    }
}
