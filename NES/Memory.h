#pragma once

#include "Common.h"

#include "NES/ArrayView.h"
#include "NES/DynamicArray.hpp"
#include "NES/StaticArray.hpp"

namespace ninmuse
{
	template <Data TData, Array<TData> TArray>
	class IConsecutiveMemory
	{
	public:
		IConsecutiveMemory() = default;
		IConsecutiveMemory( const IConsecutiveMemory& ) = default;
		IConsecutiveMemory( IConsecutiveMemory&& ) = default;
		virtual ~IConsecutiveMemory() = default;

		IConsecutiveMemory& operator=( const IConsecutiveMemory& ) = default;
		IConsecutiveMemory& operator=( IConsecutiveMemory&& ) = default;

	public:
		virtual constexpr TArray&		GetData() noexcept = 0;
		virtual constexpr const TArray& GetData() const noexcept = 0;
	};

	template <Data TData, Array<TData> TArray>
	class ConsecutiveMemory : IConsecutiveMemory<TData, TArray>
	{
	public:
		ConsecutiveMemory() = default;
		ConsecutiveMemory( const ConsecutiveMemory& ) = delete;
		explicit ConsecutiveMemory( ConsecutiveMemory&& ) noexcept = default;
		virtual ~ConsecutiveMemory() = default;

		ConsecutiveMemory& operator=( const ConsecutiveMemory& ) = delete;
		ConsecutiveMemory& operator=( ConsecutiveMemory&& ) noexcept = default;

	public:
		inline constexpr TArray&		GetData() noexcept override { return mData; };
		inline constexpr const TArray&	GetData() const noexcept override { return mData; };

	protected:
		TArray	mData;
	};

	class ConsecutiveMemory8BitView : public IConsecutiveMemory<nes::data_t, ArrayView<nes::data_t>>
	{
	public:
		ConsecutiveMemory8BitView() = delete;
		template <Array<nes::data_t> TArray>
		ConsecutiveMemory8BitView( ConsecutiveMemory<nes::data_t, TArray>& memory, const nes::address_t startAddress, const size_t size ) noexcept;
		ConsecutiveMemory8BitView( const ConsecutiveMemory8BitView& ) = delete;
		explicit ConsecutiveMemory8BitView( ConsecutiveMemory8BitView&& ) noexcept = default;
		~ConsecutiveMemory8BitView() = default;

		ConsecutiveMemory8BitView& operator=( const ConsecutiveMemory8BitView& ) = delete;
		ConsecutiveMemory8BitView& operator=( ConsecutiveMemory8BitView&& ) noexcept = default;

	public:
		inline constexpr ArrayView<nes::data_t>&		GetData() noexcept override { return mData; }
		inline constexpr const ArrayView<nes::data_t>&	GetData() const noexcept override { return mData; }

	protected:
		ArrayView<nes::data_t>	mData;
	};

	template <Data TData>
	class IRam
	{
	public:
		IRam() = delete;
		IRam( const size_t capacity ) noexcept;
		IRam( const IRam& ) = delete;
		explicit IRam( IRam&& ) noexcept = default;
		virtual ~IRam() = default;

		IRam& operator=( const IRam& ) = delete;
		IRam& operator=( IRam&& ) noexcept = default;

	public:
		inline constexpr ConsecutiveMemory<TData, DynamicArray<TData>>& GetMemory() noexcept { return mMemory; }
		inline constexpr const ConsecutiveMemory<TData, DynamicArray<TData>>& GetMemory() const noexcept { return mMemory; }

	protected:
		ConsecutiveMemory<TData, DynamicArray<TData>>	mMemory;
	};

	namespace nes
	{
		// [REF]: https://www.nesdev.org/wiki/CPU_memory_map
		class NesRam final : public IRam<data_t>
		{
		public:
			NesRam() noexcept;

		private:
			// MEMORY MAP ADDRESS RANGE AND SIZE
			static constexpr const address_t	RAM_ADDRESS						= 0x0000;
			static constexpr const size_t		RAM_SIZE						= 0x0800;
			static constexpr const address_t	RAM_MIRRORS_ADDRESS				= RAM_ADDRESS + RAM_SIZE;
			static constexpr const size_t		RAM_MIRROR_SIZE					= 0x0800;
			static constexpr const size_t		NUM_RAM_MIRRORS					= 3;
			static constexpr const address_t	RAM_MIRROR_ADDRESSES[NUM_RAM_MIRRORS]
																				= { RAM_MIRRORS_ADDRESS,										// 0x0800
																					RAM_MIRRORS_ADDRESS + RAM_MIRROR_SIZE,						// 0x1000
																					RAM_MIRRORS_ADDRESS + RAM_MIRROR_SIZE + RAM_MIRROR_SIZE };	// 0x1800
			static constexpr const address_t	PPU_REGISTERS_ADDRESS			= RAM_MIRROR_ADDRESSES[NUM_RAM_MIRRORS - 1] + RAM_MIRROR_SIZE;	// 0x2000
			static constexpr const size_t		PPU_REGISTERS_SIZE				= 0x0008;
			static constexpr const address_t	PPU_REGISTERS_MIRRORS_ADDRESS	= PPU_REGISTERS_ADDRESS + PPU_REGISTERS_SIZE;				// 0x2008
			static constexpr const size_t		PPU_REGISTERS_MIRROR_SIZE		= 0x0008;
			static constexpr const size_t		NUM_PPU_REGISTERS_MIRRORS		= 1023;
			static constexpr const address_t	APU_AND_IO_REGISTERS_ADDRESS	= PPU_REGISTERS_MIRRORS_ADDRESS + NUM_PPU_REGISTERS_MIRRORS * PPU_REGISTERS_MIRROR_SIZE;
			static constexpr const size_t		APU_AND_IO_REGISTERS_SIZE		= 0x0018;
			static constexpr const address_t	DISABLED_APU_AND_IO_ADDRESS		= APU_AND_IO_REGISTERS_ADDRESS + APU_AND_IO_REGISTERS_SIZE;
			static constexpr const size_t		DISABLED_APU_AND_IO_SIZE		= 0x0008;
			static constexpr const address_t	CARTRIDGE_ADDRESS				= DISABLED_APU_AND_IO_ADDRESS + DISABLED_APU_AND_IO_SIZE;
			static constexpr const size_t		CARTRIDGE_SIZE					= 0x10000 - CARTRIDGE_ADDRESS;

		private:
			ConsecutiveMemory8BitView						mRam;					// 2 KB internal RAM
			DynamicArray<ConsecutiveMemory8BitView>			mRamMirrors;			// Mirrors of the internal RAM
			ConsecutiveMemory8BitView						mPpuRegisters;			// PPU registers
			DynamicArray<ConsecutiveMemory8BitView>			mPpuRegistersMirrors;	// Mirrors of the PPU 
			ConsecutiveMemory8BitView						mApuAndIoRegisters;		// APU and I/O registers
			ConsecutiveMemory8BitView						mDisabledApuAndIo;		// APU and I/O functionality that is normally disabled.
			ConsecutiveMemory8BitView						mCartridge;				// Cartridge space: PRG ROM, PRG RAM, and mapper registers

		private:
			static_assert( RAM_MIRRORS_ADDRESS == 0x0800 );
			static_assert( ARRAYSIZE( RAM_MIRROR_ADDRESSES ) == NUM_RAM_MIRRORS );
			static_assert( RAM_MIRROR_ADDRESSES[0]			== 0x0800 );
			static_assert( RAM_MIRROR_ADDRESSES[1]			== 0x1000 );
			static_assert( RAM_MIRROR_ADDRESSES[2]			== 0x1800 );
			static_assert( PPU_REGISTERS_ADDRESS			== 0x2000 );
			static_assert( PPU_REGISTERS_MIRRORS_ADDRESS	== 0x2008 );
			static_assert( APU_AND_IO_REGISTERS_ADDRESS		== 0x4000 );
			static_assert( DISABLED_APU_AND_IO_ADDRESS		== 0x4018 );
			static_assert( CARTRIDGE_ADDRESS				== 0x4020 );
			static_assert( CARTRIDGE_SIZE					== 0xBFE0 );
		};
	}
}