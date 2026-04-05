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
      - - Tested on: Minecraft Java Edition 1.8.x - 1.20.x.
        - - Note: Works with most modern versions due to generic packet forwarding.
         
          - ---

          ### Security & Verification
          To ensure the integrity of the build, please verify the binary against the following details:

          - **SHA256:** `D90168BDD70C8ED062CCA654E212F11B73A0AB6AC0BC09FD5A7D5F195ED2DEDD`
          - - **VirusTotal:** [Scan Report](https://www.virustotal.com/gui/file/d90168bdd70c8ed062cca654e212f11b73a0ab6ac0bc09fd5a7d5f195ed2dedd)
           
            - ---
            Created by [nullptrflow](https://github.com/nullptrflow)
            
