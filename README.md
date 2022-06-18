# Config
## How To Use
1. Download the latest version of the static libary and the Config.hpp file from Config/Config.hpp
2. Link against that in the project you want to use (you need to do this differently for different compilers)
3. Include the Config.hpp file in your project where ever you want to use it (for me it was #include "../Config/Config.hpp" as shown in the example usage)
4. In the file you included it in make a new std::ifstream object and open a file with the std::ifstream object
5. Make a new Config object by doing Config::Config (var name){(std::ifstream object or any other class with std::istream as a base class)};
6. Do auto values = (config object).GetDataMap() or auto values = (config object).GetDataVector()
7. To Reload the Configuration file do if(bool result = (config object).ReloadConfigFile(); result == false) { /\*Error handling or return 1; in the main function\*/ return 1; }
## Config file syntax
(type) (var_name (used for data map)) = (value)
Example:
int integer = 10;
if trying to use a hexadecimal number with an int or vice versa use the functions intToHex( (integer) ) or hexToInt( (hexadecimal number) )
Examples:

int hexToIntTest = hexToInt(0xFF)
hex_number intToHexTest = intToHex(255)
Types are
1. bool
2. char
3. int
4. double
5. hex_number (int64_t in c++)
6. string

Use an array to make multiple variable (more like constants) of the same type

Array syntax:

array<type, size> (array name) = { (different values of the same type sepperated by commas and the number of declarations must be equal to the array size) }

Example:
array<int, 10> intArray = { 0, 1, 2, 3, 4, 5, 6, 7, 8 ,9 }


To print something use

print( (var name) ) (no need for spaces)

To print a new line there is 2 options

1st. Make a char with the name nl and assign it to '\n' and do print(nl)

2nd. use printnl
## TODO:
1. Add structured data (probably not possible because c++ is staticly typed)
2. Add reassigning so variables aren't constants until the next reload
3. Add operators 
