#pragma once

#include <string>
#include <vector>
#include <memory>
#include <functional>
#include <atomic>
#include "imgui.h"
#include "../utils/Logger.h"

class GameUI {
private:
    int player_number;
    std::string game_name;
    std::string player_name;
    std::atomic<bool>* my_turn;
    std::atomic<bool>* active;
    
    bool show_packet_console;
    
    std::vector<std::string> packet_log_messages;
    
    void render_main_menu_bar();
    void render_game_window();
    void render_player_info();
    void render_packet_console();

public:
    GameUI(std::atomic<bool>* my_turn, std::atomic<bool>* active);
    ~GameUI();
    
    void render();
    
    void set_player_number(int number);
    void set_game_name(const std::string& name);
    void set_player_name(const std::string& name);
    
    void add_packet_log(const std::string& message);
}; 