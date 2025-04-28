#ifndef CTS_H
#define CTS_H

#include <string>
#include "PacketTypes.h"

class Network;

class CtS {
public:
    explicit CtS(Network* network);
    ~CtS();

    bool connect(const std::string& player_name, const std::string& game_name);
    bool game_action(const json& action_data);
    bool disconnect(const std::string& reason = "Client disconnected");
    bool is_connected() const;

private:
    Network* network;
};

#endif 