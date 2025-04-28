#ifndef NETWORK_H
#define NETWORK_H

#include <string>
#include <thread>
#include <atomic>
#include "PacketTypes.h"
#include "Config.h"

class StC;

class Network {
public:
    Network();
    ~Network();

    bool connect(const std::string& player_name, const std::string& game_name);
    bool disconnect(const std::string& reason = "Client disconnected");
    bool is_connected() const { return sock != -1; }
    int get_socket() const { return sock; }

    void start_packet_receiver(StC* stc);
    void stop_packet_receiver();
    
    bool send_packet(PacketType type, const json& data);

private:
    void packet_receiver_loop(StC* stc);
    
    int sock = -1;
    Config config;
    std::thread receiver_thread;
    std::atomic<bool> receiver_running{false};
};

#endif 