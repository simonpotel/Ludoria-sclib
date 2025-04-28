#include "GameUI.h"

GameUI::GameUI(std::atomic<bool>* my_turn, std::atomic<bool>* active)
    : player_number(0), my_turn(my_turn), active(active),
      show_packet_console(true) {
    // initialize the game ui
    // atomic pointers for my turn and active (for thread safety)
    Logger::info("GameUI", "initialized game ui");
}

GameUI::~GameUI() {
    Logger::info("GameUI", "destroyed game ui");
}

void GameUI::render() {
    // setup docking space
    ImGuiViewport* viewport = ImGui::GetMainViewport();
    ImGui::SetNextWindowPos(viewport->Pos); 
    ImGui::SetNextWindowSize(viewport->Size);
    ImGui::SetNextWindowViewport(viewport->ID);
    ImGui::PushStyleVar(ImGuiStyleVar_WindowRounding, 0.0f); // no rounding
    ImGui::PushStyleVar(ImGuiStyleVar_WindowBorderSize, 0.0f); // no border
    ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2(0.0f, 0.0f)); // no padding
    ImGuiWindowFlags window_flags = ImGuiWindowFlags_MenuBar | ImGuiWindowFlags_NoDocking; // menu bar and no docking
    window_flags |= ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoMove;
    window_flags |= ImGuiWindowFlags_NoBringToFrontOnFocus | ImGuiWindowFlags_NoNavFocus;
    ImGui::Begin("Ludoria", nullptr, window_flags);
    ImGui::PopStyleVar(3);
    
    ImGuiID dockspace_id = ImGui::GetID("Ludoria");
    ImGui::DockSpace(dockspace_id, ImVec2(0.0f, 0.0f), ImGuiDockNodeFlags_None); // no flags
    
    render_main_menu_bar(); // render the main menu bar
    
    render_game_window(); // this will now dock or float
    
    if (show_packet_console) {
        render_packet_console(); // this will now dock or float
    }
    
    ImGui::End(); // end dockspace
}

void GameUI::render_main_menu_bar() {
    if (ImGui::BeginMenuBar()) { // menu bar inside dockspace window
        if (ImGui::BeginMenu("File")) {
            if (ImGui::MenuItem("Disconnect")) {
                if (active && *active) {
                    *active = false;
                    Logger::info("GameUI", "user requested disconnect");
                }
            }
            if (ImGui::MenuItem("Exit")) {
                if (active && *active) {
                    *active = false;
                    Logger::info("GameUI", "user requested exit");
                }
            }
            ImGui::EndMenu();
        }
        
        if (ImGui::BeginMenu("View")) {
            ImGui::MenuItem("Packet Console", nullptr, &show_packet_console);
            ImGui::EndMenu();
        }
        
        ImGui::EndMenuBar();
    }
}

void GameUI::render_game_window() {
    ImGui::Begin("Ludoria Game"); // this is now a dockable window
    
    render_player_info(); // render the player info
    
    ImGui::Separator(); // separator
    ImGui::Spacing(); 
    
    ImGui::Text("Game Status:"); // game status
    ImGui::Spacing(); 
    if (my_turn && *my_turn) {
        ImGui::TextColored(ImVec4(0.0f, 1.0f, 0.0f, 1.0f), "Your turn!");
    } else {
        ImGui::TextColored(ImVec4(1.0f, 0.5f, 0.0f, 1.0f), "Waiting for opponent...");
    }
    
    ImGui::End();
}

void GameUI::render_player_info() {
    ImGui::Text("Game: %s", game_name.c_str()); // game name
    ImGui::Text("Player: %s", player_name.c_str()); // player name
    ImGui::Spacing(); 
    
    ImVec4 player_color;
    std::string player_text;
    
    if (player_number == 1) {
        player_color = ImVec4(0.0f, 0.5f, 1.0f, 1.0f);
        player_text = "Player 1 (Blue)";
    } else if (player_number == 2) {
        player_color = ImVec4(1.0f, 0.0f, 0.0f, 1.0f);
        player_text = "Player 2 (Red)";
    } else {
        player_color = ImVec4(0.5f, 0.5f, 0.5f, 1.0f);
        player_text = "Unassigned";
    }
    
    ImGui::TextColored(player_color, "%s", player_text.c_str());
}

void GameUI::render_packet_console() {
    ImGui::Begin("Packet Console", &show_packet_console); // this is now a dockable window
    
    for (const auto& msg : packet_log_messages) {
        ImGui::TextWrapped("%s", msg.c_str()); // text wrapped
    }
    
    if (ImGui::GetScrollY() >= ImGui::GetScrollMaxY()) {
        ImGui::SetScrollHereY(1.0f); // set scroll to bottom
    }
    
    ImGui::End();
}

void GameUI::set_player_number(int number) {
    player_number = number;
    Logger::info("GameUI", "set player number to " + std::to_string(number));
}

void GameUI::set_game_name(const std::string& name) {
    game_name = name;
    Logger::info("GameUI", "set game name to " + name);
}

void GameUI::set_player_name(const std::string& name) {
    player_name = name;
    Logger::info("GameUI", "set player name to " + name);
}

void GameUI::add_packet_log(const std::string& message) {
    packet_log_messages.push_back(message);
    if (packet_log_messages.size() > 200) {
        packet_log_messages.erase(packet_log_messages.begin()); // erase the first message
    }
} 