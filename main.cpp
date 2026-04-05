#define WIN32_LEAN_AND_MEAN
#define _WINSOCK_DEPRECATED_NO_WARNINGS
#define _CRT_SECURE_NO_WARNINGS

#include <windows.h>
#include <winsock2.h>
#include <ws2tcpip.h>
#include <stdio.h>

#pragma comment(lib, "Ws2_32.lib")

#include"Config.h"
#include "PacketManager/PacketManager.h"

SOCKET clients[2] = { INVALID_SOCKET, INVALID_SOCKET };
int active_clients = 0;
SOCKET server = INVALID_SOCKET;
int activePlayer = 0;
int server_bytes = 0;
int client_bytes[2] = { 0, 0 };

SOCKET connectToServer() {
    SOCKET ConnectSocket = INVALID_SOCKET;
    struct addrinfo* result = NULL;
    struct addrinfo* ptr = NULL;
    struct addrinfo hints;
    int iResult;

    ZeroMemory(&hints, sizeof(hints));
    hints.ai_family = AF_UNSPEC;
    hints.ai_socktype = SOCK_STREAM;
    hints.ai_protocol = IPPROTO_TCP;
    
    iResult = getaddrinfo(CURRENT_IP, SERVER_PORT, &hints, &result);
    if (iResult != 0) {
        printf("getaddrinfo failed: %d\n", iResult);
        return INVALID_SOCKET;
    }

    for (ptr = result; ptr != NULL; ptr = ptr->ai_next) {
        ConnectSocket = socket(ptr->ai_family, ptr->ai_socktype, ptr->ai_protocol);
        if (ConnectSocket == INVALID_SOCKET) {
            printf("socket() failed: %d\n", WSAGetLastError());
            continue;
        }

        int timeout = 10000;
        setsockopt(ConnectSocket, SOL_SOCKET, SO_RCVTIMEO, (const char*)&timeout, sizeof(timeout));
        setsockopt(ConnectSocket, SOL_SOCKET, SO_SNDTIMEO, (const char*)&timeout, sizeof(timeout));

        iResult = connect(ConnectSocket, ptr->ai_addr, (int)ptr->ai_addrlen);
        if (iResult == SOCKET_ERROR) {
            printf("connect() failed: %d\n", WSAGetLastError());
            closesocket(ConnectSocket);
            ConnectSocket = INVALID_SOCKET;
            continue;
        }

        char ipstr[INET6_ADDRSTRLEN];
        void* addr;

        if (ptr->ai_family == AF_INET) {
            struct sockaddr_in* ipv4 = (struct sockaddr_in*)ptr->ai_addr;
            addr = &(ipv4->sin_addr);
            inet_ntop(AF_INET, addr, ipstr, sizeof(ipstr));
        }
        else {
            struct sockaddr_in6* ipv6 = (struct sockaddr_in6*)ptr->ai_addr;
            addr = &(ipv6->sin6_addr);
            inet_ntop(AF_INET6, addr, ipstr, sizeof(ipstr));
        }
        printf("Connected to %s (IP: %s)\n", CURRENT_IP, ipstr);
        break;
    }

    freeaddrinfo(result);

    if (ConnectSocket == INVALID_SOCKET) {
        printf("All connection attempts failed\n");
    }

    return ConnectSocket;
}

void closeAllConnections() {
    for (int i = 0; i < 2; i++) {
        if (clients[i] != INVALID_SOCKET) {
            closesocket(clients[i]);
            clients[i] = INVALID_SOCKET;
        }
    }
    active_clients = 0;
    activePlayer = 0;

    if (server != INVALID_SOCKET) {
        closesocket(server);
        server = INVALID_SOCKET;
    }

    server_bytes = 0;
    for (int i = 0; i < 2; i++) {
        client_bytes[i] = 0;
    }
}

int addClient(SOCKET new_client) {

    switch (active_clients) {
    case 0:
        {
            active_clients++;
            clients[0] = new_client;

            printf("Client connected! Total: %d\n", active_clients);
            return 0;
        }
    case 1:
        {
            active_clients++;
            clients[1] = new_client;

            printf("Client connected! Total: %d\n", active_clients);
            return 1;
        }
    default:
        closesocket(new_client);
        return -1;
        break;
    }

}

void calcNextElement(int& startPos,int&endPos,const char* buf,int bufSize,int& toReadSize) {
    startPos = endPos;
    for (int i = endPos; i < bufSize; i++) {
        if (buf[i] == ' ' || buf[i] == '\0' || buf[i] == '\r') {
            endPos = i;
            toReadSize = endPos - startPos;
            while ((buf[endPos+1] == ' ' || buf[endPos+1] == '\0' || buf[endPos+1] == '\r') && i < bufSize) {
                endPos++;

            }
            endPos++;
            return;
        }
    }
}

int __cdecl main(void) {

    memset(CURRENT_IP, 0, sizeof(CURRENT_IP));
    memset(LOCAL_PORT, 0, sizeof(LOCAL_PORT));
    memset(SERVER_PORT, 0, sizeof(SERVER_PORT));

    FILE *hSettings = fopen("server.settings","rb");
    fseek(hSettings, 0, SEEK_END);

    unsigned int cfgSize = ftell(hSettings);
    fseek(hSettings, 0, SEEK_SET);
    char* buf = new char[cfgSize];
    fread(buf, 1, cfgSize, hSettings);

    int startPos = 0, endPos = 0, toRead = 0;

    calcNextElement(startPos, endPos, buf, cfgSize, toRead);

    memcpy(CURRENT_IP, buf + startPos, toRead);

    calcNextElement(startPos, endPos, buf, cfgSize, toRead);

    memcpy(LOCAL_PORT, buf + startPos, toRead);

    calcNextElement(startPos, endPos, buf, cfgSize, toRead);

    memcpy(SERVER_PORT, buf + startPos, toRead);

    delete[] buf;

    fclose(hSettings);
    
    WSADATA wsaData;
    int iResult = WSAStartup(MAKEWORD(2, 2), &wsaData);
    if (iResult != 0) {
        printf("WSAStartup failed: %d\n", iResult);
        return 1;
    }

    while (1) {
        SOCKET ListenSocket = INVALID_SOCKET;
        struct addrinfo* result = NULL;
        struct addrinfo hints;

        ZeroMemory(&hints, sizeof(hints));
        hints.ai_family = AF_INET;
        hints.ai_socktype = SOCK_STREAM;
        hints.ai_protocol = IPPROTO_TCP;
        hints.ai_flags = AI_PASSIVE;

        iResult = getaddrinfo(NULL, LOCAL_PORT, &hints, &result);
        if (iResult != 0) {
            printf("getaddrinfo failed: %d\n", iResult);
            Sleep(5000);
            continue;
        }

        ListenSocket = socket(result->ai_family, result->ai_socktype, result->ai_protocol);
        if (ListenSocket == INVALID_SOCKET) {
            printf("socket failed: %d\n", WSAGetLastError());
            freeaddrinfo(result);
            Sleep(5000);
            continue;
        }

        iResult = bind(ListenSocket, result->ai_addr, (int)result->ai_addrlen);
        freeaddrinfo(result);

        if (iResult == SOCKET_ERROR) {
            printf("Bind failed: %d\n", WSAGetLastError());
            closesocket(ListenSocket);
            Sleep(5000);
            continue;
        }

        iResult = listen(ListenSocket, SOMAXCONN);
        if (iResult == SOCKET_ERROR) {
            printf("Listen failed: %d\n", WSAGetLastError());
            closesocket(ListenSocket);
            Sleep(5000);
            continue;
        }

        printf("Waiting for clients...\n");

        while (active_clients < 2) {
            fd_set readfds;
            struct timeval tv;

            FD_ZERO(&readfds);
            FD_SET(ListenSocket, &readfds);

            tv.tv_sec = 0;
            tv.tv_usec = 50000;

            iResult = select(ListenSocket + 1, &readfds, NULL, NULL, &tv);
            if (iResult > 0 && FD_ISSET(ListenSocket, &readfds)) {
                SOCKET new_client = accept(ListenSocket, NULL, NULL);
                if (new_client != INVALID_SOCKET) {
                    int client_idx = addClient(new_client);
                    if (client_idx == 0 && active_clients == 1) {
                        server = connectToServer();
                        if (server == INVALID_SOCKET) {
                            printf("Failed to connect to server\n");
                            closeAllConnections();
                            break;
                        }
                        u_long mode = 1;
                        ioctlsocket(server, FIONBIO, &mode);
                    }
                }
            }

        }

        closesocket(ListenSocket);

        if (active_clients < 2) {
            printf("Not enough clients connected, restarting...\n");
            closeAllConnections();
            Sleep(1000);
            continue;
        }

        char recvbuf[DEFAULT_BUFLEN];
        char client_buf[2][DEFAULT_BUFLEN];
        int running = 1;

        PacketManager pManager;

        while (running && active_clients > 0) {
            fd_set readfds;
            struct timeval tv;
            FD_ZERO(&readfds);

            if (server != INVALID_SOCKET) {
                FD_SET(server, &readfds);
            }

            for (int i = 0; i < 2; i++) {
                if (clients[i] != INVALID_SOCKET) {
                    FD_SET(clients[i], &readfds);
                }
            }

            tv.tv_sec = 0;
            tv.tv_usec = 50000;

            int max_fd = 0;
            if (server > max_fd) max_fd = server;
            for (int i = 0; i < 2; i++) {
                if (clients[i] > max_fd) max_fd = clients[i];
            }

            iResult = select(max_fd + 1, &readfds, NULL, NULL, &tv);

            if (iResult == SOCKET_ERROR) {
                printf("select error: %d\n", WSAGetLastError());
            }

            if (server != INVALID_SOCKET && FD_ISSET(server, &readfds)) {
                iResult = recv(server, recvbuf, DEFAULT_BUFLEN, 0);

                if (iResult > 0) {
                    server_bytes += iResult;

                    for (int i = 0; i < 2; i++) {
                        if (clients[i] != INVALID_SOCKET) {
                            int sent = send(clients[i], recvbuf, iResult, 0);
                            if (sent == SOCKET_ERROR) {
                                printf("Failed to send to client %d: %d\n", i + 1, WSAGetLastError());
                                closesocket(clients[i]);
                                clients[i] = INVALID_SOCKET;
                                active_clients--;

                                if (i == 0) {
                                    activePlayer = 1;
                                }
                                else {
                                    MessageBox(0, L"2nd client disconnected!\n",MB_OK,0);
                                }
                            }
                        }
                    }
                }
                else if (iResult == 0) {
                    printf("Server disconnected\n");
                    running = 0;
                }
                else {
                    int error = WSAGetLastError();
                    if (error != WSAEWOULDBLOCK) {
                        printf("Server recv error: %d\n", error);
                        running = 0;
                    }
                }
            }

            for (int i = 0; i < 2; i++) {
                if (clients[i] != INVALID_SOCKET && FD_ISSET(clients[i], &readfds)) {
                    iResult = recv(clients[i], client_buf[i], DEFAULT_BUFLEN, 0);
                    

                    if (iResult > 0) {
                        client_bytes[i] += iResult;

                        if (i == activePlayer) {

                            pManager.processPacket((uint8_t*)client_buf[i], iResult);

                            if (server != INVALID_SOCKET) {
                                int sent = send(server, client_buf[i], iResult, 0);
                                if (sent == SOCKET_ERROR) {
                                    printf("Failed to send to server: %d\n", WSAGetLastError());
                                    goto cleanup;
                                }
                            }
                        }
                        else {
                            if (clients[0] == INVALID_SOCKET) {
                                activePlayer = 1;
                            }
                        }
                    }
                    else if (iResult == 0) {
                        printf("Client %d disconnected\n", i + 1);

                        if (i == 0) {
                            activePlayer = 1;
                        }

                        clients[i] = INVALID_SOCKET;
                        active_clients--;
                    }
                    else {
                        int error = WSAGetLastError();
                        if (error != WSAEWOULDBLOCK) {
                            printf("Client %d recv error: %d\n", i + 1, error);

                            clients[i] = INVALID_SOCKET;
                            active_clients--;
                            activePlayer = 1;
                            /*if (i == activePlayer) {
                                activePlayer = 1;
                            }*/
                        }
                    }
                }
            }

            if (active_clients == 0) {
                printf("All clients disconnected\n");
                running = 0;
            }
        }
cleanup:
        closeAllConnections();

        printf("Waiting 3 seconds before new session...\n");
        Sleep(3000);
    }

    closeAllConnections();
    WSACleanup();
    return 0;
}