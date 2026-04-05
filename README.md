# Minecraft-Sync 

A high-performance C++ network proxy for Minecraft that synchronizes multiple clients and handles real-time packet manipulation. It redirects local traffic to a target server through handshake modification.

> [!CAUTION]
> **Disclaimer:** This project is for educational purposes only. It is not affiliated with, endorsed by, or associated with Mojang Studios or Microsoft Corporation. Use this software at your own risk. I am not responsible for any bans or issues caused by using this tool on official servers.

---

### Features 🚀

- **Handshake Injection:** Modifies the outgoing handshake packet to inject the target server IP and redirect traffic seamlessly.
- **Client Synchronization:** Supports up to 2 concurrent clients with primary/secondary control system.
- **Low Latency:** Written in pure C++ using WinSock2 and asynchronous I/O (`select`) for minimal overhead.
- **Auto-Reconnect Loop:** Automatically resets connection state and waits for new incoming sessions if a connection is lost.

### Version Support 🎮
- **Tested on:** Minecraft Java Edition 1.8.x - 1.20.x (and any versions using standard VarInt packet handling).
- **Note:** Works with most modern versions due to generic packet forwarding.

### Installation & Setup 🛠️

1. **Build the Project:**
   - Open the solution in **Visual Studio**.
   - Ensure you are targeting `x86` or `x64` with **Ws2_32.lib** linked.
   - Build in **Release** mode for performance.

2. **Configure `server.settings`:**
   Create a file named `server.settings` in the same folder as the executable with the following format (space or newline separated):
   ```text
   target_server_ip   local_listen_port   target_server_port
