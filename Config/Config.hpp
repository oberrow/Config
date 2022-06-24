#pragma once
#include <string>
#include <fstream>
#include <vector>
#include <filesystem>
#include <unordered_map>
#include <functional>
#include "log.hpp"
#include <cstring>
#include <array>

namespace config
{
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
		// CONFIG-0018
		INVALID_PREPROCESSING_DIRECTIVE = 0x12
	};
	std::ostream& operator<<(std::ostream& os, config_error _Other)
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
		return os;
	}
	class LogCover
	{
	private:
		Log m_Log{ Log::levelInfoId };
		Config* m_Other = nullptr;
		int* m_Ele = nullptr;
	public:
		void LogError(const std::string& msg, config_error err, bool isFatal = false);
		LogCover(Log l, Config* _Other, int* ele) : m_Log{ l }, m_Other{ _Other }, m_Ele{ ele } {}
	};
	enum class types
	{
		// An invalid type
		invalid_type = -1,
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
		basic_string = sizeof(std::string)
	};
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
			val.erase('=', std::string::npos);
			std::ranges::reverse(val.begin(), val.end());
			return std::make_pair(this->type, val);
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
		size_t array_Size = -1;
		bool is_Array = false;
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
	std::ostream& operator<<(std::ostream& os, const Store& s)
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
		std::vector<const char*> actions;
		std::vector<const char*> parameters;
		std::string func_type;
		std::string func_name;
	};
	class Config
	{
	private:
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
		static bool isdigit(char _ch)
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
		bool isNumber(const std::string& s) const
		{
			// Replaced so that there are no false positives
			// return std::atoi(s.c_str()) > 0);
			for (auto i : s)
			{
				// If the number isn't a valid digit or '-' then return false
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
		std::unordered_map<std::string, Store> data_map;
		std::unordered_map<std::string, Function> function_map;
		const static inline int size = 7;
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
			basic_math_error ec = basic_math_error::OK;
			config_error cnfg_err = config_error::OK;
			double result;
		};
		types MathGetTypeFromString(std::string str) const
		{
			if (isNumber(str)) return types::basic_int;
			else if (isHexNumber(str)) return types::basic_hex;
			else if (isDecimalNumber(str)) return types::basic_double;
			else if (!isNumber(str) && str.contains('\'')) return types::basic_character;
			else if (isNumber(str)) return types::basic_string;
			else return types::invalid_type;
		}
		/* Returns the math error status, the result and the config error status 
	       Since the type of the result can be a double it will use a double as it is the biggest integer type along with int64_t
		*/
		math_error DoMath(std::string& values)
		{
			math_error m_err = { basic_math_error::OK, config_error::OK, 0.00 };
			Log l{ Log::levelInfoId };
			LogCover lC{ l, this, &err_element };
			std::string var_one = values.substr(0, values.find(' ') - 1); // First operand
			if (!data_map.contains(var_one))
			{
				types type = MathGetTypeFromString(values);
				if (type == types::basic_string)
				{
					lC.LogError("CONFIG-0014 Type is a string!", config_error::TYPE_IS_A_STRING);
					return {
						basic_math_error::ERR,
						config_error::TYPE_IS_A_STRING,
						0.00
					};
				}
				if (values.contains('+'))
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
				else if (values.contains('-'))
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
				else if (values.contains('/'))
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
				else if (values.contains('*'))
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
				else if (values.contains('%'))
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
			else return { basic_math_error::NOTHING_TO_DO, config_error::OK, 0.00 };
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
			return val.contains("print") || val.contains("hexToInt") || val.contains("intToHex") || val.contains("toString");
		}
		constexpr bool isNotBuiltInFunction(std::string const& val) const
		{
			return !val.contains("print") && !val.contains("hexToInt") && !val.contains("intToHex") && !val.contains("toString");
		}
		bool isExperimental{};
		std::vector<int> function_lines; // Lines that contain function code that should be ignored
		bool Parse(std::istream const& is, std::string const& scope = "main")
		{
			error_list.erase(error_list.begin(), error_list.end());
			function_lines.erase(function_lines.begin(), function_lines.end());
			Log l{ Log::levelInfoId };
			LogCover lC{ l, this, &err_element };
			std::string values;
			int line = 0;
			std::stringstream it;
			it << is.rdbuf();
			while (std::getline(it, values))
			{
				line++;
				bool leave = false;
				if(isExperimental)
					for (const auto& i : function_lines)
					{
						if (line == i)
						{
							leave = true;
							break;
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
				if (values.contains('#') && !values.contains("//"))
				{
					// Pre processing
					if (std::string toChange = values.substr(1, values.find(' ') - 1); toChange == "use-beta-features")
					{
						std::string opt = values.substr(values.find(' ') + 1);
						std::istringstream(opt) >> std::boolalpha >> isExperimental;
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
				if (!values.contains("//"))
				{
					remove_trailing_whitespaces(values);
					remove_trailing_tabs(values);
					if (_stricmp(values.c_str(), "BREAK") == 0)
					{
						__debugbreak();
						continue;
					}
					else if (_stricmp(values.c_str(), "TERMINATE_PARSER") == 0) 
					{
						lC.LogError((std::ostringstream() << "CONFIG-0016 at line " << line << " TERMINATE_PARSER was called!\n").str(), config_error::TERMINATE_PARSER_WAS_CALLED, true);
						__debugbreak();
						return false;
					}
					if (values.contains('(') && isNotBuiltInFunction(values) && isExperimental)
					{
						Function f = {};
						std::string func_type = values.substr(0, values.find_first_of(' '));
						if (!isValidType(func_type)) continue;
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
						for (int i = 0; i < numberOfParameters; i++)
						{
							std::string par = parameters.substr(0, parameters.find(','));
							parameters.erase(0, par.length());
							std::erase(par, ',');
							f.parameters.push_back(par.c_str());
						}
						std::string contents = it.str();
						std::string extracted = contents.substr(contents.find(values), std::string::npos);
						std::stringstream ss{ extracted };
						std::string action;
						int index{ 1 };
						while (std::getline(ss, action))
						{
							function_lines.push_back(line + index);
							index++;
							if (action.contains(values)) 
							{
								continue;
							}
							if (action.contains('{')) 
							{
								continue;
							}
							if (action.contains('}')) 
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
					if (values.contains('='))
					{
						bool isReAssign = false;
						std::string operand1{ values.substr(0, values.find('=') - 1) }, operand2{ values.substr(values.find('='),std::string::npos) };
						// If the first word != a type name then it is a variable declaration instead of a reassgining operation
						std::string word1 = operand1.substr(0, values.find(' '));
						if (operand1.contains("array")) goto nReAssign; // If it is an array it will try to reference the array because "array" isn't a type name
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
						else if (!isNumber(pre_Values) && pre_Values.contains('\''))
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
							data[i].type = types::basic_string;
							data_map[var_name].str_Out = pre_Values;
						}
						continue;
					}
					nReAssign:
					if (size_t nval = values.find("="); nval != std::string::npos && !values.contains("print") && !values.contains("printnl"))
					{
						pre_Values = values.substr(nval + 2);
						if (!values.contains("array"))
							var_name = values.substr(values.find(' ') + 1, nval - 2);
						else
							var_name = values.substr(values.find(' ') + 4, nval - 15); 
						if (var_name.contains("="))
							var_name.erase(var_name.find('='), std::string::npos);
					}
					std::erase(var_name, 32);
					map_element = var_name;
					
					if (values.find(' ') + 1 != std::string::npos && !values.contains("print") && !values.contains("printnl"))
					{
						if (!values.contains("array"))
							type = values.substr(0, values.find(' '));
						else type = values.substr(0, values.find('>') + 1);
					}
					if (values.contains("print")) pre_Values = values;
					else if (values.contains("printnl")) pre_Values = values;
					if (type.contains("array") && type.contains(','))
					{
						isArray = true;
						std::string cpy = type;
						if (type.contains("double")) type = "double";
						else if (type.contains("char")) type = "char";
						else if (type.contains("bool")) type = "bool";
						else if (type.contains("int")) type = "int";
						else if (type.contains("hex_number")) type = "hex_number";
						else if (type.contains("string")) type = "string";
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
					if (!values.contains("print"))
					{
						for (int i = 0; i < size; i++)
						{
							if (type == type_names[i]) { break; }
							else if (type != type_names[i] && i == size - 1 && !type.contains('}'))
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
								continue;
							}
						}
					}
					if (!pre_Values.empty())
						;
					else
					{
						if (!values.contains('{') && !values.contains('}'))
						{
							lC.LogError(
								(std::stringstream() << "\nCONFIG-0004 Missing an \"=\" sign in provided std::istream& object! Line is " << line).str(),
								config_error::MISSING_EQUAL_SIGN
							);
							continue;
						}
						else continue; // in a function declaration
					}
					if (!values.contains("print"))
					{
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
					if (!pre_Values.empty() && pre_Values.front() == 32)
					{
						pre_Values.erase(0, 1);
					}
					pre_Values.append("\0");
					if (!isArray && type == "string" && !pre_Values.contains("toString"))
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
					if (isArray && type == "char" && pre_Values.contains('\"'))
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
								ch_Str = arr_Values.substr(arr_Values.find('\''), (!arr_Values.contains('\\')) ? 2 : 3);
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
					else if (values.contains('(') && isNotBuiltInFunction(values) && isExperimental /*uses experimental features?*/)
					{
						std::string function_name = values.substr(0, values.find('(') - 2);
						Function& func = function_map.at(function_name);
						std::istringstream iss{};
						std::string toIss;
						for (auto& i : func.actions)
						{
							toIss.append(i);
						}
						iss.str(toIss);
						Parse(iss, func.func_name);
					}
					else if (!isNumber(pre_Values) && pre_Values.contains("print") && pre_Values != "printnl" && pre_Values != "println")
					{
						std::string vname =
							pre_Values.substr(
							pre_Values.find('(')/* + 1*/,
							pre_Values.find(')')/* - 1*/
							);
						vname.erase(vname.find('('), 1);
						vname.erase(vname.find(')'), 1);
						if (!vname.contains('"'))
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
							printf_s(vname.substr(vname.find('\"') + 1, vname.rfind('\"') - 1).c_str());
						}
					}
					else if (!isNumber(pre_Values) && (pre_Values == "printnl" || pre_Values == "println")) printf_s("\n");
					else if (!isNumber(pre_Values) && pre_Values.contains("toString"))
					{
						std::string num = pre_Values.substr(pre_Values.find('(') + 1, pre_Values.length());
						std::erase(num, ')');
						data[element].type = types::basic_string;
						data_map[var_name].type = types::basic_string;
						data[element].str_Out = num;
						data_map[var_name].str_Out = num;
					}
					else if (!isNumber(pre_Values) && pre_Values.contains("intToHex"))
					{
						if(type != "hex_number") { 
							lC.LogError(
								"CONFIG-0001 Type 'hex_number' is in-compatible with declaration of type '" + type + "' terminating parsing of current line! Hint : try changing the type to hex_number\n",
								config_error::INCOMPATIBLE_TYPES
							);
							continue;
						}
						auto number = static_cast<int64_t>(std::atoi(
							pre_Values.substr(
								static_cast<int64_t>(pre_Values.find('(')) + 1,
								static_cast<int64_t>(pre_Values.find(')')) - 1).c_str()));

						data[element].type = types::basic_hex;
						data[element].hex_Out = number;
						data_map[var_name].type = types::basic_hex;
						data_map[var_name].hex_Out = number;

					}
					else if (!isNumber(pre_Values) && pre_Values.contains("hexToInt"))
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
						if (err != basic_math_error::NOTHING_TO_DO && cnfg_err == config_error::OK)
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
					else if (!isNumber(pre_Values) && pre_Values.contains('\''))
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
		const std::vector<config_error>& GetErrorList() { return error_list; }
		const config_error GetLastError() { return error_list[err_element]; }
		const std::vector<Store>& GetDataVector() const { return data; }
		const std::unordered_map<std::string, Store>& GetDataMap() const { return data_map; }
		explicit Config(std::istream& inStream)
		{
			Parse(inStream);
		}
		_Must_inspect_result_ _Check_return_ [[nodiscard]] bool ReloadConfigFile(std::istream& is)
		{
			// Removed erasing the error vector because it will automatically do it in Parse()
			err_element	= 0;
			element		= 0;
			for (auto& i : data)
			{
				i.Clear();
			}
			data.clear();
			data_map.clear();
			return Parse(is);
		}
		Config(const Config&) = delete;
		Config& operator=(const Config&) = delete;
		Config(Config&&) = delete;
		Config& operator=(Config&&) = delete;
		friend class LogCover;
	};
	void LogCover::LogError(const std::string& msg, config_error err, bool isFatal)
	{
		if (!isFatal)
			m_Log.LoglevelError(msg);
		else m_Log.LoglevelFatalError(msg);
		m_Other->error_list[*m_Ele] = err;
		m_Other->good = false;
		// Continue or return if fatal Statement has to be in the if statement along with this function
	}
}