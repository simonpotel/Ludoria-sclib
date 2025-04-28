## How the Client Reacts (Callbacks)

The server and client communicate through packets. When the server sends a packet to the client, it can be detected through a dedicated channel. By understanding which values correspond to specific events, we can use 'callbacks' to handle these packets. The Ludoria server sends packets when a client makes a move in the game. By setting up callbacks in your code that respond to server-to-client (StC) packets, you can implement game logic. All callback functions receive the data sent by the server, typically as a JSON object (`const json&`).

snippet :

```cpp
// callback function code
void on_your_turn(const json& data) {
    bool my_turn = true;
    
    std::cout << "It's your turn to play!" << std::endl;
    
    if (data.contains("game_state")) {
        std::cout << "Current game state: " << data["game_state"].dump() << std::endl;
    }
    
    json move;
    move["action"] = "place_piece";
    move["position"] = {3, 4};  
    
    // net.send_move(move);
}

// main logic
int main() {
    net.register_callback(PacketType::WAIT_TURN, on_wait_turn);
}
```