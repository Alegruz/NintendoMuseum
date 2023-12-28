#include "stdafx.h"

#include "NES/Cartridge.h"
#include "NES/Nes.h"

namespace ninmuse
{
    namespace nes
    {
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
            ReadCartridge();
        }

        void Nes::TurnOff() noexcept
        {
        }

        bool Nes::ReadCartridge() noexcept
        {
            if ( mCartridgeOrNull == nullptr )
            {
                return false;
            }
            
            mCartridgeOrNull->Read();
            return true;
        }

        void Nes::LoadProgramRom() noexcept
        {
            if ( mCartridgeOrNull == nullptr )
            {
                return;
            }

            //const Cartridge::ProgramRom& prgRom = mCartridgeOrNull->GetProgramRom();
        }
    }
}
