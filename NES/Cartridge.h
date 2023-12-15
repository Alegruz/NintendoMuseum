#pragma once

#include <fstream>

namespace ninmuse
{
	namespace nes
	{
		enum class eTvSystemType : uint8_t
		{
			NTSC	= 0b0,
			PAL		= 0b1,
			COUNT,
		};
		static_assert(static_cast<size_t>(eTvSystemType::COUNT) == 2);

		class Cartridge final
		{
		public:
			struct Header
			{
			public:

			public:
				static constexpr const size_t	SIZE = 16;
				static constexpr const size_t	ID_INDEX = 0;
				static constexpr const size_t	ID_SIZE = 4;
				static constexpr const size_t	PRG_ROM_SIZE_INDEX = ID_INDEX + ID_SIZE;
				static constexpr const size_t	PRG_ROM_SIZE_SIZE = 1;
				static constexpr const size_t	CHR_ROM_SIZE_INDEX = PRG_ROM_SIZE_INDEX + PRG_ROM_SIZE_SIZE;
				static constexpr const size_t	CHR_ROM_SIZE_SIZE = 1;
				static constexpr const size_t	FLAGS6_INDEX = CHR_ROM_SIZE_INDEX + CHR_ROM_SIZE_SIZE;
				static constexpr const size_t	FLAGS6_SIZE = 1;
				static constexpr const size_t	FLAGS7_INDEX = FLAGS6_INDEX + FLAGS6_SIZE;
				static constexpr const size_t	FLAGS7_SIZE = 1;
				static constexpr const size_t	FLAGS8_INDEX = FLAGS7_INDEX + FLAGS7_SIZE;
				static constexpr const size_t	FLAGS8_SIZE = 1;
				static constexpr const size_t	FLAGS9_INDEX = FLAGS8_INDEX + FLAGS8_SIZE;
				static constexpr const size_t	FLAGS9_SIZE = 1;
				static constexpr const size_t	FLAGS10_INDEX = FLAGS9_INDEX + FLAGS9_SIZE;
				static constexpr const size_t	FLAGS10_SIZE = 1;
				static constexpr const uint8_t	NES_2_0_ID = 0b10;
				static constexpr const char		VALID_ID[4] = { 0x4E, 0x45, 0x53, 0x1A };

			public:
				char	Identification[4];	// Identification String. Must be "NES<EOF>".
				union PrgRomSize			//	PRG-ROM size LSB
				{
					uint8_t	Value;			// Size of PRG ROM in 16 KB units
					uint8_t	ValueLow;		// PRG-ROM size LSB
				} PrgRomSize;
				union ChrRomSize
				{
					uint8_t Value;			// Size of CHR ROM in 8 KB units (value 0 means the board uses CHR RAM)
					uint8_t ValueLow;		// CHR-ROM size LSB
				} ChrRomSize;
				union Flags6
				{
					char Value;
					struct _
					{
						uint8_t	MapperD03				: 1;	// Mapper Number D3..D0
						uint8_t	MapperD02				: 1;
						uint8_t	MapperD01				: 1;
						uint8_t	MapperD00				: 1;
						uint8_t	MirroringMode			: 1;	// Hard-Wired Mirroring. Ignore mirroring control or above mirroring bit; instead provide four-screen VRAM
																//	0: horizontal (vertical arrangement) (CIRAM A10 = PPU A11)
																//	1: vertical(horizontal arrangement) (CIRAM A10 = PPU A10)
						bool	Trainer					: 1;	// 512-byte Trainer
																//	0: Not present
																//	1: Present between Header and PRG - ROM data
																//		512-byte trainer at $7000-$71FF (stored before PRG data)
						bool	Battery					: 1;	// "Battery" and other non-volatile memory
																//	0: Not present
																//	1: Present
																//		Cartridge contains battery-backed PRG RAM ($6000-7FFF) or other persistent memory
						uint8_t	MirroringType			: 1;	// Hard-wired nametable mirroring type
																//	0: Horizontal (vertical arrangement) or mapper-controlled (CIRAM A10 = PPU A11)
																//	1: Vertical(horizontal arrangement) (CIRAM A10 = PPU A10)
					} Bits;
				} Flags6;
				union Flags7
				{
					char Value;
					struct _
					{
						uint32_t	MapperD07	: 1;	// Mapper Number D7..D4
						uint32_t	MapperD06	: 1;
						uint32_t	MapperD05	: 1;
						uint32_t	MapperD04	: 1;
						uint32_t	NES2_0Id	: 2;	// NES 2.0 identifier. If equal to 2, flags 8-15 are in NES 2.0 format
						uint32_t	ConsoleType : 2;	// Console type
													//	0: Nintendo Entertainment System/Family Computer
													//	1: Nintendo Vs.System
													//	2: Nintendo Playchoice 10. (8 KB of Hint Screen data stored after CHR data)
													//	3: Extended Console Type
					} Bits;
				} Flags7;
				union Flags8
				{
					char Value;
					struct _
					{
						uint8_t	PrgRamSize;	// PRG RAM size
					} NESBits;
					struct __
					{
						uint32_t	SubmapperNumber : 4;
						uint32_t	MapperD11		: 1;
						uint32_t	MapperD10		: 1;
						uint32_t	MapperD09		: 1;
						uint32_t	MapperD08		: 1;
					} NES2_0Bits;
				} Flags8;
				union Flags9
				{
					char Value;
					struct _
					{
						uint8_t			Reserved : 7;	// PRG RAM size
						eTvSystemType	TvSystem : 1;	// 0: NTSC; 1: PAL
					} NESBits;
					struct __
					{
						uint32_t	ChrRomSizeHigh : 4;
						uint32_t	PrgRomSizeHigh : 4;
					} NES2_0Bits;
				} Flags9;
				union Flags10
				{
					char Value;
					struct _
					{
					private:
						uint8_t			mPadding0		: 2;
					public:
						bool			HasBusConflict	: 1;
						bool			PrgRam			: 1;
					private:
						uint8_t			mPadding1		: 2;
					public:
						uint8_t			TvSystem		: 2;
					} NESBits;
					struct __
					{
						uint32_t	PrgNvramEepromShiftCount	: 4;
						uint32_t	PrgRamShiftCount			: 4;
					} NES2_0Bits;
				} Flags10;
			};

		public:
			Cartridge() = delete;
			Cartridge(const Cartridge&) = delete;
			Cartridge(Cartridge&&) = delete;
			Cartridge(const std::filesystem::path& romFilePath) noexcept;
			~Cartridge();

			Cartridge& operator=(const Cartridge&) = delete;
			Cartridge& operator=(Cartridge&&) = delete;

		public:
			const Header& ReadHeader() noexcept;

		private:
			std::filesystem::path	mRomFilePath;
			std::ifstream			mRomFile;
			Header					mHeader;
		};
	}
}