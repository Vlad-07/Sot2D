#pragma once

#include <glm/glm.hpp>


class Island
{
public:
	// Values are probability cutoffs
	enum class Type : uint8_t
	{ NONE = 0, OUTPOST = 20, PLAIN = 100 };
	enum class Size : uint8_t
	{ NONE = 0, SMALL = 100, MEDIUM = 60, BIG = 30, HUGE = 10 };

	struct Props
	{
		Type type;
		Size size;
	};

public:
	Island() : m_CenterPos(0), m_Props({ Type::NONE, Size::NONE }), m_Col(0), m_Radius(0.0f) {}
	Island(const glm::vec2& pos, const Props& props = GetRandomProps());
	Island(const Island& i);
	Island(const Island&& i) noexcept;
	~Island() = default;

	void Init();
	void Render() const;

	glm::vec2 GetCenterPos() const { return m_CenterPos; }

	void operator =(const Island& i);

	static Type GetRandomType();
	static Size GetRandomSize();
	static Props GetRandomProps();

private:
	glm::vec2 m_CenterPos;
	Props m_Props;

	glm::vec4 m_Col; // temp for debugging terrain generation system
	float m_Radius;
};