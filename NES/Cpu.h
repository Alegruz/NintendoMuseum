#pragma once

#include <queue>

#include "Common.h"

#include "NES/Memory.h"

namespace ninmuse
{
	template <Data TData, Address TAddress>
	class ICpu
	{
	public:
		ICpu() = delete;
		inline constexpr ICpu( IRam<TData>& ram ) noexcept : mRam( ram ) {}
		ICpu( const ICpu& ) = delete;
		explicit ICpu( ICpu&& ) noexcept = default;
		virtual ~ICpu() = default;

		ICpu& operator=( const ICpu& ) = delete;
		ICpu& operator=( ICpu&& ) noexcept = default;

	public:
		constexpr const TData&	Read( const TAddress& address ) const noexcept;
		constexpr void			Write( const TAddress& address, const TData& data ) noexcept;

	private:
		IRam<TData>&	mRam;
	};

	namespace nes
	{
		class Cpu6502 : public ICpu<data_t, address_t>
		{
		public:
			Cpu6502() = delete;
			inline Cpu6502( IRam<data_t>& ram, const Cartridge* cartridgeOrNull ) noexcept
				: ICpu<data_t, address_t>( ram )
				, mRomOrNull( cartridgeOrNull )
				, mRegisters()
				, mCurrentInternalMode( eExternalMode::FETCH_OPCODE )
				, mCurrentExternalMode( eInternalMode::PREVIOUS )
				, mReadRam( false )
				, mAddressBus()
				, mDataBus()
				, mDataToDecode()
				, mDecodeCounter( 0 )
				, mExecutionInfo{ .InstructionInfoOrNull = nullptr, .IsReady = false }
				, mJumpSubroutineClock( 0 )
			{}
			Cpu6502( const Cpu6502& ) = delete;
			explicit Cpu6502( Cpu6502&& ) noexcept = default;
			virtual ~Cpu6502() = default;

			Cpu6502& operator=( const Cpu6502& ) = delete;
			Cpu6502& operator=( Cpu6502&& ) noexcept = default;

		public:
			enum class eAddressMode : uint8_t
			{
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
				COUNT,
				NONE = COUNT,
			};

			enum class eMnemonic : uint8_t
			{
				ADC = 0,
				AND,
				ASL,
				BCC,
				BCS,
				BEQ,
				BIT,
				BMI,
				BNE,
				BPL,
				BRK,
				BVC,
				BVS,
				CLC,
				CLD,
				CLI,
				CLV,
				CMP,
				CPX,
				CPY,
				DEC,
				DEX,
				DEY,
				EOR,
				INC,
				INX,
				INY,
				JMP,
				JSR,
				LDA,
				LDX,
				LDY,
				LSR,
				NOP,
				ORA,
				PHA,
				PHP,
				PLA,
				PLP,
				ROL,
				ROR,
				RTI,
				RTS,
				SBC,
				SEC,
				SED,
				SEI,
				STA,
				STX,
				STY,
				TAX,
				TAY,
				TSX,
				TXA,
				TXS,
				TYA,
				COUNT,
			};

			struct Pins {};

			struct InstructionInfo final
			{
				const char*		Name;
				eAddressMode	AddressMode;
				data_t			Opcode;
				eMnemonic		Mnemonic;
			};

			struct ExecutionInfo final
			{
				const InstructionInfo*	InstructionInfoOrNull;
				union Operand
				{
					address_t Address;
					struct Data
					{
					public:
						data_t	Value;
					private:
						data_t	Padding;
					} Data;
					data_t	Bytes[2];
				} Operand;
				bool					IsReady;

				inline constexpr void	Reset() noexcept { InstructionInfoOrNull = nullptr; IsReady = false; }
			};

		public:
			inline constexpr void	SetRom( const Cartridge& cartridge ) noexcept { mRomOrNull = &cartridge; }

			data_t	ReadRom( const address_t& address ) const noexcept;
			void	Run() noexcept;

		protected:
			// instructions
			struct Instruction
			{
				struct Lda
				{
					static constexpr const InstructionInfo ABSOLUTE =
					{
						.Name = "lda",
						.AddressMode = eAddressMode::ABSOLUTE,
						.Opcode = 0xAD,
						.Mnemonic = eMnemonic::LDA,
					};
					static constexpr const InstructionInfo ABSOLUTE_INDEXED_WITH_X =
					{
						.Name = "lda",
						.AddressMode = eAddressMode::ABSOLUTE_INDEXED_WITH_X,
						.Opcode = 0xBD,
						.Mnemonic = eMnemonic::LDA,
					};
					static constexpr const InstructionInfo ABSOLUTE_INDEXED_WITH_Y =
					{
						.Name = "lda",
						.AddressMode = eAddressMode::ABSOLUTE_INDEXED_WITH_Y,
						.Opcode = 0xB9,
						.Mnemonic = eMnemonic::LDA,
					};
					static constexpr const InstructionInfo IMMEDIATE =
					{
						.Name = "lda",
						.AddressMode = eAddressMode::IMMEDIATE,
						.Opcode = 0xA9,
						.Mnemonic = eMnemonic::LDA,
					};
					static constexpr const InstructionInfo ZERO_PAGE =
					{
						.Name = "lda",
						.AddressMode = eAddressMode::ZERO_PAGE,
						.Opcode = 0xA5,
						.Mnemonic = eMnemonic::LDA,
					};
					static constexpr const InstructionInfo ZERO_PAGE_INDEXED_INDIRECT =
					{
						.Name = "lda",
						.AddressMode = eAddressMode::ZERO_PAGE_INDEXED_INDIRECT,
						.Opcode = 0xA1,
						.Mnemonic = eMnemonic::LDA,
					};
					static constexpr const InstructionInfo ZERO_PAGE_INDEXED_WITH_X =
					{
						.Name = "lda",
						.AddressMode = eAddressMode::ZERO_PAGE_INDEXED_WITH_X,
						.Opcode = 0xB5,
						.Mnemonic = eMnemonic::LDA,
					};
					static constexpr const InstructionInfo ZERO_PAGE_INDIRECT_INDEXED_WITH_Y =
					{
						.Name = "lda",
						.AddressMode = eAddressMode::ZERO_PAGE_INDIRECT_INDEXED_WITH_Y,
						.Opcode = 0xB1,
						.Mnemonic = eMnemonic::LDA,
					};
				};
				struct Ldx
				{
					static constexpr const InstructionInfo ABSOLUTE =
					{
						.Name = "ldx",
						.AddressMode = eAddressMode::ABSOLUTE,
						.Opcode = 0xAE,
						.Mnemonic = eMnemonic::LDX,
					};
					static constexpr const InstructionInfo ABSOLUTE_INDEXED_WITH_Y =
					{
						.Name = "ldx",
						.AddressMode = eAddressMode::ABSOLUTE_INDEXED_WITH_Y,
						.Opcode = 0xBE,
						.Mnemonic = eMnemonic::LDX,
					};
					static constexpr const InstructionInfo IMMEDIATE =
					{
						.Name = "ldx",
						.AddressMode = eAddressMode::IMMEDIATE,
						.Opcode = 0xA2,
						.Mnemonic = eMnemonic::LDX,
					};
					static constexpr const InstructionInfo ZERO_PAGE =
					{
						.Name = "ldx",
						.AddressMode = eAddressMode::ZERO_PAGE,
						.Opcode = 0xA6,
						.Mnemonic = eMnemonic::LDX,
					};
					static constexpr const InstructionInfo ZERO_PAGE_INDEXED_WITH_Y =
					{
						.Name = "ldx",
						.AddressMode = eAddressMode::ZERO_PAGE_INDEXED_WITH_Y,
						.Opcode = 0xB6,
						.Mnemonic = eMnemonic::LDX,
					};
				};
				struct Ldy
				{
					static constexpr const InstructionInfo ABSOLUTE =
					{
						.Name = "ldy",
						.AddressMode = eAddressMode::ABSOLUTE,
						.Opcode = 0xAC,
						.Mnemonic = eMnemonic::LDY,
					};
					static constexpr const InstructionInfo ABSOLUTE_INDEXED_WITH_X =
					{
						.Name = "ldy",
						.AddressMode = eAddressMode::ABSOLUTE_INDEXED_WITH_X,
						.Opcode = 0xBC,
						.Mnemonic = eMnemonic::LDY,
					};
					static constexpr const InstructionInfo IMMEDIATE =
					{
						.Name = "ldy",
						.AddressMode = eAddressMode::IMMEDIATE,
						.Opcode = 0xA0,
						.Mnemonic = eMnemonic::LDY,
					};
					static constexpr const InstructionInfo ZERO_PAGE =
					{
						.Name = "ldy",
						.AddressMode = eAddressMode::ZERO_PAGE,
						.Opcode = 0xA4,
						.Mnemonic = eMnemonic::LDY,
					};
					static constexpr const InstructionInfo ZERO_PAGE_INDEXED_WITH_X =
					{
						.Name = "ldy",
						.AddressMode = eAddressMode::ZERO_PAGE_INDEXED_WITH_X,
						.Opcode = 0xB4,
						.Mnemonic = eMnemonic::LDY,
					};
				};
				struct Sta
				{
					static constexpr const InstructionInfo ABSOLUTE =
					{
						.Name = "sta",
						.AddressMode = eAddressMode::ABSOLUTE,
						.Opcode = 0x8D,
						.Mnemonic = eMnemonic::STA,
					};
					static constexpr const InstructionInfo ABSOLUTE_INDEXED_WITH_X =
					{
						.Name = "sta",
						.AddressMode = eAddressMode::ABSOLUTE_INDEXED_WITH_X,
						.Opcode = 0x9D,
						.Mnemonic = eMnemonic::STA,
					};
					static constexpr const InstructionInfo ABSOLUTE_INDEXED_WITH_Y =
					{
						.Name = "sta",
						.AddressMode = eAddressMode::ABSOLUTE_INDEXED_WITH_Y,
						.Opcode = 0x99,
						.Mnemonic = eMnemonic::STA,
					};
					static constexpr const InstructionInfo ZERO_PAGE =
					{
						.Name = "sta",
						.AddressMode = eAddressMode::ZERO_PAGE,
						.Opcode = 0x85,
						.Mnemonic = eMnemonic::STA,
					};
					static constexpr const InstructionInfo ZERO_PAGE_INDEXED_INDIRECT =
					{
						.Name = "sta",
						.AddressMode = eAddressMode::ZERO_PAGE_INDEXED_INDIRECT,
						.Opcode = 0x81,
						.Mnemonic = eMnemonic::STA,
					};
					static constexpr const InstructionInfo ZERO_PAGE_INDEXED_WITH_X =
					{
						.Name = "sta",
						.AddressMode = eAddressMode::ZERO_PAGE_INDEXED_WITH_X,
						.Opcode = 0x95,
						.Mnemonic = eMnemonic::STA,
					};
					static constexpr const InstructionInfo ZERO_PAGE_INDIRECT_INDEXED_WITH_Y =
					{
						.Name = "sta",
						.AddressMode = eAddressMode::ZERO_PAGE_INDIRECT_INDEXED_WITH_Y,
						.Opcode = 0x91,
						.Mnemonic = eMnemonic::STA,
					};
				};
				struct Stx
				{
					static constexpr const InstructionInfo ABSOLUTE =
					{
						.Name = "stx",
						.AddressMode = eAddressMode::ABSOLUTE,
						.Opcode = 0x8E,
						.Mnemonic = eMnemonic::STX,
					};
					static constexpr const InstructionInfo ZERO_PAGE =
					{
						.Name = "stx",
						.AddressMode = eAddressMode::ZERO_PAGE,
						.Opcode = 0x86,
						.Mnemonic = eMnemonic::STX,
					};
					static constexpr const InstructionInfo ZERO_PAGE_INDEXED_WITH_Y =
					{
						.Name = "stx",
						.AddressMode = eAddressMode::ZERO_PAGE_INDEXED_WITH_Y,
						.Opcode = 0x96,
						.Mnemonic = eMnemonic::STX,
					};
				};
				struct Sty
				{
					static constexpr const InstructionInfo ABSOLUTE =
					{
						.Name = "sty",
						.AddressMode = eAddressMode::ABSOLUTE,
						.Opcode = 0x8C,
						.Mnemonic = eMnemonic::STY,
					};
					static constexpr const InstructionInfo ZERO_PAGE =
					{
						.Name = "sty",
						.AddressMode = eAddressMode::ZERO_PAGE,
						.Opcode = 0x84,
						.Mnemonic = eMnemonic::STY,
					};
					static constexpr const InstructionInfo ZERO_PAGE_INDEXED_WITH_X =
					{
						.Name = "sty",
						.AddressMode = eAddressMode::ZERO_PAGE_INDEXED_WITH_X,
						.Opcode = 0x94,
						.Mnemonic = eMnemonic::STY,
					};
				};
				struct Adc
				{
					static constexpr const InstructionInfo ABSOLUTE =
					{
						.Name = "adc",
						.AddressMode = eAddressMode::ABSOLUTE,
						.Opcode = 0x6D,
						.Mnemonic = eMnemonic::ADC,
					};
					static constexpr const InstructionInfo ABSOLUTE_INDEXED_WITH_X =
					{
						.Name = "adc",
						.AddressMode = eAddressMode::ABSOLUTE_INDEXED_WITH_X,
						.Opcode = 0x7D,
						.Mnemonic = eMnemonic::ADC,
					};
					static constexpr const InstructionInfo ABSOLUTE_INDEXED_WITH_Y =
					{
						.Name = "adc",
						.AddressMode = eAddressMode::ABSOLUTE_INDEXED_WITH_Y,
						.Opcode = 0x79,
						.Mnemonic = eMnemonic::ADC,
					};
					static constexpr const InstructionInfo IMMEDIATE =
					{
						.Name = "adc",
						.AddressMode = eAddressMode::IMMEDIATE,
						.Opcode = 0x69,
						.Mnemonic = eMnemonic::ADC,
					};
					static constexpr const InstructionInfo ZERO_PAGE =
					{
						.Name = "adc",
						.AddressMode = eAddressMode::ZERO_PAGE,
						.Opcode = 0x65,
						.Mnemonic = eMnemonic::ADC,
					};
					static constexpr const InstructionInfo ZERO_PAGE_INDEXED_INDIRECT =
					{
						.Name = "adc",
						.AddressMode = eAddressMode::ZERO_PAGE_INDEXED_INDIRECT,
						.Opcode = 0x61,
						.Mnemonic = eMnemonic::ADC,
					};
					static constexpr const InstructionInfo ZERO_PAGE_INDEXED_WITH_X =
					{
						.Name = "adc",
						.AddressMode = eAddressMode::ZERO_PAGE_INDEXED_WITH_X,
						.Opcode = 0x75,
						.Mnemonic = eMnemonic::ADC,
					};
					static constexpr const InstructionInfo ZERO_PAGE_INDIRECT_INDEXED_WITH_Y =
					{
						.Name = "adc",
						.AddressMode = eAddressMode::ZERO_PAGE_INDIRECT_INDEXED_WITH_Y,
						.Opcode = 0x71,
						.Mnemonic = eMnemonic::ADC,
					};
				};
				struct Sbc
				{
					static constexpr const InstructionInfo ABSOLUTE =
					{
						.Name = "sbc",
						.AddressMode = eAddressMode::ABSOLUTE,
						.Opcode = 0xED,
						.Mnemonic = eMnemonic::SBC,
					};
					static constexpr const InstructionInfo ABSOLUTE_INDEXED_WITH_X =
					{
						.Name = "sbc",
						.AddressMode = eAddressMode::ABSOLUTE_INDEXED_WITH_X,
						.Opcode = 0xFD,
						.Mnemonic = eMnemonic::SBC,
					};
					static constexpr const InstructionInfo ABSOLUTE_INDEXED_WITH_Y =
					{
						.Name = "sbc",
						.AddressMode = eAddressMode::ABSOLUTE_INDEXED_WITH_Y,
						.Opcode = 0xF9,
						.Mnemonic = eMnemonic::SBC,
					};
					static constexpr const InstructionInfo IMMEDIATE =
					{
						.Name = "sbc",
						.AddressMode = eAddressMode::IMMEDIATE,
						.Opcode = 0xE9,
						.Mnemonic = eMnemonic::SBC,
					};
					static constexpr const InstructionInfo ZERO_PAGE =
					{
						.Name = "sbc",
						.AddressMode = eAddressMode::ZERO_PAGE,
						.Opcode = 0xE5,
						.Mnemonic = eMnemonic::SBC,
					};
					static constexpr const InstructionInfo ZERO_PAGE_INDEXED_INDIRECT =
					{
						.Name = "sbc",
						.AddressMode = eAddressMode::ZERO_PAGE_INDEXED_INDIRECT,
						.Opcode = 0xE1,
						.Mnemonic = eMnemonic::SBC,
					};
					static constexpr const InstructionInfo ZERO_PAGE_INDEXED_WITH_X =
					{
						.Name = "sbc",
						.AddressMode = eAddressMode::ZERO_PAGE_INDEXED_WITH_X,
						.Opcode = 0xF5,
						.Mnemonic = eMnemonic::SBC,
					};
					static constexpr const InstructionInfo ZERO_PAGE_INDIRECT_INDEXED_WITH_Y =
					{
						.Name = "sbc",
						.AddressMode = eAddressMode::ZERO_PAGE_INDIRECT_INDEXED_WITH_Y,
						.Opcode = 0xF1,
						.Mnemonic = eMnemonic::SBC,
					};
				};
				struct Inc
				{
					static constexpr const InstructionInfo ABSOLUTE =
					{
						.Name = "inc",
						.AddressMode = eAddressMode::ABSOLUTE,
						.Opcode = 0xEE,
						.Mnemonic = eMnemonic::INC,
					};
					static constexpr const InstructionInfo ABSOLUTE_INDEXED_WITH_X =
					{
						.Name = "inc",
						.AddressMode = eAddressMode::ABSOLUTE_INDEXED_WITH_X,
						.Opcode = 0xFE,
						.Mnemonic = eMnemonic::INC,
					};
					static constexpr const InstructionInfo ZERO_PAGE =
					{
						.Name = "inc",
						.AddressMode = eAddressMode::ZERO_PAGE,
						.Opcode = 0xE6,
						.Mnemonic = eMnemonic::INC,
					};
					static constexpr const InstructionInfo ZERO_PAGE_INDEXED_WITH_X =
					{
						.Name = "inc",
						.AddressMode = eAddressMode::ZERO_PAGE_INDEXED_WITH_X,
						.Opcode = 0xF6,
						.Mnemonic = eMnemonic::INC,
					};
				};
				struct Inx
				{
					static constexpr const InstructionInfo IMPLIED =
					{
						.Name = "inx",
						.AddressMode = eAddressMode::IMPLIED,
						.Opcode = 0xE8,
						.Mnemonic = eMnemonic::INX,
					};
				};
				struct Iny
				{
					static constexpr const InstructionInfo IMPLIED =
					{
						.Name = "iny",
						.AddressMode = eAddressMode::IMPLIED,
						.Opcode = 0xC8,
						.Mnemonic = eMnemonic::INY,
					};
				};
				struct Dec
				{
					static constexpr const InstructionInfo ABSOLUTE =
					{
						.Name = "dec",
						.AddressMode = eAddressMode::ABSOLUTE,
						.Opcode = 0xCE,
						.Mnemonic = eMnemonic::DEC,
					};
					static constexpr const InstructionInfo ABSOLUTE_INDEXED_WITH_X =
					{
						.Name = "dec",
						.AddressMode = eAddressMode::ABSOLUTE_INDEXED_WITH_X,
						.Opcode = 0xDE,
						.Mnemonic = eMnemonic::DEC,
					};
					static constexpr const InstructionInfo ZERO_PAGE =
					{
						.Name = "dec",
						.AddressMode = eAddressMode::ZERO_PAGE,
						.Opcode = 0xC6,
						.Mnemonic = eMnemonic::DEC,
					};
					static constexpr const InstructionInfo ZERO_PAGE_INDEXED_WITH_X =
					{
						.Name = "dec",
						.AddressMode = eAddressMode::ZERO_PAGE_INDEXED_WITH_X,
						.Opcode = 0xD6,
						.Mnemonic = eMnemonic::DEC,
					};
				};
				struct Dex
				{
					static constexpr const InstructionInfo IMPLIED =
					{
						.Name = "dex",
						.AddressMode = eAddressMode::IMPLIED,
						.Opcode = 0xCA,
						.Mnemonic = eMnemonic::DEX,
					};
				};
				struct Dey
				{
					static constexpr const InstructionInfo IMPLIED =
					{
						.Name = "dey",
						.AddressMode = eAddressMode::IMPLIED,
						.Opcode = 0x88,
						.Mnemonic = eMnemonic::DEY,
					};
				};
				struct Asl
				{
					static constexpr const InstructionInfo ABSOLUTE =
					{
						.Name = "asl",
						.AddressMode = eAddressMode::ABSOLUTE,
						.Opcode = 0x0E,
						.Mnemonic = eMnemonic::ASL,
					};
					static constexpr const InstructionInfo ABSOLUTE_INDEXED_WITH_X =
					{
						.Name = "asl",
						.AddressMode = eAddressMode::ABSOLUTE_INDEXED_WITH_X,
						.Opcode = 0x1E,
						.Mnemonic = eMnemonic::ASL,
					};
					static constexpr const InstructionInfo ACCUMULATOR =
					{
						.Name = "asl",
						.AddressMode = eAddressMode::ACCUMULATOR,
						.Opcode = 0x0A,
						.Mnemonic = eMnemonic::ASL,
					};
					static constexpr const InstructionInfo ZERO_PAGE =
					{
						.Name = "asl",
						.AddressMode = eAddressMode::ZERO_PAGE,
						.Opcode = 0x06,
						.Mnemonic = eMnemonic::ASL,
					};
					static constexpr const InstructionInfo ZERO_PAGE_INDEXED_WITH_X =
					{
						.Name = "asl",
						.AddressMode = eAddressMode::ZERO_PAGE_INDEXED_WITH_X,
						.Opcode = 0x16,
						.Mnemonic = eMnemonic::ASL,
					};
				};
				struct Lsr
				{
					static constexpr const InstructionInfo ABSOLUTE =
					{
						.Name = "lsr",
						.AddressMode = eAddressMode::ABSOLUTE,
						.Opcode = 0x4E,
						.Mnemonic = eMnemonic::LSR,
					};
					static constexpr const InstructionInfo ABSOLUTE_INDEXED_WITH_X =
					{
						.Name = "lsr",
						.AddressMode = eAddressMode::ABSOLUTE_INDEXED_WITH_X,
						.Opcode = 0x5E,
						.Mnemonic = eMnemonic::LSR,
					};
					static constexpr const InstructionInfo ACCUMULATOR =
					{
						.Name = "lsr",
						.AddressMode = eAddressMode::ACCUMULATOR,
						.Opcode = 0x4A,
						.Mnemonic = eMnemonic::LSR,
					};
					static constexpr const InstructionInfo ZERO_PAGE =
					{
						.Name = "lsr",
						.AddressMode = eAddressMode::ZERO_PAGE,
						.Opcode = 0x46,
						.Mnemonic = eMnemonic::LSR,
					};
					static constexpr const InstructionInfo ZERO_PAGE_INDEXED_WITH_X =
					{
						.Name = "lsr",
						.AddressMode = eAddressMode::ZERO_PAGE_INDEXED_WITH_X,
						.Opcode = 0x56,
						.Mnemonic = eMnemonic::LSR,
					};
				};
				struct Rol
				{
					static constexpr const InstructionInfo ABSOLUTE =
					{
						.Name = "rol",
						.AddressMode = eAddressMode::ABSOLUTE,
						.Opcode = 0x2E,
						.Mnemonic = eMnemonic::ROL,
					};
					static constexpr const InstructionInfo ABSOLUTE_INDEXED_WITH_X =
					{
						.Name = "rol",
						.AddressMode = eAddressMode::ABSOLUTE_INDEXED_WITH_X,
						.Opcode = 0x3E,
						.Mnemonic = eMnemonic::ROL,
					};
					static constexpr const InstructionInfo ACCUMULATOR =
					{
						.Name = "rol",
						.AddressMode = eAddressMode::ACCUMULATOR,
						.Opcode = 0x2A,
						.Mnemonic = eMnemonic::ROL,
					};
					static constexpr const InstructionInfo ZERO_PAGE =
					{
						.Name = "rol",
						.AddressMode = eAddressMode::ZERO_PAGE,
						.Opcode = 0x26,
						.Mnemonic = eMnemonic::ROL,
					};
					static constexpr const InstructionInfo ZERO_PAGE_INDEXED_WITH_X =
					{
						.Name = "rol",
						.AddressMode = eAddressMode::ZERO_PAGE_INDEXED_WITH_X,
						.Opcode = 0x36,
						.Mnemonic = eMnemonic::ROL,
					};
				};
				struct Ror
				{
					static constexpr const InstructionInfo ABSOLUTE =
					{
						.Name = "ror",
						.AddressMode = eAddressMode::ABSOLUTE,
						.Opcode = 0x6E,
						.Mnemonic = eMnemonic::ROR,
					};
					static constexpr const InstructionInfo ABSOLUTE_INDEXED_WITH_X =
					{
						.Name = "ror",
						.AddressMode = eAddressMode::ABSOLUTE_INDEXED_WITH_X,
						.Opcode = 0x7E,
						.Mnemonic = eMnemonic::ROR,
					};
					static constexpr const InstructionInfo ACCUMULATOR =
					{
						.Name = "ror",
						.AddressMode = eAddressMode::ACCUMULATOR,
						.Opcode = 0x6A,
						.Mnemonic = eMnemonic::ROR,
					};
					static constexpr const InstructionInfo ZERO_PAGE =
					{
						.Name = "ror",
						.AddressMode = eAddressMode::ZERO_PAGE,
						.Opcode = 0x66,
						.Mnemonic = eMnemonic::ROR,
					};
					static constexpr const InstructionInfo ZERO_PAGE_INDEXED_WITH_X =
					{
						.Name = "ror",
						.AddressMode = eAddressMode::ZERO_PAGE_INDEXED_WITH_X,
						.Opcode = 0x76,
						.Mnemonic = eMnemonic::ROR,
					};
				};
				struct And
				{
					static constexpr const InstructionInfo ABSOLUTE =
					{
						.Name = "and",
						.AddressMode = eAddressMode::ABSOLUTE,
						.Opcode = 0x2D,
						.Mnemonic = eMnemonic::AND,
					};
					static constexpr const InstructionInfo ABSOLUTE_INDEXED_WITH_X =
					{
						.Name = "and",
						.AddressMode = eAddressMode::ABSOLUTE_INDEXED_WITH_X,
						.Opcode = 0x3D,
						.Mnemonic = eMnemonic::AND,
					};
					static constexpr const InstructionInfo ABSOLUTE_INDEXED_WITH_Y =
					{
						.Name = "and",
						.AddressMode = eAddressMode::ABSOLUTE_INDEXED_WITH_Y,
						.Opcode = 0x39,
						.Mnemonic = eMnemonic::AND,
					};
					static constexpr const InstructionInfo IMMEDIATE =
					{
						.Name = "and",
						.AddressMode = eAddressMode::IMMEDIATE,
						.Opcode = 0x29,
						.Mnemonic = eMnemonic::AND,
					};
					static constexpr const InstructionInfo ZERO_PAGE =
					{
						.Name = "and",
						.AddressMode = eAddressMode::ZERO_PAGE,
						.Opcode = 0x25,
						.Mnemonic = eMnemonic::AND,
					};
					static constexpr const InstructionInfo ZERO_PAGE_INDEXED_INDIRECT =
					{
						.Name = "and",
						.AddressMode = eAddressMode::ZERO_PAGE_INDEXED_INDIRECT,
						.Opcode = 0x21,
						.Mnemonic = eMnemonic::AND,
					};
					static constexpr const InstructionInfo ZERO_PAGE_INDEXED_WITH_X =
					{
						.Name = "and",
						.AddressMode = eAddressMode::ZERO_PAGE_INDEXED_WITH_X,
						.Opcode = 0x35,
						.Mnemonic = eMnemonic::AND,
					};
					static constexpr const InstructionInfo ZERO_PAGE_INDIRECT_INDEXED_WITH_Y =
					{
						.Name = "and",
						.AddressMode = eAddressMode::ZERO_PAGE_INDIRECT_INDEXED_WITH_Y,
						.Opcode = 0x31,
						.Mnemonic = eMnemonic::AND,
					};
				};
				struct Ora
				{
					static constexpr const InstructionInfo ABSOLUTE =
					{
						.Name = "ora",
						.AddressMode = eAddressMode::ABSOLUTE,
						.Opcode = 0x0D,
						.Mnemonic = eMnemonic::ORA,
					};
					static constexpr const InstructionInfo ABSOLUTE_INDEXED_WITH_X =
					{
						.Name = "ora",
						.AddressMode = eAddressMode::ABSOLUTE_INDEXED_WITH_X,
						.Opcode = 0x1D,
						.Mnemonic = eMnemonic::ORA,
					};
					static constexpr const InstructionInfo ABSOLUTE_INDEXED_WITH_Y =
					{
						.Name = "ora",
						.AddressMode = eAddressMode::ABSOLUTE_INDEXED_WITH_Y,
						.Opcode = 0x19,
						.Mnemonic = eMnemonic::ORA,
					};
					static constexpr const InstructionInfo IMMEDIATE =
					{
						.Name = "ora",
						.AddressMode = eAddressMode::IMMEDIATE,
						.Opcode = 0x09,
						.Mnemonic = eMnemonic::ORA,
					};
					static constexpr const InstructionInfo ZERO_PAGE =
					{
						.Name = "ora",
						.AddressMode = eAddressMode::ZERO_PAGE,
						.Opcode = 0x05,
						.Mnemonic = eMnemonic::ORA,
					};
					static constexpr const InstructionInfo ZERO_PAGE_INDEXED_INDIRECT =
					{
						.Name = "ora",
						.AddressMode = eAddressMode::ZERO_PAGE_INDEXED_INDIRECT,
						.Opcode = 0x01,
						.Mnemonic = eMnemonic::ORA,
					};
					static constexpr const InstructionInfo ZERO_PAGE_INDEXED_WITH_X =
					{
						.Name = "ora",
						.AddressMode = eAddressMode::ZERO_PAGE_INDEXED_WITH_X,
						.Opcode = 0x15,
						.Mnemonic = eMnemonic::ORA,
					};
					static constexpr const InstructionInfo ZERO_PAGE_INDIRECT_INDEXED_WITH_Y =
					{
						.Name = "ora",
						.AddressMode = eAddressMode::ZERO_PAGE_INDIRECT_INDEXED_WITH_Y,
						.Opcode = 0x11,
						.Mnemonic = eMnemonic::ORA,
					};
				};
				struct Eor
				{
					static constexpr const InstructionInfo ABSOLUTE =
					{
						.Name = "eor",
						.AddressMode = eAddressMode::ABSOLUTE,
						.Opcode = 0x4D,
						.Mnemonic = eMnemonic::EOR,
					};
					static constexpr const InstructionInfo ABSOLUTE_INDEXED_WITH_X =
					{
						.Name = "eor",
						.AddressMode = eAddressMode::ABSOLUTE_INDEXED_WITH_X,
						.Opcode = 0x5D,
						.Mnemonic = eMnemonic::EOR,
					};
					static constexpr const InstructionInfo ABSOLUTE_INDEXED_WITH_Y =
					{
						.Name = "eor",
						.AddressMode = eAddressMode::ABSOLUTE_INDEXED_WITH_Y,
						.Opcode = 0x59,
						.Mnemonic = eMnemonic::EOR,
					};
					static constexpr const InstructionInfo IMMEDIATE =
					{
						.Name = "eor",
						.AddressMode = eAddressMode::IMMEDIATE,
						.Opcode = 0x49,
						.Mnemonic = eMnemonic::EOR,
					};
					static constexpr const InstructionInfo ZERO_PAGE =
					{
						.Name = "eor",
						.AddressMode = eAddressMode::ZERO_PAGE,
						.Opcode = 0x45,
						.Mnemonic = eMnemonic::EOR,
					};
					static constexpr const InstructionInfo ZERO_PAGE_INDEXED_INDIRECT =
					{
						.Name = "eor",
						.AddressMode = eAddressMode::ZERO_PAGE_INDEXED_INDIRECT,
						.Opcode = 0x41,
						.Mnemonic = eMnemonic::EOR,
					};
					static constexpr const InstructionInfo ZERO_PAGE_INDEXED_WITH_X =
					{
						.Name = "eor",
						.AddressMode = eAddressMode::ZERO_PAGE_INDEXED_WITH_X,
						.Opcode = 0x55,
						.Mnemonic = eMnemonic::EOR,
					};
					static constexpr const InstructionInfo ZERO_PAGE_INDIRECT_INDEXED_WITH_Y =
					{
						.Name = "eor",
						.AddressMode = eAddressMode::ZERO_PAGE_INDIRECT_INDEXED_WITH_Y,
						.Opcode = 0x51,
						.Mnemonic = eMnemonic::EOR,
					};
				};
				struct Cmp
				{
					static constexpr const InstructionInfo ABSOLUTE =
					{
						.Name = "cmp",
						.AddressMode = eAddressMode::ABSOLUTE,
						.Opcode = 0xCD,
						.Mnemonic = eMnemonic::CMP,
					};
					static constexpr const InstructionInfo ABSOLUTE_INDEXED_WITH_X =
					{
						.Name = "cmp",
						.AddressMode = eAddressMode::ABSOLUTE_INDEXED_WITH_X,
						.Opcode = 0xDD,
						.Mnemonic = eMnemonic::CMP,
					};
					static constexpr const InstructionInfo ABSOLUTE_INDEXED_WITH_Y =
					{
						.Name = "cmp",
						.AddressMode = eAddressMode::ABSOLUTE_INDEXED_WITH_Y,
						.Opcode = 0xD9,
						.Mnemonic = eMnemonic::CMP,
					};
					static constexpr const InstructionInfo IMMEDIATE =
					{
						.Name = "cmp",
						.AddressMode = eAddressMode::IMMEDIATE,
						.Opcode = 0xC9,
						.Mnemonic = eMnemonic::CMP,
					};
					static constexpr const InstructionInfo ZERO_PAGE =
					{
						.Name = "cmp",
						.AddressMode = eAddressMode::ZERO_PAGE,
						.Opcode = 0xC5,
						.Mnemonic = eMnemonic::CMP,
					};
					static constexpr const InstructionInfo ZERO_PAGE_INDEXED_INDIRECT =
					{
						.Name = "cmp",
						.AddressMode = eAddressMode::ZERO_PAGE_INDEXED_INDIRECT,
						.Opcode = 0xC1,
						.Mnemonic = eMnemonic::CMP,
					};
					static constexpr const InstructionInfo ZERO_PAGE_INDEXED_WITH_X =
					{
						.Name = "cmp",
						.AddressMode = eAddressMode::ZERO_PAGE_INDEXED_WITH_X,
						.Opcode = 0xD5,
						.Mnemonic = eMnemonic::CMP,
					};
					static constexpr const InstructionInfo ZERO_PAGE_INDIRECT_INDEXED_WITH_Y =
					{
						.Name = "cmp",
						.AddressMode = eAddressMode::ZERO_PAGE_INDIRECT_INDEXED_WITH_Y,
						.Opcode = 0xD1,
						.Mnemonic = eMnemonic::CMP,
					};
				};
				struct Cpx
				{
					static constexpr const InstructionInfo ABSOLUTE =
					{
						.Name = "cpx",
						.AddressMode = eAddressMode::ABSOLUTE,
						.Opcode = 0xEC,
						.Mnemonic = eMnemonic::CPX,
					};
					static constexpr const InstructionInfo IMMEDIATE =
					{
						.Name = "cpx",
						.AddressMode = eAddressMode::IMMEDIATE,
						.Opcode = 0xE0,
						.Mnemonic = eMnemonic::CPX,
					};
					static constexpr const InstructionInfo ZERO_PAGE =
					{
						.Name = "cpx",
						.AddressMode = eAddressMode::ZERO_PAGE,
						.Opcode = 0xE4,
						.Mnemonic = eMnemonic::CPX,
					};
				};
				struct Cpy
				{
					static constexpr const InstructionInfo ABSOLUTE =
					{
						.Name = "cpy",
						.AddressMode = eAddressMode::ABSOLUTE,
						.Opcode = 0xCC,
						.Mnemonic = eMnemonic::CPY,
					};
					static constexpr const InstructionInfo IMMEDIATE =
					{
						.Name = "cpy",
						.AddressMode = eAddressMode::IMMEDIATE,
						.Opcode = 0xC0,
						.Mnemonic = eMnemonic::CPY,
					};
					static constexpr const InstructionInfo ZERO_PAGE =
					{
						.Name = "cpy",
						.AddressMode = eAddressMode::ZERO_PAGE,
						.Opcode = 0xC4,
						.Mnemonic = eMnemonic::CPY,
					};
				};
				struct Bit
				{
					static constexpr const InstructionInfo ABSOLUTE =
					{
						.Name = "bit",
						.AddressMode = eAddressMode::ABSOLUTE,
						.Opcode = 0x2C,
						.Mnemonic = eMnemonic::BIT,
					};
					static constexpr const InstructionInfo IMMEDIATE =
					{
						.Name = "bit",
						.AddressMode = eAddressMode::IMMEDIATE,
						.Opcode = 0x89,
						.Mnemonic = eMnemonic::BIT,
					};
					static constexpr const InstructionInfo ZERO_PAGE =
					{
						.Name = "bit",
						.AddressMode = eAddressMode::ZERO_PAGE,
						.Opcode = 0x24,
						.Mnemonic = eMnemonic::BIT,
					};
				};
				struct Bcc
				{
					static constexpr const InstructionInfo RELATIVE =
					{
						.Name = "bcc",
						.AddressMode = eAddressMode::RELATIVE,
						.Opcode = 0x90,
						.Mnemonic = eMnemonic::BCC,
					};
				};
				struct Bcs
				{
					static constexpr const InstructionInfo RELATIVE =
					{
						.Name = "bcs",
						.AddressMode = eAddressMode::RELATIVE,
						.Opcode = 0xB0,
						.Mnemonic = eMnemonic::BCS,
					};
				};
				struct Bne
				{
					static constexpr const InstructionInfo RELATIVE = {
						.Name = "bne",
						.AddressMode = eAddressMode::RELATIVE,
						.Opcode = 0xD0,
						.Mnemonic = eMnemonic::BNE,
					};
				};
				struct Beq
				{
					static constexpr const InstructionInfo RELATIVE =
					{
						.Name = "beq",
						.AddressMode = eAddressMode::RELATIVE,
						.Opcode = 0xF0,
						.Mnemonic = eMnemonic::BEQ,
					};
				};
				struct Bpl
				{
					static constexpr const InstructionInfo RELATIVE =
					{
						.Name = "bpl",
						.AddressMode = eAddressMode::RELATIVE,
						.Opcode = 0x10,
						.Mnemonic = eMnemonic::BPL,
					};
				};
				struct Bmi
				{
					static constexpr const InstructionInfo RELATIVE =
					{
						.Name = "bmi",
						.AddressMode = eAddressMode::RELATIVE,
						.Opcode = 0x30,
						.Mnemonic = eMnemonic::BMI,
					};
				};
				struct Bvc
				{
					static constexpr const InstructionInfo RELATIVE =
					{
						.Name = "bvc",
						.AddressMode = eAddressMode::RELATIVE,
						.Opcode = 0x50,
						.Mnemonic = eMnemonic::BVC,
					};
				};
				struct Bvs
				{
					static constexpr const InstructionInfo RELATIVE =
					{
						.Name = "bvs",
						.AddressMode = eAddressMode::RELATIVE,
						.Opcode = 0x70,
						.Mnemonic = eMnemonic::BVS,
					};
				};
				struct Tax
				{
					static constexpr const InstructionInfo IMPLIED =
					{
						.Name = "tax",
						.AddressMode = eAddressMode::IMPLIED,
						.Opcode = 0xAA,
						.Mnemonic = eMnemonic::TAX,
					};
				};
				struct Txa
				{
					static constexpr const InstructionInfo IMPLIED =
					{
						.Name = "txa",
						.AddressMode = eAddressMode::IMPLIED,
						.Opcode = 0x8A,
						.Mnemonic = eMnemonic::TXA,
					};
				};
				struct Tay
				{
					static constexpr const InstructionInfo IMPLIED =
					{
						.Name = "tay",
						.AddressMode = eAddressMode::IMPLIED,
						.Opcode = 0xA8,
						.Mnemonic = eMnemonic::TAY,
					};
				};
				struct Tya
				{
					static constexpr const InstructionInfo IMPLIED =
					{
						.Name = "tya",
						.AddressMode = eAddressMode::IMPLIED,
						.Opcode = 0x98,
						.Mnemonic = eMnemonic::TYA,
					};
				};
				struct Tsx
				{
					static constexpr const InstructionInfo IMPLIED =
					{
						.Name = "tsx",
						.AddressMode = eAddressMode::IMPLIED,
						.Opcode = 0xBA,
						.Mnemonic = eMnemonic::TSX,
					};
				};
				struct Txs
				{
					static constexpr const InstructionInfo IMPLIED =
					{
						.Name = "txs",
						.AddressMode = eAddressMode::IMPLIED,
						.Opcode = 0x9A,
						.Mnemonic = eMnemonic::TXS,
					};
				};
				struct Pha
				{
					static constexpr const InstructionInfo IMPLIED =
					{
						.Name = "pha",
						.AddressMode = eAddressMode::IMPLIED,
						.Opcode = 0x48,
						.Mnemonic = eMnemonic::PHA,
					};
				};
				struct Pla
				{
					static constexpr const InstructionInfo IMPLIED =
					{
						.Name = "pla",
						.AddressMode = eAddressMode::IMPLIED,
						.Opcode = 0x68,
						.Mnemonic = eMnemonic::PLA,
					};
				};
				struct Php
				{
					static constexpr const InstructionInfo IMPLIED =
					{
						.Name = "php",
						.AddressMode = eAddressMode::IMPLIED,
						.Opcode = 0x08,
						.Mnemonic = eMnemonic::PHP,
					};
				};
				struct Plp
				{
					static constexpr const InstructionInfo IMPLIED =
					{
						.Name = "plp",
						.AddressMode = eAddressMode::IMPLIED,
						.Opcode = 0x28,
						.Mnemonic = eMnemonic::PLP,
					};
				};
				struct Jmp
				{
					static constexpr const InstructionInfo ABSOLUTE =
					{
						.Name = "jmp",
						.AddressMode = eAddressMode::ABSOLUTE,
						.Opcode = 0x4C,
						.Mnemonic = eMnemonic::JMP,
					};
					static constexpr const InstructionInfo ABSOLUTE_INDIRECT =
					{
						.Name = "jmp",
						.AddressMode = eAddressMode::ABSOLUTE_INDIRECT,
						.Opcode = 0x6C,
						.Mnemonic = eMnemonic::JMP,
					};
				};
				struct Jsr
				{
					static constexpr const InstructionInfo ABSOLUTE =
					{
						.Name = "jsr",
						.AddressMode = eAddressMode::ABSOLUTE,
						.Opcode = 0x20,
						.Mnemonic = eMnemonic::JSR,
					};
				};
				struct Rts
				{
					static constexpr const InstructionInfo IMPLIED =
					{
						.Name = "rts",
						.AddressMode = eAddressMode::IMPLIED,
						.Opcode = 0x60,
						.Mnemonic = eMnemonic::RTS,
					};
				};
				struct Rti
				{
					static constexpr const InstructionInfo IMPLIED =
					{
						.Name = "rti",
						.AddressMode = eAddressMode::IMPLIED,
						.Opcode = 0x40,
						.Mnemonic = eMnemonic::RTI,
					};
				};
				struct Clc
				{
					static constexpr const InstructionInfo IMPLIED =
					{
						.Name = "clc",
						.AddressMode = eAddressMode::IMPLIED,
						.Opcode = 0x18,
						.Mnemonic = eMnemonic::CLC,
					};
				};
				struct Sec
				{
					static constexpr const InstructionInfo IMPLIED =
					{
						.Name = "sec",
						.AddressMode = eAddressMode::IMPLIED,
						.Opcode = 0x38,
						.Mnemonic = eMnemonic::SEC,
					};
				};
				struct Cld
				{
					static constexpr const InstructionInfo IMPLIED =
					{
						.Name = "cld",
						.AddressMode = eAddressMode::IMPLIED,
						.Opcode = 0xD8,
						.Mnemonic = eMnemonic::CLD,
					};
				};
				struct Sed
				{
					static constexpr const InstructionInfo IMPLIED =
					{
						.Name = "sed",
						.AddressMode = eAddressMode::IMPLIED,
						.Opcode = 0xF8,
						.Mnemonic = eMnemonic::SED,
					};
				};
				struct Cli
				{
					static constexpr const InstructionInfo IMPLIED =
					{
						.Name = "cli",
						.AddressMode = eAddressMode::IMPLIED,
						.Opcode = 0x58,
						.Mnemonic = eMnemonic::CLI,
					};
				};
				struct Sei
				{
					static constexpr const InstructionInfo IMPLIED =
					{
						.Name = "sei",
						.AddressMode = eAddressMode::IMPLIED,
						.Opcode = 0x78,
						.Mnemonic = eMnemonic::SEI,
					};
				};
				struct Clv
				{
					static constexpr const InstructionInfo IMPLIED =
					{
						.Name = "clv",
						.AddressMode = eAddressMode::IMPLIED,
						.Opcode = 0xB8,
						.Mnemonic = eMnemonic::CLV,
					};
				};
				struct Brk
				{
					static constexpr const InstructionInfo IMPLIED =
					{
						.Name = "brk",
						.AddressMode = eAddressMode::IMPLIED,
						.Opcode = 0x00,
						.Mnemonic = eMnemonic::BRK,
					};
				};
				struct Nop
				{
					static constexpr const InstructionInfo IMPLIED =
					{
						.Name = "nop",
						.AddressMode = eAddressMode::IMPLIED,
						.Opcode = 0xEA,
						.Mnemonic = eMnemonic::NOP,
					};
				};
			};

			struct Registers final
			{
				address_t	ProgramCounter;
				data_t		StackPointer;
				data_t		Accumulator;
				data_t		IndexX;
				data_t		IndexY;
				union Status
				{
					data_t	Value;
					struct StatusBits
					{
					public:
						bool CarryFlag				: 1;
						bool ZeroFlag				: 1;
						bool InterruptDisableFlag	: 1;
						bool DecimalModeFlag		: 1;
						bool BreakCommandFlag		: 1;
					private:
						bool Padding				: 1;
					public:
						bool OverflowFlag			: 1;
						bool NegativeFlag			: 1;
					} StatusBits;
				} Status;
			};

			enum class eExternalMode : uint8_t
			{
				//FETCH = 0,
				FETCH_OPCODE = 0,
				FETCH_DATA_FROM_ROM,
				FETCH_DATA_FROM_RAM,
				FETCH_LOW_ADDRESS_FROM_ROM,
				FETCH_LOW_ADDRESS_FROM_RAM,
				FETCH_HIGH_ADDRESS_FROM_ROM,
				FETCH_HIGH_ADDRESS_FROM_RAM,
				JRS_NONE,
				SAVE_PROGRAM_COUNTER_HIGH_TO_RAM,
				SAVE_PROGRAM_COUNTER_LOW_TO_RAM,
				RTS_NONE,
				NEXT,
			};

			enum class eInternalMode : uint8_t
			{
				DECODE = 0,
				EXECUTE,
				SET_ADDRESS_BUS_ABSOLUTE_MODE,
				SET_PROGRAM_COUNTER,
				CHECK_STATUS,
				PAGE_CROSSING,
				DECREASE_STACK_POINTER,
				INCREASE_STACK_POINTER,
				PREVIOUS,
				NONE,
			};

			enum class eAddressBusType : uint8_t
			{
				PROGRAM_COUNTER,
				ADDRESS,
				STACK_POINTER,
			};

			struct CycleJob
			{
				const eAddressBusType	AddressBusType;
				bool					IncrementProgramCounter = true;
				eExternalMode			ExternalOperation = eExternalMode::FETCH_OPCODE;
				eInternalMode			InternalOperation = eInternalMode::NONE;
			};

		protected:
			static constexpr const InstructionInfo* const INSTRUCTION_TABLE[] =
			{
				&Instruction::Brk::IMPLIED, // 0x00
				&Instruction::Ora::ZERO_PAGE_INDEXED_INDIRECT, // 0x01
				nullptr, // 0x02
				nullptr, // 0x03
				nullptr, // 0x04
				&Instruction::Ora::ZERO_PAGE, // 0x05
				&Instruction::Asl::ZERO_PAGE, // 0x06
				nullptr, // 0x07
				&Instruction::Php::IMPLIED, // 0x08
				&Instruction::Ora::IMMEDIATE, // 0x09
				&Instruction::Asl::ACCUMULATOR, // 0x0A
				nullptr, // 0x0B
				nullptr, // 0x0C
				&Instruction::Ora::ABSOLUTE, // 0x0D
				&Instruction::Asl::ABSOLUTE, // 0x0E
				nullptr, // 0x0F
				&Instruction::Bpl::RELATIVE, // 0x10
				&Instruction::Ora::ZERO_PAGE_INDIRECT_INDEXED_WITH_Y, // 0x11
				nullptr, // 0x12
				nullptr, // 0x13
				nullptr, // 0x14
				&Instruction::Ora::ZERO_PAGE_INDEXED_WITH_X, // 0x15
				&Instruction::Asl::ZERO_PAGE_INDEXED_WITH_X, // 0x16
				nullptr, // 0x17
				&Instruction::Clc::IMPLIED, // 0x18
				&Instruction::Ora::ABSOLUTE_INDEXED_WITH_Y, // 0x19
				nullptr, // 0x1A
				nullptr, // 0x1B
				nullptr, // 0x1C
				&Instruction::Ora::ABSOLUTE_INDEXED_WITH_X, // 0x1D
				&Instruction::Asl::ABSOLUTE_INDEXED_WITH_X, // 0x1E
				nullptr, // 0x1F
				&Instruction::Jsr::ABSOLUTE, // 0x20
				&Instruction::And::ZERO_PAGE_INDEXED_INDIRECT, // 0x21
				nullptr, // 0x22
				nullptr, // 0x23
				&Instruction::Bit::ZERO_PAGE, // 0x24
				&Instruction::And::ZERO_PAGE, // 0x25
				&Instruction::Rol::ZERO_PAGE, // 0x26
				nullptr, // 0x27
				&Instruction::Plp::IMPLIED, // 0x28
				&Instruction::And::IMMEDIATE, // 0x29
				&Instruction::Rol::ACCUMULATOR, // 0x2A
				nullptr, // 0x2B
				&Instruction::Bit::ABSOLUTE, // 0x2C
				&Instruction::And::ABSOLUTE, // 0x2D
				&Instruction::Rol::ABSOLUTE, // 0x2E
				nullptr, // 0x2F
				&Instruction::Bmi::RELATIVE, // 0x30
				&Instruction::And::ZERO_PAGE_INDIRECT_INDEXED_WITH_Y, // 0x31
				nullptr, // 0x32
				nullptr, // 0x33
				nullptr, // 0x34
				&Instruction::And::ZERO_PAGE_INDEXED_WITH_X, // 0x35
				&Instruction::Rol::ZERO_PAGE_INDEXED_WITH_X, // 0x36
				nullptr, // 0x37
				&Instruction::Sec::IMPLIED, // 0x38
				&Instruction::And::ABSOLUTE_INDEXED_WITH_Y, // 0x39
				nullptr, // 0x3A
				nullptr, // 0x3B
				nullptr, // 0x3C
				&Instruction::And::ABSOLUTE_INDEXED_WITH_X, // 0x3D
				&Instruction::Rol::ABSOLUTE_INDEXED_WITH_X, // 0x3E
				nullptr, // 0x3F
				&Instruction::Rti::IMPLIED, // 0x40
				&Instruction::Eor::ZERO_PAGE_INDEXED_INDIRECT, // 0x41
				nullptr, // 0x42
				nullptr, // 0x43
				nullptr, // 0x44
				&Instruction::Eor::ZERO_PAGE, // 0x45
				&Instruction::Lsr::ZERO_PAGE, // 0x46
				nullptr, // 0x47
				&Instruction::Pha::IMPLIED, // 0x48
				&Instruction::Eor::IMMEDIATE, // 0x49
				&Instruction::Lsr::ACCUMULATOR, // 0x4A
				nullptr, // 0x4B
				&Instruction::Jmp::ABSOLUTE, // 0x4C
				&Instruction::Eor::ABSOLUTE, // 0x4D
				&Instruction::Lsr::ABSOLUTE, // 0x4E
				nullptr, // 0x4F
				&Instruction::Bvc::RELATIVE, // 0x50
				&Instruction::Eor::ZERO_PAGE_INDIRECT_INDEXED_WITH_Y, // 0x51
				nullptr, // 0x52
				nullptr, // 0x53
				nullptr, // 0x54
				&Instruction::Eor::ZERO_PAGE_INDEXED_WITH_X, // 0x55
				&Instruction::Lsr::ZERO_PAGE_INDEXED_WITH_X, // 0x56
				nullptr, // 0x57
				&Instruction::Cli::IMPLIED, // 0x58
				&Instruction::Eor::ABSOLUTE_INDEXED_WITH_Y, // 0x59
				nullptr, // 0x5A
				nullptr, // 0x5B
				nullptr, // 0x5C
				&Instruction::Eor::ABSOLUTE_INDEXED_WITH_X, // 0x5D
				&Instruction::Lsr::ABSOLUTE_INDEXED_WITH_X, // 0x5E
				nullptr, // 0x5F
				&Instruction::Rts::IMPLIED, // 0x60
				&Instruction::Adc::ZERO_PAGE_INDEXED_INDIRECT, // 0x61
				nullptr, // 0x62
				nullptr, // 0x63
				nullptr, // 0x64
				&Instruction::Adc::ZERO_PAGE, // 0x65
				&Instruction::Ror::ZERO_PAGE, // 0x66
				nullptr, // 0x67
				&Instruction::Pla::IMPLIED, // 0x68
				&Instruction::Adc::IMMEDIATE, // 0x69
				&Instruction::Ror::ACCUMULATOR, // 0x6A
				nullptr, // 0x6B
				&Instruction::Jmp::ABSOLUTE_INDIRECT, // 0x6C
				&Instruction::Adc::ABSOLUTE, // 0x6D
				&Instruction::Ror::ABSOLUTE, // 0x6E
				nullptr, // 0x6F
				&Instruction::Bvs::RELATIVE, // 0x70
				&Instruction::Adc::ZERO_PAGE_INDIRECT_INDEXED_WITH_Y, // 0x71
				nullptr, // 0x72
				nullptr, // 0x73
				nullptr, // 0x74
				&Instruction::Adc::ZERO_PAGE_INDEXED_WITH_X, // 0x75
				&Instruction::Ror::ZERO_PAGE_INDEXED_WITH_X, // 0x76
				nullptr, // 0x77
				&Instruction::Sei::IMPLIED, // 0x78
				&Instruction::Adc::ABSOLUTE_INDEXED_WITH_Y, // 0x79
				nullptr, // 0x7A
				nullptr, // 0x7B
				nullptr, // 0x7C
				&Instruction::Adc::ABSOLUTE_INDEXED_WITH_X, // 0x7D
				&Instruction::Ror::ABSOLUTE_INDEXED_WITH_X, // 0x7E
				nullptr, // 0x7F
				nullptr, // 0x80
				&Instruction::Sta::ZERO_PAGE_INDEXED_INDIRECT, // 0x81
				nullptr, // 0x82
				nullptr, // 0x83
				&Instruction::Sty::ZERO_PAGE, // 0x84
				&Instruction::Sta::ZERO_PAGE, // 0x85
				&Instruction::Stx::ZERO_PAGE, // 0x86
				nullptr, // 0x87
				&Instruction::Dey::IMPLIED, // 0x88
				nullptr, // 0x89
				&Instruction::Txa::IMPLIED, // 0x8A
				nullptr, // 0x8B
				&Instruction::Sty::ABSOLUTE, // 0x8C
				&Instruction::Sta::ABSOLUTE, // 0x8D
				&Instruction::Stx::ABSOLUTE, // 0x8E
				nullptr, // 0x8F
				&Instruction::Bcc::RELATIVE, // 0x90
				&Instruction::Sta::ZERO_PAGE_INDIRECT_INDEXED_WITH_Y, // 0x91
				nullptr, // 0x92
				nullptr, // 0x93
				&Instruction::Sty::ZERO_PAGE_INDEXED_WITH_X, // 0x94
				&Instruction::Sta::ZERO_PAGE_INDEXED_WITH_X, // 0x95
				&Instruction::Stx::ZERO_PAGE_INDEXED_WITH_Y, // 0x96
				nullptr, // 0x97
				&Instruction::Tya::IMPLIED, // 0x98
				&Instruction::Sta::ABSOLUTE_INDEXED_WITH_Y, // 0x99
				&Instruction::Txs::IMPLIED, // 0x9A
				nullptr, // 0x9B
				nullptr, // 0x9C
				&Instruction::Sta::ABSOLUTE_INDEXED_WITH_X, // 0x9D
				nullptr, // 0x9E
				nullptr, // 0x9F
				&Instruction::Ldy::IMMEDIATE, // 0xA0
				&Instruction::Lda::ZERO_PAGE_INDEXED_INDIRECT, // 0xA1
				&Instruction::Ldx::IMMEDIATE, // 0xA2
				nullptr, // 0xA3
				&Instruction::Ldy::ZERO_PAGE, // 0xA4
				&Instruction::Lda::ZERO_PAGE, // 0xA5
				&Instruction::Ldx::ZERO_PAGE, // 0xA6
				nullptr, // 0xA7
				&Instruction::Tay::IMPLIED, // 0xA8
				&Instruction::Lda::IMMEDIATE, // 0xA9
				&Instruction::Tax::IMPLIED, // 0xAA
				nullptr, // 0xAB
				&Instruction::Ldy::ABSOLUTE, // 0xAC
				&Instruction::Lda::ABSOLUTE, // 0xAD
				&Instruction::Ldx::ABSOLUTE, // 0xAE
				nullptr, // 0xAF
				&Instruction::Bcs::RELATIVE, // 0xB0
				&Instruction::Lda::ZERO_PAGE_INDIRECT_INDEXED_WITH_Y, // 0xB1
				nullptr, // 0xB2
				nullptr, // 0xB3
				&Instruction::Ldy::ZERO_PAGE_INDEXED_WITH_X, // 0xB4
				&Instruction::Lda::ZERO_PAGE_INDEXED_WITH_X, // 0xB5
				&Instruction::Ldx::ZERO_PAGE_INDEXED_WITH_Y, // 0xB6
				nullptr, // 0xB7
				&Instruction::Clv::IMPLIED, // 0xB8
				&Instruction::Lda::ABSOLUTE_INDEXED_WITH_Y, // 0xB9
				&Instruction::Tsx::IMPLIED, // 0xBA
				nullptr, // 0xBB
				&Instruction::Ldy::ABSOLUTE_INDEXED_WITH_X, // 0xBC
				&Instruction::Lda::ABSOLUTE_INDEXED_WITH_X, // 0xBD
				&Instruction::Ldx::ABSOLUTE_INDEXED_WITH_Y, // 0xBE
				nullptr, // 0xBF
				&Instruction::Cpy::IMMEDIATE, // 0xC0
				&Instruction::Cmp::ZERO_PAGE_INDEXED_INDIRECT, // 0xC1
				nullptr, // 0xC2
				nullptr, // 0xC3
				&Instruction::Cpy::ZERO_PAGE, // 0xC4
				&Instruction::Cmp::ZERO_PAGE, // 0xC5
				&Instruction::Dec::ZERO_PAGE, // 0xC6
				nullptr, // 0xC7
				&Instruction::Iny::IMPLIED, // 0xC8
				&Instruction::Cmp::IMMEDIATE, // 0xC9
				&Instruction::Dex::IMPLIED, // 0xCA
				nullptr, // 0xCB
				&Instruction::Cpy::ABSOLUTE, // 0xCC
				&Instruction::Cmp::ABSOLUTE, // 0xCD
				&Instruction::Dec::ABSOLUTE, // 0xCE
				nullptr, // 0xCF
				&Instruction::Bne::RELATIVE, // 0xD0
				&Instruction::Cmp::ZERO_PAGE_INDIRECT_INDEXED_WITH_Y, // 0xD1
				nullptr, // 0xD2
				nullptr, // 0xD3
				nullptr, // 0xD4
				&Instruction::Cmp::ZERO_PAGE_INDEXED_WITH_X, // 0xD5
				&Instruction::Dec::ZERO_PAGE_INDEXED_WITH_X, // 0xD6
				nullptr, // 0xD7
				&Instruction::Cld::IMPLIED, // 0xD8
				&Instruction::Cmp::ABSOLUTE_INDEXED_WITH_Y, // 0xD9
				nullptr, // 0xDA
				nullptr, // 0xDB
				nullptr, // 0xDC
				&Instruction::Cmp::ABSOLUTE_INDEXED_WITH_X, // 0xDD
				&Instruction::Dec::ABSOLUTE_INDEXED_WITH_X, // 0xDE
				nullptr, // 0xDF
				&Instruction::Cpx::IMMEDIATE, // 0xE0
				&Instruction::Sbc::ZERO_PAGE_INDEXED_INDIRECT, // 0xE1
				nullptr, // 0xE2
				nullptr, // 0xE3
				&Instruction::Cpx::ZERO_PAGE, // 0xE4
				&Instruction::Sbc::ZERO_PAGE, // 0xE5
				&Instruction::Inc::ZERO_PAGE, // 0xE6
				nullptr, // 0xE7
				&Instruction::Inx::IMPLIED, // 0xE8
				&Instruction::Sbc::IMMEDIATE, // 0xE9
				&Instruction::Nop::IMPLIED, // 0xEA
				nullptr, // 0xEB
				&Instruction::Cpx::ABSOLUTE, // 0xEC
				&Instruction::Sbc::ABSOLUTE, // 0xED
				&Instruction::Inc::ABSOLUTE, // 0xEE
				nullptr, // 0xEF
				&Instruction::Beq::RELATIVE, // 0xF0
				&Instruction::Sbc::ZERO_PAGE_INDIRECT_INDEXED_WITH_Y, // 0xF1
				nullptr, // 0xF2
				nullptr, // 0xF3
				nullptr, // 0xF4
				&Instruction::Sbc::ZERO_PAGE_INDEXED_WITH_X, // 0xF5
				&Instruction::Inc::ZERO_PAGE_INDEXED_WITH_X, // 0xF6
				nullptr, // 0xF7
				&Instruction::Sed::IMPLIED, // 0xF8
				&Instruction::Sbc::ABSOLUTE_INDEXED_WITH_Y, // 0xF9
				nullptr, // 0xFA
				nullptr, // 0xFB
				nullptr, // 0xFC
				&Instruction::Sbc::ABSOLUTE_INDEXED_WITH_X, // 0xFD
				&Instruction::Inc::ABSOLUTE_INDEXED_WITH_X, // 0xFE
				nullptr, // 0xFF
			};

			static constexpr const data_t STACK_PAGE_ADDRESS_HI = 0x01;

			static constexpr const size_t		BUFFER_SIZE = 64;
			static constexpr const char* const	EMPTY_BYTE = "..";
			static constexpr const char* const	HEX_CHAR_TABLE = "0123456789ABCDEF";

		protected:
			static constexpr const char*	convertAddressModeToString( const eAddressMode addressMode ) noexcept;
			static constexpr size_t			getRequiredOperandNumBytes( const eAddressMode addressMode ) noexcept;

		protected:
			void			decode( bool& inoutSkipFetch ) noexcept;
			constexpr const data_t* disassemble( char* out_buffer64, const data_t* mem ) noexcept;
			constexpr bool			execute() noexcept;
			void					processSingleClock() noexcept;

		protected:
			const Cartridge*	mRomOrNull;
			Registers			mRegisters;

			eExternalMode			mCurrentInternalMode;
			eInternalMode		mCurrentExternalMode;
			bool				mReadRam;
			address_t			mAddressBus;
			data_t				mDataBus;
			data_t				mDataToDecode;
			size_t				mDecodeCounter;
			ExecutionInfo		mExecutionInfo;

			size_t				mJumpSubroutineClock;
			std::queue<CycleJob>	mCycleJobs;
		};

		class CpuNes : public Cpu6502
		{
		public:
			CpuNes() = delete;
			inline CpuNes( NesRam& ram, const Cartridge* cartridgeOrNull ) noexcept : Cpu6502( ram, cartridgeOrNull ) {}
			CpuNes( const CpuNes& ) = delete;
			explicit CpuNes( CpuNes&& ) noexcept = default;
			virtual ~CpuNes() = default;

			CpuNes& operator=( const CpuNes& ) = delete;
			CpuNes& operator=( CpuNes&& ) noexcept = default;

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
			// [REF]: https://www.nesdev.org/wiki/2A03
			struct RegisterMap final
			{
				struct Pulse1
				{
					static constexpr const address_t	Sequence1Volume = 0x4000;	// Duty cycle and volume
					static constexpr const address_t	Sequence1Sweep	= 0x4001;	// Sweep control register
					static constexpr const address_t	Sequence1Low	= 0x4002;	// Low byte of period
					static constexpr const address_t	Sequence1High	= 0x4003;	// High byte of period and length counter value
				};

				struct Pulse2
				{
					static constexpr const address_t	Sequence2Volume = 0x4004;
					static constexpr const address_t	Sequence2Sweep	= 0x4005;
					static constexpr const address_t	Sequence2Low	= 0x4006;
					static constexpr const address_t	Sequence2High	= 0x4007;
				};

				struct Triangle
				{
					static constexpr const address_t	LinearCounter	= 0x4008;
					static constexpr const address_t	Low				= 0x400A;
					static constexpr const address_t	High			= 0x400B;
				};

				struct Noise
				{
					static constexpr const address_t	Volume			= 0x400C;
					static constexpr const address_t	Low				= 0x400E;	// Period and waveform shape
					static constexpr const address_t	High			= 0x400F;	// Length counter value
				};

				struct DeltaModulationChannel
				{
					static constexpr const address_t	Frequency		= 0x4010;	// IRQ flag, loop flag and frequency
					static constexpr const address_t	Raw				= 0x4011;	// 7-bit DAC
					static constexpr const address_t	StartAddress	= 0x4012;	// Start address = $C000 + $40*$xx
					static constexpr const address_t	SampleLength	= 0x4013;	// Sample length = $10*$xx + 1 bytes (128*$xx + 8 samples)
				};

				static constexpr const address_t		ObjectAttributeMemoryDma = 0x4014;	// OAM DMA: Copy 256 bytes from $xx00-$xxFF into OAM via OAMDATA ($2004)
				static constexpr const address_t		SoundChannel	= 0x4015;	// Sound channels enable

				static constexpr const address_t		Joystick1		= 0x4016;	// Joystick strobe
				static constexpr const address_t		Joystick2		= 0x4017;	// Frame counter control
			};
		};
	}
}