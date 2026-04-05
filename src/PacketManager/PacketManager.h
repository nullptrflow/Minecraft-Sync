#pragma once
#include <cstring>
#include <cstdint>

enum GameState {
	HANDSHAKING,
	STATUS,
	LOGIN,
	PLAY
};

struct PacketManager {

    GameState gameState = GameState::HANDSHAKING;

    void processInHandshake(uint8_t* buf, int& len);

	// c -> s
	void processPacket(uint8_t* buf, int& len);
};