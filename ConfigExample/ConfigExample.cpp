#include "../Config/Config.hpp"
#include <iostream>
#include <conio.h>
#include <sstream>
#include <chrono>

int main()
{   
    thread_local std::ifstream inFileStream;
    inFileStream.open("Config.txt");
    inFileStream.clear();
    inFileStream.seekg(0, std::ios::beg);
    auto start = std::chrono::system_clock::now();
    config::Config c{ inFileStream };
    auto end = std::chrono::system_clock::now();
    std::cout
        << "Parsed Data is : \n";
    for (auto&[key, value] : c.GetDataMap())
    {
        std::cout << value << '\n';
    }
    for (auto& i : c.GetErrorList())
        if (i == config::config_error::TERMINATE_PARSER_WAS_CALLED) return 1;
    inFileStream.close();
    std::cout
        << "Took " << ((end - start).count() / 1000.0) << " microseconds"
        << "\nPress any key to reload the config file...\n";
    (void)_getch();
    inFileStream.open("Config.txt");
    auto start2 = std::chrono::system_clock::now();
    if (bool result = c.ReloadConfigFile(inFileStream); result == false) return 1;
    auto end2 = std::chrono::system_clock::now();
    for (auto& i : c.GetErrorList())
        if (i == config::config_error::TERMINATE_PARSER_WAS_CALLED || i == config::config_error::INVALID_PREPROCESSING_DIRECTIVE) return 1;
    std::cout
        << "Newly Parsed Data is : \n";
    for (auto&[key, value] : c.GetDataMap())
    {
        std::cout << value << '\n';
    }
    std::cout 
        << "Took " << ((end2 - start2).count() / 1000.0) << " microseconds"
        << "\nPress any key to continue...";
    (void)_getch();
    inFileStream.close();
    return (int)c.GetDataMap().at("return_value").hex_Out;
}