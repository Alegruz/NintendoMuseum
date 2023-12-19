#pragma once

#include "Common.h"

namespace ninmuse
{

	class Cpu
	{
	public:
	private:
	};

	namespace nes
	{

		class Cpu6502 : public Cpu
		{
		public:
			struct Pins
			{
			};
		private:
		};

		class CpuNes : public Cpu6502
		{
		public:
			// [REF]: https://www.nesdev.org/wiki/CPU_pinout
			struct NesPins : public Cpu6502::Pins
			{
			public:
				bool	AudioOut1				: 1 = false;	// Both pulse waves
				bool	AudioOut2				: 1 = false;	// Triangle, noise, and DPCM
				bool	ResetBar				: 1 = true;		// When low, holds CPU in reset state, during which all CPU pins (except pin 2) are in high impedance state.
																// When released, CPU starts executing code (read $FFFC, read $FFFD, ...) after 6 M2 clocks.
				bool	Adress0					: 1 = false;	// Address bus.
																// Address holds the target address during the entire read/write cycle.
				bool	Adress1					: 1 = false;
				bool	Adress2					: 1 = false;
				bool	Adress3					: 1 = false;
				bool	Adress4					: 1 = false;
				bool	Adress5					: 1 = false;
				bool	Adress6					: 1 = false;
				bool	Adress7					: 1 = false;
				bool	Adress8					: 1 = false;
				bool	Adress9					: 1 = false;
				bool	Adress10				: 1 = false;
				bool	Adress11				: 1 = false;
				bool	Adress12				: 1 = false;
				bool	Adress13				: 1 = false;
				bool	Adress14				: 1 = false;
				bool	Adress15				: 1 = false;
				bool	Ground					: 1 = false;
				bool	Data7					: 1 = false;	// Data bus.
																// For reads, the value is read from Dx during ¥õ2.
				bool	Data6					: 1 = false;	// For writes, the value appears on Dx during ¥õ2 (and no sooner).
				bool	Data5					: 1 = false;
				bool	Data4					: 1 = false;
				bool	Data3					: 1 = false;
				bool	Data2					: 1 = false;
				bool	Data1					: 1 = false;
				bool	Data0					: 1 = false;
				bool	Clock					: 1 = false;	// 21.47727 MHz (NTSC) or 26.6017 MHz (PAL) clock input.
																// Internally, this clock is divided by 12 (NTSC 2A03) or 16 (PAL 2A07) to feed the 6502's clock input ¥õ0,
																// which is in turn inverted to form ¥õ1, which is then inverted to form ¥õ2.
																// ¥õ1 is high during the first phase (half-cycle) of each CPU cycle, while ¥õ2 is high during the second phase.
				bool	Test					: 1 = false;	// Pin 30 is special: normally it is grounded in the NES, Famicom, PC10/VS. NES and other Nintendo Arcade Boards (Punch-Out!! and Donkey Kong 3).
																// But if it is pulled high on the RP2A03G, extra diagnostic registers to test the sound hardware are enabled from $4018 through $401A,
																// and the joystick ports $4016 and $4017 become open bus.
																// On the RP2A07 (and presumably also the RP2A03E),
																// pulling pin 30 high instead causes the CPU to stop execution by means of activating the embedded 6502's RDY input.
				bool	M2						: 1 = false;	// Can be considered as a "signals ready" pin.
																// It is a modified version the 6502's ¥õ2 (which roughly corresponds to the CPU input clock ¥õ0) that allows for slower ROMs.
																// CPU cycles begin at the point where M2 goes low.
																	// In the NTSC 2A03E, G, and H, M2 has a duty cycle of 15/24 (5/8), or 350ns/559ns.
																	//	Equivalently, a CPU read (which happens during the second, high phase of M2) takes 1 and 7/8th PPU cycles.
																	//	The internal ¥õ2 duty cycle is exactly 1/2 (one half).
																	// In the PAL 2A07, M2 has a duty cycle of 19 / 32, or 357ns / 601ns, or 1.9 out of 3.2 pixels.
																	// In the original NTSC 2A03(no letter), M2 has a duty cycle of 17 / 24, or 396ns / 559ns, or 2 and 1 / 8th pixels.
				bool	InterruptRequestBar		: 1 = true;		// Interrupt pin.
				bool	NonMaskableInterruptBar : 1 = true;		// Non-maskable interrupt pin.
				bool	ReadWrite				: 1 = false;	// Read/write signal, which is used to indicate operations of the same names.
																// Low is write. R/W stays high/low during the entire read/write cycle.
				bool	ControllerBar2			: 1 = true;		// Controller ports (for controller #1 and #2 respectively).
				bool	ControllerBar1			: 1 = true;		// Each enable the output of their respective controller, if present.
				bool	Output2					: 1 = false;	// Output pins used by the controllers ($4016 output latch bits 0-2).
				bool	Output1					: 1 = false;	// These 3 pins are connected to either the NES or Famicom's expansion port,
				bool	Output0					: 1 = false;	// and OUT0 is additionally used as the "strobe" signal (OUT) on both controller ports.
				bool	SupplyVoltage			: 1 = false;
			};
			static_assert(sizeof(NesPins) == 5, "There are 40 pins in 2A03/2A07");

		protected:
			struct RegisterMap final
			{
				static constexpr address_t	Sequence1Volume = 0x4000;
				static constexpr address_t	Sequence1Sweep = 0x4001;
				//static constexpr address_t
			};
		};
	}
}