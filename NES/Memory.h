#pragma once

#include "Common.h"

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
		inline constexpr ConsecutiveMemory( const size_t capacity ) noexcept : mData( capacity, 0 ) {}

	private:
		std::vector<TData>	mData;
	};

	namespace nes
	{
		class Ram : public ConsecutiveMemory<data_t>
		{
			
		};
	}
}