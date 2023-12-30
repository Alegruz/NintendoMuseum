#include "stdafx.h"

#include <bitset>

#include "NES/Cartridge.h"
#include "NES/DynamicArray.hpp"

namespace ninmuse
{
	namespace nes
	{
		Cartridge::Cartridge( const std::filesystem::path& romFilePath ) noexcept
			: mRomFilePath( romFilePath )
			, mRomFile()
			, mHeader()
			, mTrainer()
			, mProgramRom()
			, mCharacterRom()
		{
			mRomFile.open( mRomFilePath.c_str(), std::ios_base::binary | std::ios_base::in );
			NM_ASSERT( mRomFile.is_open(), "Given ROM file is not open!!" );
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
							NM_ASSERT( false, "Invalid TV System Type!!" );
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
							NM_ASSERT( false, "Invalid system!!" );
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
			NM_ASSERT( mHeader->Flags06.Bits.IsTrainerPresent, "Trainer data is not present!!" );
			mTrainer = std::make_unique<Cartridge::Trainer>();

			mRomFile.read( mTrainer->Data, Trainer::DATA_SIZE );

			return *mTrainer;
		}

		const DynamicArray<data_t>& Cartridge::ReadProgramRom() noexcept
		{
			// [TODO]: Vs. Dual System calculates program ROM differently.
			const size_t programRomSize = getProgramRomSize();
			std::cout << "PRG-ROM Size: " << programRomSize << std::endl;

			mProgramRom.Data.SetSize( programRomSize );
			for ( size_t i = 0; i < programRomSize; ++i )
			{
				mRomFile.read( reinterpret_cast<char*>( &mProgramRom.Data[i] ), 1 );
			}

			return mProgramRom.Data;
		}

		const DynamicArray<data_t>& Cartridge::ReadCharacterRom() noexcept
		{
			const size_t characterRomSize = getCharacterRomSize();
			std::cout << "CHR-ROM Size: " << characterRomSize << std::endl;

			mCharacterRom.Data.SetSize( characterRomSize );
			mRomFile.read( reinterpret_cast<char*>( mCharacterRom.Data.GetData() ), characterRomSize );
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
	}
}
