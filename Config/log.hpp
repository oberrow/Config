#pragma once
#include <string>
#include <iostream>
#include "colorAttributes.hpp"

namespace Config
{
	class Log
	{
	private:
		int m_level;
	public:
		static const int levelInfoId = 0;
		static const int levelWarningId = 1;
		static const int levelErrorId = 2;
		void __cdecl LoglevelInfo(std::string message);
		void __cdecl LoglevelWarn(std::string message);
		void __cdecl LoglevelError(std::string message);
		void __cdecl LogsetLevel(int level);
		Log(int level)
			:m_level{level} { }
	};
}