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
			Nes();
			~Nes() = default;

			void	InsertCartridge( std::unique_ptr<Cartridge>&& cartridge ) noexcept;

			void	TurnOn() noexcept;
			void	TurnOff() noexcept;

		private:
			void					loadProgramRom() noexcept;
			bool					readCartridge() noexcept;

		private:
			std::unique_ptr<Cartridge>	mCartridgeOrNull;
			NesRam						mMemoryMap;
			CpuNes						mCpu;
		};
	}
}