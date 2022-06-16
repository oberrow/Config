#include "../Config/Config.hpp"
#include "../Config/log.hpp"
#include <iostream>
#include <conio.h>
#include <sstream>
#include <chrono>

int main()
{
    std::ifstream inFileStream;
    inFileStream.open("Config.txt");
    inFileStream.clear();
    inFileStream.seekg(0, std::ios::beg);
    auto start = std::chrono::system_clock::now();
    Config::Config config{ inFileStream };
    auto end = std::chrono::system_clock::now();
    std::cout
        << "Parsed Data is : \n";
    for (auto&[key, value] : config.GetDataMap())
    {
        std::cout << value << '\n';
    }
    inFileStream.close();
    std::cout
        << "Took " << ((end - start) / 1000).count() << " microseconds"
        << "\nPress any key to reload the config file...\n";
    (void)_getch();
    inFileStream.open("Config.txt");
    auto start2 = std::chrono::system_clock::now();
    if (bool result = config.ReloadConfigFile(); result == false) return 1;
    auto end2 = std::chrono::system_clock::now();
    std::cout
        << "Newly Parsed Data is : \n";
    for (auto&[key, value] : config.GetDataMap())
    {
        std::cout << value << '\n';
    }
    std::cout
        << "Took " << ((end2 - start2) / 1000).count() << " microseconds"
        << "\nPress any key to continue...";
    (void)_getch();
    inFileStream.close();
    return (int)config.GetDataMap().at("return_value").hex_Out;
}
