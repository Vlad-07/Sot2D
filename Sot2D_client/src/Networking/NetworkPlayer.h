#pragma once

#include "Game/Player.h"


typedef uint16_t ClientId;


class NetPlayer : public Player
{
public:
	NetPlayer(ClientId id, const glm::vec2& pos = glm::vec2(0)) : Player(pos), m_Id(id), m_ActualPos(pos) {}

	ClientId GetId() const { return m_Id; }

	void SetActualPos(const glm::vec2& pos) { m_ActualPos = pos; }

	void Interpolate()
	{
		glm::vec2 diff = m_ActualPos - m_Pos;

		m_Pos += diff * 0.5f;
	}

private:
	ClientId m_Id;
	glm::vec2 m_ActualPos;
};