#include "../Config/Config.hpp"
#include <iostream>
#include <conio.h>
#include <sstream>
#include <thread>
#include <chrono>

std::ifstream inFileStream;
volatile bool ctrlCReached = 0;
BOOL WINAPI CtrlCHandler(_In_ DWORD dwCtrlType)
{
    switch (dwCtrlType)
    {
    case CTRL_C_EVENT:
        printf("Ending! CTRL_C_EVENT (0)\n");
        if(inFileStream) inFileStream.close();
        ctrlCReached = true;
        return TRUE;
    case CTRL_CLOSE_EVENT:
        printf("Ending! CTRL_CLOSE_EVENT (2)\n");
        if(inFileStream) inFileStream.close();
        ctrlCReached = true;
        return TRUE;
    default:
        return FALSE;
    }
}

int main(int argc, char** argv, char** envp)
{
    SetConsoleCtrlHandler(CtrlCHandler, TRUE);
    std::jthread handle{ [&]()
        {
            while (true)
            {
                if (ctrlCReached)
                {
                    ExitProcess(0);
                }
            }
        }
    };
    inFileStream.open("Config.txt");
    inFileStream.clear();
    inFileStream.seekg(0, std::ios::beg);
    try
    {
        auto start = std::chrono::system_clock::now();
        config::Config c{ std::cin };
        auto end = std::chrono::system_clock::now();
        std::cout
            << "Parsed Data is : \n";
        for (auto& [key, value] : c.GetDataMap())
        {
            std::cout << value << '\n';
        }
        for (auto& i : c.GetErrorList())
            if (i == config::config_error::TERMINATE_PARSER_WAS_CALLED || i == config::config_error::INVALID_PREPROCESSING_DIRECTIVE) ExitProcess(1);
        inFileStream.close();
        std::cout
            << "Took " << ((end - start).count() / 1000.0) << " microseconds"
            << "\nPress any key to reload the config file...\n";
        (void)_getch();
        inFileStream.open("Config.txt");
        auto start2 = std::chrono::system_clock::now();
        if (!c.ReloadConfigFile(inFileStream, true)) ExitProcess(1);
        auto end2 = std::chrono::system_clock::now();
        for (auto& i : c.GetErrorList())
            if (i == config::config_error::TERMINATE_PARSER_WAS_CALLED || i == config::config_error::INVALID_PREPROCESSING_DIRECTIVE) ExitProcess(1);
        std::cout
            << "Newly Parsed Data is : \n";
        for (auto& [key, value] : c.GetDataMap())
        {
            std::cout << value << '\n';
        }
        std::cout 
            << "Took " << ((end2 - start2).count() / 1000.0) << " microseconds"
            << "\nPress any key to continue...";
        (void)_getch();
        inFileStream.close();
        ExitProcess((int)c.GetDataMap().at("return_value").hex_Out);
    }
    catch (std::exception &e) { std::cout << "Exception thrown! what()" << e.what() << '\n'; }
}