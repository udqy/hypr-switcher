#include <iostream>
#include <string>
#include <vector>
#include <cstdio>
#include <array>
#include <memory> // Added for std::unique_ptr
#include <stdexcept> // Added for std::runtime_error
#include "nlohmann/json.hpp"

using json = nlohmann::json;

/*
/
                       _oo0oo_
                      o8888888o
                      88" . "88
                      (| -_- |)
                      0\  =  /0
                    ___/`---'\___
                  .' \\|     |// '.
                 / \\|||  :  |||// \
                / _||||| -:- |||||- \
               |   | \\\  -  /// |   |
               | \_|  ''\---/''  |_/ |
               \  .-\__  '-'  ___/-. /
             ___'. .'  /--.--\  `. .'___
          ."" '<  `.___\_<|>_/___.' >' "".
         | | :  `- \`.;`\ _ /`;.`/ - ` : | |
         \  \ `_.   \_ __\ /__ _/   .-` /  /
     =====`-.____`.___ \_____/___.-`___.-'=====
                       `=---='
     ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

       Buddha blesses you with no bugs forever
*/

// Client struct
struct client {
    std::string address;
    std::string title;
    int workspace;
    int monitor;
    std::string windowClass;

    // Constructor
    client(const std::string& _address, const std::string& _title, int _workspace, int _monitor, const std::string& _windowClass)
        : address(_address), title(_title), workspace(_workspace), monitor(_monitor), windowClass(_windowClass) {}
};

/*

what i know:
focusing to a specific window works like this:--}  hyprctl dispatch focuswindow address:0x55a98c55b850

*/

/*********************/
/*  helper functions */
/*********************/

static void help() {
    std::cout << "hypr-switcher usage: hypr-switcher [arg [...]].\n\nArguments:\n"
              << " -d | --daemon            | Start as a daemon\n"
              << " -l | --launch            | Start as a standalone qt app\n"
              << " -h | --help              | Show this help message\n"
              << std::endl;
}

std::string exec(const char* cmd) {
    std::array<char, 128> buffer;
    std::string result;
    std::unique_ptr<FILE, decltype(&pclose)> pipe(popen(cmd, "r"), pclose);
    if (!pipe) {
        throw std::runtime_error("popen() failed!");
    }
    while (fgets(buffer.data(), buffer.size(), pipe.get()) != nullptr) {
        result += buffer.data();
    }
    return result;
}

std::vector<client> jsonToClient(const std::string& jsonData) {
    auto json = json::parse(jsonData); // Changed to use 'json' alias
    std::vector<client> clients;

    for (const auto& item : json) {
        clients.emplace_back(
            item["address"].get<std::string>(),
            item["title"].get<std::string>(),
            item["workspace"]["id"].get<int>(),
            item["monitor"].get<int>(),
            item["class"].get<std::string>()
        );
    }

    return clients;
}

/*------------------------------------*/
/* core functions for fetching things */
/*------------------------------------*/

bool switchFocus(std::string address) {
  std::string command = "hyprctl dispatch focuswindow address:" + address;
  return system(command.c_str()) == 0;
}

int main(int argc, char** argv) {
    for (int i = 1; i < argc; i++) {
        std::string arg = argv[i];

        if (arg == "-h" || arg == "--help") {
            help();
            return 0;
        } 
        
        else if (arg == "-d" || arg == "--daemon") {
            std::cout << "Starting as a daemon..." << std::endl;
            // Add daemon logic here
        } 
        
        else if (arg == "-l" || arg == "--launch") {
            std::cout << "Starting as a standalone qt app..." << std::endl;            
            std::string jsonData = exec("hyprctl clients -j");
            std::vector<client> clients = jsonToClient(jsonData);
            // Output the clients for debugging
            for (const auto& c : clients) {
                std::cout << "Client Address: " << c.address << "\n"
                          << "Title: " << c.title << "\n"
                          << "Workspace: " << c.workspace << "\n"
                          << "Monitor: " << c.monitor << "\n"
                          << "Class: " << c.windowClass << "\n\n";
            }

            // Add additional logic for the Qt app here

        } else {
            help();
            return 1;
        }
    }

    return 0;
}
