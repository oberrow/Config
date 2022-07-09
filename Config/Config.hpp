#pragma once
#include <string>
#include <fstream>
#include <vector>
#include <filesystem>
#include <sstream>
#include <unordered_map>
#include <functional>
#include "log.hpp"
#include <cstring>
#include <array>
#include <map>

// If x == true prints msg and cals __fastfail(exitCode) (int 29h in asm)
#define FATAL_ASSERT(x, msg, exitCode) if(x)\
{\
	std::cout << msg;\
	__fastfail(exitCode); /*does mov ecx, exitCode and then int 29h*/\
}
// If x == true prints msg and cals __debugbreak() (int 3 in asm)
#define ASSERT(x, msg, run) if(x)\
{\
	std::cout << msg;\
	__debugbreak(); /*does int 3*/\
	run\
}

namespace config
{
	inline const std::string rickrolllyrics ="\
		We're no strangers to love\n\
		You know the rules and so do I\n\
		A full commitment's what I'm thinking of\n\
		You wouldn't get this from any other guy\n\
\n\
		I just wanna tell you how I'm feeling\n\
		Gotta make you understand\n\
\n\
		Never gonna give you up\n\
		Never gonna let you down\n\
		Never gonna run around and desert you\n\
		Never gonna make you cry\n\
		Never gonna say goodbye\n\
		Never gonna tell a lie and hurt you\n\
		\n\
		We’ve known each other for so long\n\
		Your heart's been aching but you're too shy to say it\n\
		Inside we both know what's been going on\n\
		We know the game and we're gonna play it\n\
		\n\
		And if you ask me how I'm feeling\n\
		Don't tell me you're too blind to see\n\
		\n\
		Never gonna give you up\n\
		Never gonna let you down\n\
		Never gonna run around and desert you\n\
		Never gonna make you cry\n\
		Never gonna say goodbye\n\
		Never gonna tell a lie and hurt you\n"
		;
	class Config;
	enum class config_error
	{
		// A line was commented out
		COMMENTED_LINE = -2,
		// No error
		OK = -1,
		// CONFIG-0000
		TYPE_UNRECOGNIZED = 0x00,
		// CONFIG-0001
		INCOMPATIBLE_TYPES = 0x01,
		// CONFIG-0002
		DOUBLE_INT_CONVERSION = 0x02,
		// CONFIG-0003
		MISSING_BRACKET_IN_ARRAY_DECLARATION = 0x03,
		// CONFIG-0004
		MISSING_EQUAL_SIGN = 0x04,
		// CONFIG-0005
		MISSING_COMMA_IN_ARRAY_DECLARATION = 0x05,
		// CONFIG-0006
		TO_LITTLE_OR_TOO_MUCH_DECLARATIONS_IN_ARRAY = 0x06,
		// CONFIG-0007
		MISSING_ARRAY_SIZE = 0x07,
		// CONFIG-0008
		MIS_MATCHED_TYPES_IN_ARRAY = 0x08,
		// CONFIG-0009
		TOO_MUCH_OR_TO_LITTLE_QUOTATION_MARKS_IN_STRING_DECLARATION = 0x09,
		// CONFIG-0010
		COULD_NOT_FIND_IN_DATA_MAP = 0x0A,
		// CONFIG-0011
		ARRAY_IS_TOO_BIG_OR_TOO_SMALL_TO_HOLD_STRING = 0x0B,
		// CONFIG-0012
		VAR_NAME_IS_A_TYPE_NAME = 0x0C,
		// CONFIG-0013
		VAR_IS_NOT_DECLARED_IN_SCOPE = 0x0D,
		// CONFIG-0014
		TYPE_IS_A_STRING = 0x0E,
		// CONFIG-0015 WARNING
		USE_STRING_INSTEAD_OF_CHAR_ARRAY = 0x0F,
		// Fatal Error CONFIG-0016
		TERMINATE_PARSER_WAS_CALLED = 0x10,
		// CONFIG-0017
		CANNOT_MODULO_DOUBLE = 0x11,
		// Fatal Error CONFIG-0018
		INVALID_PREPROCESSING_DIRECTIVE = 0x12,
		// CONFIG-0019
		RETURN_IS_NOT_IN_FUNCTION = 0x13,
		// CONFIG-0020
		TOO_MANY_OR_TO_FEW_PARAMETERS_IN_FUNCTION_CALL = 0x14,
		// Fatal Error CONFIG-0021
		ISTREAM_OBJECT_IS_CIN = 0x15,
		// CONFIG-0022
		INCOMPLETE_TYPE_IS_NOT_ALLOWED = 0x16
	};
	inline std::ostream& operator<<(std::ostream& os, const config_error& _Other)
	{
		using ce = config_error;
		os << "Details are unknown to the operator<< so specific information is un-avalibable\n";
		if (_Other == ce::TYPE_UNRECOGNIZED) os << "CONFIG-0000 Type is un-recognized";
		else if (_Other == ce::INCOMPATIBLE_TYPES) os << "CONFIG-0001 Type [] is in-compatable with declaration of type []";
		else if (_Other == ce::DOUBLE_INT_CONVERSION) os << "CONFIG-0002 Cannot convert from type 'double' to 'int'!";
		else if (_Other == ce::MISSING_BRACKET_IN_ARRAY_DECLARATION) os << "CONFIG-0003 Missing an '[' or a ']' in array declaration!";
		else if (_Other == ce::MISSING_EQUAL_SIGN) os << "CONFIG-0004 Missing an equal sign in variable declaration!";
		else if (_Other == ce::MISSING_COMMA_IN_ARRAY_DECLARATION) os << "CONFIG-0005 Missing a comma in array declaration!";
		else if (_Other == ce::TO_LITTLE_OR_TOO_MUCH_DECLARATIONS_IN_ARRAY) os << "CONFIG-0006 Too little or too much declarations in array declaration!";
		else if (_Other == ce::MISSING_ARRAY_SIZE) os << "CONFIG-0007 Missing array size in array declaration!";
		else if (_Other == ce::MIS_MATCHED_TYPES_IN_ARRAY) os << "CONFIG-0008 Cannot convert from [] to [] in array!";
		else if (_Other == ce::TOO_MUCH_OR_TO_LITTLE_QUOTATION_MARKS_IN_STRING_DECLARATION) os << "CONFIG-0009 Too much or too little quotation marks in string";
		else if (_Other == ce::COULD_NOT_FIND_IN_DATA_MAP) os << "CONFIG-0010 Could not find [] in data map!";
		else if (_Other == ce::ARRAY_IS_TOO_BIG_OR_TOO_SMALL_TO_HOLD_STRING) os << "CONFIG-0011 Char array is either too small or too big too hold the string!";
		else if (_Other == ce::VAR_NAME_IS_A_TYPE_NAME) os << "CONFIG-0012 Variable name is a type name!";
		else if (_Other == ce::VAR_IS_NOT_DECLARED_IN_SCOPE) os << "CONFIG-0013 Variable wasn't declared in this scope!";
		else if (_Other == ce::TYPE_IS_A_STRING) os << "CONFIG-0014 Type is a string!";
		else if (_Other == ce::USE_STRING_INSTEAD_OF_CHAR_ARRAY) os << "CONFIG-0015 [WARNING] Use a string instead of a char array";
		else if (_Other == ce::TERMINATE_PARSER_WAS_CALLED) os << "CONFIG-0016 [FATAL ERROR] Terminate parser was called!";
		else if (_Other == ce::CANNOT_MODULO_DOUBLE) os << "CONFIG-0017 Cannot modulo a double";
		else if (_Other == ce::INVALID_PREPROCESSING_DIRECTIVE) os << "CONFIG-0018 [FATAL ERROR] Unrecognized preprocessing directive";
		else if (_Other == ce::RETURN_IS_NOT_IN_FUNCTION) os << "CONFIG-0019 Return statement is not in function!";
		else if (_Other == ce::TOO_MANY_OR_TO_FEW_PARAMETERS_IN_FUNCTION_CALL) os << "CONFIG-0020 Too many or too little parameters in function call!";
		else if (_Other == ce::ISTREAM_OBJECT_IS_CIN) os << "CONFIG-0021 (FATAL ERROR) Inputed std::istream& object == std::cin! Terminating parser!\n";
		else if (_Other == ce::INCOMPLETE_TYPE_IS_NOT_ALLOWED) os << "CONFIG-0022 Incomplete type is not allowed! Is the type void?\n";
		return os;
	}
	class config_exception
	{
	public:
		const char* what() { return m_ErrorText; }
		config_exception() = default;
		config_exception(const char* errMessage, config_error err, int line, bool isHeap) :
			m_Err{ err }, 
			m_ErrorText{ errMessage },
			m_Line{ line },
			m_IsHeap{ isHeap }
		{}
		~config_exception()
		{
			if (m_IsHeap) free((void*)m_ErrorText);
		}
	private:
		config_error m_Err = { config_error::OK };
		const char* m_ErrorText = nullptr;
		int m_Line{};
		bool m_IsHeap{};
	};
	class LogCover
	{
	public:
		void LogError(std::string msg, config_error err, bool isFatal = false);
		void LogError(const std::stringstream& msg_sstream, config_error err, bool isFatal = false);
		void LogError(const std::ostringstream& msg_osstream, config_error err, bool isFatal = false);
		LogCover(Log l, bool throwExceptionOnError, Config* _Other, int* ele, bool isFunction, int* line) :
			m_Log{ l },
			m_ThrowExceptionOnError{ throwExceptionOnError },
			m_Other{ _Other },
			m_Ele{ ele },
			m_IsFunction{ isFunction }, 
			m_Line{ line } 
		{}
	private:
		Log m_Log{ Log::levelInfoId };
		Config* m_Other = nullptr;
		int* m_Ele = nullptr;
		int* m_Line = nullptr;
		bool m_IsFunction = false;
		bool m_ThrowExceptionOnError{};
	};
	enum class types
	{
		// An invalid type
		invalid_type = -2,
		// A boolean type
		basic_boolean = 0,
		// 1 Character
		basic_character = sizeof(char),
		// An integer type
		basic_int = sizeof(int),
		// A decimal number type (double)
		basic_double = sizeof(double),
		// A hexadecimal number
		basic_hex = 0x10,
		// A string type
		basic_string = sizeof(std::string),
		// No type but needed to check functions return value
		basic_void = -1
	};
	/*struct configStruct
	{
		std::vector<std::string> variables;
		std::string struct_name;
	};*/
	struct Store {
		void Clear()
		{
			type = types::invalid_type;
			var_name = "";
			str_Out = "";
			int_Out = 0;
			boolean_Out = false;
			hex_Out = 0x00;
			decimal_Out = 0.00;
			array_Size = -1;
			character_array_Out.clear();
			int_array_Out.clear();
			string_array_Out.clear();
			decimal_array_Out.clear();
			hex_array_Out.clear();
			boolean_array_Out .clear();
			character_Out = '\0';
		}
		std::pair<types, std::string> Get()
		{
			std::ostringstream oss;
			oss << *this;
			std::string val = oss.str();
			std::ranges::reverse(val.begin(), val.end());
			val.erase(val.find('='), std::string::npos);
			std::ranges::reverse(val.begin(), val.end());
			return std::make_pair(this->type, val);
		}
		static std::string TypeToTypeName(types t)
		{
			if (t == types::basic_boolean) return "bool";
			else if (t == types::basic_character) return "char";
			else if (t == types::basic_int) return "int";
			else if (t == types::basic_double) return "double";
			else if (t == types::basic_hex) return "hex_number";
			else if (t == types::basic_string) return "string";
			else if (t == types::basic_void) return "void";
			else return "invalid_type";
		}
		types type = types::invalid_type;
		std::string scope{};
		std::string var_name{};
		std::string str_Out{};
		int int_Out{};
		double decimal_Out = 0;
		int64_t hex_Out{};
		bool boolean_Out{};
		char character_Out = '\0';
		// Array definitions
		// --------------
		std::vector<char>			character_array_Out;
		std::vector<int>			int_array_Out;
		std::vector<std::string>	string_array_Out;
		std::vector<double>			decimal_array_Out;
		std::vector<long long>		hex_array_Out;
		std::vector<bool>			boolean_array_Out;
		size_t array_Size = static_cast<size_t>(-1);
		bool is_Array = false;
		bool didCallToString = false; // Can only be true if object.type == types::basic_string
		// --------------
		Store operator+(const Store& st1) const
		{
			Store st2 = {
				st1.type,
				(var_name + st1.var_name),
				(str_Out + st1.str_Out),
				(scope + st1.scope),
				(int_Out + st1.int_Out),
				(decimal_Out + st1.decimal_Out),
				(hex_Out + st1.hex_Out),
				(boolean_Out && st1.boolean_Out) };
			return st2;
		}
		Store operator+(Store& st1) const
		{
			Store st2 = {
				st1.type,
				(var_name + st1.var_name),
				(str_Out + st1.str_Out),
				(scope + st1.scope),
				(int_Out + st1.int_Out),
				(decimal_Out + st1.decimal_Out),
				(hex_Out + st1.hex_Out),
				(boolean_Out && st1.boolean_Out) };
			return st2;
		}
		const Store& operator+=(Store const& st1)
		{
			*this = st1 + *this;
			return *this;
		}
		bool operator==(const Store& s) const
		{
			bool equality = (
				s.int_Out == this->int_Out
				&& s.str_Out == this->str_Out
				&& s.boolean_Out == this->boolean_Out
				&& s.type == this->type
				&& s.var_name == this->var_name);
			return equality;
		}
		friend std::ostream& operator<<(std::ostream& os, const Store& s);
	};
	inline std::ostream& operator<<(std::ostream& os, const Store& s)
	{
		if (	 s.type == types::basic_string && !s.str_Out.empty() && !s.is_Array)
		{
			os << s.var_name << " = " << s.str_Out;
		}
		else if (s.type == types::basic_character && !s.is_Array)
		{
			os << s.var_name << " = " << s.character_Out;
		}
		else if (s.type == types::basic_int && !s.is_Array)
		{
			os << s.var_name << " = " << s.int_Out;
		}
		else if (s.type == types::basic_hex && !s.is_Array)
		{
			os << s.var_name << " = 0x" << std::hex << std::uppercase << s.hex_Out << std::dec << std::nouppercase;
		}
		else if (s.type == types::basic_boolean && !s.is_Array)
		{
			os << s.var_name << " = " << std::boolalpha << s.boolean_Out << std::noboolalpha;
		}
		else if (s.type == types::basic_double && !s.is_Array)
		{
			os << s.var_name << " = " << s.decimal_Out;
		}
		else if (s.type == types::basic_string && s.is_Array)
		{
			int index = 0;
			for (auto& i : s.string_array_Out)
			{
				os << s.var_name << "[" << index << "] = " << i << '\n';
				index++;
			}
		}
		else if (s.type == types::basic_character && s.is_Array)
		{
			std::string arr;
			for (auto& i : s.character_array_Out)
			{
				arr.push_back(i);
			}
			char last = '\0';
			if (arr[arr.size() - 1] != '\n') last = '\n';
			os << s.var_name << " = " << arr << last;
		}
		else if (s.type == types::basic_int && s.is_Array)
		{
			int index = 0;
			for (auto& i : s.int_array_Out)
			{
				os << s.var_name << "[" << index << "] = " << i << '\n';
				index++;
			}
		}
		else if (s.type == types::basic_hex && s.is_Array)
		{
			int index = 0;
			for (auto& i : s.hex_array_Out)
			{
				os << s.var_name << "[" << index << "] = 0x" << std::hex << std::uppercase << i << std::dec << std::nouppercase << '\n';
				index++;
			}
		}
		else if (s.type == types::basic_boolean && s.is_Array)
		{
			int index = 0;
			for (auto i : s.boolean_array_Out)
			{
				os << s.var_name << "[" << index << "] = " << std::boolalpha << i << std::noboolalpha << '\n';
				index++;
			}
		}
		else if (s.type == types::basic_double && s.is_Array)
		{
			int index = 0;
			for (auto& i : s.decimal_array_Out)
			{
				os << s.var_name << "[" << index << "] = " << i << '\n';
				index++;
			}
		}
		else if (s.type == types::invalid_type)
		{
			os << "I don't know how you could've gotten an invalid type. Is the Store object default initalized?";
		}
		else
		{
			os << "I'm not sure how you got this type so I can't process it. Is str_Out empty?";
		}
		return os;
	}
	struct Function
	{
		std::vector<std::string> actions;
		std::vector<std::string> parameters;
		long long numberOfParameters{};
		std::string func_type;
		std::string func_name;
		int startLine = 0;
	};
	class Config
	{
	private:
		int line = 0;
		std::vector<config_error> error_list;
		static std::string remove_trailing_whitespaces(std::string& remove)
		{
			size_t spacecount = remove.find_first_not_of(' ');
			if (spacecount == 0) return remove;
			remove.erase(0, spacecount);
			return remove;
		}
		static std::string remove_trailing_tabs(std::string& remove)
		{
			size_t spacecount = remove.find_first_not_of('\t');
			if (spacecount == 0) return remove;
			remove.erase(0, spacecount);
			return remove;
		}
		size_t element = 0;
		std::string map_element{};
		std::string seperateSemiColonsToNl(std::stringstream& ss)
		{
			std::string it{};
			std::string content{};
			std::string currentSemicolon{};
			while (std::getline(ss, content))
			{
				if (content.find("//") != std::string::npos) continue;
				for (int i = 0; i < std::count(content.begin(), content.end(), ';') + 1; i++)
				{
					currentSemicolon = content.substr(0, content.find(';'));
					if (currentSemicolon.find_first_not_of(' ') != 0)
						currentSemicolon.erase(0, currentSemicolon.find_first_not_of(' '));
					if (content.find_first_not_of(' ') != 0)
						content.erase(0, content.find_first_not_of(' '));
					it.append(currentSemicolon);
					it.push_back('\n');
					content.erase(content.find(currentSemicolon), currentSemicolon.length() + 1);
				}
			}
			return it;
		}
		bool isdigit(char _ch)
		{
			bool not_found = false;
			std::string num = "0123456789";
			for (int i = 0; i < 10; i++)
			{
				if (_ch == num[i])
				{
					return true;
				}
				else
				{
					not_found = true;
				}
			}
			return !not_found;
		}
		bool isDecimalNumber(const std::string& s) const
		{
			return atof(s.c_str()) > 0;
		}
		bool isNumber(const std::string& s)
		{
			// Replaced so that it never returns false when it should return true
			// return std::atoi(s.c_str()) > 0);
			if (s.empty()) return false;
			for (auto i : s)
			{
				// If the number isn't a valid digit or '-' or any other operators then return false
				if (!isdigit(i) && i != '-' && i != '+' && i != '/' && i != '*' && i != '%' && i != ' ') return false;
				// If it is then it will continue the loop
				else continue;
			}
			// If nothing returns false then it will return true
			return true;
		}
		bool isHexNumber(std::string_view const& string) const
		{
			return string.find("0x") != std::string::npos;
		}
		void SetType(const std::string_view& type)
		{
			if (type == type_names[0])
			{
				data[element].type = types::basic_int;
				data_map[map_element].type = types::basic_int;
			}
			else if (type == type_names[1])
			{
				data[element].type = types::basic_boolean;
				data_map[map_element].type = types::basic_boolean;
			}
			else if (type == type_names[2])
			{
				data[element].type = types::basic_hex;
				data_map[map_element].type = types::basic_hex;
			}
			else if (type == type_names[3])
			{
				data[element].type = types::basic_double;
				data_map[map_element].type = types::basic_double;
			}
			else if (type == type_names[4])
			{
				data[element].type = types::basic_string;
				data_map[map_element].type = types::basic_string;
			}
			else if (type == type_names[5])
			{
				data[element].type = types::basic_character;
				data_map[map_element].type = types::basic_character;
			}
		}
		std::vector<Store> data;
		Store returnValue{}; // Will only not be default initalized when scope != "main" and functionType != "void"
		std::unordered_map<std::string, Store> data_map;
		std::map<std::string, Function> function_map;
		const static inline int size = 7;
		bool isStdCin = false; // Hopes the user is a good one and gives an std::ifstream object instead of std::cin
		const std::array<std::string, size> type_names =
		{
			"int",
			"bool",
			"hex_number",
			"double",
			"string",
			"char",
			"void"
		};
		bool good = true;
		int err_element = 0;
		void FreeArrayByType(const std::string_view& type)
		{
			if (type == "int")
			{
				data[element].int_array_Out.clear();
			}
			else if (type == "bool")
			{
				data[element].boolean_array_Out.clear();
			}
			else if (type == "hex_number")
			{
				data[element].hex_array_Out.clear();
			}
			else if (type == "double")
			{
				data[element].decimal_array_Out.clear();
			}
			else if (type == "string")
			{
				data[element].string_array_Out.clear();
			}
			else if (type == "char")
			{
				data[element].character_array_Out.clear();
			}
		}
		enum class basic_math_error { 
			OK, // No error
			ERR, // Error
			NOTHING_TO_DO // No operation to calculate
		};
		struct math_error
		{
			basic_math_error ec = basic_math_error::OK; // Error state
			config_error cnfg_err = config_error::OK; // Config error state
			double result; // Result
		};
		types GetTypeFromString(std::string str)
		{
			if ((!isNumber(str) && str == "true") || (!isNumber(str) && str == "false")) return types::basic_boolean;
			else if (isNumber(str)) return types::basic_int;
			else if (isHexNumber(str)) return types::basic_hex;
			else if (isDecimalNumber(str)) return types::basic_double;
			else if (!isNumber(str) && str.find('\'') != std::string::npos) return types::basic_character;
			else if (!isNumber(str)) return types::basic_string;
			else return types::invalid_type;
		}
		/* Returns the math error status, the result and the config error status 
	       Since the type of the result can be a double it will use a double as it is the biggest integer type along with int64_t
		*/
		math_error DoMath(std::string& values)
		{
			math_error m_err = { basic_math_error::OK, config_error::OK, 0.00 };
			Log l{ Log::levelInfoId };
			LogCover lC{ l, throwExceptionOnError, this, &err_element, isFunction, &line };
			std::string var_one = values.substr(0, values.find(' ') - 1); // First operand
			if (data_map.contains(var_one))
			{
				types type = GetTypeFromString(values);
				if (type == types::basic_string)
				{
					lC.LogError("CONFIG-0014 Type is a string!", config_error::TYPE_IS_A_STRING);
					return {
						basic_math_error::ERR,
						config_error::TYPE_IS_A_STRING,
						0.00
					};
				}
				if (values.		find('+') != std::string::npos)
				{
					std::string operand1 = values.substr(0, values.find('+') - 1);
					std::string operand2 = values.substr(values.find('+') + 1, std::string::npos);
					std::erase(operand2, ' ');
					if (type == types::basic_int)
					{
						return { basic_math_error::OK, config_error::OK, static_cast<double>(std::atoi(operand1.c_str()) + std::atoi(operand2.c_str())) };
					}
					else if (type == types::basic_double)
					{
						return { basic_math_error::OK, config_error::OK, std::atof(operand1.c_str()) + std::atof(operand2.c_str()) };
					}
					else if (type == types::basic_hex)
					{
						int64_t temp1{};
						int64_t temp2{};
						std::istringstream(operand1) >> std::hex >> temp1;
						std::istringstream(operand2) >> std::hex >> temp2;
						return { basic_math_error::OK, config_error::OK, static_cast<double>(temp1 + temp2) };
					}
					else if (type == types::basic_character)
					{
						if (!(std::atoi(operand2.c_str()) < 127) || !(std::atoi(operand2.c_str()) > -128))
						{
							return { basic_math_error::OK, config_error::OK, static_cast<double>(std::atoi(operand1.c_str()) + std::atoi(operand2.c_str())) };
						}
					}
				}
				else if (values.find('-') != std::string::npos)
				{
					std::string operand1 = values.substr(0, values.find('-') - 1); // Variable name to add to
					std::string operand2 = values.substr(values.find('-') + 1, std::string::npos);
					std::erase(operand2, ' ');
					if (!data_map.contains(operand1))
					{
						lC.LogError((std::ostringstream() << "CONFIG-0013 " << operand1 << " is not declared in this scope!").str(), config_error::VAR_IS_NOT_DECLARED_IN_SCOPE);
						m_err.cnfg_err = config_error::VAR_IS_NOT_DECLARED_IN_SCOPE;
						m_err.ec = basic_math_error::ERR;
						return m_err;
					}
					else
					{
						int i = 0;
						for (; i < data.size(); i++)
						{
							if (data[i].var_name == operand1) break;
							else continue;
						}
						if (type == types::basic_string)
						{
							lC.LogError((std::ostringstream() << "CONFIG-0014 " << operand1 << " is a string!").str(), config_error::TYPE_IS_A_STRING);
							m_err.cnfg_err = config_error::TYPE_IS_A_STRING;
							m_err.ec = basic_math_error::ERR;
							return m_err;
						}
						else
						{
							if (type == types::basic_int)
							{
								return { basic_math_error::OK, config_error::OK, static_cast<double>(data_map[operand1].int_Out - std::atoi(operand2.c_str())) };
							}
							else if (type == types::basic_double)
							{
								return { basic_math_error::OK, config_error::OK, data_map[operand1].decimal_Out - std::atof(operand2.c_str()) };
							}
							else if (type == types::basic_hex)
							{
								int64_t temp{};
								std::istringstream(operand2) >> std::hex >> temp;
								return { basic_math_error::OK, config_error::OK, static_cast<double>(data_map[operand1].hex_Out - temp) };
							}
							else if (type == types::basic_character)
							{
								if (!(std::atoi(operand2.c_str()) < CHAR_MIN) || !(std::atoi(operand2.c_str()) > CHAR_MAX))
								{
									return { basic_math_error::OK, config_error::OK, static_cast<double>(data_map[operand1].character_Out - std::atoi(operand2.c_str())) };
								}
							}
						}
					}
				}
				else if (values.find('/') != std::string::npos)
				{
					std::string operand1 = values.substr(0, values.find('-') - 1); // Variable name to add to
					std::string operand2 = values.substr(values.find('-') + 1, std::string::npos);
					std::erase(operand2, ' ');
					if (!data_map.contains(operand1))
					{
						lC.LogError((std::ostringstream() << "CONFIG-0013 " << operand1 << " is not declared in this scope!").str(), config_error::VAR_IS_NOT_DECLARED_IN_SCOPE);
						m_err.cnfg_err = config_error::VAR_IS_NOT_DECLARED_IN_SCOPE;
						m_err.ec = basic_math_error::ERR;
						return m_err;
					}
					else
					{
						int i = 0;
						for (; i < data.size(); i++)
						{
							if (data[i].var_name == operand1) break;
							else continue;
						}
						if (type == types::basic_string)
						{
							lC.LogError((std::ostringstream() << "CONFIG-0014 " << operand1 << " is a string!").str(), config_error::TYPE_IS_A_STRING);
							m_err.cnfg_err = config_error::TYPE_IS_A_STRING;
							m_err.ec = basic_math_error::ERR;
							return m_err;
						}
						else
						{
							if (type == types::basic_int)
							{
								return { basic_math_error::OK, config_error::OK, static_cast<double>(data_map[operand1].int_Out / std::atoi(operand2.c_str())) };
							}
							else if (type == types::basic_double)
							{
								return { basic_math_error::OK, config_error::OK, static_cast<double>(data_map[operand1].decimal_Out / std::atoi(operand2.c_str())) };
							}
							else if (type == types::basic_hex)
							{
								int64_t temp{};
								std::istringstream(operand2) >> std::hex >> temp;
								return { basic_math_error::OK, config_error::OK, static_cast<double>(data_map[operand1].hex_Out / temp) };
							}
							else if (type == types::basic_character)
							{
								if (!(std::atoi(operand2.c_str()) < 127) || !(std::atoi(operand2.c_str()) > -128))
								{
									return { basic_math_error::OK, config_error::OK, static_cast<double>(data_map[operand1].character_Out / std::atoi(operand2.c_str())) };
								}
							}
						}
					}
				}
				else if (values.find('*') != std::string::npos)
				{
					std::string operand1 = values.substr(0, values.find('-') - 1); // Variable name to add to
					std::string operand2 = values.substr(values.find('-') + 1, std::string::npos);
					std::erase(operand2, ' ');
					if (!data_map.contains(operand1))
					{
						lC.LogError((std::ostringstream() << "CONFIG-0013 " << operand1 << " is not declared in this scope!").str(), config_error::VAR_IS_NOT_DECLARED_IN_SCOPE);
						m_err.cnfg_err = config_error::VAR_IS_NOT_DECLARED_IN_SCOPE;
						m_err.ec = basic_math_error::ERR;
						return m_err;
					}
					else
					{
						int i = 0;
						for (; i < data.size(); i++)
						{
							if (data[i].var_name == operand1) break;
							else continue;
						}
						if (type == types::basic_string)
						{
							lC.LogError((std::ostringstream() << "CONFIG-0014 " << operand1 << " is a string!").str(), config_error::TYPE_IS_A_STRING);
							m_err.cnfg_err = config_error::TYPE_IS_A_STRING;
							m_err.ec = basic_math_error::ERR;
							return m_err;
						}
						else
						{
							if (type == types::basic_int)
							{
								return { basic_math_error::OK, config_error::OK, static_cast<double>(data_map[operand1].int_Out + std::atoi(operand2.c_str())) };
							}
							else if (type == types::basic_double)
							{
								return { basic_math_error::OK, config_error::OK, static_cast<double>(data_map[operand1].decimal_Out * std::atoi(operand2.c_str())) };
							}
							else if (type == types::basic_hex)
							{
								int64_t temp{};
								std::istringstream(operand2) >> std::hex >> temp;
								return { basic_math_error::OK, config_error::OK, static_cast<double>(data_map[operand1].hex_Out * temp) };
							}
							else if (type == types::basic_character)
							{
								if (!(std::atoi(operand2.c_str()) < 127) || !(std::atoi(operand2.c_str()) > -128))
								{
									return { basic_math_error::OK, config_error::OK, static_cast<double>(data_map[operand1].character_Out * std::atoi(operand2.c_str())) };
								}
							}
						}
					}
				}
				else if (values.find('%') != std::string::npos)
				{
					std::string operand1 = values.substr(0, values.find('-') - 1); // Variable name to add to
					std::string operand2 = values.substr(values.find('-') + 1, std::string::npos);
					std::erase(operand2, ' ');								
					if (type == types::basic_int)
							{
								return { basic_math_error::OK, config_error::OK, static_cast<double>(data_map[operand1].int_Out % std::atoi(operand2.c_str())) };
							}
					else if (type == types::basic_double)
							{
								lC.LogError("CONFIG-0017 Cannot modulo a variable of type double!\n", config_error::CANNOT_MODULO_DOUBLE);
								return { basic_math_error::ERR, config_error::CANNOT_MODULO_DOUBLE, 0.00 };
							}
					else if (type == types::basic_hex)
							{
								int64_t temp{};
								std::istringstream(operand2) >> std::hex >> temp;
								return { basic_math_error::OK, config_error::OK, static_cast<double>(data_map[operand1].hex_Out % temp) };
							}
					else if (type == types::basic_character)
							{
								if (!(std::atoi(operand2.c_str()) < 127) || !(std::atoi(operand2.c_str()) > -128))
								{
									return { basic_math_error::OK, config_error::OK, static_cast<double>(data_map[operand1].character_Out % std::atoi(operand2.c_str())) };
								}
							}
				}
			}
			return { basic_math_error::NOTHING_TO_DO, config_error::OK, 0.00 };
		}
		bool isValidType(std::string type)
		{
			for (int i = 0; i < size; i++)
			{
				if (type_names[i] == type) return true;
				else if (type != type_names[i] && i == size - 1) return false;
			}
		}
		constexpr bool isBuiltIn(std::string const& val) const
		{
			return val.find("print") != std::string::npos || val.find("hexToInt") != std::string::npos || val.find("intToHex") != std::string::npos || val.find("toString") != std::string::npos;
		}
		constexpr bool isNotBuiltInFunction(std::string const& val) const
		{
			return val.find("print") == std::string::npos && val.find("hexToInt") == std::string::npos && val.find("intToHex") == std::string::npos && val.find("toString") == std::string::npos;
		}
		constexpr bool isNotBuiltInFunction(std::string const& val, size_t nval) const
		{
			size_t print, hexToInt, intToHex, toString;
			print = val.find("print");
			hexToInt = val.find("hexToInt");
			intToHex = val.find("intToHex");
			toString = val.find("toString");
			return !(print >= nval) && !(hexToInt >= nval) && !(intToHex >= nval) && !(toString >= nval);
		}
		types GetTypeFromTypename(std::string tyName)
		{
			if (tyName == "bool")									return types::basic_boolean;
			else if (tyName == "char")								return types::basic_character;
			else if (tyName == "int")								return types::basic_int;
			else if (tyName == "double")							return types::basic_double;
			else if (tyName == "hex_number")						return types::basic_hex;
			else if (tyName == "string")							return types::basic_string;
			else if (tyName == "void")								return types::basic_void;
			else													return types::invalid_type;
		}
		// If you input rickroll it will print the lyrics of never gonna give you up and add a variable called rickroll and fill it with the lyrics
		// Waste of memory yes waste of time yes funny yes
		bool canRickRoll = true;
		bool isExperimental{};
		bool isLanguage = false; // Use language features? eg. functions, print(), println, printnl etc.
		bool throwExceptionOnError{};
		bool isFunction{};
		/* Lines that contain function / struct (not implemented yet) code that should be ignored used instead of deleting the line from the 
		   string stream to avoid invalidating iterators
		*/
		std::vector<int> ignored_lines; 
		bool RunFunction(Function& f, std::vector<std::string> passedInParameters, bool& willTerminate, bool& errState, Store& functionReturnValue)
		{
			LogCover lC{ Log(Log::levelErrorId), throwExceptionOnError, this, &err_element, isFunction, &line};
			// Runs a function using the f parameter
			std::istringstream it;
			std::string toIt;
			int index{};
			bool willExit = false;
			toIt.append("#use-beta-features true\n");
			toIt.append("#use-language-features true\n");
			for (auto& [k, i] : function_map)
			{
				std::string declar = i.func_type + ' ' + i.func_name;
				bool first = true;
				bool last{};
				int cIEle{};
				for (auto& i2 : i.parameters)
				{
					last = cIEle == i.numberOfParameters - 1;
					if(first) declar.push_back('(');
					if(cIEle != 0) declar.append(", ");
					declar.append(i2);
					cIEle++;
					if (last) cIEle = 0;
					if (last) declar.append(")\n{\n");
					first = false;
				}
				if (i.numberOfParameters == 0) declar.append("()\n{\n"); // Adds function body so no error happens
				for (auto& i2 : i.actions)
				{
					declar.append(i2);
					declar.push_back('\n');
				}
				declar.append("}\n");
				toIt.append(declar);
			}
			for (auto& i : f.parameters)
			{
				std::string v = passedInParameters[index];
				std::erase(v, ',');
				remove_trailing_tabs(v);
				remove_trailing_whitespaces(v);
				std::string parTyStr = i.substr(0, i.find(' '));
				std::erase(parTyStr, 32);
				types parType = GetTypeFromTypename(parTyStr);
				types passedInParType = GetTypeFromString(v);
				bool contains = data_map.contains(v);
				if (parType != passedInParType && !contains)
				{
					lC.LogError(std::ostringstream() << "CONFIG-0001 Type " << Store::TypeToTypeName(passedInParType) << " is incompatible with parameter of type " << parTyStr << '\n', config_error::INCOMPATIBLE_TYPES);
					willExit = true;
					break;
				}
				else if (contains && parType != data_map.at(v).type && v.find("toString") == std::string::npos)
				{
					lC.LogError(std::ostringstream() << "CONFIG-0001 Type " << Store::TypeToTypeName(passedInParType) << " is incompatible with parameter of type " << Store::TypeToTypeName(data_map.at(v).type) << '\n', config_error::INCOMPATIBLE_TYPES);
					willExit = true;
					break;
				}
				if (!data_map.contains(v))
				{
					if (passedInParameters[index].find("toString") == std::string::npos)
					{
						if(!data_map.contains(passedInParameters[index]))
							toIt.append(i + " = " + passedInParameters[index] + '\n');
						else
							toIt.append(i + " = " + data_map.at(passedInParameters[index]).Get().second + '\n');
					}
					else
					{
						std::string var_name = passedInParameters[index].substr(passedInParameters[index].find('(') + 1, passedInParameters[index].length() - 1);
						std::string toStringVarDefinitionNexist = i + " = " + passedInParameters[index];
						std::string data{};
						try
						{
							data = data_map.at(var_name).Get().second;
						} catch(std::out_of_range) 
						{
							lC.LogError(std::stringstream() << "CONFIG-0010 Could not find " << var_name << " in data map", config_error::COULD_NOT_FIND_IN_DATA_MAP);
						}
						std::string toStringVarDefinitionExist = i + " = " + "toString(" + remove_trailing_whitespaces(data) + ")";
						if(!data_map.contains(var_name))
							toIt.append(toStringVarDefinitionNexist);
						else 
							toIt.append(toStringVarDefinitionExist + '\n');
					}
				}
				else
				{
					Store& stRef = data_map.at(v);
					auto [type, s] = stRef.Get();
					std::erase(s, ',');
					if (type != types::basic_string)
						toIt.append(i + " = " + s + '\n');
					else
					{
						remove_trailing_whitespaces(s);
						std::string vToString = i + " = toString(" + s + ")\n";
						if (!stRef.didCallToString)
							toIt.append(i + " = \"" + s + "\"\n");
						else
							toIt.append(vToString);
					}
					index++;
				}
			}
			if (willExit) return !willExit;
			for (auto& i : f.actions)
			{
				toIt.append(i);
				toIt.push_back('\n');
			}
			it.str(toIt);
			types ty = GetTypeFromTypename(f.func_type);
			Config c{ it, f.func_name, ty }; // internal function scope
			functionReturnValue = c.returnValue;
			if (!c.good)
			{
				for (auto& i : c.GetErrorList())
				{
					if (i == config_error::TERMINATE_PARSER_WAS_CALLED) willTerminate = true;
				}
				return false;
			}
			return true;
		}
		bool Parse(std::istream const& is, bool isIfStatment, std::string const& scope = "main", types this_function_type = types::basic_int)
		{
			isFunction = scope != "main";
			if (!isIfStatment)
			{
				error_list.erase(error_list.begin(), error_list.end());
				ignored_lines.erase(ignored_lines.begin(), ignored_lines.end());
			}
			Log l{ Log::levelInfoId };
			LogCover lC{ l, throwExceptionOnError, this, &err_element, isFunction, &line };
			std::string values;
			std::stringstream preIt;
			std::stringstream it;
			if (!isStdCin)
			{
				preIt << is.rdbuf();
				it << seperateSemiColonsToNl(preIt);
			}
			else it.set_rdbuf(is.rdbuf());
			while (std::getline(it, values))
			{
				if (isStdCin && values == "EOF") break;
				line++;
				std::string valuesCpy = values;
				remove_trailing_whitespaces(valuesCpy);
				remove_trailing_tabs(valuesCpy);
				if (!values.empty() && values.front() == '{') continue;
				bool leave = false;
				if (isExperimental && !ignored_lines.empty() && !isIfStatment)
				{
					for (int i = 0; i < ignored_lines.size() - 1; i++)
					{
						if (line == ignored_lines.at(i))
						{
							leave = true;
							break;
						}
					}
				}
				if (leave) continue;
				error_list.push_back(config_error::OK);
				if (values.empty()) continue;
				bool isArray = false;
				size_t arraySize = -1;
				std::string pre_Values{};
				std::string var_name{};
				std::string type{};
				if (values.find('#') != std::string::npos && values.find("//") == std::string::npos)
				{
					// Pre processing
					std::string toChange = values.substr(1, values.find(' ') - 1);
					if (toChange == "use-beta-features")
					{
						std::string opt = values.substr(values.find(' ') + 1);
						std::istringstream(opt) >> std::boolalpha >> isExperimental;
						throwExceptionOnError = throwExceptionOnError && isExperimental;
					}
					else if (toChange == "use-language-features")
					{
						std::string opt = values.substr(values.find(' ') + 1);
						std::istringstream(opt) >> std::boolalpha >> isLanguage;
					}
					else if (toChange == "can-rick-roll")
					{
						std::string opt = values.substr(values.find(' ') + 1);
						std::istringstream(opt) >> std::boolalpha >> canRickRoll;
					}
					else 
					{ 
						lC.LogError(
							(std::ostringstream() << "CONFIG-0018 Invalid pre-processing directive! Directive is " << toChange << " Line is " << line).str(),
							config_error::INVALID_PREPROCESSING_DIRECTIVE,
							true
						);
					}
					continue;
				}
				if (values.find("//") == std::string::npos)
				{
					if (values.find("print") != std::string::npos && !isLanguage) continue; // ignore line
					remove_trailing_whitespaces(values);
					remove_trailing_tabs(values);
					if (_stricmp(values.c_str(), "BREAK") == 0)
					{
						__debugbreak();
						continue;
					}
					else if (_stricmp(values.c_str(), "TERMINATE_PARSER") == 0) 
					{
						if (!isFunction)
						{
							lC.LogError((std::ostringstream() << "CONFIG-0016 at line " << line << " TERMINATE_PARSER was called!\n").str(), config_error::TERMINATE_PARSER_WAS_CALLED, true);
							__debugbreak();
						}
						else
						{
							error_list[err_element] = config_error::TERMINATE_PARSER_WAS_CALLED;
							good = false;
						}
						return false;
					}
					else if (values == "rickroll" && canRickRoll)
					{
						std::cout <<
							"You asked for this:\n"
							<< rickrolllyrics;
						ShellExecute(0, 0, L"https://www.youtube.com/watch?v=dQw4w9WgXcQ", 0, 0, SW_MINIMIZE);
						Store var = {};
						var.str_Out = rickrolllyrics;
						var.type = types::basic_string;
						var.var_name = "rick_roll";
						data_map.try_emplace("rick_roll", var);
						data.push_back(var);
						continue;
					}
					if (values.find('(') != std::string::npos && isNotBuiltInFunction(values) && isExperimental && values.find("if") == std::string::npos && isLanguage)
					{
						Function f = {};
						f.startLine = line;
						std::string func_type = values.substr(0, values.find_first_of(' '));
						if (!isValidType(func_type)) goto runFunc;
						std::string function_name = values.substr(values.find_first_of(' '), values.find('(') - 1);
						std::string parameters = values.substr(values.find('('), std::string::npos);
						function_name.erase(function_name.find('('), std::string::npos);
						std::erase(parameters, '(');
						std::erase(parameters, ')');
						remove_trailing_whitespaces(function_name);
						remove_trailing_tabs(function_name);
						f.func_name = function_name;
						f.func_type = func_type;
					    auto pre = std::count(parameters.begin(), parameters.end(), ',');
					    auto numberOfParameters = (!parameters.empty()) ? pre + 1 : pre;
						f.numberOfParameters = numberOfParameters;
						for (int i = 0; i < numberOfParameters; i++)
						{
							std::string par;
							par = parameters.substr(0, parameters.find(',')); 
							//if(i != 0) par.erase(par.find(','), std::string::npos);
							parameters.erase(0, par.length() + 1);
							if (par.front() == 32) remove_trailing_whitespaces(par);
							std::erase(par, ',');
							const char* parCstr = par.c_str();
							f.parameters.push_back(parCstr);
						}
						std::string contents = it.str();
						std::string extracted = contents.substr(contents.find(values), std::string::npos);
						std::stringstream ss{ extracted };
						std::string action;
						int index{ 1 };
						while (std::getline(ss, action))
						{
							ignored_lines.push_back(line + index);
							index++;
							if (action.find(values) != std::string::npos)
							{
								continue;
							}
							if (action.find('{') != std::string::npos) 
							{
								continue;
							}
							if (action.find('}') != std::string::npos) 
							{
								break;
							}
							remove_trailing_tabs(action);
							remove_trailing_whitespaces(action);
							if (!action.empty())
							{
								f.actions.push_back(action.c_str());
							}
						}
						function_map.try_emplace(function_name, f);
						continue;
					}
				runFunc:
					if (values.find("return") != std::string::npos && isFunction && isLanguage)
					{
						if (scope == "main")
						{
							lC.LogError("CONFIG-0019 Return statement is not in a function!", config_error::RETURN_IS_NOT_IN_FUNCTION);
						}
						if (this_function_type == types::basic_void) break;
						std::string operand1 = values.substr(values.find_first_of('n') + 2, std::string::npos);
						types type = GetTypeFromString(operand1);
						if (type != this_function_type)
						{
							lC.LogError(std::ostringstream() << "CONFIG-0001 'return' Type " << Store::TypeToTypeName(type) << " is incompatible with type " << Store::TypeToTypeName(this_function_type), config_error::INCOMPATIBLE_TYPES);
							continue;
						}
						if (type == types::basic_boolean)
						{
							returnValue.type = types::basic_boolean;
							returnValue.boolean_Out = operand1.find("true") != std::string::npos;
						}
						else if (type == types::basic_character)
						{
							returnValue.type = types::basic_character;
							char ch = operand1[1];
							if (ch == '\\')
							{
								if		(operand1[2] == '\'') ch = '\'';
								else if (operand1[2] == '\"') ch = '\"';
								else if (operand1[2] == '?') ch = '\?';
								else if (operand1[2] == 'a') ch = '\a';
								else if (operand1[2] == 'b') ch = '\b';
								else if (operand1[2] == 'f') ch = '\f';
								else if (operand1[2] == 'n') ch = '\n';
								else if (operand1[2] == 'r') ch = '\r';
								else if (operand1[2] == 't') ch = '\t';
								else if (operand1[2] == 'v') ch = '\v';
								else if (operand1[2] == '0') ch = '\0';
							}
							returnValue.character_Out = ch;
						}
						else if (type == types::basic_int)
						{
							returnValue.type = types::basic_int;
							returnValue.int_Out = std::atoi(operand1.c_str());
						}
						else if (type == types::basic_double)
						{
							returnValue.type = types::basic_double;
							returnValue.decimal_Out = std::atof(operand1.c_str());
						}
						else if (type == types::basic_hex)
						{
							returnValue.type = types::basic_hex;
							std::istringstream(operand1) >> std::hex >> returnValue.int_Out;
						}
						else if (type == types::basic_string)
						{
							size_t count = std::ranges::count(pre_Values.begin(), pre_Values.end(), '\"');
							if (count > 2)
							{
								lC.LogError("CONFIG-0009 Too much \"s in string!", config_error::TOO_MUCH_OR_TO_LITTLE_QUOTATION_MARKS_IN_STRING_DECLARATION);
								continue;
							}
							else if (count < 2)
							{
								lC.LogError("CONFIG-0009 Too little \"s in string!", config_error::TOO_MUCH_OR_TO_LITTLE_QUOTATION_MARKS_IN_STRING_DECLARATION);
								continue;
							}
							std::string pre_str_val = operand1.substr(operand1.find_first_of('\"'));
							std::string str_Val;
							for (auto& i : pre_str_val)
							{
								if (i == '\"') break;
								else str_Val.push_back(i);
							}
						}
						break;
					}
					if (values.find('=') != std::string::npos)
					{
						bool isReAssign = false;
						std::string operand1{ values.substr(0, values.find('=') - 1) }, operand2{ values.substr(values.find('='),std::string::npos) };
						// If the first word != a type name then it is a variable declaration instead of a reassgining operation
						std::string word1 = operand1.substr(0, values.find(' '));
						if (operand1.find("array") != std::string::npos || operand1.find("if") != std::string::npos) goto nReAssign; // If it is an array or an if statment it will try to reference the array because "array" and "if([statment])" isn't a type name
						for (int i = 0; i < size; i++)
						{
							if (word1 == type_names[i]) break;
							else if (word1 != type_names[i] && i == size - 1) isReAssign = true;
						}
						if (!isReAssign) goto nReAssign;
						if (!data_map.contains(word1))
						{
							lC.LogError((std::ostringstream() << "CONFIG-0013 " << word1 << " was not declared in this scope!").str(), config_error::VAR_IS_NOT_DECLARED_IN_SCOPE);
							continue;
						}
						int i{};
						for (; i < data.size(); i++)
						{
							if (data[i].var_name == word1) break;
							else continue;
						}
						operand2.erase(0, 2);
						pre_Values = operand2;
						types type_t = data_map[operand1].type;
						if (!isNumber(pre_Values) && pre_Values == "true")
						{
							if (type_t != types::basic_boolean)
							{
								lC.LogError(
									"CONFIG-0001 Type 'string' is in-compatible with declaration of type '" + type + "' terminating parsing of current line!\n",
									config_error::INCOMPATIBLE_TYPES
								);
								continue;
							}
							data[i]			  .boolean_Out = true;
							data_map[operand1].boolean_Out = true;
						}
						else if (!isNumber(pre_Values) && pre_Values == "false")
						{
							if (type_t != types::basic_boolean)
							{
								lC.LogError(
									"CONFIG-0001 Type 'string' is in-compatible with declaration of type '" + type + "' terminating parsing of current line!\n",
									config_error::INCOMPATIBLE_TYPES
								);
								continue;
							}
							data[i]			  .boolean_Out = false;
							data_map[operand1].boolean_Out = false;
						}
						else if (isHexNumber(pre_Values))
						{
							if (type_t != types::basic_hex)
							{
								lC.LogError(
									"CONFIG-0001 Type 'hex_number' is in-compatible with declaration of type '" + type + "' terminating parsing of current line!\n",
									config_error::INCOMPATIBLE_TYPES
								);
								continue;
							}
							int64_t temp = 0;
							std::istringstream(operand2) >> std::hex >> temp;
							data[i]		    .hex_Out = temp;
							data_map[word1] .hex_Out = temp;
						}
						else if (isNumber(pre_Values))
						{
							if (type_t != types::basic_int)
							{
								lC.LogError(
									"CONFIG-0001 Type 'int' is in-compatible with declaration of type '" + type + "' terminating parsing of current line!\n",
									config_error::INCOMPATIBLE_TYPES
								);
								continue;
							}
							data[i].int_Out = std::atoi(pre_Values.c_str());
							data_map[word1].int_Out = std::atoi(pre_Values.c_str());
						}
						else if (isDecimalNumber(pre_Values))
						{
							if (type_t != types::basic_double)
							{
								lC.LogError(
									"CONFIG-0001 Type 'double' is in-compatible with declaration of type '" + type + "' terminating parsing of current line!\n",
									config_error::INCOMPATIBLE_TYPES
								);
								continue;
							}
							data[i]			  .decimal_Out = std::atof(pre_Values.c_str());
							data_map[operand1].decimal_Out = std::atof(pre_Values.c_str());
						}
						else if (!isNumber(pre_Values) && pre_Values.find('\'') != std::string::npos)
						{
							types isInt = types::basic_character;
							if (type == "int")
							{
								isInt = types::basic_int;
							}
							if (type_t != types::basic_character && isInt != types::basic_int)
							{
								lC.LogError(
									"CONFIG-0001 Type 'char' is in-compatible with declaration of type '" + type + "' terminating parsing of current line!\n",
									config_error::INCOMPATIBLE_TYPES
								);
								continue;
							}
							char ch = pre_Values[1];
							if (ch == '\\')
							{
								if (pre_Values[2] == '\'') ch = '\'';
								else if (pre_Values[2] == '\"') ch = '\"';
								else if (pre_Values[2] == '?') ch = '\?';
								else if (pre_Values[2] == 'a') ch = '\a';
								else if (pre_Values[2] == 'b') ch = '\b';
								else if (pre_Values[2] == 'f') ch = '\f';
								else if (pre_Values[2] == 'n') ch = '\n';
								else if (pre_Values[2] == 'r') ch = '\r';
								else if (pre_Values[2] == 't') ch = '\t';
								else if (pre_Values[2] == 'v') ch = '\v';
								else if (pre_Values[2] == '0') ch = '\0';
							}
							data[element].type = isInt;
							if (isInt == types::basic_character)
								data[i].character_Out = ch;
							else data[i].int_Out = ch;
							data_map[var_name].type = isInt;
							if (isInt == types::basic_character)
								data_map[operand1].character_Out = ch;
							else data_map[operand1].int_Out = ch;
						}
						else if (!isNumber(pre_Values))
						{
							if (type_t != types::basic_string)
							{
								lC.LogError(
									"CONFIG-0001 Type 'string' is in-compatible with declaration of type '" + type + "' terminating parsing of current line!\n",
									config_error::INCOMPATIBLE_TYPES
								);
								continue;
							}
							std::erase(pre_Values, '\"');
							remove_trailing_whitespaces(pre_Values);
							data[(i == data.size()) ? i - 1 : i ].type = types::basic_string;
							data_map[var_name].str_Out = pre_Values;
						}
						continue;
					}
					nReAssign:
					if (size_t nval = values.find("="); nval != std::string::npos && values.find("print") == std::string::npos && values.find("printnl") == std::string::npos)
					{
						pre_Values = values.substr(nval + 2);
						if (values.find("array") == std::string::npos)
							var_name = values.substr(values.find(' ') + 1, nval - 2);
						else
							var_name = values.substr(values.find(' ') + 4, nval - 15); 
						if (var_name.find("=") != std::string::npos)
							var_name.erase(var_name.find('='), std::string::npos);
					}
					std::erase(var_name, 32);
					map_element = var_name;
					
					if (values.find(' ') + 1 != std::string::npos && values.find("print") == std::string::npos && values.find("printnl") == std::string::npos)
					{
						if (values.find("array") == std::string::npos)
							type = values.substr(0, values.find(' '));
						else type = values.substr(0, values.find('>') + 1);
					}
					if (values.find("print") != std::string::npos) pre_Values = values;
					else if (values.find("printnl") != std::string::npos) pre_Values = values;
					if (type.find("array") != std::string::npos && type.find(',') != std::string::npos)
					{
						isArray = true;
						std::string cpy = type;
						if (type.	  find("double") != std::string::npos) type = "double";
						else if (type.find("char") != std::string::npos) type = "char";
						else if (type.find("bool") != std::string::npos) type = "bool";
						else if (type.find("int") != std::string::npos) type = "int";
						else if (type.find("hex_number") != std::string::npos) type = "hex_number";
						else if (type.find("string") != std::string::npos) type = "string";
						std::string num = "\0";
						for (auto i : cpy)
						{
							if (!isdigit(i)) continue;
							else
							{
								num.push_back(i);
							}
						}
						if (!num.empty()) arraySize = std::atoi(num.c_str());
						else
						{
							l.LoglevelError("CONFIG-0007 Array declaration is missing array size!");
							good = false;
							continue;
						}
					}
					if (values.find("print") == std::string::npos && values.find("return") == std::string::npos) // Type checking
					{
						bool eState = false;
						for (int i = 0; i < size; i++)
						{
							if (type == "void")
							{
								lC.LogError(std::ostringstream() << "CONFIG-0022 Incomplete type" << type << "is not allowed! Line : " << line, config_error::INCOMPLETE_TYPE_IS_NOT_ALLOWED);
								eState = true;
								break;
							}
							if (type == type_names[i]) { break; }
							else if (type != type_names[i] && i == size - 1 && type.find('}') == std::string::npos && type.find('(') == std::string::npos)
							{
								lC.LogError((std::stringstream() << "CONFIG-0000 Un-recognized type! Type is " << type << "\n").str(),
									config_error::TYPE_UNRECOGNIZED);
								continue;
							}
							if (var_name == type_names[i])
							{
								lC.LogError(
									(std::ostringstream() << "CONFIG-0012 Variable name cannot be a type name! Type is " << type).str(),
									config_error::VAR_NAME_IS_A_TYPE_NAME);
								eState = true;
								break;
							}
						}
						if (eState) continue;
					}
					if (!pre_Values.empty())
						;
					else
					{
						if (values.find('{') == std::string::npos && values.find('}') == std::string::npos && values.find('(') == std::string::npos && values.find("return") == std::string::npos)
						{
							lC.LogError(
								(std::stringstream() << "\nCONFIG-0004 Missing an \"=\" sign in provided std::istream& object! Line is " << line).str(),
								config_error::MISSING_EQUAL_SIGN
							);
							continue;
						}
						else if(values.find('(') == std::string::npos) continue; // in a function declaration
					}
					if (values.find("print") == std::string::npos)
					{
						size_t paren = values.find('(');
						if (paren != std::string::npos)
						{
							std::string valCpy = values;
							remove_trailing_whitespaces(values);
							remove_trailing_tabs(values);
							std::string fname = valCpy.substr(0, paren);
							if (isNotBuiltInFunction(fname)) goto func;
						}
						Store toVec = { };
						data.push_back(toVec);
						std::erase(var_name, 32);
						data_map.try_emplace(var_name, toVec);
						element = data.size() - 1;
						data[element].var_name = var_name;
						data_map[var_name].var_name = var_name;
						data[element].scope = scope;
						data_map[var_name].scope = scope;
						SetType(type);
					}
					func:
					if (values.find("if") != std::string::npos)
					{
						ignored_lines.push_back(line);
						std::string operand1 = values.substr(values.find('(') + 1);
						std::string operand2 = values.substr(values.find('(') + 1);
						std::erase(operand1, ')');
						std::erase(operand2, ')');
						size_t _Operator = values.find('=');
						size_t _Operator2 = values.rfind('=');
						if (_Operator == std::string::npos) continue;
						operand1.erase(_Operator - 3, std::string::npos);
						std::reverse(operand2.begin(), operand2.end());
						operand2.erase(4 + _Operator2 / 5, std::string::npos);
						std::reverse(operand2.begin(), operand2.end());
						if(operand2.find('=') != std::string::npos) operand2.erase(operand2.find('='), 1 + std::count(operand2.begin(), operand2.end(), '=') - 1);
						std::erase(operand1, ' ');
						std::erase(operand2, ' ');
						
						Store st;
						try
						{
							if (GetTypeFromString(operand1) != types::basic_boolean)
							{
								st = data_map.at(operand1);
							}
							else
							{
								st.type = types::basic_boolean;
								st.var_name = "temp";
								st.boolean_Out = operand1.find("true") != std::string::npos;	 
							}
						}
						catch (std::out_of_range)
						{
							lC.LogError(std::ostringstream() << "CONFIG-0010 Could not find " << operand1 << " in data map!", config_error::COULD_NOT_FIND_IN_DATA_MAP);
							continue;
						}
						std::string val = st.Get().second;
						remove_trailing_whitespaces(val);
						if (val == operand2)
						{
							std::string contents = it.str();
							std::string extracted = contents.substr(contents.find(values));
							std::stringstream ss{ extracted };
							std::stringstream toRun;
							std::string action;
							int index{ 1 };
							while (getline(ss, action))
							{
								ignored_lines.push_back(line + index);
								index++;
								if (action.find(values) != std::string::npos) continue;
								else if (action.find('{') != std::string::npos) { continue; }
								else if (action.find('}') != std::string::npos) break;
								toRun.str(toRun.str() + action + "\n");
 							}
							Parse(toRun, true);
						}
						continue;
					}
					if (!pre_Values.empty() && pre_Values.front() == 32)
					{
						pre_Values.erase(0, 1);
					}
					pre_Values.append("\0");
					if (!isArray && type == "string" && pre_Values.find("toString") == std::string::npos)
					{
						size_t count = std::ranges::count(pre_Values.begin(), pre_Values.end(), '\"');
						if (count > 2)
						{
							lC.LogError("CONFIG-0009 Too much \"s in string!", config_error::TOO_MUCH_OR_TO_LITTLE_QUOTATION_MARKS_IN_STRING_DECLARATION);
							continue;
						}
						else if (count < 2)
						{
							lC.LogError("CONFIG-0009 Too little \"s in string!", config_error::TOO_MUCH_OR_TO_LITTLE_QUOTATION_MARKS_IN_STRING_DECLARATION);
							continue;

						}
					}
					if (isArray && type == "char" && pre_Values.find('\"') != std::string::npos)
					{
						l.LoglevelWarn("CONFIG-0015 Do not use a char array as a string! Use the string type instead");
						size_t count = std::ranges::count(pre_Values.begin(), pre_Values.end(), '\"');
						if (count > 2)
						{
							lC.LogError("CONFIG-0009 Too much \"s in string!", config_error::TOO_MUCH_OR_TO_LITTLE_QUOTATION_MARKS_IN_STRING_DECLARATION);
							continue;
						}
						else if (count < 2)
						{
							lC.LogError("CONFIG-0009 Too little \"s in string!", config_error::TOO_MUCH_OR_TO_LITTLE_QUOTATION_MARKS_IN_STRING_DECLARATION);
							continue;
						}
						std::erase(pre_Values, '\"');
						remove_trailing_whitespaces(pre_Values);
						if (pre_Values.length() < arraySize)
						{
							lC.LogError((std::ostringstream() << "CONFIG-0011 Array is too big! Array Size is " << arraySize).str(), config_error::ARRAY_IS_TOO_BIG_OR_TOO_SMALL_TO_HOLD_STRING);
							continue;
						}
						else if (pre_Values.length() > arraySize)
						{
							lC.LogError("CONFIG-0011 Array is to small to hold string", config_error::ARRAY_IS_TOO_BIG_OR_TOO_SMALL_TO_HOLD_STRING);
							continue;
						}
						isArray = true;
						data	[element] .is_Array = isArray;
						data_map[var_name].is_Array = isArray;
						for (auto i : pre_Values)
						{
							data[element].character_array_Out.push_back(i);
							data_map[var_name].character_array_Out.push_back(i);
						}
						continue;
					}
					if (isArray)
					{
						// Array definition - "array<[type], [size]> array = { [values] }"
						size_t begin = pre_Values.find('{');
						size_t end = pre_Values.find('}');
						if (begin == std::string::npos)
						{
							lC.LogError("CONFIG-0003 Missing an '{' for an array declaration! Terminating parsing of current line!\n", config_error::MISSING_BRACKET_IN_ARRAY_DECLARATION);
							continue;
						}
						if (end == std::string::npos)
						{
							lC.LogError("CONFIG-0003 Missing an '}' for an array declaration! Terminating parsing of current line!\n", config_error::MISSING_BRACKET_IN_ARRAY_DECLARATION);
							continue;
						}
						data[element].array_Size = arraySize;
						data_map[var_name].array_Size = arraySize;
						data[element].is_Array = isArray;
						data_map[var_name].is_Array = isArray;
						const ::config::types& type_ref = data[element].type;
						std::string arr_Values = pre_Values.substr(begin + 1, pre_Values.length() - 2);
						// Credits : https://www.techiedelight.com/count-occurrences-char-string-cpp/
						// ---------------------------
						{
							char ch = ',';
							auto count = std::ranges::count(arr_Values.begin(), arr_Values.end(), ch) + 1;
							// No more credit
							if (count > arraySize)
							{
								lC.LogError(
									"CONFIG-0006 Number of declarations is greater then array size! To fix this error make sure to have the correct array size",
									config_error::TO_LITTLE_OR_TOO_MUCH_DECLARATIONS_IN_ARRAY
								);
								continue;
							}
							else if (count < arraySize)
							{
								lC.LogError(
									"CONFIG-0006 Number of declarations is less then array size! To fix this error make sure to have the correct array size",
									config_error::TO_LITTLE_OR_TOO_MUCH_DECLARATIONS_IN_ARRAY
								);
								continue;
							}
						}
						bool first = true;
						for (int i = 0; i < arraySize; i++)
						{
							// Reads array values
							if (type_ref == types::basic_int)
							{
								if (pre_Values.find(",") == std::string::npos)
								{
									lC.LogError("CONFIG-0005 Cannot find an ',' in array declaration!", config_error::MISSING_COMMA_IN_ARRAY_DECLARATION);
									FreeArrayByType("int");
									break;
								}
								std::string num;
								for (auto i2 : arr_Values)
								{
									if (i2 == ',') break;
									if (!isdigit(i2)) continue;
									else
										num.push_back(i2);
								}
								std::string cpy = num;
								arr_Values.erase((size_t)arr_Values.find(cpy), (size_t)num.length() + 1);
								if (isNumber(num))
								{
									data[element].int_array_Out.push_back(std::atoi(num.c_str()));
									data_map[var_name].int_array_Out.push_back(std::atoi(num.c_str()));
								}
								else
								{
									lC.LogError(
										(std::stringstream() << "CONFIG-0008 " << num << " is not a integer!").str(),
										config_error::MIS_MATCHED_TYPES_IN_ARRAY);
									FreeArrayByType("int");
									break;
								}
							}
							else if (type_ref == types::basic_double)
							{
								if (pre_Values.find(",") == std::string::npos)
								{
									lC.LogError("CONFIG-0005 Cannot find an ',' in array declaration!", config_error::MISSING_COMMA_IN_ARRAY_DECLARATION);
									FreeArrayByType("double");
									break;
								}
								std::string num;
								for (auto i2 : arr_Values)
								{
									if (i2 == ',') break;
									if (!isdigit(i2) && i2 != '.') continue;
									else
										num.push_back(i2);
								}
								arr_Values.erase((size_t)arr_Values.find(num), (size_t)num.length() + 1);
								if (isDecimalNumber(num))
								{
									data[element].decimal_array_Out.push_back(std::atof(num.c_str()));
									data_map[var_name].decimal_array_Out.push_back(std::atof(num.c_str()));
								}
								else
								{
									lC.LogError(
										(std::stringstream() << "CONFIG-0008 " << num << " is not a decimal number!").str(),
										config_error::MIS_MATCHED_TYPES_IN_ARRAY);
									FreeArrayByType("double");
									break;
								}
							}
							else if (type_ref == types::basic_character)
							{
								if (pre_Values.find(",") == std::string::npos)
								{
									lC.LogError("CONFIG-0005 Cannot find an ',' in array declaration!", config_error::MISSING_COMMA_IN_ARRAY_DECLARATION);
									FreeArrayByType("char");
									break;
								}
								std::string ch_Str;
								ch_Str = arr_Values.substr(arr_Values.find('\''), (!(arr_Values.find('\\') != std::string::npos)) ? 2 : 3);
								char ch = ch_Str[1];
								if (ch == '\\')
								{
									if		(ch_Str[2] == '\'') ch = '\'';
									else if (ch_Str[2] == '\"') ch = '\"';
									else if (ch_Str[2] == '?') ch = '\?';
									else if (ch_Str[2] == 'a') ch = '\a';
									else if (ch_Str[2] == 'b') ch = '\b';
									else if (ch_Str[2] == 'f') ch = '\f';
									else if (ch_Str[2] == 'n') ch = '\n';
									else if (ch_Str[2] == 'r') ch = '\r';
									else if (ch_Str[2] == 't') ch = '\t';
									else if (ch_Str[2] == 'v') ch = '\v';
									else if (ch_Str[2] == '0') ch = '\0';
								}
								arr_Values.erase(arr_Values.find(ch_Str), ch_Str.length() + 1);
								data[element].character_array_Out.push_back(ch);
								data_map[var_name].character_array_Out.push_back(ch);
							}
							else if (type_ref == types::basic_boolean)
							{
								if (pre_Values.find(",") == std::string::npos)
								{
									lC.LogError("CONFIG-0005 Cannot find an ',' in array declaration!", config_error::MISSING_COMMA_IN_ARRAY_DECLARATION);
									FreeArrayByType("bool");
									break;
								}
								bool val = false;
								std::string val_str = "";
								int first_element = 0;
								int numCh = 0;
								if (arr_Values.find("true") < arr_Values.find("false"))
								{
									first_element = arr_Values.find("true");
									val = true;
									numCh = 4;
								}
								else
								{
									val = false;
									first_element = arr_Values.find("false");
									numCh = 5;
								}
								val_str = arr_Values.substr(first_element, numCh);
								arr_Values.erase(arr_Values.find(val_str), val_str.length() + 1);
								data[element].boolean_array_Out.push_back(val);
								data_map[var_name].boolean_array_Out.push_back(val);
							}
							else if (type_ref == types::basic_hex)
							{
								if(pre_Values.find(",") == std::string::npos)
								{
									lC.LogError("CONFIG-0005 Cannot find an ',' in array declaration!", config_error::MISSING_COMMA_IN_ARRAY_DECLARATION);
									FreeArrayByType("hex_number");
									break;
								}
								std::string num;
								for (auto i2 : arr_Values)
								{
									if (i2 == ',') break;
									if (!isdigit(i2) && i2 != 'x' && i2 != 'X' && 
										i2 != 'a' && i2 != 'A' && 
										i2 != 'b' && i2 != 'B' &&
										i2 != 'c' && i2 != 'C' &&
										i2 != 'd' && i2 != 'D' &&
										i2 != 'e' && i2 != 'E' &&
										i2 != 'f' && i2 != 'F' ) continue;
									else
										num.push_back(i2);
								}
								int64_t temp = 0;
								std::istringstream iss{ num };
								iss >> std::hex >> temp;
								arr_Values.erase((size_t)arr_Values.find(num), (size_t)num.length() + 1);
								std::ranges::replace(num.begin(), num.end(), 'X', 'x');
								if (isHexNumber(num))
								{
									data[element].hex_array_Out.push_back(temp);
									data_map[var_name].hex_array_Out.push_back(temp);
								}
								else
								{
									lC.LogError(
										(std::stringstream() << "CONFIG-0008 " << num << " is not a hexadecimal number!").str(),
										config_error::MIS_MATCHED_TYPES_IN_ARRAY);
									FreeArrayByType("hex_number");
									break;
								}
							}
							else if (type_ref == types::basic_string)
							{
								std::string str{};
								size_t _Begin = arr_Values.find('\"');
								arr_Values.erase(_Begin, 1);
								for (auto i2 = (int)_Begin; i2 < arr_Values.length(); i2++)
								{
									char _Ch = arr_Values[i2];
									if (_Ch == '\"') break;
									else str.push_back(_Ch);
								}
								arr_Values.erase(arr_Values.find(str), str.length() + 1);
								data[element].string_array_Out.push_back(str);
								data_map[var_name].string_array_Out.push_back(str);
							}
							(first == true) ? first = false : 0;
						}
						continue;
					}
					else if (values.find('(') != std::string::npos && function_map.contains(values.substr(0, values.find('('))) && isExperimental && isLanguage)
					{
						std::string function_name = values.substr(0, values.find('('));
						std::string par = values.substr(values.find('(') + 1, values.find(')') - 1);
						std::erase(par, ')');
						std::vector<std::string> pars;
						Function func;
						try
						{
							func = function_map.at(function_name);
						} 
						catch(std::out_of_range) 
						{
							lC.LogError("CONFIG-0010 Could not find function in function map", config_error::COULD_NOT_FIND_IN_DATA_MAP);
							continue;
						}
						bool errorState = false;
						auto count = std::count(par.begin(), par.end(), ',') + 1;
						if (count > func.numberOfParameters && func.numberOfParameters != 0)
						{
							lC.LogError("CONFIG-0020 Too many parameters in function call!", config_error::TOO_MANY_OR_TO_FEW_PARAMETERS_IN_FUNCTION_CALL, false);
							errorState = true;
							continue;
						}
						else if (count < func.numberOfParameters)
						{
							lC.LogError("CONFIG-0020 Too few parameters in function call!", config_error::TOO_MANY_OR_TO_FEW_PARAMETERS_IN_FUNCTION_CALL, false);
							errorState = true;
							continue;
						}
						for (int i = 0; i < func.numberOfParameters; i++)
						{
							if (func.numberOfParameters == 1)
							{
								pars.push_back(par);
							}
							else
							{
								std::string thisPar = par.substr(i != func.numberOfParameters - 1 ? i : i - 1, par.find(','));
								par.erase(par.find(thisPar), thisPar.length() + 1);
								pars.push_back(thisPar);
							}
						}
						if (errorState) continue;
						bool willTerminate{};
						bool errState{};
						Store st{};
						RunFunction(func, pars, willTerminate, errState, st);
						if (willTerminate)
						{
							int lnTerm = 1;
							for (auto& i : func.actions)
							{
								lnTerm++;
								const char* ln = i.c_str();
								if (_stricmp(ln, "TERMINATE_PARSER") == 0) break;
								else continue;
							}
							lC.LogError(std::ostringstream() << "CONFIG-0016 TERMINATE_PARSER was called at line " << lnTerm + func.startLine << " in function " << func.func_name << " called from function " << scope << '\n', config_error::TERMINATE_PARSER_WAS_CALLED, true);
							__debugbreak();
							return false;
						}
						if (errState) continue;
						continue;
					}
					else if (!isNumber(pre_Values) && pre_Values.find("print") != std::string::npos && pre_Values != "printnl" && pre_Values != "println" && isLanguage )
					{
						std::string vname =
							pre_Values.substr(
							pre_Values.find('(')/* + 1*/,
							pre_Values.find(')')/* - 1*/
							);
						vname.erase(vname.find('('), 1);
						vname.erase(vname.find(')'), 1);
						if (vname.find('"') == std::string::npos)
						{
							Store i{};
							if(data_map.contains(vname))
								i = data_map.at(vname);
							else
							{
								lC.LogError("CONFIG-0010 Could not find " + vname + " in data map!", config_error::COULD_NOT_FIND_IN_DATA_MAP);
								good = false;
								continue;
							}
							std::string str = "";
							std::stringstream iss{};
							iss << i;
							str = iss.str();
							std::ranges::reverse(str.begin(), str.end());
							str.erase(str.find('='), std::string::npos);
							std::ranges::reverse(str.begin(), str.end());
							remove_trailing_whitespaces(str);
							if (str == "\\n") str = "\n";
							printf_s(str.c_str());
						}
						else
						{
							// Easter egg that's also kinda a troll
							if (vname == "\"Your mom\"") vname = "\"Your mom is fat\"";
							printf_s("%s", vname.substr(vname.find('\"') + 1, vname.rfind('\"') - 1).c_str());
						}
					}
					else if (!isNumber(pre_Values) && (pre_Values == "printnl" || pre_Values == "println") && isLanguage) printf_s("\n");
					else if (!isNumber(pre_Values) && pre_Values.find("toString") != std::string::npos)
					{
					std::string num = pre_Values.substr(pre_Values.find('(') + 1, pre_Values.length());
					std::erase(num, ')');
					if (!data_map.contains(num))
					{
						remove_trailing_tabs(num);
						remove_trailing_whitespaces(num);
						data[element].type = types::basic_string;
						data_map.at(var_name).type = types::basic_string;
						data[element].str_Out = num;
						data_map.at(var_name).str_Out = num;
						data[element].didCallToString = true;
						data_map.at(var_name).didCallToString = true;
					}
					else
					{
						Store& s = data_map.at(num);
						auto [t, d] = s.Get(); // Gets value as a string
						remove_trailing_tabs(d);
						remove_trailing_whitespaces(d);
						data[element].str_Out = d;
						data_map.at(var_name).str_Out = d;
						data[element].didCallToString = true;
						data_map.at(var_name).didCallToString = true;
					}
					}
					else if (!isNumber(pre_Values) && pre_Values.find("intToHex") != std::string::npos)
					{
					if (type != "hex_number") {
						lC.LogError(
							"CONFIG-0001 Type 'hex_number' is in-compatible with declaration of type '" + type + "' terminating parsing of current line! Hint : try changing the type to hex_number\n",
							config_error::INCOMPATIBLE_TYPES
						);
						continue;
					}
					std::string strNum =
						pre_Values.substr(
							static_cast<int64_t>(pre_Values.find('(')) + 1,
							static_cast<int64_t>(pre_Values.find(')')));
					if (strNum[strNum.length() - 1] == ')') strNum.erase(strNum.length() - 1, 1);
					auto number = static_cast<int64_t>(std::atoi(strNum.c_str()));

					data[element].type = types::basic_hex;
					data[element].hex_Out = number;
					data_map[var_name].type = types::basic_hex;
					data_map[var_name].hex_Out = number;

					}
					else if (!isNumber(pre_Values) && pre_Values.find("hexToInt") != std::string::npos)
					{
						if(type != "int") 
						{ 
							lC.LogError(
								"CONFIG-0001 Type 'int' is in-compatible with declaration of type '" + type + "' terminating parsing of current line! Hint : try changing the type to int\n",
								config_error::INCOMPATIBLE_TYPES
							);
							continue;
						}
						int temp = 0;
						std::string number = 
							pre_Values.substr(
								static_cast<int64_t>(pre_Values.find('(')) + 1,
								static_cast<int64_t>(pre_Values.find(')')) - 1);
						number.erase(number.find(')'));
						std::istringstream iss{ number };
						iss >> std::hex >> temp;
						data[element].type = types::basic_int;
						data[element].int_Out = temp;
						data_map[var_name].type = types::basic_int;
						data_map[var_name].int_Out = temp;

					}
					else if (!isNumber(pre_Values) && pre_Values == "true")
					{
						if (type != "bool")
						{ 
							lC.LogError(
								"CONFIG-0001 Type 'bool' is in-compatible with declaration of type '" + type + "' terminating parsing of current line!\n",
								config_error::INCOMPATIBLE_TYPES
							); 
							continue;
						}
						data[element].type = types::basic_boolean;
						data[element].boolean_Out = true;
						data_map[var_name].type = types::basic_boolean;
						data_map[var_name].boolean_Out = true;
					}
					else if (!isNumber(pre_Values) && pre_Values == "false")
					{
						if (type != "bool")
						{
							lC.LogError(
								"CONFIG-0001 Type 'bool' is in-compatible with declaration of type '" + type + "' terminating parsing of current line!\n",
								config_error::INCOMPATIBLE_TYPES
							);
							continue;
						}
						data[element].type = types::basic_boolean;
						data[element].boolean_Out = false;
						data_map[var_name].type = types::basic_boolean;
						data_map[var_name].boolean_Out = false;
					}
					else if (isHexNumber(pre_Values))
					{
						if (type != "hex_number")
						{
							lC.LogError(
								"CONFIG-0001 Type 'hex_number' is in-compatible with declaration of type '" + type + "' terminating parsing of current line!\n",
								config_error::INCOMPATIBLE_TYPES
							);
							continue;
						}
						auto [err, cnfg_err, res] = DoMath(pre_Values);
						if (err == basic_math_error::NOTHING_TO_DO && cnfg_err == config_error::OK)
						{
							int64_t temp = 0;
							std::istringstream iss{ pre_Values };
							iss >> std::hex >> temp;
							data[element].type = types::basic_hex;
							data[element].hex_Out = temp;
							data_map[var_name].type = types::basic_hex;
							data_map[var_name].hex_Out = temp;
						}
						else if (err != basic_math_error::OK)
						{
							data[element].type = types::basic_hex;
							data[element].hex_Out = res;
							data_map[var_name].type = types::basic_hex;
							data_map[var_name].hex_Out = res;
						}
						else if (err == basic_math_error::ERR)
						{
							continue;
						}
					}
					else if (isNumber(pre_Values))
					{
						if (type != "int")
						{
							lC.LogError(
								"CONFIG-0001 Type 'int' is in-compatible with declaration of type '" + type + "' terminating parsing of current line!\n",
								config_error::INCOMPATIBLE_TYPES
							);
							continue;
						}
						auto [err, cnfg_err, res] = DoMath(pre_Values);
						if (err == basic_math_error::NOTHING_TO_DO && cnfg_err == config_error::OK)
						{
							data[element].type = types::basic_int;
							data[element].int_Out = std::atoi(pre_Values.c_str());
							data_map[var_name].type = types::basic_int;
							data_map[var_name].int_Out = std::atoi(pre_Values.c_str());
						}
						else if (err == basic_math_error::OK)
						{
							data[element].type = types::basic_int;
							data[element].int_Out = res;
							data_map[var_name].type = types::basic_int;
							data_map[var_name].int_Out = res;
						}
						else if (err == basic_math_error::ERR)
						{
							continue;
						}
					}
					else if (isDecimalNumber(pre_Values))
					{
						if (type == "int") 
						{ 
							lC.LogError(
								"CONFIG-0002 Cannot convert from 'double' to 'int' Loss of data Line is " + std::to_string(line),
								config_error::DOUBLE_INT_CONVERSION
							);
							continue;
						}
						if (type != "double")
						{
							lC.LogError(
								"CONFIG-0001 Type 'double' is in-compatible with declaration of type '" + type + "' terminating parsing of current line!\n",
								config_error::INCOMPATIBLE_TYPES
							);
							continue;
						}
						auto [err, cnfg_err, res] = DoMath(pre_Values);
						if (err == basic_math_error::NOTHING_TO_DO && cnfg_err == config_error::OK)
						{
							data[element].type = types::basic_double;
							data[element].decimal_Out = std::stod(pre_Values.c_str());
							data_map[var_name].type = types::basic_double;
							data_map[var_name].decimal_Out = std::stod(pre_Values.c_str());
						}
						else if (err == basic_math_error::OK)
						{
							data[element].type = types::basic_double;
							data[element].decimal_Out = res;
							data_map[var_name].type = types::basic_double;
							data_map[var_name].decimal_Out = res;
						}
						else if (err == basic_math_error::ERR)
						{
							continue;
						}
						
					}
					else if (!isNumber(pre_Values) && pre_Values.find('\'') != std::string::npos)
					{
						types isInt = types::basic_character;
						if (type == "int")
						{
							isInt = types::basic_int;
						}
						if (type != "char")
						{
							lC.LogError(
								"CONFIG-0001 Type 'char' is in-compatible with declaration of type '" + type + "' terminating parsing of current line!\n",
								config_error::INCOMPATIBLE_TYPES
							);
							continue;
						}
						char ch = pre_Values[1];
						if (ch == '\\')
						{
							if (pre_Values[2] == '\'') ch = '\'';
							else if (pre_Values[2] == '\"') ch = '\"';
							else if (pre_Values[2] == '?') ch = '\?';
							else if (pre_Values[2] == 'a') ch = '\a';
							else if (pre_Values[2] == 'b') ch = '\b';
							else if (pre_Values[2] == 'f') ch = '\f';
							else if (pre_Values[2] == 'n') ch = '\n';
							else if (pre_Values[2] == 'r') ch = '\r';
							else if (pre_Values[2] == 't') ch = '\t';
							else if (pre_Values[2] == 'v') ch = '\v';
							else if (pre_Values[2] == '0') ch = '\0';
						}
						data[element].type = isInt;
						if (isInt == types::basic_character)
							data[element].character_Out = ch;
						else data[element].int_Out = ch;
						data_map[var_name].type = isInt;
						if (isInt == types::basic_character)
							data_map[var_name].character_Out = ch;
						else data_map[var_name].int_Out = ch;
					}
					else if (!isNumber(pre_Values))
					{
						if (type != "string")
						{
							lC.LogError(
								"CONFIG-0001 Type 'string' is in-compatible with declaration of type '" + type + "' terminating parsing of current line!\n",
								config_error::INCOMPATIBLE_TYPES
							);
							continue;
						}
						std::erase(pre_Values, '\"');
						remove_trailing_whitespaces(pre_Values);
						data[element].type = types::basic_string;
						data[element].str_Out = pre_Values;
						data_map[var_name].type = types::basic_string;
						data_map[var_name].str_Out = pre_Values;
					}
					else
					{
						data_map[var_name].type = types::invalid_type;
						data[element].type = types::invalid_type;
						l.LoglevelWarn("Config file parser warning! Could not find any strings, booleans or numbers after the \"=\" sign");
						good = false;
					}
				}
				else
				{
					// Nothing to do if there is a commented line
					error_list[err_element] = config_error::COMMENTED_LINE;
				}
				err_element++;
			}
			return good;
		}
	public:
		const std::vector<config_error>& GetErrorList() const { return error_list; }
		const config_error& GetLastError() { return error_list.at(err_element); }
		const std::vector<Store>& GetDataVector() const { return data; }
		const std::unordered_map<std::string, Store>& GetDataMap() const { return data_map; }
		Config() = delete;
		Config(std::istream& inStream, const std::string& scope = "main", types function_type = types::basic_int)
			:isFunction{ scope != "main" }, throwExceptionOnError(false)
		{
			//ASSERT(inStream.rdbuf() == std::cin.rdbuf(), "\a[FATAL_ERROR] CONFIG-0021 inStream.rdbuf() == std::cin.rdbuf()",
			//error_list.push_back(config_error::ISTREAM_OBJECT_IS_CIN); 
			//good = false;
			//return;);
			isStdCin = inStream.rdbuf() == std::cin.rdbuf();
			Parse(inStream, false, scope, function_type);
		}
		_Must_inspect_result_ [[nodiscard]] bool ReloadConfigFile(std::istream& is, bool throwExceptionOnError, const std::string& scope = "main", types function_type = types::basic_int)
		{
			/*ASSERT(is.rdbuf() == std::cin.rdbuf(), "\a[FATAL_ERROR] CONFIG-0021 is.rdbuf() == std::cin.rdbuf()",
				error_list.push_back(config_error::ISTREAM_OBJECT_IS_CIN);
			good = false;
			return false;);*/
			isStdCin = is.rdbuf() == std::cin.rdbuf();
			// Removed erasing the error vector because it will automatically do it in Parse()
			err_element	= 0;
			element		= 0;
			line		= 0;
			for (auto& i : data)
			{
				i.Clear();
			}
			data.clear();
			data_map.clear();
			return Parse(is, false, "main", types::basic_int);
		}
		Config(const Config&) = delete;
		Config& operator=(const Config&) = delete;
		Config(Config&&) = delete;
		Config& operator=(Config&&) = delete;
		friend class LogCover;
	};
	inline void LogCover::LogError(std::string msg_str, config_error err, bool isFatal)
	{
		std::string msg = msg_str;
		if (msg.find("Line") == std::string::npos && !m_IsFunction) msg.append(std::string(" Line : ") + std::to_string(*this->m_Line));
		if(msg[msg.length() - 1] != '\n') msg.push_back('\n');
		if (!isFatal)
			m_Log.LoglevelError(msg);
		else m_Log.LoglevelFatalError(msg);
		m_Other->error_list[*m_Ele] = err;
		m_Other->good = false;
		if (m_ThrowExceptionOnError)
		{
			config_exception cExc = { (const char*)calloc(msg.length() + 2, sizeof(char)), err, *m_Line, true };
			memcpy((void*)cExc.what(), msg.c_str(), msg.length() + 2);
			throw cExc;
		}
		// Continue or return if fatal Statement has to be in the if statement along with this function
	}
	inline void LogCover::LogError(const std::stringstream& msg_sstream, config_error err, bool isFatal)
	{
		std::string msg = msg_sstream.str();
		LogError(msg, err, isFatal);
		// Continue or return if fatal Statement has to be in the if statement along with this function
	}
	inline void LogCover::LogError(const std::ostringstream& msg_osstream, config_error err, bool isFatal)
	{
		std::string msg = msg_osstream.str();			
		LogError(msg, err, isFatal);
		// Continue or return if fatal Statement has to be in the if statement along with this function
	}
}