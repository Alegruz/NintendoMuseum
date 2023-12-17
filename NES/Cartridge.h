#pragma once

#include <fstream>
#include <vector>

namespace ninmuse
{
	namespace nes
	{
		// Types
		enum eConsoleType : uint8_t
		{
			NES_FAMICOM = 0b00,
			NINTENDO_VS_SYSTEM,
			NINTENDO_PLAYCHOICE_10,
			EXTENDED_CONSOLE_TYPE,
			COUNT,
		};
		static_assert( static_cast< size_t >( eConsoleType::COUNT ) == 4 );

		enum class eTvSystemType : uint8_t
		{
			NTSC = 0b0,
			PAL = 0b1,
			COUNT,
		};
		static_assert( static_cast< size_t >( eTvSystemType::COUNT ) == 2 );

		enum class eCpuPpuTimingMode : uint8_t
		{
			RP2C02 = 0b00, // NTSC NES
			RP2C07,		   // Licensed PAL NES
			MULTIPLE_REGION,
			UA6538, // Dendy
			COUNT,
		};
		static_assert( static_cast< size_t >( eCpuPpuTimingMode::COUNT ) == 4 );

		// https://www.nesdev.org/wiki/Vs._System
		enum class eVsHardwareType : uint8_t
		{
			VS_UNISYSTEM_NORMAL = 0x0,
			VS_UNISYSTEM_RBI_BASEBALL_PROTECTION,
			VS_UNISYSTEM_TKO_BOXING_PROTECTION,
			VS_UNISYSTEM_SUPER_XEVIOUS_PROTECTION,
			VS_UNISYSTEM_VS_ICE_CLIMBER_JAPAN_PROTECTION,
			VS_DUALSYSTEM_NORMAL,
			VS_DUALSYSTEM_RAID_ON_BUNGELING_BAY_PROTECTION,
			COUNT
		};
		static_assert( static_cast< size_t >( eVsHardwareType::COUNT ) == 0x7 );

		// For copy protection purposes, these PPU types have different palettes;
		// the RC2C05 PPUs furthermore swap PPU registers $2000 and $2001 and return a signature in the lower bits of $2002.
		// If a game uses the DIP switches to select different PPU models,
		// this field represents the correct PPU model when those DIP switches are all set to zero.
		enum class ePpuType : uint8_t
		{
			RP2C03B = 0x0,
			RP2C03G,
			RP2C04_0001,
			RP2C04_0002,
			RP2C04_0003,
			RP2C04_0004,
			RC2C03B,
			RC2C03C,
			RC2C05_01, // ($2002 AND $?? =$1B)
			RC2C05_02, // ($2002 AND $3F =$3D)
			RC2C05_03, // ($2002 AND $1F =$1C)
			RC2C05_04, // ($2002 AND $1F =$1B)
			RC2C05_05, // ($2002 AND $1F =unknown)
			RESERVED_D,
			RESERVED_E,
			RESERVED_F,
			COUNT,
		};
		static_assert( static_cast< size_t >( ePpuType::COUNT ) == 0x10 );

		enum class eExtendedConsoleType : uint8_t
		{
			NES_FAMICOM_DENDY = 0x0,
			NINTENDO_VS_SYSTEM,
			PLAYCHOICE_10,
			FAMICLONE_WITH_DECIMAL_MODE,
			NES_FAMICOM_WITH_ESPM_MODULE_OR_PLUG_THROUGH_CARTRIDGE,
			VR_TECHNOLOGY_VT01_WITH_RED_CYAN_STN_PALETTE,
			VR_TECHNOLOGY_VT02,
			VR_TECHNOLOGY_VT03,
			VR_TECHNOLOGY_VT09,
			VR_TECHNOLOGY_VT32,
			VR_TECHNOLOGY_VT369,
			UMC_UM6578,
			FAMICOM_NETWORK_SYSTEM,
			RESERVED_D,
			RESERVED_E,
			RESERVED_F,
			COUNT,
		};
		static_assert( static_cast< size_t >( eExtendedConsoleType::COUNT ) == 0x10 );

		// Typedefs
		using tv_system_type_bits_t = uint8_t;

		// Global Variables
		constexpr const tv_system_type_bits_t TV_SYSTEM_TYPE_BITS[static_cast< uint32_t >( eTvSystemType::COUNT )] =
		{
			0b00, // NTSC
			0b10, // PAL
		};
		static_assert( ARRAYSIZE( TV_SYSTEM_TYPE_BITS ) == 2 );

		// Global Functions
		inline constexpr const char* ConsoleTypeToString( const eConsoleType consoleType ) noexcept
		{
			switch ( consoleType )
			{
			case eConsoleType::NES_FAMICOM:
				return "Nintendo Entertainment System/Family Computer";
			case eConsoleType::NINTENDO_VS_SYSTEM:
				return "Nintendo Vs. System";
			case eConsoleType::NINTENDO_PLAYCHOICE_10:
				return "Nintendo Playchoice 10";
			case eConsoleType::EXTENDED_CONSOLE_TYPE:
				return "Extended Console Type";
			default:
				assert( false );
				break;
			}

			return nullptr;
		}

		inline constexpr const char* TvSystemTypeToString( const eTvSystemType tvSystemType ) noexcept
		{
			switch ( tvSystemType )
			{
			case eTvSystemType::NTSC:
				return "NTSC";
			case eTvSystemType::PAL:
				return "PAL";
			default:
				assert( false );
				break;
			}
		}

		inline constexpr const char* CpuPpuTimingModeToString( const eCpuPpuTimingMode cpuPpuTimingMode ) noexcept
		{
			switch ( cpuPpuTimingMode )
			{
			case eCpuPpuTimingMode::RP2C02:
				return "RP2C02::NTSC";
			case eCpuPpuTimingMode::RP2C07:
				return "RP2C07::PAL";
			case eCpuPpuTimingMode::MULTIPLE_REGION:
				return "Multiple Region";
			case eCpuPpuTimingMode::UA6538:
				return "UA6538::Dendy";
			default:
				assert( false );
				break;
			}
			return nullptr;
		}

		inline constexpr bool HasTvSystemType( const tv_system_type_bits_t tvSystemTypeBits, const eTvSystemType tvSystemType ) noexcept
		{
			const size_t tvSystemTypeIndex = static_cast< size_t >( tvSystemType );
			if ( tvSystemTypeIndex < ARRAYSIZE( TV_SYSTEM_TYPE_BITS ) )
			{
				return tvSystemTypeBits | TV_SYSTEM_TYPE_BITS[tvSystemTypeIndex];
			}

			return false;
		}

		inline constexpr const char* TvSystemTypeBitsToString( const tv_system_type_bits_t tvSystemTypeBits ) noexcept
		{
			eTvSystemType tvSystemType = eTvSystemType::NTSC;
			const bool hasNtsc = HasTvSystemType( tvSystemTypeBits, eTvSystemType::NTSC );
			const bool hasPal = HasTvSystemType( tvSystemTypeBits, eTvSystemType::PAL );
			const char* ntsc = hasNtsc ? TvSystemTypeToString( eTvSystemType::NTSC ) : nullptr;
			const char* pal = hasPal ? TvSystemTypeToString( eTvSystemType::PAL ) : nullptr;

			if ( hasNtsc && hasPal )
			{
				return CONCATENATE_STR( NTSC, PAL );
			}

			if ( hasNtsc )
			{
				return ntsc;
			}

			if ( hasPal )
			{
				return pal;
			}

			assert( false );
			return nullptr;
		}

		inline constexpr const char* PpuTypeToString( const ePpuType ppuType ) noexcept
		{
			switch ( ppuType )
			{
			case ePpuType::RP2C03B:		return "RP2C03B";
			case ePpuType::RP2C03G:		return "RP2C03G";
			case ePpuType::RP2C04_0001:	return "RP2C04-0001";
			case ePpuType::RP2C04_0002:	return "RP2C04-0002";
			case ePpuType::RP2C04_0003:	return "RP2C04-0003";
			case ePpuType::RP2C04_0004:	return "RP2C04-0004";
			case ePpuType::RC2C03B:		return "RC2C03B";
			case ePpuType::RC2C03C:		return "RC2C03C";
			case ePpuType::RC2C05_01:	return "RC2C05-01";
			case ePpuType::RC2C05_02:	return "RC2C05-02";
			case ePpuType::RC2C05_03:	return "RC2C05-03";
			case ePpuType::RC2C05_04:	return "RC2C05-04";
			case ePpuType::RC2C05_05:	return "RC2C05-05";
			case ePpuType::RESERVED_D:	return "Reserved D";
			case ePpuType::RESERVED_E:	return "Reserved E";
			case ePpuType::RESERVED_F:	return "Reserved F";
			default:
				assert( false );
				break;
			}
			return nullptr;
		}

		inline constexpr const char* VsHardwareTypeToString( const eVsHardwareType vsHardwareType ) noexcept
		{
			switch ( vsHardwareType )
			{
			case eVsHardwareType::VS_UNISYSTEM_NORMAL:								return "Vs. Unisystem (normal)";
			case eVsHardwareType::VS_UNISYSTEM_RBI_BASEBALL_PROTECTION:				return "Vs. Unisystem (RBI Baseball protection)";
			case eVsHardwareType::VS_UNISYSTEM_TKO_BOXING_PROTECTION:				return "Vs. Unisystem (TKO Boxing protection)";
			case eVsHardwareType::VS_UNISYSTEM_SUPER_XEVIOUS_PROTECTION:			return "Vs. Unisystem (Super Xevious protection)";
			case eVsHardwareType::VS_UNISYSTEM_VS_ICE_CLIMBER_JAPAN_PROTECTION:		return "Vs. Unisystem (Vs. Ice Climber Japan protection)";
			case eVsHardwareType::VS_DUALSYSTEM_NORMAL:								return "Vs. Dual System (normal)";
			case eVsHardwareType::VS_DUALSYSTEM_RAID_ON_BUNGELING_BAY_PROTECTION:	return "Vs. Dual System (Raid on Bungeling Bay protection)";
			default:
				assert( false );
				break;
			}
			return nullptr;
		}

		inline constexpr const char* ExtendedConsoleTypeToString( const eExtendedConsoleType extendedConsoleType ) noexcept
		{
			switch ( extendedConsoleType )
			{
			case eExtendedConsoleType::NES_FAMICOM_DENDY:										return	"Regular NES/Famicom/Dendy";
			case eExtendedConsoleType::NINTENDO_VS_SYSTEM:										return	"Nintendo Vs. System";
			case eExtendedConsoleType::PLAYCHOICE_10:											return	"Playchoice 10";
			case eExtendedConsoleType::FAMICLONE_WITH_DECIMAL_MODE:								return	"Regular Famiclone, but with CPU that supports Decimal Mode";
			case eExtendedConsoleType::NES_FAMICOM_WITH_ESPM_MODULE_OR_PLUG_THROUGH_CARTRIDGE:	return	"Regular NES/Famicom with EPSM module or plug-through cartridge";
			case eExtendedConsoleType::VR_TECHNOLOGY_VT01_WITH_RED_CYAN_STN_PALETTE:			return	"V.R. Technology VT01 with red/cyan STN palette";
			case eExtendedConsoleType::VR_TECHNOLOGY_VT02:										return	"V.R. Technology VT02";
			case eExtendedConsoleType::VR_TECHNOLOGY_VT03:										return	"V.R. Technology VT03";
			case eExtendedConsoleType::VR_TECHNOLOGY_VT09:										return	"V.R. Technology VT09";
			case eExtendedConsoleType::VR_TECHNOLOGY_VT32:										return	"V.R. Technology VT32";
			case eExtendedConsoleType::VR_TECHNOLOGY_VT369:										return	"V.R. Technology VT369";
			case eExtendedConsoleType::UMC_UM6578:												return	"UMC UM6578";
			case eExtendedConsoleType::FAMICOM_NETWORK_SYSTEM:									return	"Famicom Network System";
			case eExtendedConsoleType::RESERVED_D:												return	"Reserved D";
			case eExtendedConsoleType::RESERVED_E:												return	"Reserved E";
			case eExtendedConsoleType::RESERVED_F:												return	"Reserved F";
			default:
				assert( false );
				break;
			}
			return nullptr;
		}

		// Classes
		class Cartridge final
		{
		public:
			struct Header
			{
			public:
				static constexpr const size_t SIZE = 16;
				static constexpr const size_t ID_INDEX = 0;
				static constexpr const size_t ID_SIZE = 4;
				static constexpr const size_t PRG_ROM_SIZE_INDEX = ID_INDEX + ID_SIZE;
				static constexpr const size_t PRG_ROM_SIZE_SIZE = 1;
				static constexpr const size_t CHARACTER_ROM_SIZE_INDEX = PRG_ROM_SIZE_INDEX + PRG_ROM_SIZE_SIZE;
				static constexpr const size_t CHARACTER_ROM_SIZE_SIZE = 1;
				static constexpr const size_t FLAGS06_INDEX = CHARACTER_ROM_SIZE_INDEX + CHARACTER_ROM_SIZE_SIZE;
				static constexpr const size_t FLAGS06_SIZE = 1;
				static constexpr const size_t FLAGS07_INDEX = FLAGS06_INDEX + FLAGS06_SIZE;
				static constexpr const size_t FLAGS07_SIZE = 1;
				static constexpr const size_t FLAGS08_INDEX = FLAGS07_INDEX + FLAGS07_SIZE;
				static constexpr const size_t FLAGS08_SIZE = 1;
				static constexpr const size_t FLAGS09_INDEX = FLAGS08_INDEX + FLAGS08_SIZE;
				static constexpr const size_t FLAGS09_SIZE = 1;
				static constexpr const size_t FLAGS10_INDEX = FLAGS09_INDEX + FLAGS09_SIZE;
				static constexpr const size_t FLAGS10_SIZE = 1;
				static constexpr const size_t FLAGS11_INDEX = FLAGS10_INDEX + FLAGS10_SIZE;
				static constexpr const size_t FLAGS11_SIZE = 1;
				static constexpr const size_t FLAGS12_INDEX = FLAGS11_INDEX + FLAGS11_SIZE;
				static constexpr const size_t FLAGS12_SIZE = 1;
				static constexpr const size_t FLAGS13_INDEX = FLAGS12_INDEX + FLAGS12_SIZE;
				static constexpr const size_t FLAGS13_SIZE = 1;
				static constexpr const size_t FLAGS14_INDEX = FLAGS13_INDEX + FLAGS13_SIZE;
				static constexpr const size_t FLAGS14_SIZE = 1;
				static constexpr const size_t FLAGS15_INDEX = FLAGS14_INDEX + FLAGS14_SIZE;
				static constexpr const size_t FLAGS15_SIZE = 1;
				static constexpr const uint8_t NES_2_0_ID = 0b10;
				static constexpr const char VALID_ID[4] = { 0x4E, 0x45, 0x53, 0x1A };

			public:
				char Identification[4] = { 0, }; // Identification String. Must be "NES<EOF>".
				union ProgramRomSize		//	PRG-ROM size LSB
				{
					uint8_t Value = 0;	  // Size of PRG ROM in 16 KB units
					uint8_t ValueLow; // PRG-ROM size LSB
				} ProgramRomSize;
				union CharacterRomSize
				{
					uint8_t Value = 0;	// Size of CHR ROM in 8 KB units (value 0 means the board uses CHR RAM)
					uint8_t ValueLow; 	// CHR-ROM size LSB
				} CharacterRomSize;
				union Flags06
				{
					char Value = 0;
					struct NESBits
					{
						uint8_t MirroringType		: 1;	// Hard-wired nametable mirroring type
															//	0: Horizontal (vertical arrangement) or mapper-controlled (CIRAM A10 = PPU A11)
															//	1: Vertical(horizontal arrangement) (CIRAM A10 = PPU A10)
						bool	Battery				: 1;	// "Battery" and other non-volatile memory
															//	0: Not present
															//	1: Present
															//		Cartridge contains battery-backed PRG RAM ($6000-7FFF) or other persistent memory
						bool	IsTrainerPresent	: 1;	// 512-byte Trainer
															//	0: Not present
															//	1: Present between Header and PRG - ROM data
															//		512-byte trainer at $7000-$71FF (stored before PRG data)
						bool	FourScreenMode		: 1;	// Hard-wired four-screen mode
															// 	0: No
															//	1: Yes
						uint8_t MapperD00			: 1;	// Mapper Number D3..D0
						uint8_t MapperD01			: 1;
						uint8_t MapperD02			: 1;
						uint8_t MapperD03			: 1;
					} Bits;
				} Flags06;
				union Flags07
				{
					char Value = 0;
					struct NESBits
					{
						eConsoleType	ConsoleType	: 2;	// Console type
															//	0: Nintendo Entertainment System/Family Computer
															//	1: Nintendo Vs.System
															//	2: Nintendo Playchoice 10. (8 KB of Hint Screen data stored after CHR data)
															//	3: Extended Console Type
						uint32_t		NES2_0Id	: 2;	// NES 2.0 identifier. If equal to 2, flags 8-15 are in NES 2.0 format
						uint32_t		MapperD04	: 1;	// Mapper Number D7..D4
						uint32_t		MapperD05	: 1;
						uint32_t		MapperD06	: 1;
						uint32_t		MapperD07	: 1;
					} Bits;
				} Flags07;
				union Flags08
				{
					char Value = 0;
					struct NESBits
					{
						uint8_t PrgRamSize; // PRG RAM size
					} NESBits;
					struct NES2_0Bits
					{
						uint32_t MapperD08			: 1;
						uint32_t MapperD09			: 1;
						uint32_t MapperD10			: 1;
						uint32_t MapperD11			: 1;
						uint32_t SubmapperNumber	: 4;
					} NES2_0Bits;
				} Flags08;
				union Flags09
				{
					char Value = 0;
					struct NESBits
					{
						eTvSystemType	TvSystem : 1;	// 0: NTSC; 1: PAL
						uint8_t			Reserved : 7;	// PRG RAM size
					} NESBits;
					struct NES2_0Bits
					{
						uint32_t PrgRomSizeHigh : 4;
						uint32_t ChrRomSizeHigh : 4;
					} NES2_0Bits;
				} Flags09;
				union Flags10
				{
					char Value = 0;
					struct NESBits
					{
					public:
						tv_system_type_bits_t	TvSystemBits	: 2;
					private:
						uint8_t					mPadding0		: 2;
					public:
						bool					PrgRam			: 1;
						bool					HasBusConflict	: 1;
					private:
						uint8_t					mPadding1		: 2;
					} NESBits;
					struct NES2_0Bits
					{
						uint32_t PrgRamShiftCount			: 4;
						uint32_t PrgNvramEepromShiftCount	: 4;
					} NES2_0Bits;
				} Flags10;
				union Flags11
				{
					char Value = 0;
					struct NES2_0Bits
					{
						// If the shift count is zero, there is no CHR-(NV)RAM.
						// If the shift count is non-zero, the actual size is
						// "64 << shift count" bytes, i.e. 8192 bytes for a shift count of 7.
						uint32_t ChrRamShiftCount	: 4;	 // CHR-RAM size (volatile) shift count
						uint32_t ChrNvramShiftCount : 4; // CHR-NVRAM size (non-volatile) shift count
					} NES2_0Bits;
				} Flags11;
				union Flags12
				{
					char Value = 0;
					struct NES2_0Bits
					{
					public:
						eCpuPpuTimingMode	CpuPpuTimingMode	: 2;
					private:
						uint8_t				mPadding			: 6;
					} NES2_0Bits;
				} Flags12;
				union Flags13
				{
					char Value = 0;
					struct NES2_0BitsVsSystem
					{
						ePpuType		PpuType			: 4;
						eVsHardwareType	VsHardwareType	: 4;
					} NES2_0BitsVsSystem;
					struct NES2_0BitsExtendedConsoleType
					{
					public:
						eExtendedConsoleType	ExtendedConsoleType : 4;
					private:
						uint8_t					mPadding			: 4;
					} NES2_0BitsExtendedConsoleType;
				} Flags13;
				union Flags14
				{
					char Value = 0;
					struct NES2_0Bits
					{
					public:
						uint8_t NumMiscellaneousRomsPresent : 2;
					private:
						uint8_t mPadding					: 6;
					} NES2_0Bits;
				} Flags14;
				union Flags15
				{
					char Value = 0;
					struct NES2_0Bits
					{
					public:
						uint8_t DefaultExpansionDevice	: 6;
					private:
						uint8_t mPadding				: 2;
					} NES2_0Bits;
				} Flags15;
			};

			struct Trainer
			{
				static constexpr const size_t DATA_SIZE = 512;
				char	Data[Trainer::DATA_SIZE];
			};

			struct ProgramRom
			{
				static constexpr const size_t PROGRAM_SIZE_UNIT = 16'384;
				union Size
				{
					uint16_t Value = 0;
					struct NESBits
					{
					public:
						uint8_t Size		: 8;
					private:
						uint8_t mPadding	: 8;
					} NESBits;
					struct NES2_0Bits
					{
					public:
						union SizeLow
						{
							uint8_t Value;
							struct ExponentMultiplier
							{
								uint8_t Multiplier	: 2;
								uint8_t Exponent	: 6;
							} ExponentMultiplier;
						} SizeLow;
						uint8_t SizeHigh	: 4;
					private:
						uint8_t mPadding	: 4;
					} NES2_0Bits;
				};
				std::vector<char>	Data;
			};

			struct CharacterRom
			{
				static constexpr const size_t PROGRAM_SIZE_UNIT = 8'192;
				union Size
				{
					uint16_t Value = 0;
					struct NESBits
					{
					public:
						uint8_t Size		: 8;
					private:
						uint8_t mPadding	: 8;
					} NESBits;
					struct NES2_0Bits
					{
					public:
						union SizeLow
						{
							uint8_t Value;
							struct ExponentMultiplier
							{
								uint8_t Multiplier	: 2;
								uint8_t Exponent	: 6;
							} ExponentMultiplier;
						} SizeLow;
						uint8_t SizeHigh	: 4;
					private:
						uint8_t mPadding	: 4;
					} NES2_0Bits;
				};
				std::vector<char>	Data;
			};

		public:
			Cartridge() = delete;
			Cartridge( const Cartridge& ) = delete;
			Cartridge( Cartridge&& ) = delete;
			Cartridge( const std::filesystem::path& romFilePath ) noexcept;
			~Cartridge();

			Cartridge& operator=( const Cartridge& ) = delete;
			Cartridge& operator=( Cartridge&& ) = delete;

		public:
			void						Read() noexcept;
			const Header&				ReadHeader() noexcept;
			const Trainer&				ReadTrainer() noexcept;
			const std::vector<char>&	ReadProgramRom() noexcept;
			const std::vector<char>&	ReadCharacterRom() noexcept;

		private:
			inline constexpr bool isNes2_0Format() const noexcept { return mHeader != nullptr && mHeader.get()->Flags07.Bits.NES2_0Id == Header::NES_2_0_ID; }
			inline constexpr bool isTvSystemType( const eTvSystemType tvSystemType ) const noexcept { return mHeader != nullptr && mHeader.get()->Flags09.NESBits.TvSystem == tvSystemType; }
			inline constexpr bool isNtsc() const noexcept { return isTvSystemType( eTvSystemType::NTSC ); }
			inline constexpr bool isPal() const noexcept { return isTvSystemType( eTvSystemType::PAL ); }
			inline constexpr bool isExtendedConsoleType() const noexcept { return mHeader != nullptr && mHeader.get()->Flags07.Bits.ConsoleType == eConsoleType::EXTENDED_CONSOLE_TYPE; }
			inline constexpr bool isVsSystem() const noexcept { return mHeader != nullptr && mHeader.get()->Flags07.Bits.ConsoleType == eConsoleType::NINTENDO_VS_SYSTEM; }
			constexpr size_t getProgramRomSize() const noexcept;
			constexpr size_t getCharacterRomSize() const noexcept;

		private:
			std::filesystem::path		mRomFilePath;
			std::ifstream				mRomFile;
			std::unique_ptr<Header>		mHeader;
			std::unique_ptr<Trainer>	mTrainer;
			ProgramRom					mProgramRom;
			CharacterRom				mCharacterRom;
		};
	}
}