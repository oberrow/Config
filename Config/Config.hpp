#pragma once
#include <string>
#include <fstream>
#include <vector>
#include <filesystem>
#include <unordered_map>
#include <functional>
#include "log.hpp"
#include <array>


namespace Config
{
	static int lasterror = 0;
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
		types type = types::invalid_type;
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
	class Config
	{
	private:
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
		static bool isdigit(char _ch)
		{
			bool not_found = false;
			char num[10] = { '0', '1', '2', '3', '4', '5', '6', '7', '8', '9' };
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
			for (auto i : s)
			{
				if (!isdigit(i)) return false;
				else continue;
			}
			return true;
		}
		bool isHexNumber(std::string_view const& string) const
		{
			return string.find("0x") != std::string::npos;
		}
		void SetType(const std::string_view& type, const std::string& map_element)
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
		const static inline int size = 7;
		const std::array<std::string, size> type_names =
		{
			"int",
			"bool",
			"hex_number",
			"double",
			"string",
			"char"
		};
		std::istream& is;
		bool good = true;
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
		bool Parse()
		{
			Log l{ Log::levelInfoId };
			std::string values;
			int line = 0;
			while (std::getline(is, values))
			{
				if (values.empty()) continue;
				bool isArray = false;
				size_t arraySize = -1;
				line++;
				std::string pre_Values{};
				std::string var_name{};
				std::string type{};
				if (!values.contains("//"))
				{
					remove_trailing_whitespaces(values);
					remove_trailing_tabs(values);
					if (size_t nval = values.find("="); nval != std::string::npos && !values.contains("print") && !values.contains("printnl"))
					{
						pre_Values = values.substr(nval + 2);
						if (!values.contains("array"))
							var_name = values.substr(values.find(' ') + 1, nval - 2);
						else
							var_name = values.substr(values.find(' ') + 4, nval - 15); \
							if (var_name.contains("="))
								var_name.erase(var_name.find('='), std::string::npos);
					}
					std::erase(var_name, 32);
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
					for (int i = 0; i < size; i++)
					{
						if (type == type_names[i]) { break; }
						else if (type != type_names[i] && i == size - 1)
						{
							l.LoglevelError((std::stringstream() << "CONFIG-0000 Un-recognized type! Type is " << type << " Terminating parsing of current line!\n").str());
							good = false;
							continue;
						}
					}
					if (!pre_Values.empty())
						;
					else
					{
						l.LoglevelError((std::stringstream() << "\nCONFIG-0004 Missing an \"=\" sign in provided std::istream& object! Line is " << line << " Terminating parsing of current line!").str());
						good = false;
						continue;
					}
					Store toVec = { };
					data.push_back(toVec);
					std::erase(var_name, 32);
					data_map.try_emplace(var_name, toVec);
					element = data.size() - 1;
					data[element].var_name = var_name;
					data_map[var_name].var_name = var_name;
					SetType(type, var_name);
					if (!pre_Values.empty() && pre_Values.front() == 32)
					{
						pre_Values.erase(0, 1);
					}
					pre_Values.append("\0");
					if (!isArray && type == "string")
					{
						size_t count = std::ranges::count(pre_Values.begin(), pre_Values.end(), '\"');
						if (count > 2)
						{
							l.LoglevelError("CONFIG-0009 Too much \"s in string!");
							good = false;
							continue;
						}
						else if (count < 2)
						{
							l.LoglevelError("CONFIG-0009 Too little \"s in string!");
							good = false;
							continue;

						}
					}
					if (isArray && type == "char" && pre_Values.contains('\"'))
					{
						size_t count = std::ranges::count(pre_Values.begin(), pre_Values.end(), '\"');
						if (count > 2)
						{
							l.LoglevelError("CONFIG-0009 Too much \"s in string!");
							good = false;
							continue;
						}
						else if (count < 2)
						{
							l.LoglevelError("CONFIG-0009 Too little \"s in string!");
							good = false;
							continue;
						}
						std::erase(pre_Values, '\"');
						remove_trailing_whitespaces(pre_Values);
						if (pre_Values.length() < arraySize)
						{
							l.LoglevelError("CONFIG-0011 Array is too big!");
							good = false;
							continue;
						}
						else if (pre_Values.length() > arraySize)
						{
							l.LoglevelError("CONFIG-0011 Array is too small too hold string!");
							good = false;
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
							l.LoglevelError("CONFIG-0003 Missing an '{' for an array declaration! Terminating parsing of current line!");
							good = false;
							continue;
						}
						if (end == std::string::npos)
						{
							l.LoglevelError("CONFIG-0003 Missing an '}' for an array declaration! Terminating parsing of current line!");
							good = false;
							continue;
						}
						data[element].array_Size = arraySize;
						data_map[var_name].array_Size = arraySize;
						data[element].is_Array = isArray;
						data_map[var_name].is_Array = isArray;
						const ::Config::types& type_ref = data[element].type;
						std::string arr_Values = pre_Values.substr(begin + 1, pre_Values.length() - 2);
						// Credits : https://www.techiedelight.com/count-occurrences-char-string-cpp/
						// ---------------------------
						{
							char ch = ',';
							auto count = std::ranges::count(arr_Values.begin(), arr_Values.end(), ch) + 1;
							// No more credit
							if (count > arraySize)
							{
								l.LoglevelError("CONFIG-0006 Number of declarations is greater then array size! To fix this error make sure to have the correct array size");
								good = false;
								continue;
							}
							else if (count < arraySize)
							{
								l.LoglevelError("CONFIG-0006 Number of declarations is less then array size! To fix this error make sure to have the correct array size");
								good = false;
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
									l.LoglevelError("CONFIG-0005 Cannot find an ',' in array declaration");
									data[element].Clear();
									data_map[var_name].Clear();
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
									l.LoglevelError((std::stringstream() << "CONFIG-0008 " << num << " is not a integer!").str());
									FreeArrayByType("int");
									break;
								}
							}
							else if (type_ref == types::basic_double)
							{
								if (pre_Values.find(",") == std::string::npos)
								{
									l.LoglevelError("CONFIG-0005 Cannot find an ',' in array declaration");
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
									l.LoglevelError((std::stringstream() << "CONFIG-0008 " << num << " is not a decimal number!").str());
									FreeArrayByType("double");
									break;
								}
							}
							else if (type_ref == types::basic_character)
							{
								if (pre_Values.find(",") == std::string::npos)
								{
									l.LoglevelError("CONFIG-0005 Cannot find an ',' in array declaration");
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
									l.LoglevelError("CONFIG-0005 Cannot find an ',' in array declaration");
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
								if (pre_Values.find(",") == std::string::npos)
								{
									l.LoglevelError("CONFIG-0005 Cannot find an ',' in array declaration");
									data[element].Clear();
									data_map[var_name].Clear();
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
										i2 != 'f' && i2 != 'F' 
) continue;
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
									l.LoglevelError((std::stringstream() << "CONFIG-0008 " << num << " is not a hexadecimal number!").str());
									FreeArrayByType("hex_number");
									break;
								}
							}
							else if (type_ref == types::basic_string)
							{
								std::string str{};
								size_t _Begin = arr_Values.find('\"');
								arr_Values.erase(_Begin, 1);
								for (int i2 = (int)_Begin; i2 < arr_Values.length(); i2++)
								{
									char _Ch = arr_Values[i2];
									if (_Ch == '\"') break;
									else str.push_back(_Ch);
								}
								arr_Values.erase((size_t)arr_Values.find(str), (size_t)str.length() + 1);
								data[element].string_array_Out.push_back(str);
								data_map[var_name].string_array_Out.push_back(str);
							}
							(first == true) ? first = false : 0;
						}
						continue;
					}
					else if (!isNumber(pre_Values) && pre_Values.contains("print") && pre_Values != "printnl")
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
								l.LoglevelError("CONFIG-0010 Could not find " + vname + " in data map!");
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
							std::cout << str;
						}
						else
						{
							std::cout << vname.substr(vname.find('\"') + 1, vname.rfind('\"') - 1);
						}
					}
					else if (!isNumber(pre_Values) && pre_Values == "printnl") std::cout << '\n';
					else if (!isNumber(pre_Values) && pre_Values.contains("intToHex"))
					{
						if(type != "hex_number") { l.LoglevelError("CONFIG-0001 Type 'hex_number' is in-compatible with declaration of type '" + type + "' terminating parsing of current line! Hint : try changing the type to hex_number\n"); good = false; continue; }
						int64_t number = static_cast<int64_t>(std::atoi(
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
						if(type != "int") { l.LoglevelError("CONFIG-0001 Type 'int' is in-compatible with declaration of type '" + type + "' terminating parsing of current line! Hint : try changing the type to hex_number\n"); good = false; continue; }
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
						if (type != "bool") { l.LoglevelError("CONFIG-0001 Type 'bool' is in-compatible with declaration of type '" + type + "' terminating parsing of current line!\n"); good = false; continue; }
						data[element].type = types::basic_boolean;
						data[element].boolean_Out = true;
						data_map[var_name].type = types::basic_boolean;
						data_map[var_name].boolean_Out = true;
					}
					else if (!isNumber(pre_Values) && pre_Values == "false")
					{
						if (type != "bool") { l.LoglevelError("CONFIG-0001 Type 'bool' is in-compatible with declaration of type '" + type + "' terminating parsing of current line!\n"); good = false; continue; }
						data[element].type = types::basic_boolean;
						data[element].boolean_Out = false;
						data_map[var_name].type = types::basic_boolean;
						data_map[var_name].boolean_Out = false;
					}
					else if (isHexNumber(pre_Values))
					{
						if (type != "hex_number") { l.LoglevelError("CONFIG-0001 Type 'hex_number' is in-compatible with declaration of type '" + type + "' terminating parsing of current line!\n"); good = false; continue; }
						int64_t temp = 0;
						std::istringstream iss{ pre_Values };
						iss >> std::hex >> temp;
						data[element].type = types::basic_hex;
						data[element].hex_Out = temp;
						data_map[var_name].type = types::basic_hex;
						data_map[var_name].hex_Out = temp;
					}
					else if (isNumber(pre_Values))
					{
						if (type != "int") { l.LoglevelError("CONFIG-0001 Type 'int' is in-compatible with declaration of type '" + type + "' terminating parsing of current line!\n"); good = false; continue; }
						data[element].type = types::basic_int;
						data[element].int_Out = std::atoi(pre_Values.c_str());
						data_map[var_name].type = types::basic_int;
						data_map[var_name].int_Out = std::atoi(pre_Values.c_str());
					}
					else if (isDecimalNumber(pre_Values))
					{
						if (type == "int") { l.LoglevelError("CONFIG-0002 Invalid narrowing conversion from 'double' to 'int' Terminating parsing of curent line!"); good = false; continue; }
						if (type != "double") { l.LoglevelError("CONFIG-0001 Type 'double' is in-compatible with declaration of type '" + type + "' terminating parsing of current line!\n"); good = false; continue; }
						data[element].type = types::basic_double;
						data[element].decimal_Out = std::stod(pre_Values.c_str());
						data_map[var_name].type = types::basic_double;
						data_map[var_name].decimal_Out = std::stod(pre_Values.c_str());
					}
					else if (!isNumber(pre_Values) && pre_Values.contains('\''))
					{
						types isInt = types::basic_character;
						if (type == "int")
						{
							isInt = types::basic_int;
						}
						if (type != "char") { l.LoglevelError("CONFIG-0001 Type 'char' is in-compatible with declaration of type '" + type + "' terminating parsing of current line!\n"); good = false; continue; }
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
						if (type != "string") { l.LoglevelError("CONFIG-0001 Type 'string' is in-compatible with declaration of type '" + type + "' terminating parsing of current line!\n"); good = false; continue; }
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
						l.LoglevelWarn("Config file parser warning! Could not find any strings, booleans or numbers after the \"=\" sign");
						good = false;
					}
				}
				else
				{
					// Nothing to do if there is a commented line
					continue;
				}
			}
			return good;
		}
	public:
		const std::vector<Store>& GetDataVector() const { return data; }
		const std::unordered_map<std::string, Store>& GetDataMap() const { return data_map; }
		explicit Config(std::istream& inStream)
			:is{ inStream }
		{
			Parse();
		}
		_Must_inspect_result_ _Check_return_ [[nodiscard]] bool ReloadConfigFile()
		{
			for (auto& i : data)
			{
				i.Clear();
			}
			data.clear();
			data_map.clear();
			return Parse();
		}
		Config(const Config&) = delete;
		Config& operator=(const Config&) = delete;
		Config(Config&&) = delete;
		Config& operator=(Config&&) = delete;
	};
}