#include "log.hpp"

namespace config {
	void __cdecl Log::LoglevelInfo(std::string const& message)
	{
		colorAttributes color{ GREEN };
		if (m_level <= 0)
			std::cout << "[INFO] " << message;
		else
			return;
	}
	void __cdecl Log::LoglevelWarn(std::string const& message)
	{
		colorAttributes color{ YELLOW };
		if (m_level <= 1)
			std::clog << "[WARNING] " << message;
		else
			return;
	}
	void __cdecl Log::LoglevelError(std::string const& message)
	{
		colorAttributes color{ RED | FOREGROUND_INTENSITY };
		if (m_level <= 2)
			std::cerr << "[ERROR] " << message;
		else
			return;
	}
	void __cdecl Log::LoglevelFatalError(std::string const& message)
	{
		colorAttributes color{ FOREGROUND_RED | FOREGROUND_INTENSITY };
		if (m_level <= 2)
			std::cerr << "[FATAL_ERROR] " << message;
		else
			return;
	}
	void __cdecl Log::LogsetLevel(int level)
	{
		m_level = level;
	}
}