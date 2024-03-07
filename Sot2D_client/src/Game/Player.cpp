#include "Player.h"


void Player::SetPos(const glm::vec2& pos)
{
	m_Rotation = asin((pos - m_Pos).y / glm::length(pos - m_Pos) * ((pos - m_Pos).x > 0 ? 1 : -1));
	if (isnan(m_Rotation)) m_Rotation = 0.0f;
	m_Pos = pos;
}