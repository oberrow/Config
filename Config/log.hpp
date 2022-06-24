#pragma once
#include <string>
#include <iostream>
#include "colorAttributes.hpp"

namespace config
{
	class Log
	{
	private:
		int m_level;
	public:
		static const int levelInfoId = 0;
		static const int levelWarningId = 1;
		static const int levelErrorId = 2;
		void __cdecl LoglevelInfo(std::string const& message);
		void __cdecl LoglevelWarn(std::string const& message);
		void __cdecl LoglevelError(std::string const& message);
		void __cdecl LoglevelFatalError(std::string const& message);
		void __cdecl LogsetLevel(int level);
		Log(int level)
			:m_level{level} { }
	};
}