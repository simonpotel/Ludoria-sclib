#ifndef PACKET_TYPES_H
#define PACKET_TYPES_H

#include <functional>
#include "nlohmann/json.hpp"

using json = nlohmann::json;

enum class PacketType : uint8_t {
    CONNECT = 0x1,
    GAME_ACTION = 0x5,
    DISCONNECT = 0x8,
    
    PLAYER_ASSIGNMENT = 0x2,
    YOUR_TURN = 0x3,
    WAIT_TURN = 0x4,
    GAME_STATE = 0x6,
    PLAYER_DISCONNECTED = 0x7
};

using PacketCallback = std::function<void(const json&)>;

#endif 