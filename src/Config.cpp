#include "Config.h"
#include "utils/Logger.h"
#include <iostream>
#include <fstream>
#include <stdexcept>

Config::Config() { // brute config constructor in case of errors (from config.json)
    host = "127.0.0.1";
    port = 8080;
}

Config::~Config() { // config destructor
}

bool Config::load_server_config(const std::string& config_path) { // loads the server config from the config file
    std::ifstream config_file(config_path); // opens the config file
    
    if (!config_file.is_open()) {
        Logger::error("Config", "cant open config file: " + config_path); // error if cant open config file
        return false;
    }
    
    try {
        json config_json; // json object to store the config
        config_file >> config_json;
        
        host = config_json.at("host").get<std::string>();
        port = config_json.at("port").get<int>();
        
        Logger::info("Config", "loaded config: host=" + host + ", port=" + std::to_string(port));
        return true;
    } 
    catch (const json::exception& e) {
        Logger::error("Config", "cant parse config file: " + std::string(e.what()));
        return false;
    } 
    catch (const std::exception& e) {
        Logger::error("Config", "error reading config file: " + std::string(e.what()));
        return false;
    }
} 