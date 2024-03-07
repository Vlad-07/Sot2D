#pragma once

#include <glm/glm.hpp>


class Island
{
public:
	enum class Type : uint8_t
	{ NONE = 0, OUTPOST, PLAIN };
	enum class Size : uint8_t
	{ NONE = 0, SMALL, MEDIUM, BIG, HUGE };

	struct Props
	{
		Type type;
		Size size;
	};

public:
	Island() : m_CenterPos(0), m_Props({ Type::NONE, Size::NONE }), m_Col(0), m_Radius(0.0f) {}
	Island(const glm::vec2& pos, const Props& props);
	~Island() = default;

	void Init();
	void Render();

	glm::vec2 GetCenterPos() const { return m_CenterPos; }



private:
	glm::vec2 m_CenterPos;
	Props m_Props;

	glm::vec4 m_Col; // temp for debugging terrain generation system
	float m_Radius;
};