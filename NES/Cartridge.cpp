#include "stdafx.h"

#include <bitset>

#include "NES/Cartridge.h"

namespace ninmuse
{
	namespace nes
	{
		const uint8_t* Disassemble( char* out_buffer64, const uint8_t* mem );

		Cartridge::Cartridge( const std::filesystem::path& romFilePath ) noexcept
			: mRomFilePath( romFilePath )
			, mRomFile()
			, mHeader()
			, mTrainer()
			, mProgramRom()
			, mCharacterRom()
		{
			mRomFile.open( mRomFilePath.c_str(), std::ios_base::binary | std::ios_base::in );
			assert( mRomFile.is_open() );
		}

		Cartridge::~Cartridge()
		{
			mRomFile.close();
		}

		static constexpr const char* PRESENT = "Present";
		static constexpr const char* NOT_PRESENT = "Not Present";
		static constexpr const char* IsPresentToString( const bool isPresent ) noexcept
		{
			return isPresent ? PRESENT : NOT_PRESENT;
		}

		void Cartridge::Read() noexcept
		{
			ReadHeader();
			if ( mHeader->Flags06.Bits.IsTrainerPresent )
			{
				ReadTrainer();
			}
			ReadProgramRom();
		}

		const Cartridge::Header& Cartridge::ReadHeader() noexcept
		{
			mHeader = std::make_unique<Header>();
			static constexpr const size_t HEADER_DATA_KEY_WIDTH = 32;
#define HEADER_DATA_KEY_ALIGNMENT (std::right)
			// Read Header (16 bytes)
			{
				char headerData[Header::SIZE] = {
					0,
				};
				mRomFile.read( headerData, Header::SIZE );
				memcpy( mHeader->Identification, &headerData[Header::ID_INDEX], Header::ID_SIZE );
				memcpy( &mHeader->ProgramRomSize, &headerData[Header::PRG_ROM_SIZE_INDEX], Header::PRG_ROM_SIZE_SIZE );
				memcpy( &mHeader->CharacterRomSize, &headerData[Header::CHARACTER_ROM_SIZE_INDEX], Header::CHARACTER_ROM_SIZE_SIZE );
				memcpy( &mHeader->Flags06.Value, &headerData[Header::FLAGS06_INDEX], Header::FLAGS06_SIZE );
				memcpy( &mHeader->Flags07.Value, &headerData[Header::FLAGS07_INDEX], Header::FLAGS07_SIZE );
				memcpy( &mHeader->Flags08.Value, &headerData[Header::FLAGS08_INDEX], Header::FLAGS08_SIZE );
				memcpy( &mHeader->Flags09.Value, &headerData[Header::FLAGS09_INDEX], Header::FLAGS09_SIZE );
				memcpy( &mHeader->Flags10.Value, &headerData[Header::FLAGS10_INDEX], Header::FLAGS10_SIZE );
				memcpy( &mHeader->Flags11.Value, &headerData[Header::FLAGS11_INDEX], Header::FLAGS11_SIZE );
				memcpy( &mHeader->Flags12.Value, &headerData[Header::FLAGS12_INDEX], Header::FLAGS12_SIZE );
				memcpy( &mHeader->Flags13.Value, &headerData[Header::FLAGS13_INDEX], Header::FLAGS13_SIZE );
				memcpy( &mHeader->Flags14.Value, &headerData[Header::FLAGS14_INDEX], Header::FLAGS14_SIZE );
				memcpy( &mHeader->Flags15.Value, &headerData[Header::FLAGS15_INDEX], Header::FLAGS15_SIZE );

				// Read Identification String (4 bytes)
				{
					std::cout << std::setw( HEADER_DATA_KEY_WIDTH ) << HEADER_DATA_KEY_ALIGNMENT << "Identification: ";
					std::cout << mHeader->Identification << std::endl;
				}
				// PRG-ROM Size
				{
					std::cout << std::setw( HEADER_DATA_KEY_WIDTH ) << HEADER_DATA_KEY_ALIGNMENT;
					if ( isNes2_0Format() == false )
					{
						std::cout << "PRG-ROM Size: ";
					}
					else
					{
						std::cout << "PRG-ROM Size Low: ";
					}
					std::cout << static_cast< uint32_t >( mHeader->ProgramRomSize.Value ) << std::endl;
				}
				// CHR-ROM Size
				{
					std::cout << std::setw( HEADER_DATA_KEY_WIDTH ) << HEADER_DATA_KEY_ALIGNMENT;
					if ( isNes2_0Format() == false )
					{
						std::cout << "CHR-ROM Size: ";
					}
					else
					{
						std::cout << "CHR-ROM Size Low: ";
					}
					std::cout << static_cast< uint32_t >( mHeader->CharacterRomSize.Value ) << std::endl;
				}
				// Flags06
				{
					// Mapper
					std::cout << std::setw( HEADER_DATA_KEY_WIDTH ) << HEADER_DATA_KEY_ALIGNMENT << "Mapper D03...D00: ";
					std::cout << "0b"
						<< static_cast< uint32_t >( mHeader->Flags06.Bits.MapperD03 )
						<< static_cast< uint32_t >( mHeader->Flags06.Bits.MapperD02 )
						<< static_cast< uint32_t >( mHeader->Flags06.Bits.MapperD01 )
						<< static_cast< uint32_t >( mHeader->Flags06.Bits.MapperD00 ) << std::endl;
					// Four Screen Mode
					std::cout << std::setw( HEADER_DATA_KEY_WIDTH ) << HEADER_DATA_KEY_ALIGNMENT << "Hard-Wired Four Screen Mode: ";
					std::cout << std::boolalpha << mHeader->Flags06.Bits.FourScreenMode << std::endl;
					// Trainer
					std::cout << std::setw( HEADER_DATA_KEY_WIDTH ) << HEADER_DATA_KEY_ALIGNMENT << "512-byte Trainer: ";
					std::cout << IsPresentToString( mHeader->Flags06.Bits.IsTrainerPresent ) << std::endl;
					// Battery
					std::cout << std::setw( HEADER_DATA_KEY_WIDTH ) << HEADER_DATA_KEY_ALIGNMENT << "Battery: ";
					std::cout << IsPresentToString( mHeader->Flags06.Bits.Battery ) << std::endl;
					// Mirroring Type
					std::cout << std::setw( HEADER_DATA_KEY_WIDTH ) << HEADER_DATA_KEY_ALIGNMENT << "Mirroring Type: ";
					std::cout << ( mHeader->Flags06.Bits.MirroringType ? "Vertical" : "Horizontal" ) << std::endl;
				}
				// Flags07
				{
					// Mapper
					std::cout << std::setw( HEADER_DATA_KEY_WIDTH ) << HEADER_DATA_KEY_ALIGNMENT << "Mapper D07...D04: ";
					std::cout << "0b"
						<< mHeader->Flags07.Bits.MapperD07
						<< mHeader->Flags07.Bits.MapperD06
						<< mHeader->Flags07.Bits.MapperD05
						<< mHeader->Flags07.Bits.MapperD04 << std::endl;
					// NES 2.0 Identifier
					std::cout << std::setw( HEADER_DATA_KEY_WIDTH ) << HEADER_DATA_KEY_ALIGNMENT << "Is NES 2.0 Format?: ";
					std::cout << std::boolalpha << isNes2_0Format() << std::endl;
					// Console Type
					std::cout << std::setw( HEADER_DATA_KEY_WIDTH ) << HEADER_DATA_KEY_ALIGNMENT << "Console Type: ";
					std::cout << ConsoleTypeToString( mHeader->Flags07.Bits.ConsoleType ) << std::endl;
				}
				// Flags08
				{
					// NES
					if ( isNes2_0Format() == false )
					{
						std::cout << std::setw( HEADER_DATA_KEY_WIDTH ) << HEADER_DATA_KEY_ALIGNMENT << "PRG RAM Size: ";
						std::cout << static_cast< uint32_t >( mHeader->Flags08.NESBits.PrgRamSize ) << std::endl;
					}
					// NES 2.0
					else
					{
						// Submapper number
						std::cout << std::setw( HEADER_DATA_KEY_WIDTH ) << HEADER_DATA_KEY_ALIGNMENT << "Mapper D11...D08: ";
						std::cout << "0b"
							<< std::bitset<4>( mHeader->Flags08.NES2_0Bits.SubmapperNumber ) << std::endl;

						// Mapper number
						std::cout << std::setw( HEADER_DATA_KEY_WIDTH ) << HEADER_DATA_KEY_ALIGNMENT << "Mapper D11...D08: ";
						std::cout << "0b"
							<< mHeader->Flags08.NES2_0Bits.MapperD11
							<< mHeader->Flags08.NES2_0Bits.MapperD10
							<< mHeader->Flags08.NES2_0Bits.MapperD09
							<< mHeader->Flags08.NES2_0Bits.MapperD08 << std::endl;
					}
				}
				// Flags09
				{
					// NES
					if ( isNes2_0Format() == false )
					{
						std::cout << std::setw( HEADER_DATA_KEY_WIDTH ) << HEADER_DATA_KEY_ALIGNMENT << "TV System: ";
						switch ( mHeader->Flags09.NESBits.TvSystem )
						{
						case eTvSystemType::NTSC:
							std::cout << "NTSC";
							break;
						case eTvSystemType::PAL:
							std::cout << "PAL";
							break;
						default:
							assert( false );
							break;
						}
						std::cout << std::endl;
					}
					// NES 2.0
					else
					{
						// CHR-Rom Size MSB
						std::cout << std::setw( HEADER_DATA_KEY_WIDTH ) << HEADER_DATA_KEY_ALIGNMENT << "CHR-ROM Size High: ";
						std::cout << mHeader->Flags09.NES2_0Bits.ChrRomSizeHigh << std::endl;

						// PRG-ROM Size MSB
						std::cout << std::setw( HEADER_DATA_KEY_WIDTH ) << HEADER_DATA_KEY_ALIGNMENT << "PRG-ROM Size High: ";
						std::cout << mHeader->Flags09.NES2_0Bits.PrgRomSizeHigh << std::endl;
					}
				}
				// Flags10
				{
					// NES
					if ( isNes2_0Format() == false )
					{
						if ( ( isNtsc() && HasTvSystemType( mHeader->Flags10.NESBits.TvSystemBits, eTvSystemType::NTSC ) ) || ( isPal() && HasTvSystemType( mHeader->Flags10.NESBits.TvSystemBits, eTvSystemType::PAL ) ) )
						{
							std::cout << std::setw( HEADER_DATA_KEY_WIDTH ) << HEADER_DATA_KEY_ALIGNMENT << "Board has confict: ";
							std::cout << std::boolalpha << mHeader->Flags10.NESBits.HasBusConflict << std::endl;
							std::cout << std::setw( HEADER_DATA_KEY_WIDTH ) << HEADER_DATA_KEY_ALIGNMENT << "Is PRG-RAM present: ";
							std::cout << std::boolalpha << mHeader->Flags10.NESBits.PrgRam << std::endl;
							std::cout << std::setw( HEADER_DATA_KEY_WIDTH ) << HEADER_DATA_KEY_ALIGNMENT << "TV System Bits: ";

							std::cout << TvSystemTypeBitsToString( mHeader->Flags10.NESBits.TvSystemBits ) << std::endl;
						}
					}
					// NES 2.0
					else
					{
						std::cout << std::setw( HEADER_DATA_KEY_WIDTH ) << HEADER_DATA_KEY_ALIGNMENT << "PRG-NVRAM/EEPROM (non-volatile) shift count: ";
						std::cout << mHeader->Flags10.NES2_0Bits.PrgNvramEepromShiftCount << std::endl;
						std::cout << std::setw( HEADER_DATA_KEY_WIDTH ) << HEADER_DATA_KEY_ALIGNMENT << "PRG-RAM (volatile) shift count: ";
						std::cout << mHeader->Flags10.NES2_0Bits.PrgRamShiftCount << std::endl;
					}
				}
				if ( isNes2_0Format() )
				{
					// Flags11
					{
						std::cout << std::setw( HEADER_DATA_KEY_WIDTH ) << HEADER_DATA_KEY_ALIGNMENT << "CHR-NVRAM (non-volatile) shift count: ";
						std::cout << mHeader->Flags11.NES2_0Bits.ChrNvramShiftCount << std::endl;
						std::cout << std::setw( HEADER_DATA_KEY_WIDTH ) << HEADER_DATA_KEY_ALIGNMENT << "CHR-RAM (volatile) shift count: ";
						std::cout << mHeader->Flags11.NES2_0Bits.ChrRamShiftCount << std::endl;
					}
					// Flags12
					{
						std::cout << std::setw( HEADER_DATA_KEY_WIDTH ) << HEADER_DATA_KEY_ALIGNMENT << "CPU/PPU timing mode: ";
						std::cout << CpuPpuTimingModeToString( mHeader->Flags12.NES2_0Bits.CpuPpuTimingMode ) << std::endl;
					}
					// Flags13
					{
						if ( isVsSystem() )
						{
							std::cout << std::setw( HEADER_DATA_KEY_WIDTH ) << HEADER_DATA_KEY_ALIGNMENT << "Vs. Hardware Type: ";
							std::cout << VsHardwareTypeToString( mHeader->Flags13.NES2_0BitsVsSystem.VsHardwareType ) << std::endl;
							std::cout << std::setw( HEADER_DATA_KEY_WIDTH ) << HEADER_DATA_KEY_ALIGNMENT << "PPU Type: ";
							std::cout << PpuTypeToString( mHeader->Flags13.NES2_0BitsVsSystem.PpuType ) << std::endl;
						}
						else if ( isExtendedConsoleType() )
						{
							std::cout << std::setw( HEADER_DATA_KEY_WIDTH ) << HEADER_DATA_KEY_ALIGNMENT << "Extended Console Type: ";
							std::cout << ExtendedConsoleTypeToString( mHeader->Flags13.NES2_0BitsExtendedConsoleType.ExtendedConsoleType ) << std::endl;
						}
						else
						{
							assert( false );
						}
					}
					// Flags14
					{
						std::cout << std::setw( HEADER_DATA_KEY_WIDTH ) << HEADER_DATA_KEY_ALIGNMENT << "Number of miscellaneous ROMs present: ";
						std::cout << mHeader->Flags14.NES2_0Bits.NumMiscellaneousRomsPresent << std::endl;
					}
					// Flags15
					{
						std::cout << std::setw( HEADER_DATA_KEY_WIDTH ) << HEADER_DATA_KEY_ALIGNMENT << "Default Expansion Device: ";
						std::cout << mHeader->Flags15.NES2_0Bits.DefaultExpansionDevice << std::endl;
					}
				}
			}

			return *mHeader;
		}

		const Cartridge::Trainer& Cartridge::ReadTrainer() noexcept
		{
			assert( mHeader->Flags06.Bits.IsTrainerPresent );
			mTrainer = std::make_unique<Cartridge::Trainer>();

			mRomFile.read( mTrainer->Data, Trainer::DATA_SIZE );

			return *mTrainer;
		}

		const std::vector<char>& Cartridge::ReadProgramRom() noexcept
		{
			// [TODO]: Vs. Dual System calculates program ROM differently.
			const size_t programRomSize = getProgramRomSize();
			std::cout << "PRG-ROM Size: " << programRomSize << std::endl;

			mProgramRom.Data.resize( programRomSize );
			for ( size_t i = 0; i < programRomSize; ++i )
			{
				mRomFile.read( &mProgramRom.Data.data()[i], 1 );
			}
			
			const data_t addressLow = mProgramRom.Data.data()[0xFFFC];
			const data_t addressHigh = mProgramRom.Data.data()[0xFFFD];
			const address_t address = CreateAddress( addressLow, addressHigh );
			const data_t* mem = reinterpret_cast< data_t* >( &( mProgramRom.Data.data()[address] ) );
			char buffer[64] = { 0, };
			mem = Disassemble( buffer, mem );
			std::cout << buffer << std::endl;
			mem = Disassemble( buffer, mem );
			std::cout << buffer << std::endl;
			mem = Disassemble( buffer, mem );
			std::cout << buffer << std::endl;
			mem = Disassemble( buffer, mem );
			std::cout << buffer << std::endl;
			mem = Disassemble( buffer, mem );
			std::cout << buffer << std::endl;
			mem = Disassemble( buffer, mem );
			std::cout << buffer << std::endl;
			return mProgramRom.Data;
		}

		const std::vector<char>& Cartridge::ReadCharacterRom() noexcept
		{
			const size_t characterRomSize = getCharacterRomSize();
			std::cout << "CHR-ROM Size: " << characterRomSize << std::endl;

			mCharacterRom.Data.resize( characterRomSize );
			mRomFile.read( mCharacterRom.Data.data(), characterRomSize );
			return mCharacterRom.Data;
		}

		size_t Cartridge::getProgramRomSize() const noexcept
		{
			ProgramRom::Size programRomSizeData;
			if ( isNes2_0Format() )
			{
				programRomSizeData.NES2_0Bits.SizeHigh = mHeader->Flags09.NES2_0Bits.PrgRomSizeHigh;
			}
			programRomSizeData.NESBits.Size = mHeader->ProgramRomSize.Value;

			if ( isNes2_0Format() && programRomSizeData.NES2_0Bits.SizeHigh != 0b1111 )
			{
				const size_t programRomSize =
					( static_cast< size_t >( 2 ) << static_cast< size_t >( programRomSizeData.NES2_0Bits.SizeLow.ExponentMultiplier.Exponent ) ) * ( static_cast< size_t >( programRomSizeData.NES2_0Bits.SizeLow.ExponentMultiplier.Multiplier ) * 2 + 1 );
				return programRomSize;
			}
			else
			{
				const size_t programRomSize = programRomSizeData.Value * ProgramRom::PROGRAM_SIZE_UNIT;
				return programRomSize;
			}
		}

		size_t Cartridge::getCharacterRomSize() const noexcept
		{
			CharacterRom::Size characterRomSizeData;
			if ( isNes2_0Format() )
			{
				characterRomSizeData.NES2_0Bits.SizeHigh = mHeader->Flags09.NES2_0Bits.ChrRomSizeHigh;
			}
			characterRomSizeData.NESBits.Size = mHeader->CharacterRomSize.Value;

			if ( isNes2_0Format() && characterRomSizeData.NES2_0Bits.SizeHigh != 0b1111 )
			{
				const size_t characterRomSize =
					( static_cast< size_t >( 2 ) << static_cast< size_t >( characterRomSizeData.NES2_0Bits.SizeLow.ExponentMultiplier.Exponent ) ) * ( static_cast< size_t >( characterRomSizeData.NES2_0Bits.SizeLow.ExponentMultiplier.Multiplier ) * 2 + 1 );
				return characterRomSize;
			}
			else
			{
				const size_t characterRomSize = characterRomSizeData.Value * CharacterRom::PROGRAM_SIZE_UNIT;
				return characterRomSize;
			}
		}

		typedef enum address_mode {
			ACCUMULATOR,
			IMMEDIATE,
			ABSOLUTE,
			ZERO_PAGE,
			IMPLIED,
			RELATIVE,
			ABSOLUTE_INDIRECT,
			ABSOLUTE_INDEXED_WITH_X,
			ABSOLUTE_INDEXED_WITH_Y,
			ZERO_PAGE_INDEXED_WITH_X,
			ZERO_PAGE_INDEXED_WITH_Y,
			ZERO_PAGE_INDEXED_INDIRECT,
			ZERO_PAGE_INDIRECT_INDEXED_WITH_Y,
			ADDRESS_MODE_COUNT,
		} address_mode_t;

		const char* const g_address_mode_to_string[ADDRESS_MODE_COUNT] = {
			"A", // ACCUMULATOR
			"#", // IMMEDIATE
			"a", // ABSOLUTE
			"zp", // ZERO_PAGE
			"i", // IMPLIED
			"r", // RELATIVE
			"(a)", // ABSOLUTE_INDIRECT
			"a,x", // ABSOLUTE_INDEXED_WITH_X
			"a,y", // ABSOLUTE_INDEXED_WITH_Y
			"zp,x", // ZERO_PAGE_INDEXED_WITH_X
			"zp,y", // ZERO_PAGE_INDEXED_WITH_Y
			"(zp,x)", // ZERO_PAGE_INDEXED_INDIRECT
			"(zp),y" // ZERO_PAGE_INDIRECT_INDEXED_WITH_Y
		};

		const uint8_t g_address_mode_operand_bytes_needed[ADDRESS_MODE_COUNT] = {
			0, // ACCUMULATOR
			1, // IMMEDIATE
			2, // ABSOLUTE
			1, // ZERO_PAGE
			0, // IMPLIED
			1, // RELATIVE
			2, // ABSOLUTE_INDIRECT
			2, // ABSOLUTE_INDEXED_WITH_X
			2, // ABSOLUTE_INDEXED_WITH_Y
			1, // ZERO_PAGE_INDEXED_WITH_X
			1, // ZERO_PAGE_INDEXED_WITH_Y
			1, // ZERO_PAGE_INDEXED_INDIRECT
			1 // ZERO_PAGE_INDIRECT_INDEXED_WITH_Y
		};

		typedef struct instruction_info {
			const char* name;
			address_mode_t address_mode;
			uint16_t opcode;
		} instruction_info_t;

		// instructions
		// lda
		const instruction_info_t g_load_accumulator_with_memory_absolute = {
			.name = "lda",
			.address_mode = ABSOLUTE,
			.opcode = 0xAD,
		};

		const instruction_info_t g_load_accumulator_with_memory_absolute_indexed_with_x = {
			.name = "lda",
			.address_mode = ABSOLUTE_INDEXED_WITH_X,
			.opcode = 0xBD,
		};

		const instruction_info_t g_load_accumulator_with_memory_absolute_indexed_with_y = {
			.name = "lda",
			.address_mode = ABSOLUTE_INDEXED_WITH_Y,
			.opcode = 0xB9,
		};

		const instruction_info_t g_load_accumulator_with_memory_immediate = {
			.name = "lda",
			.address_mode = IMMEDIATE,
			.opcode = 0xA9,
		};

		const instruction_info_t g_load_accumulator_with_memory_zero_page = {
			.name = "lda",
			.address_mode = ZERO_PAGE,
			.opcode = 0xA5,
		};

		const instruction_info_t g_load_accumulator_with_memory_zero_page_indexed_indirect = {
			.name = "lda",
			.address_mode = ZERO_PAGE_INDEXED_INDIRECT,
			.opcode = 0xA1,
		};

		const instruction_info_t g_load_accumulator_with_memory_zero_page_indexed_with_x = {
			.name = "lda",
			.address_mode = ZERO_PAGE_INDEXED_WITH_X,
			.opcode = 0xB5
		};

		const instruction_info_t g_load_accumulator_with_memory_zero_page_indirect_indexed_with_y = {
			.name = "lda",
			.address_mode = ZERO_PAGE_INDIRECT_INDEXED_WITH_Y,
			.opcode = 0xB1,
		};

		// ldx
		const instruction_info_t g_load_index_x_with_memory_absolute = {
			.name = "ldx",
			.address_mode = ABSOLUTE,
			.opcode = 0xAE,
		};

		const instruction_info_t g_load_index_x_with_memory_absolute_indexed_with_y = {
			.name = "ldx",
			.address_mode = ABSOLUTE_INDEXED_WITH_Y,
			.opcode = 0xBE
		};

		const instruction_info_t g_load_index_x_with_memory_immediate = {
			.name = "ldx",
			.address_mode = IMMEDIATE,
			.opcode = 0xA2
		};

		const instruction_info_t g_load_index_x_with_memory_zero_page = {
			.name = "ldx",
			.address_mode = ZERO_PAGE,
			.opcode = 0xA6
		};

		const instruction_info_t g_load_index_x_with_memory_zero_page_indexed_with_y = {
			.name = "ldx",
			.address_mode = ZERO_PAGE_INDEXED_WITH_Y,
			.opcode = 0xB6
		};

		// ldy
		const instruction_info_t g_load_index_y_with_memory_absolute = {
			.name = "ldy",
			.address_mode = ABSOLUTE,
			.opcode = 0xAC
		};

		const instruction_info_t g_load_index_y_with_memory_absolute_indexed_with_x = {
			.name = "ldy",
			.address_mode = ABSOLUTE_INDEXED_WITH_X,
			.opcode = 0xBC
		};

		const instruction_info_t g_load_index_y_with_memory_immediate = {
			.name = "ldy",
			.address_mode = IMMEDIATE,
			.opcode = 0xA0
		};

		const instruction_info_t g_load_index_y_with_memory_zero_page = {
			.name = "ldy",
			.address_mode = ZERO_PAGE,
			.opcode = 0xA4
		};

		const instruction_info_t g_load_index_y_with_memory_zero_page_indexed_with_x = {
			.name = "ldy",
			.address_mode = ZERO_PAGE_INDEXED_WITH_X,
			.opcode = 0xB4
		};

		// sta
		const instruction_info_t g_store_accumulator_in_memory_absolute = {
			.name = "sta",
			.address_mode = ABSOLUTE,
			.opcode = 0x8D
		};

		const instruction_info_t g_store_accumulator_in_memory_absolute_indexed_with_x = {
			.name = "sta",
			.address_mode = ABSOLUTE_INDEXED_WITH_X,
			.opcode = 0x9D
		};

		const instruction_info_t g_store_accumulator_in_memory_absolute_indexed_with_y = {
			.name = "sta",
			.address_mode = ABSOLUTE_INDEXED_WITH_Y,
			.opcode = 0x99
		};

		const instruction_info_t g_store_accumulator_in_memory_zero_page = {
			.name = "sta",
			.address_mode = ZERO_PAGE,
			.opcode = 0x85
		};

		const instruction_info_t g_store_accumulator_in_memory_zero_page_indexed_indirect = {
			.name = "sta",
			.address_mode = ZERO_PAGE_INDEXED_INDIRECT,
			.opcode = 0x81
		};

		const instruction_info_t g_store_accumulator_in_memory_zero_page_indexed_with_x = {
			.name = "sta",
			.address_mode = ZERO_PAGE_INDEXED_WITH_X,
			.opcode = 0x95
		};

		const instruction_info_t g_store_accumulator_in_memory_zero_page_indirect_indexed_with_y = {
			.name = "sta",
			.address_mode = ZERO_PAGE_INDIRECT_INDEXED_WITH_Y,
			.opcode = 0x91
		};

		// stx
		const instruction_info_t g_store_index_x_in_memory_absolute = {
			.name = "stx",
			.address_mode = ABSOLUTE,
			.opcode = 0x8E
		};

		const instruction_info_t g_store_index_x_in_memory_zero_page = {
			.name = "stx",
			.address_mode = ZERO_PAGE,
			.opcode = 0x86
		};

		const instruction_info_t g_store_index_x_in_memory_zero_page_indexed_with_y = {
			.name = "stx",
			.address_mode = ZERO_PAGE_INDEXED_WITH_Y,
			.opcode = 0x96
		};

		// sty
		const instruction_info_t g_store_index_y_in_memory_absolute = {
			.name = "sty",
			.address_mode = ABSOLUTE,
			.opcode = 0x8C
		};

		const instruction_info_t g_store_index_y_in_memory_zero_page = {
			.name = "sty",
			.address_mode = ZERO_PAGE,
			.opcode = 0x84
		};

		const instruction_info_t g_store_index_y_in_memory_zero_page_indexed_with_x = {
			.name = "sty",
			.address_mode = ZERO_PAGE_INDEXED_WITH_X,
			.opcode = 0x94
		};

		// adc
		const instruction_info_t g_add_memory_to_accumulator_with_carry_absolute = {
			.name = "adc",
			.address_mode = ABSOLUTE,
			.opcode = 0x6D
		};

		const instruction_info_t g_add_memory_to_accumulator_with_carry_absolute_indexed_with_x = {
			.name = "adc",
			.address_mode = ABSOLUTE_INDEXED_WITH_X,
			.opcode = 0x7D
		};

		const instruction_info_t g_add_memory_to_accumulator_with_carry_absolute_indexed_with_y = {
			.name = "adc",
			.address_mode = ABSOLUTE_INDEXED_WITH_Y,
			.opcode = 0x79
		};

		const instruction_info_t g_add_memory_to_accumulator_with_carry_immediate = {
			.name = "adc",
			.address_mode = IMMEDIATE,
			.opcode = 0x69
		};

		const instruction_info_t g_add_memory_to_accumulator_with_carry_zero_page = {
			.name = "adc",
			.address_mode = ZERO_PAGE,
			.opcode = 0x65
		};

		const instruction_info_t g_add_memory_to_accumulator_with_carry_zero_page_indexed_indirect = {
			.name = "adc",
			.address_mode = ZERO_PAGE_INDEXED_INDIRECT,
			.opcode = 0x61
		};

		const instruction_info_t g_add_memory_to_accumulator_with_carry_zero_page_indexed_with_x = {
			.name = "adc",
			.address_mode = ZERO_PAGE_INDEXED_WITH_X,
			.opcode = 0x75
		};

		const instruction_info_t g_add_memory_to_accumulator_with_carry_zero_page_indirect_indexed_with_y = {
			.name = "adc",
			.address_mode = ZERO_PAGE_INDIRECT_INDEXED_WITH_Y,
			.opcode = 0x71
		};

		// sbc
		const instruction_info_t g_subtract_memory_from_accumulator_with_borrow_absolute = {
			.name = "sbc",
			.address_mode = ABSOLUTE,
			.opcode = 0xED
		};

		const instruction_info_t g_subtract_memory_from_accumulator_with_borrow_absolute_indexed_with_x = {
			.name = "sbc",
			.address_mode = ABSOLUTE_INDEXED_WITH_X,
			.opcode = 0xFD
		};

		const instruction_info_t g_subtract_memory_from_accumulator_with_borrow_absolute_indexed_with_y = {
			.name = "sbc",
			.address_mode = ABSOLUTE_INDEXED_WITH_Y,
			.opcode = 0xF9
		};

		const instruction_info_t g_subtract_memory_from_accumulator_with_borrow_immediate = {
			.name = "sbc",
			.address_mode = IMMEDIATE,
			.opcode = 0xE9
		};

		const instruction_info_t g_subtract_memory_from_accumulator_with_borrow_zero_page = {
			.name = "sbc",
			.address_mode = ZERO_PAGE,
			.opcode = 0xE5
		};

		const instruction_info_t g_subtract_memory_from_accumulator_with_borrow_zero_page_indexed_indirect = {
			.name = "sbc",
			.address_mode = ZERO_PAGE_INDEXED_INDIRECT,
			.opcode = 0xE1
		};

		const instruction_info_t g_subtract_memory_from_accumulator_with_borrow_zero_page_indexed_with_x = {
			.name = "sbc",
			.address_mode = ZERO_PAGE_INDEXED_WITH_X,
			.opcode = 0xF5
		};

		const instruction_info_t g_subtract_memory_from_accumulator_with_borrow_zero_page_indirect_indexed_with_y = {
			.name = "sbc",
			.address_mode = ZERO_PAGE_INDIRECT_INDEXED_WITH_Y,
			.opcode = 0xF1
		};

		// inc
		const instruction_info_t g_increment_memory_by_one_absolute = {
			.name = "inc",
			.address_mode = ABSOLUTE,
			.opcode = 0xEE
		};

		const instruction_info_t g_increment_memory_by_one_absolute_indexed_with_x = {
			.name = "inc",
			.address_mode = ABSOLUTE_INDEXED_WITH_X,
			.opcode = 0xFE
		};

		const instruction_info_t g_increment_memory_by_one_zero_page = {
			.name = "inc",
			.address_mode = ZERO_PAGE,
			.opcode = 0xE6
		};

		const instruction_info_t g_increment_memory_by_one_zero_page_indexed_with_x = {
			.name = "inc",
			.address_mode = ZERO_PAGE_INDEXED_WITH_X,
			.opcode = 0xF6
		};

		// inx
		const instruction_info_t g_increment_index_x_by_one = {
			.name = "inx",
			.address_mode = IMPLIED,
			.opcode = 0xE8
		};

		// iny
		const instruction_info_t g_increment_index_y_by_one = {
			.name = "iny",
			.address_mode = IMPLIED,
			.opcode = 0xC8
		};

		// dec
		const instruction_info_t g_decrement_memory_by_one_absolute = {
			.name = "dec",
			.address_mode = ABSOLUTE,
			.opcode = 0xCE
		};

		const instruction_info_t g_decrement_memory_by_one_absolute_indexed_with_x = {
			.name = "dec",
			.address_mode = ABSOLUTE_INDEXED_WITH_X,
			.opcode = 0xDE
		};

		const instruction_info_t g_decrement_memory_by_one_zero_page = {
			.name = "dec",
			.address_mode = ZERO_PAGE,
			.opcode = 0xC6
		};

		const instruction_info_t g_decrement_memory_by_one_zero_page_indexed_with_x = {
			.name = "dec",
			.address_mode = ZERO_PAGE_INDEXED_WITH_X,
			.opcode = 0xD6
		};

		// dex
		const instruction_info_t g_decrement_index_x_by_one = {
			.name = "dex",
			.address_mode = IMPLIED,
			.opcode = 0xCA
		};

		// dey
		const instruction_info_t g_decrement_index_y_by_one = {
			.name = "dey",
			.address_mode = IMPLIED,
			.opcode = 0x88
		};

		// asl
		const instruction_info_t g_arithmetic_shift_left_one_bit_absolute = {
			.name = "asl",
			.address_mode = ABSOLUTE,
			.opcode = 0x0E
		};

		const instruction_info_t g_arithmetic_shift_left_one_bit_absolute_indexed_with_x = {
			.name = "asl",
			.address_mode = ABSOLUTE_INDEXED_WITH_X,
			.opcode = 0x1E
		};

		const instruction_info_t g_arithmetic_shift_left_one_bit_accumulator = {
			.name = "asl",
			.address_mode = ACCUMULATOR,
			.opcode = 0x0A
		};

		const instruction_info_t g_arithmetic_shift_left_one_bit_zero_page = {
			.name = "asl",
			.address_mode = ZERO_PAGE,
			.opcode = 0x06
		};

		const instruction_info_t g_arithmetic_shift_left_one_bit_zero_page_indexed_with_x = {
			.name = "asl",
			.address_mode = ZERO_PAGE_INDEXED_WITH_X,
			.opcode = 0x16
		};

		// lsr
		const instruction_info_t g_logical_shift_right_one_bit_absolute = {
			.name = "lsr",
			.address_mode = ABSOLUTE,
			.opcode = 0x4E
		};

		const instruction_info_t g_logical_shift_right_one_bit_absolute_indexed_with_x = {
			.name = "lsr",
			.address_mode = ABSOLUTE_INDEXED_WITH_X,
			.opcode = 0x5E
		};

		const instruction_info_t g_logical_shift_right_one_bit_accumulator = {
			.name = "lsr",
			.address_mode = ACCUMULATOR,
			.opcode = 0x4A
		};

		const instruction_info_t g_logical_shift_right_one_bit_zero_page = {
			.name = "lsr",
			.address_mode = ZERO_PAGE,
			.opcode = 0x46
		};

		const instruction_info_t g_logical_shift_right_one_bit_zero_page_indexed_with_x = {
			.name = "lsr",
			.address_mode = ZERO_PAGE_INDEXED_WITH_X,
			.opcode = 0x56
		};

		// rol
		const instruction_info_t g_rotate_left_one_bit_absolute = {
			.name = "rol",
			.address_mode = ABSOLUTE,
			.opcode = 0x2E
		};

		const instruction_info_t g_rotate_left_one_bit_absolute_indexed_with_x = {
			.name = "rol",
			.address_mode = ABSOLUTE_INDEXED_WITH_X,
			.opcode = 0x3E
		};

		const instruction_info_t g_rotate_left_one_bit_accumulator = {
			.name = "rol",
			.address_mode = ACCUMULATOR,
			.opcode = 0x2A
		};

		const instruction_info_t g_rotate_left_one_bit_zero_page = {
			.name = "rol",
			.address_mode = ZERO_PAGE,
			.opcode = 0x26
		};

		const instruction_info_t g_rotate_left_one_bit_zero_page_indexed_with_x = {
			.name = "rol",
			.address_mode = ZERO_PAGE_INDEXED_WITH_X,
			.opcode = 0x36
		};

		// ror
		const instruction_info_t g_rotate_right_one_bit_absolute = {
			.name = "ror",
			.address_mode = ABSOLUTE,
			.opcode = 0x6E
		};

		const instruction_info_t g_rotate_right_one_bit_absolute_indexed_with_x = {
			.name = "ror",
			.address_mode = ABSOLUTE_INDEXED_WITH_X,
			.opcode = 0x7E
		};

		const instruction_info_t g_rotate_right_one_bit_accumulator = {
			.name = "ror",
			.address_mode = ACCUMULATOR,
			.opcode = 0x6A
		};

		const instruction_info_t g_rotate_right_one_bit_zero_page = {
			.name = "ror",
			.address_mode = ZERO_PAGE,
			.opcode = 0x66
		};

		const instruction_info_t g_rotate_right_one_bit_zero_page_indexed_with_x = {
			.name = "ror",
			.address_mode = ZERO_PAGE_INDEXED_WITH_X,
			.opcode = 0x76
		};

		// and
		const instruction_info_t g_and_memory_with_accumulator_absolute = {
			.name = "and",
			.address_mode = ABSOLUTE,
			.opcode = 0x2D
		};

		const instruction_info_t g_and_memory_with_accumulator_absolute_indexed_with_x = {
			.name = "and",
			.address_mode = ABSOLUTE_INDEXED_WITH_X,
			.opcode = 0x3D
		};

		const instruction_info_t g_and_memory_with_accumulator_absolute_indexed_with_y = {
			.name = "and",
			.address_mode = ABSOLUTE_INDEXED_WITH_Y,
			.opcode = 0x39
		};

		const instruction_info_t g_and_memory_with_accumulator_immediate = {
			.name = "and",
			.address_mode = IMMEDIATE,
			.opcode = 0x29
		};

		const instruction_info_t g_and_memory_with_accumulator_zero_page = {
			.name = "and",
			.address_mode = ZERO_PAGE,
			.opcode = 0x25
		};

		const instruction_info_t g_and_memory_with_accumulator_zero_page_indexed_indirect = {
			.name = "and",
			.address_mode = ZERO_PAGE_INDEXED_INDIRECT,
			.opcode = 0x21
		};

		const instruction_info_t g_and_memory_with_accumulator_zero_page_indexed_with_x = {
			.name = "and",
			.address_mode = ZERO_PAGE_INDEXED_WITH_X,
			.opcode = 0x35
		};

		const instruction_info_t g_and_memory_with_accumulator_zero_page_indirect_indexed_with_y = {
			.name = "and",
			.address_mode = ZERO_PAGE_INDIRECT_INDEXED_WITH_Y,
			.opcode = 0x31
		};

		// ora
		const instruction_info_t g_or_memory_with_accumulator_absolute = {
			.name = "ora",
			.address_mode = ABSOLUTE,
			.opcode = 0x0D
		};

		const instruction_info_t g_or_memory_with_accumulator_absolute_indexed_with_x = {
			.name = "ora",
			.address_mode = ABSOLUTE_INDEXED_WITH_X,
			.opcode = 0x1D
		};

		const instruction_info_t g_or_memory_with_accumulator_absolute_indexed_with_y = {
			.name = "ora",
			.address_mode = ABSOLUTE_INDEXED_WITH_Y,
			.opcode = 0x19
		};

		const instruction_info_t g_or_memory_with_accumulator_immediate = {
			.name = "ora",
			.address_mode = IMMEDIATE,
			.opcode = 0x09
		};

		const instruction_info_t g_or_memory_with_accumulator_zero_page = {
			.name = "ora",
			.address_mode = ZERO_PAGE,
			.opcode = 0x05
		};

		const instruction_info_t g_or_memory_with_accumulator_zero_page_indexed_indirect = {
			.name = "ora",
			.address_mode = ZERO_PAGE_INDEXED_INDIRECT,
			.opcode = 0x01
		};

		const instruction_info_t g_or_memory_with_accumulator_zero_page_indexed_with_x = {
			.name = "ora",
			.address_mode = ZERO_PAGE_INDEXED_WITH_X,
			.opcode = 0x15
		};

		const instruction_info_t g_or_memory_with_accumulator_zero_page_indirect_indexed_with_y = {
			.name = "ora",
			.address_mode = ZERO_PAGE_INDIRECT_INDEXED_WITH_Y,
			.opcode = 0x11
		};

		// eor
		const instruction_info_t g_exlusive_or_memory_with_accumulator_absolute = {
			.name = "eor",
			.address_mode = ABSOLUTE,
			.opcode = 0x4D
		};

		const instruction_info_t g_exlusive_or_memory_with_accumulator_absolute_indexed_with_x = {
			.name = "eor",
			.address_mode = ABSOLUTE_INDEXED_WITH_X,
			.opcode = 0x5D
		};

		const instruction_info_t g_exlusive_or_memory_with_accumulator_absolute_indexed_with_y = {
			.name = "eor",
			.address_mode = ABSOLUTE_INDEXED_WITH_Y,
			.opcode = 0x59
		};

		const instruction_info_t g_exlusive_or_memory_with_accumulator_immediate = {
			.name = "eor",
			.address_mode = IMMEDIATE,
			.opcode = 0x49
		};

		const instruction_info_t g_exlusive_or_memory_with_accumulator_zero_page = {
			.name = "eor",
			.address_mode = ZERO_PAGE,
			.opcode = 0x45
		};

		const instruction_info_t g_exlusive_or_memory_with_accumulator_zero_page_indexed_indirect = {
			.name = "eor",
			.address_mode = ZERO_PAGE_INDEXED_INDIRECT,
			.opcode = 0x41
		};

		const instruction_info_t g_exlusive_or_memory_with_accumulator_zero_page_indexed_with_x = {
			.name = "eor",
			.address_mode = ZERO_PAGE_INDEXED_WITH_X,
			.opcode = 0x55
		};

		const instruction_info_t g_exlusive_or_memory_with_accumulator_zero_page_indirect_indexed_with_y = {
			.name = "eor",
			.address_mode = ZERO_PAGE_INDIRECT_INDEXED_WITH_Y,
			.opcode = 0x51
		};

		// cmp
		const instruction_info_t g_compare_memory_and_accumulator_absolute = {
			.name = "cmp",
			.address_mode = ABSOLUTE,
			.opcode = 0xCD
		};

		const instruction_info_t g_compare_memory_and_accumulator_absolute_indexed_with_x = {
			.name = "cmp",
			.address_mode = ABSOLUTE_INDEXED_WITH_X,
			.opcode = 0xDD
		};

		const instruction_info_t g_compare_memory_and_accumulator_absolute_indexed_with_y = {
			.name = "cmp",
			.address_mode = ABSOLUTE_INDEXED_WITH_Y,
			.opcode = 0xD9
		};

		const instruction_info_t g_compare_memory_and_accumulator_immediate = {
			.name = "cmp",
			.address_mode = IMMEDIATE,
			.opcode = 0xC9
		};

		const instruction_info_t g_compare_memory_and_accumulator_zero_page = {
			.name = "cmp",
			.address_mode = ZERO_PAGE,
			.opcode = 0xC5
		};

		const instruction_info_t g_compare_memory_and_accumulator_zero_page_indexed_indirect = {
			.name = "cmp",
			.address_mode = ZERO_PAGE_INDEXED_INDIRECT,
			.opcode = 0xC1
		};

		const instruction_info_t g_compare_memory_and_accumulator_zero_page_indexed_with_x = {
			.name = "cmp",
			.address_mode = ZERO_PAGE_INDEXED_WITH_X,
			.opcode = 0xD5
		};

		const instruction_info_t g_compare_memory_and_accumulator_zero_page_indirect_indexed_with_y = {
			.name = "cmp",
			.address_mode = ZERO_PAGE_INDIRECT_INDEXED_WITH_Y,
			.opcode = 0xD1
		};

		// cpx
		const instruction_info_t g_compare_memory_and_index_x_absolute = {
			.name = "cpx",
			.address_mode = ABSOLUTE,
			.opcode = 0xEC
		};

		const instruction_info_t g_compare_memory_and_index_x_immediate = {
			.name = "cpx",
			.address_mode = IMMEDIATE,
			.opcode = 0xE0
		};

		const instruction_info_t g_compare_memory_and_index_x_zero_page = {
			.name = "cpx",
			.address_mode = ZERO_PAGE,
			.opcode = 0xE4
		};

		// cpy
		const instruction_info_t g_compare_memory_and_index_y_absolute = {
			.name = "cpy",
			.address_mode = ABSOLUTE,
			.opcode = 0xCC
		};

		const instruction_info_t g_compare_memory_and_index_y_immediate = {
			.name = "cpy",
			.address_mode = IMMEDIATE,
			.opcode = 0xC0
		};

		const instruction_info_t g_compare_memory_and_index_y_zero_page = {
			.name = "cpy",
			.address_mode = ZERO_PAGE,
			.opcode = 0xC4
		};

		// bit
		const instruction_info_t g_test_bits_in_memory_with_accumulator_absolute = {
			.name = "bit",
			.address_mode = ABSOLUTE,
			.opcode = 0x2C
		};

		const instruction_info_t g_test_bits_in_memory_with_accumulator_immediate = {
			.name = "bit",
			.address_mode = IMMEDIATE,
			.opcode = 0x89
		};

		const instruction_info_t g_test_bits_in_memory_with_accumulator_zero_page = {
			.name = "bit",
			.address_mode = ZERO_PAGE,
			.opcode = 0x24
		};

		// bcc
		const instruction_info_t g_branch_on_carry_clear = {
			.name = "bcc",
			.address_mode = RELATIVE,
			.opcode = 0x90
		};

		// bcs
		const instruction_info_t g_branch_on_carry_set = {
			.name = "bcs",
			.address_mode = RELATIVE,
			.opcode = 0xB0
		};

		// bne
		const instruction_info_t g_branch_on_result_not_zero = {
			.name = "bne",
			.address_mode = RELATIVE,
			.opcode = 0xD0
		};

		// beq
		const instruction_info_t g_branch_on_result_zero = {
			.name = "beq",
			.address_mode = RELATIVE,
			.opcode = 0xF0
		};

		// bpl
		const instruction_info_t g_branch_on_result_plus = {
			.name = "bpl",
			.address_mode = RELATIVE,
			.opcode = 0x10
		};

		// bmi
		const instruction_info_t g_branch_on_result_minus = {
			.name = "bmi",
			.address_mode = RELATIVE,
			.opcode = 0x30
		};

		// bvc
		const instruction_info_t g_branch_on_overflow_clear = {
			.name = "bvc",
			.address_mode = RELATIVE,
			.opcode = 0x50
		};

		// bvs
		const instruction_info_t g_branch_on_overflow_set = {
			.name = "bvs",
			.address_mode = RELATIVE,
			.opcode = 0x70
		};

		// tax
		const instruction_info_t g_transfer_accumulator_to_index_x = {
			.name = "tax",
			.address_mode = IMPLIED,
			.opcode = 0xAA
		};

		// txa
		const instruction_info_t g_transfer_index_x_to_accumulator = {
			.name = "txa",
			.address_mode = IMPLIED,
			.opcode = 0x8A
		};

		// tay
		const instruction_info_t g_transfer_accumulator_to_index_y = {
			.name = "tay",
			.address_mode = IMPLIED,
			.opcode = 0xA8
		};

		// tya
		const instruction_info_t g_transfer_index_y_to_accumulator = {
			.name = "tya",
			.address_mode = IMPLIED,
			.opcode = 0x98
		};

		// tsx
		const instruction_info_t g_transfer_stack_pointer_to_index_x = {
			.name = "tsx",
			.address_mode = IMPLIED,
			.opcode = 0xBA
		};

		// txs
		const instruction_info_t g_transfer_index_x_to_stack_pointer = {
			.name = "txs",
			.address_mode = IMPLIED,
			.opcode = 0x9A
		};

		// pha
		const instruction_info_t g_push_accumulator_on_stack = {
			.name = "pha",
			.address_mode = IMPLIED,
			.opcode = 0x48
		};

		// pla
		const instruction_info_t g_pull_accumulator_from_stack = {
			.name = "pla",
			.address_mode = IMPLIED,
			.opcode = 0x68
		};

		// php
		const instruction_info_t g_push_processor_status_on_stack = {
			.name = "php",
			.address_mode = IMPLIED,
			.opcode = 0x08
		};

		// plp
		const instruction_info_t g_pull_processor_status_from_stack = {
			.name = "plp",
			.address_mode = IMPLIED,
			.opcode = 0x28
		};

		// jmp
		const instruction_info_t g_jump_absolute = {
			.name = "jmp",
			.address_mode = ABSOLUTE,
			.opcode = 0x4C
		};

		const instruction_info_t g_jump_absolute_indirect = {
			.name = "jmp",
			.address_mode = ABSOLUTE_INDIRECT,
			.opcode = 0x6C
		};

		// jsr
		const instruction_info_t g_jump_to_new_location_saving_return = {
			.name = "jsr",
			.address_mode = ABSOLUTE,
			.opcode = 0x20
		};

		// rts
		const instruction_info_t g_return_from_subroutine = {
			.name = "rts",
			.address_mode = IMPLIED,
			.opcode = 0x60
		};

		// rti
		const instruction_info_t g_return_from_interrupt = {
			.name = "rti",
			.address_mode = IMPLIED,
			.opcode = 0x40
		};

		// clc
		const instruction_info_t g_clear_carry_flag = {
			.name = "clc",
			.address_mode = IMPLIED,
			.opcode = 0x18
		};

		// sec
		const instruction_info_t g_set_carry_flag = {
			.name = "sec",
			.address_mode = IMPLIED,
			.opcode = 0x38
		};

		// cld
		const instruction_info_t g_clear_decimal_mode = {
			.name = "cld",
			.address_mode = IMPLIED,
			.opcode = 0xD8
		};

		// sed
		const instruction_info_t g_set_decimal_mode = {
			.name = "sed",
			.address_mode = IMPLIED,
			.opcode = 0xF8
		};

		// cli
		const instruction_info_t g_clear_interrupt_disable_status = {
			.name = "cli",
			.address_mode = IMPLIED,
			.opcode = 0x58
		};

		// sei
		const instruction_info_t g_set_interrupt_disable_status = {
			.name = "sei",
			.address_mode = IMPLIED,
			.opcode = 0x78
		};

		// clv
		const instruction_info_t g_clear_overflow_flag = {
			.name = "clv",
			.address_mode = IMPLIED,
			.opcode = 0xB8
		};

		// brk
		const instruction_info_t g_break = {
			.name = "brk",
			.address_mode = IMPLIED,
			.opcode = 0x00
		};

		// nop
		const instruction_info_t g_no_operation = {
			.name = "nop",
			.address_mode = IMPLIED,
			.opcode = 0xEA
		};

		const instruction_info_t* const g_instruction_table[] = {
			&g_break, // 0x00
			&g_or_memory_with_accumulator_zero_page_indexed_indirect, // 0x01
			NULL, // 0x02
			NULL, // 0x03
			NULL, // 0x04
			&g_or_memory_with_accumulator_zero_page, // 0x05
			&g_arithmetic_shift_left_one_bit_zero_page, // 0x06
			NULL, // 0x07
			&g_push_processor_status_on_stack, // 0x08
			&g_or_memory_with_accumulator_immediate, // 0x09
			&g_arithmetic_shift_left_one_bit_accumulator, // 0x0A
			NULL, // 0x0B
			NULL, // 0x0C
			&g_or_memory_with_accumulator_absolute, // 0x0D
			&g_arithmetic_shift_left_one_bit_absolute, // 0x0E
			NULL, // 0x0F
			&g_branch_on_result_plus, // 0x10
			&g_or_memory_with_accumulator_zero_page_indirect_indexed_with_y, // 0x11
			NULL, // 0x12
			NULL, // 0x13
			NULL, // 0x14
			&g_or_memory_with_accumulator_zero_page_indexed_with_x, // 0x15
			&g_arithmetic_shift_left_one_bit_zero_page_indexed_with_x, // 0x16
			NULL, // 0x17
			&g_clear_carry_flag, // 0x18
			&g_or_memory_with_accumulator_absolute_indexed_with_y, // 0x19
			NULL, // 0x1A
			NULL, // 0x1B
			NULL, // 0x1C
			&g_or_memory_with_accumulator_absolute_indexed_with_x, // 0x1D
			&g_arithmetic_shift_left_one_bit_absolute_indexed_with_x, // 0x1E
			NULL, // 0x1F
			&g_jump_to_new_location_saving_return, // 0x20
			&g_and_memory_with_accumulator_zero_page_indexed_indirect, // 0x21
			NULL, // 0x22
			NULL, // 0x23
			&g_test_bits_in_memory_with_accumulator_zero_page, // 0x24
			&g_and_memory_with_accumulator_zero_page, // 0x25
			&g_rotate_left_one_bit_zero_page, // 0x26
			NULL, // 0x27
			&g_pull_processor_status_from_stack, // 0x28
			&g_and_memory_with_accumulator_immediate, // 0x29
			&g_rotate_left_one_bit_accumulator, // 0x2A
			NULL, // 0x2B
			&g_test_bits_in_memory_with_accumulator_absolute, // 0x2C
			&g_and_memory_with_accumulator_absolute, // 0x2D
			&g_rotate_left_one_bit_absolute, // 0x2E
			NULL, // 0x2F
			&g_branch_on_result_minus, // 0x30
			&g_and_memory_with_accumulator_zero_page_indirect_indexed_with_y, // 0x31
			NULL, // 0x32
			NULL, // 0x33
			NULL, // 0x34
			&g_and_memory_with_accumulator_zero_page_indexed_with_x, // 0x35
			&g_rotate_left_one_bit_zero_page_indexed_with_x, // 0x36
			NULL, // 0x37
			&g_set_carry_flag, // 0x38
			&g_and_memory_with_accumulator_absolute_indexed_with_y, // 0x39
			NULL, // 0x3A
			NULL, // 0x3B
			NULL, // 0x3C
			&g_and_memory_with_accumulator_absolute_indexed_with_x, // 0x3D
			&g_rotate_left_one_bit_absolute_indexed_with_x, // 0x3E
			NULL, // 0x3F
			&g_return_from_interrupt, // 0x40
			&g_exlusive_or_memory_with_accumulator_zero_page_indexed_indirect, // 0x41
			NULL, // 0x42
			NULL, // 0x43
			NULL, // 0x44
			&g_exlusive_or_memory_with_accumulator_zero_page, // 0x45
			&g_logical_shift_right_one_bit_zero_page, // 0x46
			NULL, // 0x47
			&g_push_accumulator_on_stack, // 0x48
			&g_exlusive_or_memory_with_accumulator_immediate, // 0x49
			&g_logical_shift_right_one_bit_accumulator, // 0x4A
			NULL, // 0x4B
			&g_jump_absolute, // 0x4C
			&g_exlusive_or_memory_with_accumulator_absolute, // 0x4D
			&g_logical_shift_right_one_bit_absolute, // 0x4E
			NULL, // 0x4F
			&g_branch_on_overflow_clear, // 0x50
			&g_exlusive_or_memory_with_accumulator_zero_page_indirect_indexed_with_y, // 0x51
			NULL, // 0x52
			NULL, // 0x53
			NULL, // 0x54
			&g_exlusive_or_memory_with_accumulator_zero_page_indexed_with_x, // 0x55
			&g_logical_shift_right_one_bit_zero_page_indexed_with_x, // 0x56
			NULL, // 0x57
			&g_clear_interrupt_disable_status, // 0x58
			&g_exlusive_or_memory_with_accumulator_absolute_indexed_with_y, // 0x59
			NULL, // 0x5A
			NULL, // 0x5B
			NULL, // 0x5C
			&g_exlusive_or_memory_with_accumulator_absolute_indexed_with_x, // 0x5D
			&g_logical_shift_right_one_bit_absolute_indexed_with_x, // 0x5E
			NULL, // 0x5F
			&g_return_from_subroutine, // 0x60
			&g_add_memory_to_accumulator_with_carry_zero_page_indexed_indirect, // 0x61
			NULL, // 0x62
			NULL, // 0x63
			NULL, // 0x64
			&g_add_memory_to_accumulator_with_carry_zero_page, // 0x65
			&g_rotate_right_one_bit_zero_page, // 0x66
			NULL, // 0x67
			&g_pull_accumulator_from_stack, // 0x68
			&g_add_memory_to_accumulator_with_carry_immediate, // 0x69
			&g_rotate_right_one_bit_accumulator, // 0x6A
			NULL, // 0x6B
			&g_jump_absolute_indirect, // 0x6C
			&g_add_memory_to_accumulator_with_carry_absolute, // 0x6D
			&g_rotate_right_one_bit_absolute, // 0x6E
			NULL, // 0x6F
			&g_branch_on_overflow_set, // 0x70
			&g_add_memory_to_accumulator_with_carry_zero_page_indirect_indexed_with_y, // 0x71
			NULL, // 0x72
			NULL, // 0x73
			NULL, // 0x74
			&g_add_memory_to_accumulator_with_carry_zero_page_indexed_with_x, // 0x75
			&g_rotate_right_one_bit_zero_page_indexed_with_x, // 0x76
			NULL, // 0x77
			&g_set_interrupt_disable_status, // 0x78
			&g_add_memory_to_accumulator_with_carry_absolute_indexed_with_y, // 0x79
			NULL, // 0x7A
			NULL, // 0x7B
			NULL, // 0x7C
			&g_add_memory_to_accumulator_with_carry_absolute_indexed_with_x, // 0x7D
			&g_rotate_right_one_bit_absolute_indexed_with_x, // 0x7E
			NULL, // 0x7F
			NULL, // 0x80
			&g_store_accumulator_in_memory_zero_page_indexed_indirect, // 0x81
			NULL, // 0x82
			NULL, // 0x83
			&g_store_index_y_in_memory_zero_page, // 0x84
			&g_store_accumulator_in_memory_zero_page, // 0x85
			&g_store_index_x_in_memory_zero_page, // 0x86
			NULL, // 0x87
			&g_decrement_index_y_by_one, // 0x88
			NULL, // 0x89
			&g_transfer_index_x_to_accumulator, // 0x8A
			NULL, // 0x8B
			&g_store_index_y_in_memory_absolute, // 0x8C
			&g_store_accumulator_in_memory_absolute, // 0x8D
			&g_store_index_x_in_memory_absolute, // 0x8E
			NULL, // 0x8F
			&g_branch_on_carry_clear, // 0x90
			&g_store_accumulator_in_memory_zero_page_indirect_indexed_with_y, // 0x91
			NULL, // 0x92
			NULL, // 0x93
			&g_store_index_y_in_memory_zero_page_indexed_with_x, // 0x94
			&g_store_accumulator_in_memory_zero_page_indexed_with_x, // 0x95
			&g_store_index_x_in_memory_zero_page_indexed_with_y, // 0x96
			NULL, // 0x97
			&g_transfer_index_y_to_accumulator, // 0x98
			&g_store_accumulator_in_memory_absolute_indexed_with_y, // 0x99
			&g_transfer_index_x_to_stack_pointer, // 0x9A
			NULL, // 0x9B
			NULL, // 0x9C
			&g_store_accumulator_in_memory_absolute_indexed_with_x, // 0x9D
			NULL, // 0x9E
			NULL, // 0x9F
			&g_load_index_y_with_memory_immediate, // 0xA0
			&g_load_accumulator_with_memory_zero_page_indexed_indirect, // 0xA1
			&g_load_index_x_with_memory_immediate, // 0xA2
			NULL, // 0xA3
			&g_load_index_y_with_memory_zero_page, // 0xA4
			&g_load_accumulator_with_memory_zero_page, // 0xA5
			&g_load_index_x_with_memory_zero_page, // 0xA6
			NULL, // 0xA7
			&g_transfer_accumulator_to_index_y, // 0xA8
			&g_load_accumulator_with_memory_immediate, // 0xA9
			&g_transfer_accumulator_to_index_x, // 0xAA
			NULL, // 0xAB
			&g_load_index_y_with_memory_absolute, // 0xAC
			&g_load_accumulator_with_memory_absolute, // 0xAD
			&g_load_index_x_with_memory_absolute, // 0xAE
			NULL, // 0xAF
			&g_branch_on_carry_set, // 0xB0
			&g_load_accumulator_with_memory_zero_page_indirect_indexed_with_y, // 0xB1
			NULL, // 0xB2
			NULL, // 0xB3
			&g_load_index_y_with_memory_zero_page_indexed_with_x, // 0xB4
			&g_load_accumulator_with_memory_zero_page_indexed_with_x, // 0xB5
			&g_load_index_x_with_memory_zero_page_indexed_with_y, // 0xB6
			NULL, // 0xB7
			&g_clear_overflow_flag, // 0xB8
			&g_load_accumulator_with_memory_absolute_indexed_with_y, // 0xB9
			&g_transfer_stack_pointer_to_index_x, // 0xBA
			NULL, // 0xBB
			&g_load_index_y_with_memory_absolute_indexed_with_x, // 0xBC
			&g_load_accumulator_with_memory_absolute_indexed_with_x, // 0xBD
			&g_load_index_x_with_memory_absolute_indexed_with_y, // 0xBE
			NULL, // 0xBF
			&g_compare_memory_and_index_y_immediate, // 0xC0
			&g_compare_memory_and_accumulator_zero_page_indexed_indirect, // 0xC1
			NULL, // 0xC2
			NULL, // 0xC3
			&g_compare_memory_and_index_y_zero_page, // 0xC4
			&g_compare_memory_and_accumulator_zero_page, // 0xC5
			&g_decrement_memory_by_one_zero_page, // 0xC6
			NULL, // 0xC7
			&g_increment_index_y_by_one, // 0xC8
			&g_compare_memory_and_accumulator_immediate, // 0xC9
			&g_decrement_index_x_by_one, // 0xCA
			NULL, // 0xCB
			&g_compare_memory_and_index_y_absolute, // 0xCC
			&g_compare_memory_and_accumulator_absolute, // 0xCD
			&g_decrement_memory_by_one_absolute, // 0xCE
			NULL, // 0xCF
			&g_branch_on_result_not_zero, // 0xD0
			&g_compare_memory_and_accumulator_zero_page_indirect_indexed_with_y, // 0xD1
			NULL, // 0xD2
			NULL, // 0xD3
			NULL, // 0xD4
			&g_compare_memory_and_accumulator_zero_page_indexed_with_x, // 0xD5
			&g_decrement_memory_by_one_zero_page_indexed_with_x, // 0xD6
			NULL, // 0xD7
			&g_clear_decimal_mode, // 0xD8
			&g_compare_memory_and_accumulator_absolute_indexed_with_y, // 0xD9
			NULL, // 0xDA
			NULL, // 0xDB
			NULL, // 0xDC
			&g_compare_memory_and_accumulator_absolute_indexed_with_x, // 0xDD
			&g_decrement_memory_by_one_absolute_indexed_with_x, // 0xDE
			NULL, // 0xDF
			&g_compare_memory_and_index_x_immediate, // 0xE0
			&g_subtract_memory_from_accumulator_with_borrow_zero_page_indexed_indirect, // 0xE1
			NULL, // 0xE2
			NULL, // 0xE3
			&g_compare_memory_and_index_x_zero_page, // 0xE4
			&g_subtract_memory_from_accumulator_with_borrow_zero_page, // 0xE5
			&g_increment_memory_by_one_zero_page, // 0xE6
			NULL, // 0xE7
			&g_increment_index_x_by_one, // 0xE8
			&g_subtract_memory_from_accumulator_with_borrow_immediate, // 0xE9
			&g_no_operation, // 0xEA
			NULL, // 0xEB
			&g_compare_memory_and_index_x_absolute, // 0xEC
			&g_subtract_memory_from_accumulator_with_borrow_absolute, // 0xED
			&g_increment_memory_by_one_absolute, // 0xEE
			NULL, // 0xEF
			&g_branch_on_result_zero, // 0xF0
			&g_subtract_memory_from_accumulator_with_borrow_zero_page_indirect_indexed_with_y, // 0xF1
			NULL, // 0xF2
			NULL, // 0xF3
			NULL, // 0xF4
			&g_subtract_memory_from_accumulator_with_borrow_zero_page_indexed_with_x, // 0xF5
			&g_increment_memory_by_one_zero_page_indexed_with_x, // 0xF6
			NULL, // 0xF7
			&g_set_decimal_mode, // 0xF8
			&g_subtract_memory_from_accumulator_with_borrow_absolute_indexed_with_y, // 0xF9
			NULL, // 0xFA
			NULL, // 0xFB
			NULL, // 0xFC
			&g_subtract_memory_from_accumulator_with_borrow_absolute_indexed_with_x, // 0xFD
			&g_increment_memory_by_one_absolute_indexed_with_x, // 0xFE
			NULL, // 0xFF
		};

		const uint32_t G_BUFFER_SIZE = 64;
		const char* const g_empty_byte = "..";
		const char* const g_hex_table = "0123456789ABCDEF";

		const uint8_t* Disassemble( char* out_buffer64, const uint8_t* mem )
		{
			// read instruction
			const uint8_t opcode = mem[0];
			const instruction_info_t* const p_instruction = g_instruction_table[opcode];
			if ( p_instruction == NULL ) {
				return mem;
			}

			const instruction_info_t instruction = *p_instruction;
			const char* const mnemonic = instruction.name;
			const char* const address_mode = g_address_mode_to_string[instruction.address_mode];
			const uint8_t operand_bytes = g_address_mode_operand_bytes_needed[instruction.address_mode];
			const uint8_t operand_hi_byte = operand_bytes > 1 ? mem[2] : 0;
			const uint8_t operand_lo_byte = operand_bytes > 0 ? mem[1] : 0;

			const char operand_hi_byte_string[3] = {
				operand_bytes > 1 ? g_hex_table[( operand_hi_byte >> 4 ) & 0xF] : '.',
				operand_bytes > 1 ? g_hex_table[operand_hi_byte & 0xF] : '.',
				'\0'
			};

			const char operand_lo_byte_string[3] = {
				operand_bytes > 0 ? g_hex_table[( operand_lo_byte >> 4 ) & 0xF] : '.',
				operand_bytes > 0 ? g_hex_table[operand_lo_byte & 0xF] : '.',
				'\0'
			};

			memset( out_buffer64, 0, G_BUFFER_SIZE );
			sprintf_s( out_buffer64, G_BUFFER_SIZE, "OPCODE=%02X[%s %s] OPERAND=%s %s", opcode, mnemonic, address_mode, operand_hi_byte_string, operand_lo_byte_string );

			return mem + 1 + operand_bytes;
		}
	}
}
