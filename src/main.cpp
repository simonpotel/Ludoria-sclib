#include "CtS.h"
#include "StC.h"
#include "Network.h"
#include "utils/Logger.h"
#include <iostream>
#include <string>
#include <thread>
#include <chrono>
#include <atomic>
#include "nlohmann/json.hpp"

int player_num = 0; // player number (1 or 2)
std::atomic<bool> my_turn{false}; // my turn (false or true)
std::atomic<bool> active{true}; // active (true or false)

// callback
void on_player_assignment(const json& data) {
    try {
        player_num = data["player_number"].get<int>();
        Logger::info("Game", "Assigned as player " + std::to_string(player_num));
    } catch (const json::exception& e) {
        Logger::error("Game", "Error processing player assignment: " + std::string(e.what()));
    }
}

int main() {
    Logger::initialize(); 

    std::string name;
    std::string game;

    std::cout << "Enter player name: ";
    std::getline(std::cin, name);
    
    if (name.empty()) {
        name = "p" + std::to_string(rand() % 1000);
        std::cout << "using default: " << name << std::endl;
    }
    
    std::cout << "Enter game name: ";
    std::getline(std::cin, game);
    
    if (game.empty()) {
        game = "game" + std::to_string(rand() % 100);
        std::cout << "using default: " << game << std::endl;
    }

    try {
        Network network; // network object
        CtS cts(&network); // client to server object
        StC stc; // server to client object
        
        stc.register_callback(PacketType::PLAYER_ASSIGNMENT, on_player_assignment); // register callback for player assignment

        std::cout << "connecting: " << game << " as " << name << std::endl;
        
        bool ok = cts.connect(name, game); // connect to the server
        
        if (ok) {
            std::cout << "connected!" << std::endl;
            
            network.start_packet_receiver(&stc); // start the packet receiver
            
            while (active && cts.is_connected()) {
                std::this_thread::sleep_for(std::chrono::milliseconds(100));
                // wait for the server to send packets
                // and process them using callbacks
            }
            
            std::cout << "disconnecting..." << std::endl;
            active = false;
            if (cts.is_connected()) {
                cts.disconnect(); // disconnect from the server
            }
            
            network.stop_packet_receiver(); // stop the packet receiver
        } 
        else {
            std::cerr << "failed to connect." << std::endl;
            return 1;
        }
    } 
    catch (const std::exception& e) {
        std::cerr << "error: " << e.what() << std::endl;
        return 1;
    }

    std::cout << "done." << std::endl;
    return 0;
} 