#pragma once

#include "Common.h"

#include "NES/ArrayView.h"
#include "NES/DynamicArray.hpp"

namespace ninmuse
{
	class IMemory
	{
	public:
	private:
	};

	template <typename TData>
	requires IsData<TData>
	class ConsecutiveMemory
	{
	public:
		inline constexpr const std::unique_ptr<IArray<TData>>& GetData() const noexcept { return mData; }

	protected:
		std::unique_ptr<IArray<TData>>	mData;
	};

	class ConsecutiveMemory8BitView : public ConsecutiveMemory<nes::data_t>
	{
	public:
		ConsecutiveMemory8BitView( const ConsecutiveMemory<nes::data_t>& memory, const nes::address_t startAddress, const size_t size ) noexcept;

	protected:
		ArrayView<nes::data_t>	mData;
	};

	namespace nes
	{
		class MemoryMap final : ConsecutiveMemory<data_t>
		{
		public:
			MemoryMap() noexcept;

		private:
			ConsecutiveMemory8BitView	mRam;
			ConsecutiveMemory8BitView	mRom;
		};
	}
}