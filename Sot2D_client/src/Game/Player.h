#pragma once

#include <Eis.h>


class Player
{
public:
	Player() : m_Pos(0), m_Rotation(0.0f) {}
	Player(const glm::vec2& pos) : m_Pos(pos), m_Rotation(0.0f) {}
	virtual ~Player() = default;


	glm::vec2 GetPos() const { return m_Pos; }
	virtual void SetPos(const glm::vec2& pos);

	float GetRotation() const { return m_Rotation; }
	virtual void SetRotation(float rot) { m_Rotation = rot; }

	void LoadTexture(const std::string& texturePath = "assets/textures/player.png")
	{ m_Texture = Eis::Texture2D::Create(texturePath); }
	Eis::Ref<Eis::Texture2D> GetTexture() const { return m_Texture; }

protected:
	glm::vec2 m_Pos;
	float m_Rotation;
	Eis::Ref<Eis::Texture2D> m_Texture;
};