#include <iostream>
#include <string>
#include <vector>
#include <cstdio>
#include <array>
#include <memory> // for std::unique_ptr
#include <stdexcept> // for std::runtime_error
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

struct client 
{
    std::string address;
    std::string title;
    int workspace;
    int monitor;
    std::string windowClass;

    // Constructor
    client(const std::string& _address, const std::string& _title, int _workspace, int _monitor, const std::string& _windowClass)
        : address(_address), title(_title), workspace(_workspace), monitor(_monitor), windowClass(_windowClass) {}
};

std::vector<client> clientInfo;

/*********************/
/*  helper functions */
/*********************/

static void help() 
{
    std::cout << "hypr-switcher usage: hypr-switcher [arg [...]].\n\nArguments:\n"
              << " -d | --daemon            | Start as a daemon\n"
              << " -l | --launch            | Start as a standalone qt app\n"
              << " -h | --help              | Show this help message\n"
              << std::endl;
}

std::string exec(const char* cmd) 
{
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

std::vector<client> jsonToClient(const std::string& jsonData) 
{
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

/*-----------------------------------------*/
/* core functions for hyprland interaction */
/*-----------------------------------------*/

/*

functions required:
    switch window
    get all window information
    get window titles
    
*/

bool switchFocus(std::string address) 
{
    std::string command = "hyprctl dispatch focuswindow address:" + address;
    return system(command.c_str()) == 0;
}

std::string getWindowTitle(std::string address) 
{
    /* finds the window title of the corresponding window address */
    for (const auto& client : clientInfo) {
        if (client.address == address) {
            return client.title;
        }
    }
    return "";
}


std::vector<std::tuple<std::string, int>> getTitleAndWorkspace(const std::vector<std::string>& addresses)
{
    /* returns a vector of tuples that has a Title and Workspace */
    /* could be used with getAddresses() function like so: */
    /* Example:  std::vector<std::tuple<std::string, int>> titlesAndWorkspaces = getTitleAndWorkspace(getAddresses()); */

    std::vector<std::tuple<std::string, int>> result;

    for (const auto& address : addresses) {
        for (const auto& client : clientInfo) {
            if (client.address == address) {
                result.emplace_back(client.title, client.workspace);
                break;
            }
        }
    }

    return result;
}

std::vector<std::string> getAddresses() {
    /* returns a vector of string with all address */
    std::string command = "hyprctl clients -j";
    std::string jsonData = exec(command.c_str());

    std::vector<std::string> addresses;
    try {
        auto json = nlohmann::json::parse(jsonData);

        for (const auto& item : json) {
            if (item.contains("address")) {
                addresses.push_back(item["address"].get<std::string>());
            }
        }
    } catch (const std::exception& e) {
        std::cerr << "Error parsing JSON: " << e.what() << std::endl;
    }

    return addresses;
}


/*------*/
/* main */
/*------*/

int main(int argc, char** argv) 
{
    try 
    {
        /* populates the client list with initial window values */
        std::string command = "hyprctl clients -j";
        std::string jsonData = exec(command.c_str());
        clientInfo = jsonToClient(jsonData);
    } 
    catch (const std::exception& e) 
    {
        std::cerr << "Failed to fetch or parse client information: " << e.what() << std::endl;
        return 1;
    }

    for (int i = 1; i < argc; i++) 
    {
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
            // Add additional logic for the Qt app here

        } else {
            help();
            return 1;
        }
    }

    return 0;
}
