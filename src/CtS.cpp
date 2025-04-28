#include "CtS.h"
#include "Network.h"
#include "utils/Logger.h"
#include "ui/GameUI.h"
#include <stdexcept>
#include <memory>

extern std::unique_ptr<GameUI> game_ui;

CtS::CtS(Network* network) : network(network) {
    if (!network) {
        throw std::invalid_argument("Network pointer cannot be null");
    }
}

CtS::~CtS() {
}

bool CtS::connect(const std::string& player_name, const std::string& game_name) {
    if (game_ui) {
        game_ui->add_packet_log("[CtS] Requesting connect: Player=" + player_name + ", Game=" + game_name);
    }
    return network->connect(player_name, game_name);
}

bool CtS::game_action(const json& action_data) {
    if (game_ui) {
        game_ui->add_packet_log("[CtS] Requesting game action: Data=" + action_data.dump());
    }
    return network->send_packet(PacketType::GAME_ACTION, action_data);
}

bool CtS::disconnect(const std::string& reason) {
    if (game_ui) {
        game_ui->add_packet_log("[CtS] Requesting disconnect: Reason=" + reason);
    }
    return network->disconnect(reason);
}

bool CtS::is_connected() const {
    return network->is_connected();
} 