#ifndef CONFIG_H
#define CONFIG_H

#include <string>
#include "nlohmann/json.hpp"

using json = nlohmann::json;

class Config {
public:
    Config();
    ~Config();

    bool load_server_config(const std::string& config_path = "configs/server.json");
    
    std::string get_host() const { return host; }
    int get_port() const { return port; }

private:
    std::string host;
    int port;
};

#endif 