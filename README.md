# Ludoria-sclib

## Build project

```bash
mkdir build
cd build

# configure the build folder
cmake ..

# build 
cmake --build .

# run 
./HelloWorld
``` 


Client 
```Json
./Ludoria-scbot 
Enter player name: hey
Enter game name: hey
[2025-04-28 22:19:22] [INFO] [Config] loaded config: host=127.0.0.1, port=5000
[2025-04-28 22:19:22] [INFO] [StC] registered callback for packet type: 2
connecting: hey as hey
[2025-04-28 22:19:22] [INFO] [Network] sending packet type: 1
connected!
[2025-04-28 22:19:22] [INFO] [Game] Assigned as player 1
```

Server
```Json
[2025-04-28 22:19:22] [INFO] [Server] New connection from ('127.0.0.1', 64676)
[2025-04-28 22:19:22] [INFO] [Server] Received Raw JSON from 127.0.0.1:64676: {"data":{"game_name":"hey","player_name":"hey"},"type":1}
[2025-04-28 22:19:22] [INFO] [Server] Processing packet from ('127.0.0.1', 64676): Type=CONNECT, Data={'game_name': 'hey', 'player_name': 'hey'}
[2025-04-28 22:19:22] [INFO] [Server] Connect request from hey for game 'hey'
[2025-04-28 22:19:22] [INFO] [Server] Created new game session: hey
[2025-04-28 22:19:22] [INFO] [Server] Sent JSON to ('127.0.0.1', 64676): {"type": 2, "data": {"player_number": 1, "game_id": "hey"}}
[2025-04-28 22:19:22] [INFO] [Server] Assigned player hey (('127.0.0.1', 64676)) as Player 1 in game hey
```