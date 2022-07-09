#include "log.hpp"

namespace config {
	void __cdecl Log::LoglevelInfo(std::string const& message)
	{
		std::cout << "\x1b[32m";
		if (m_level <= 0)
			std::cout << "[INFO] " << message;
		else
			return;
		std::clog << "\x1b[39m";
	}
	void __cdecl Log::LoglevelWarn(std::string const& message)
	{
		std::clog << "\x1b[33m";
		if (m_level <= 1)
			std::clog << "[WARNING] " << message;
		else
			return;
		std::clog << "\x1b[39m";
	}
	void __cdecl Log::LoglevelError(std::string const& message)
	{
		std::cerr << "\x1b[31m" << "\x1b[1m";
		if (m_level <= 2)
			std::cerr << "[ERROR] " << message;
		else
			return;
		std::cerr << "\x1b[39m";
	}
	void __cdecl Log::LoglevelFatalError(std::string const& message)
	{
		std::cerr << "\x1b[31m";
		if (m_level <= 2)
			std::cerr << "[FATAL_ERROR] " << message;
		else
			return;
		std::cerr << "\x1b[39m";
	}
	void __cdecl Log::LogsetLevel(int level)
	{
		m_level = level;
	}
}