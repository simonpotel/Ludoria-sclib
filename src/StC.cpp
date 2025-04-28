#include "StC.h"
#include "utils/Logger.h"

StC::StC() {
}

StC::~StC() {
}

void StC::register_callback(PacketType packet_type, PacketCallback callback) {
    std::lock_guard<std::mutex> lock(callback_mutex);
    callbacks[packet_type] = callback; // add the callback to the map
    Logger::info("StC", "registered callback for packet type: " + std::to_string(static_cast<int>(packet_type))); // log the registered callback for packet type
}

void StC::unregister_callback(PacketType packet_type) {
    std::lock_guard<std::mutex> lock(callback_mutex);
    callbacks.erase(packet_type); // erase the callback from the map
    Logger::info("StC", "unregistered callback for packet type: " + std::to_string(static_cast<int>(packet_type))); // log the unregistered callback for packet type
}

void StC::register_any_callback(PacketCallback callback) {
    std::lock_guard<std::mutex> lock(any_callback_mutex);
    any_packet_callback = callback; // set the any_packet_callback
    Logger::info("StC", "registered any_packet callback"); // log the registered any_packet callback
}

bool StC::process_packet(PacketType packet_type, const json& data) {
    // first process with any_packet_callback if registered
    std::lock_guard<std::mutex> lock(any_callback_mutex);
    if (any_packet_callback) {
        try {
            any_packet_callback(data); // call the any_packet_callback
        } catch (const std::exception& e) {
            Logger::error("StC", "exception in any_packet callback: " + std::string(e.what())); // log the exception in the any_packet callback
        }
    }

    // then process with the specific callback if registered
    std::lock_guard<std::mutex> callback_lock(callback_mutex);
    auto it = callbacks.find(packet_type);
    
    if (it != callbacks.end()) { // if the packet type is registered
        try {
            it->second(data); // call the callback
            return true;
        } catch (const std::exception& e) {
            Logger::error("StC", "exception in callback for packet type " + 
                        std::to_string(static_cast<int>(packet_type)) + ": " + std::string(e.what()));
            return false;
        }
    } else {
        Logger::warning("StC", "no callback registered for packet type: " + 
                     std::to_string(static_cast<int>(packet_type))); // log the no callback registered for packet type
        return true;
    }
} 