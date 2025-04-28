#ifndef STC_H
#define STC_H

#include <map>
#include <mutex>
#include <functional>
#include "PacketTypes.h"

class StC {
public:
    StC();
    ~StC();

    void register_callback(PacketType packet_type, PacketCallback callback);
    void unregister_callback(PacketType packet_type);
    void register_any_callback(PacketCallback callback);
    bool process_packet(PacketType packet_type, const json& data);

private:
    std::mutex callback_mutex;
    std::map<PacketType, PacketCallback> callbacks;

    PacketCallback any_packet_callback = nullptr;
    std::mutex any_callback_mutex;
};

#endif 