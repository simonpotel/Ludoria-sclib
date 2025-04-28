#include "CtS.h"
#include "StC.h"
#include "Network.h"
#include "utils/Logger.h"
#include "ui/UIManager.h"
#include "ui/GameUI.h"
#include <iostream>
#include <string>
#include <thread>
#include <chrono>
#include <atomic>
#include "nlohmann/json.hpp"

int player_num = 0; // player number (1 or 2)
std::atomic<bool> my_turn{false}; // my turn (false or true)
std::atomic<bool> active{true}; // active (true or false)
std::unique_ptr<GameUI> game_ui; // game ui pointer

// callback
void on_player_assignment(const json& data) {
    try {
        player_num = data["player_number"].get<int>();
        Logger::info("Game", "Assigned as player " + std::to_string(player_num));
        
        if (game_ui) {
            game_ui->set_player_number(player_num);
        }
    } catch (const json::exception& e) {
        Logger::error("Game", "Error processing player assignment: " + std::string(e.what()));
    }
}

int main() {
    Logger::initialize(); 

    UIManager ui_manager;
    if (!ui_manager.initialize("Ludoria-sclib ", 800, 600)) {
        std::cerr << "failed to initialize ui" << std::endl;
        return 1;
    }

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

    game_ui = std::make_unique<GameUI>(&my_turn, &active);
    game_ui->set_player_name(name);
    game_ui->set_game_name(game);

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
            
            while (active && cts.is_connected() && !ui_manager.should_close_window()) {
                if (!ui_manager.begin_frame()) {
                    break;
                }
                
                game_ui->render();
                
                ui_manager.end_frame();
                
                std::this_thread::sleep_for(std::chrono::milliseconds(16));
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

    game_ui.reset();
    ui_manager.shutdown();

    std::cout << "done." << std::endl;
    return 0;
} 