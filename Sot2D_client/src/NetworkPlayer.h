#pragma once

#include "Player.h"

typedef uint16_t ClientId;

class NetPlayer : public Player
{
public:
	NetPlayer(ClientId id) : Player(), m_Id(id) {}

	ClientId GetId() const { return m_Id; }

private:
	ClientId m_Id;
};