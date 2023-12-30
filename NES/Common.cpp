#include "stdafx.h"

#include "Common.h"

namespace ninmuse
{
	bool Assert( const bool expression, const char* description, const int32_t lineNumber, const char* filename ) noexcept
	{
		if ( expression == true )
			return false;

		std::cout << filename << "[" << lineNumber << "]: " << description << '\n';

		return true;
	}
}
