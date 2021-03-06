#pragma once
#include <string>
#include <iostream>

#ifdef _WIN32
#define WINDOWS 1
#include <Windows.h>
#define CDECL __cdecl
#else
#define CDECL
#endif

namespace config
{
	class Log
	{
	private:
		int m_level;
	public:
		int err{};
		static const int levelInfoId = 0;
		static const int levelWarningId = 1;
		static const int levelErrorId = 2;
		void CDECL LoglevelInfo(std::string const& message);
		void CDECL LoglevelWarn(std::string const& message);
		void CDECL LoglevelError(std::string const& message);
		void CDECL LoglevelFatalError(std::string const& message);
		void CDECL LogsetLevel(int level);
		Log(int level)
			:m_level{level} 
		{
#ifdef WINDOWS
			// Virtual console sequences not on by default
			// https://docs.microsoft.com/en-us/windows/console/console-virtual-terminal-sequences
			// Set output mode to handle virtual terminal sequences
			HANDLE hOut = GetStdHandle(STD_OUTPUT_HANDLE);
			if (hOut == INVALID_HANDLE_VALUE)
			{
				err = GetLastError();
				return;
			}

			DWORD dwMode = 0;
			if (!GetConsoleMode(hOut, &dwMode))
			{
				err = GetLastError();
				return;
			}

			dwMode |= ENABLE_VIRTUAL_TERMINAL_PROCESSING;
			if (!SetConsoleMode(hOut, dwMode))
			{
				err = GetLastError();
				return;
			}
#endif
		}
	};
}