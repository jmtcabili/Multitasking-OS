#include <iostream>
#include <mutex>

#ifndef CONSOLE
#define CONSOLE
class IConsole{
    public: 
        virtual ~IConsole() = default; 
        virtual void handleCommand() = 0; //virtual function for different handling between main and screen consoles
        std::string getUserInput(){ //default user input 
            std::string input;
            std::cout << "Enter command: "; // Generic prompt
            std::getline(std::cin, input);
            return input;
        }

        void displayConsoleHeader() { //can be overidden if we want diff header on screen consoles
            std::cout << " _______  _______  _______  _______  _______  _______             _______  __      ___           _______  _______ \n";
            std::cout << "(  ____ \\(  ____ \\(  ___  )(  ____ )(  ____ \\(  ____ \\|\\     /|  (  ____ \\/  \\    /   )         (  ____ \\/ ___   )\n";
            std::cout << "| (    \\/| (    \\/| (   ) || (    )|| (    \\/| (    \\/( \\   / )  | (    \\/\\/) )  / /) |         | (    \\/\\/   )  |\n";
            std::cout << "| |      | (_____ | |   | || (____)|| (__    | (_____  \\ (_) /   | (_____   | | / (_) (_  _____ | |          /   )\n";
            std::cout << "| |      (_____  )| |   | ||  _____)|  __)   (_____  )  \\   /    (_____  )  | |(____   _)(_____)| | ____   _/   / \n";
            std::cout << "| |            ) || |   | || (      | (            ) |   ) (           ) |  | |     ) (         | | \\_  ) /   _/  \n";
            std::cout << "| (____/\\/\\____) || (___) || )      | (____/\\/\\____) |   | |     /\\____) |__) (_    | |         | (___) |(   (__/\\\n";
            std::cout << "(_______/\\_______)(_______)|/       (_______/\\_______)   \\_/     \\_______)\\____/    (_)         (_______)\\_______/\n" << std::endl;

            std::cout << "Welcome to CSOPESY Emulator\n\n";
            std::cout << "Developers: \n" 
                << "  - Cabili, Johan Marlo\n"
                << "  - Carandang, Matthew Ryan\n"
                << "  - Veracruz, Sean Riley\n\n";
            std::cout << "Last updated: June 19, 2025\n";
        }
        
};
#endif