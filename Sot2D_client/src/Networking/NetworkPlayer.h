#pragma once


typedef uint32_t ClientId;


class NetworkPlayer
{
public:
	NetworkPlayer(ClientId id, const glm::vec2& pos = glm::vec2(0))
		: m_Id(id), m_DisplayPos(pos), m_ActualPos(pos), m_DisplayRotation(), m_ActualRotation() {}


	void LoadTexture() { m_Texture = Eis::Texture2D::Create("assets/textures/player.png"); }
	const auto& GetTexture() const { return m_Texture; }

	ClientId GetId() const { return m_Id; }
	float GetRotation() const { return m_DisplayRotation; }

	glm::vec2 GetPos() const { return m_DisplayPos; }
	void SetActualPos(const glm::vec2& pos)
	{
		m_ActualPos = pos;
		m_ActualRotation = glm::degrees(atan2f(m_ActualPos.y - m_DisplayPos.y, -m_ActualPos.x + m_DisplayPos.x) * -1.0f);
	}

	void Update(Eis::TimeStep ts)
	{
		glm::vec2 diff = m_ActualPos - m_DisplayPos;
		m_DisplayPos += diff * 0.5f;

		if (fabs(m_ActualRotation - m_DisplayRotation) > 181.0f) // fix going from -pi to pi/2
			m_DisplayRotation += 360.0f * (m_ActualRotation - m_DisplayRotation > 0.0f ? 1.0f : -1.0f);

		m_DisplayRotation += (m_ActualRotation - m_DisplayRotation) * 15.0f * ts; // interpolate rotation
	}

private:
	ClientId m_Id;
	glm::vec2 m_DisplayPos,
			  m_ActualPos;
	float m_DisplayRotation,
		  m_ActualRotation;
	Eis::Ref<Eis::Texture2D> m_Texture;
};