# Minecraft-Sync

![C++](https://img.shields.io/badge/C++-00599C?style=for-the-badge&logo=c%2B%2B&logoColor=white) 
![License](https://img.shields.io/badge/license-MIT-green?style=for-the-badge)
![Minecraft](https://img.shields.io/badge/Minecraft-1.8--1.20-blue?style=for-the-badge&logo=minecraft&logoColor=white)

High-performance Minecraft packet proxy designed for seamless traffic synchronization and real-time packet manipulation. It redirects local traffic to a target server through handshake modification.

---

### Disclaimer
This project is for educational purposes only. It is not affiliated with, endorsed by, or associated with Mojang Studios or Microsoft Corporation. Use this software at your own risk. I am not responsible for any bans or issues caused by using this tool on official servers.

---

### Features

- Handshake Injection: Modifies the outgoing handshake packet to inject the target server IP and redirect traffic seamlessly.
- - Client Synchronization: Supports up to 2 concurrent clients with primary/secondary control system.
  - - Low Latency: Written in pure C++ using WinSock2 and asynchronous I/O (select) for minimal overhead.
    - - Auto-Reconnect Loop: Automatically resets connection state and waits for new incoming sessions if a connection is lost.
     
      - ### Version Support
      - - Tested on: Minecraft Java Edition 1.8.x - 1.20.x (and any versions using standard VarInt packet handling).
        - - Note: Works with most modern versions due to generic packet forwarding.
         
          - ### Installation & Setup
         
          - 1. Build the Project:
            2.    - Open the solution in Visual Studio.
                  -    - Ensure you are targeting x86 or x64 with Ws2_32.lib linked.
                       -    - Build in Release mode for performance.
                        
                            - 2. Configure server.settings:
                              3.    Create a file named server.settings in the same folder as the executable with the following format (space or newline separated):
                              4.   target_server_ip   local_listen_port   target_server_port
                              5.      Example: mc.hypixel.net 25565 25565
                             
                              6.  3. Usage:
                                  4.    - Run the executable (as administrator if using protected ports).
                                        -    - Connect your Minecraft client to localhost:your_port.
                                         
                                             - ---

                                             ### Security & Verification
                                             To ensure the integrity of the build, please verify the binary against the following details:

                                             - SHA256: [PLEASE_INSERT_SHA256_HERE]
                                             - - VirusTotal Report: [Scan Link Placeholder]
                                              
                                               - ---
                                               Created by [nullptrflow](https://github.com/nullptrflow)
                                               
