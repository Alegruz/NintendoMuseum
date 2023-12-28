#pragma once

#include "Common.h"

#include "NES/Cpu.h"
#include "NES/Memory.h"

namespace ninmuse
{
	namespace nes
	{
		class Cartridge;

		class Nes final
		{
		public:
			Nes() = default;
			~Nes() = default;

			void	InsertCartridge( std::unique_ptr<Cartridge>&& cartridge ) noexcept;

			void	TurnOn() noexcept;
			void	TurnOff() noexcept;

		private:
			bool	ReadCartridge() noexcept;
			void	LoadProgramRom() noexcept;

		private:
			CpuNes						mCpu;
			std::unique_ptr<Cartridge>	mCartridgeOrNull;
			MemoryMap					mMemoryMap;
		};
	}
}