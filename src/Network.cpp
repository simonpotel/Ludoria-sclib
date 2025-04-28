#include "Network.h"
#include "StC.h"
#include "utils/Logger.h"
#include <iostream>
#include <fstream>
#include <stdexcept>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <cstring>
#include <cerrno>
#include <thread>

Network::Network() {
    // load the server config
    if (!config.load_server_config()) {
        throw std::runtime_error("Failed to load server configuration");
    }
}

Network::~Network() {
    // disconnect from the server
    if (sock != -1) {
        disconnect();
    }
    // stop the packet receiver
    stop_packet_receiver();
}

bool Network::connect(const std::string& player_name, const std::string& game_name) {
    try {
        // create a socket
        sock = socket(AF_INET, SOCK_STREAM, 0);
        
        if (sock < 0) {
            Logger::error("Network", "socket creation failed: " + std::string(strerror(errno)));
            return false;
        }

        // create a server address
        sockaddr_in serv_addr; // server address
        memset(&serv_addr, 0, sizeof(serv_addr)); // clear the server address
        serv_addr.sin_family = AF_INET; // set the family to internet
        serv_addr.sin_port = htons(config.get_port()); // set the port to the port in the config

        // convert the host to a server address
        int convert_result = inet_pton(AF_INET, config.get_host().c_str(), &serv_addr.sin_addr); // convert the host to a server address
        
        if (convert_result <= 0) {
            // if the address is not supported, close the socket and return false
            Logger::error("Network", "bad address not supported: " + config.get_host());
            close(sock);
            sock = -1;
            return false;
        }

        int connection_status = ::connect(sock, (struct sockaddr*)&serv_addr, sizeof(serv_addr)); // connect to the server
        
        if (connection_status < 0) {
            // if the connection fails, close the socket and return false
            Logger::error("Network", "connection failed to " + config.get_host() + ":" + 
                        std::to_string(config.get_port()) + " - " + std::string(strerror(errno)));
            close(sock);
            sock = -1;
            return false;
        }

        // create a connect data (packet data)
        json connect_data = {
            {"player_name", player_name},
            {"game_name", game_name}
        };

        // send the connect data to the server
        return send_packet(PacketType::CONNECT, connect_data);
    } 
    catch (const std::exception& e) {
        // if an exception occurs, close the socket and return false
        Logger::error("Network", "exception during connection: " + std::string(e.what()));
        
        if (sock != -1) {
            close(sock);
            sock = -1;
        }
        
        return false;
    }
}

bool Network::disconnect(const std::string& reason) {
    // if the socket is not connected, return false
    if (sock == -1) {
        return false;
    }
    
    // create a disconnect data (packet data)
    json disconnect_data = {{"reason", reason}};
    
    // send the disconnect data to the server
    bool result = send_packet(PacketType::DISCONNECT, disconnect_data);
    
    // shutdown the socket
    shutdown(sock, SHUT_RDWR);
    // close the socket
    close(sock);
    // set the socket to -1
    sock = -1;
    
    return result;
}

void Network::start_packet_receiver(StC* stc) {
    // if the packet receiver is already running, return
    if (receiver_running) {
        return;
    }
    
    // if the StC is null, return
    if (!stc) {
        Logger::error("Network", "Cannot start packet receiver with null StC");
        return;
    }
    
    // set the receiver running to true
    receiver_running = true;
    receiver_thread = std::thread(&Network::packet_receiver_loop, this, stc);
}

void Network::stop_packet_receiver() {
    if (!receiver_running) {
        return;
    }
    
    receiver_running = false;
    
    if (receiver_thread.joinable()) { // if the receiver thread is joinable
        receiver_thread.join(); // join the receiver thread
    }
}

void Network::packet_receiver_loop(StC* stc) {
    const size_t buffer_size = 4096; // buffer size
    char buffer[buffer_size]; // buffer
    std::string partial_data; // partial data
    
    fd_set read_fds; // read file descriptor set
    struct timeval tv; // timeout
    
    while (receiver_running && sock != -1) { // while the receiver is running and the socket is connected
        FD_ZERO(&read_fds);
        FD_SET(sock, &read_fds);
        
        tv.tv_sec = 0;
        tv.tv_usec = 100000;  // 100ms timeout
        
        int select_result = select(sock + 1, &read_fds, NULL, NULL, &tv);
        
        if (select_result < 0) {
            Logger::error("Network", "select failed: " + std::string(strerror(errno)));
            break;
        }
        
        if (select_result == 0) {
            continue;  // no data
        }
        
        if (FD_ISSET(sock, &read_fds)) { // if the socket is set to read
            ssize_t bytes_read = recv(sock, buffer, buffer_size - 1, 0); // read the data from the socket
            
            if (bytes_read <= 0) { // if the bytes read are less than or equal to 0
                if (bytes_read < 0) { // if the read failed
                    Logger::error("Network", "recv failed: " + std::string(strerror(errno))); // log the error
                } else {
                    Logger::info("Network", "server closed connection"); // log the server closed connection
                }
                break;
            }
            
            buffer[bytes_read] = '\0'; // null terminate the buffer
            partial_data += buffer; // add the buffer to the partial data
            
            size_t pos; // position
            while ((pos = partial_data.find('\n')) != std::string::npos) { // while the position is not the end of the string
                std::string packet_str = partial_data.substr(0, pos); // get the packet string
                partial_data.erase(0, pos + 1); // erase the packet string from the partial data
                
                try {
                    json packet = json::parse(packet_str); // parse the packet string
                    int type = packet["type"].get<int>(); // get the packet type
                    json data = packet["data"]; // get the packet data
                    
                    stc->process_packet(static_cast<PacketType>(type), data); // process the packet
                } catch (const json::exception& e) {
                    Logger::error("Network", "json parsing error: " + std::string(e.what())); // log the json parsing error
                } catch (const std::exception& e) {
                    Logger::error("Network", "Error processing packet: " + std::string(e.what())); // log the error processing packet
                }
            }
        }
    }
    
    Logger::info("Network", "packet receiver thread exiting"); // log the packet receiver thread exiting
}

bool Network::send_packet(PacketType type, const json& data) {
    // if the socket is not connected, return false
    if (sock == -1) {
        Logger::error("Network", "cant send packet: not connected");
        return false;
    }

    try {
        // create a packet json
        json packet_json;
        packet_json["type"] = static_cast<int>(type);
        packet_json["data"] = data;

        // create a message to send
        std::string message_to_send = packet_json.dump() + "\n";

        Logger::info("Network", "sending packet type: " + std::to_string(static_cast<int>(type))); // log the sending packet type

        ssize_t bytes_sent = send(sock, message_to_send.c_str(), message_to_send.length(), MSG_NOSIGNAL); // send the message to the server

        if (bytes_sent < 0) {
            Logger::error("Network", "send failed: " + std::string(strerror(errno))); // log the send failed
            return false;
        }
        
        if (static_cast<size_t>(bytes_sent) != message_to_send.length()) {
            // if the bytes sent are not the same as the message to send, log the partial send
            Logger::warning("Network", "partial send happened. sent " + std::to_string(bytes_sent) + " of " + std::to_string(message_to_send.length()) + " bytes.");
            return false;
        }

        return true;
    } 
    catch (const json::exception& e) {
        // if the json error occurs, log the json error before sending
        Logger::error("Network", "json error before sending: " + std::string(e.what()));
        return false;
    } 
    catch (const std::exception& e) {
        // if the error occurs, log the error before sending
        Logger::error("Network", "error preparing packet: " + std::string(e.what()));
        return false;
    }
} 