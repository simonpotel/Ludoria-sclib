#include "CtS.h"
#include "Network.h"
#include "utils/Logger.h"
#include <stdexcept>

CtS::CtS(Network* network) : network(network) {
    if (!network) {
        throw std::invalid_argument("Network pointer cannot be null");
    }
}

CtS::~CtS() {
}

bool CtS::connect(const std::string& player_name, const std::string& game_name) {
    return network->connect(player_name, game_name);
}

bool CtS::game_action(const json& action_data) {
    return network->send_packet(PacketType::GAME_ACTION, action_data);
}

bool CtS::disconnect(const std::string& reason) {
    return network->disconnect(reason);
}

bool CtS::is_connected() const {
    return network->is_connected();
} 