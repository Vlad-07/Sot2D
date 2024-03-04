#pragma once

#include <Eis.h>

class Player
{
public:
	Player() : m_Pos(0), m_OldPos(0) {}
	virtual ~Player() = default;

	void LoadTexture(const std::string& texturePath = "assets/textures/player.png")
	{ m_Texture = Eis::Texture2D::Create(texturePath); }

	glm::vec2 GetPos() const { return m_Pos; }
	void SetPos(const glm::vec2& pos) { m_OldPos = m_Pos; m_Pos = pos; }
	void AddPos(const glm::vec2& pos) { m_OldPos = m_Pos; m_Pos += pos; }

	bool HasMoved() { if (m_OldPos != m_Pos) { m_OldPos = m_Pos; return true; } else return false; }

	Eis::Ref<Eis::Texture2D> GetTexture() const { return m_Texture; }

protected:
	glm::vec2 m_Pos, m_OldPos;
	Eis::Ref<Eis::Texture2D> m_Texture;
};