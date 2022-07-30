#include "../Config/Config.hpp"
#include <iostream>
#include <sstream>
#include <thread>
#include <chrono>

std::ifstream inFileStream;

#define ASSERT(x, msg, run) if(x) \
{\
    std::cout << msg;\
    std::raise(SIGINT);\
    run\
}

int main(int argc, char** argv, char** envp)
{
    if(argc == 1) inFileStream.open("Config.txt");
    else inFileStream.open(argv[1]);
    inFileStream.clear();
    inFileStream.seekg(0, std::ios::beg);
    try
    {
        auto start = std::chrono::system_clock::now();
        config::Config c{ inFileStream, true };
        auto end = std::chrono::system_clock::now();
        std::cout
            << "Parsed Data is : \n";
        for (auto& [key, value] : c.GetDataMap())
        {
            std::cout << value << '\n';
        }
        for (auto& i : c.GetErrorList())
            if (i == config::config_error::TERMINATE_PARSER_WAS_CALLED || i == config::config_error::INVALID_PREPROCESSING_DIRECTIVE) return 1;
        inFileStream.close();
        std::cout
            << "Took " << ((end - start).count() / 1000.0) << " microseconds"
            << "\nPress any key to reload the config file...\n";
	std::cin.get();
    if (argc == 1) inFileStream.open("Config.txt");
    else inFileStream.open(argv[1]);
        auto start2 = std::chrono::system_clock::now();
        if(!c.ReloadConfigFile(inFileStream, true)) return 1;
        auto end2 = std::chrono::system_clock::now();
        for (auto& i : c.GetErrorList())
            if (i == config::config_error::TERMINATE_PARSER_WAS_CALLED || i == config::config_error::INVALID_PREPROCESSING_DIRECTIVE) return 0;
        std::cout
            << "Newly Parsed Data is : \n";
        for (auto& [key, value] : c.GetDataMap())
        {
            std::cout << value << '\n';
        }
        std::cout 
            << "Took " << ((end2 - start2).count() / 1000.0) << " microseconds"
            << "\nPress any key to continue...";
        std::cin.get();
        inFileStream.close();
        return (int)c.GetDataMap().at("return_value").hex_Out;
    }
    catch (std::exception& e) 
    { 
        std::cout << "Exception thrown! what()" << e.what() << "\nPress any key to continue...\n";
        inFileStream.close();
        std::cin.get();
        return 1;
    }
}
