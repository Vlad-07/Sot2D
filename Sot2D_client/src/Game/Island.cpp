#include "Island.h"

#include <Eis.h>


Island::Island(const glm::vec2& pos, const Props& props) : m_CenterPos(pos), m_Props(props), m_Col(0), m_Radius(0.0f)
{}
Island::Island(const Island& i)
	: m_CenterPos(i.m_CenterPos), m_Props(i.m_Props), m_Col(i.m_Col), m_Radius(i.m_Radius)
{}
Island::Island(const Island&& i) noexcept
	: m_CenterPos(i.m_CenterPos), m_Props(i.m_Props), m_Col(i.m_Col), m_Radius(i.m_Radius)
{}

void Island::operator=(const Island& i)
{
	m_CenterPos = i.m_CenterPos;
	m_Props = i.m_Props;
	m_Col = i.m_Col;
	m_Radius = i.m_Radius;
}

void Island::Init()
{
	switch (m_Props.type)
	{
	case Type::NONE:
		EIS_ASSERT(false, "Invalid island type! (NONE)");
		break;
	case Type::PLAIN:
		m_Col = glm::vec4(0.1f, 0.9f, 0.1f, 1.0f);
		break;
	case Type::OUTPOST:
		m_Col = glm::vec4(0.5f, 0.5f, 0.1f, 1.0f);
		break;
	}

	switch (m_Props.size)
	{
	case Size::NONE:
		EIS_ASSERT(false, "Invalid island size! (NONE)");
		break;
	case Size::SMALL:
		m_Radius = 10.0f;
		break;
	case Size::MEDIUM:
		m_Radius = 25.0f;
		break;
	case Size::BIG:
		m_Radius = 50.0f;
		break;
	case Size::HUGE:
		m_Radius = 70.0f;
		break;
	}
}


void Island::Render() const
{
	Eis::Renderer2D::DrawCircle(m_CenterPos, glm::vec2(m_Radius), m_Col);
}



Island::Type Island::GetRandomType()
{
	uint8_t roll = Eis::Random::UInt(0, 100);
	if (roll <= (uint8_t)Type::OUTPOST) return Type::OUTPOST;
	if (roll <= (uint8_t)Type::PLAIN) return Type::PLAIN;
	EIS_ASSERT(false, "Probabilities not set correctly!");
	return Type::NONE;
}
Island::Size Island::GetRandomSize()
{
	uint8_t roll = Eis::Random::UInt(0, 100);
	if (roll <= (uint8_t)Size::HUGE) return Size::HUGE;
	if (roll <= (uint8_t)Size::BIG) return Size::BIG;
	if (roll <= (uint8_t)Size::MEDIUM) return Size::MEDIUM;
	if (roll <= (uint8_t)Size::SMALL) return Size::SMALL;
	EIS_ASSERT(false, "Probabilities not set correctly!");
	return Size::NONE;
}
Island::Props Island::GetRandomProps()
{
	Props p;
	p.type = GetRandomType();
	_RETRY:
	p.size = GetRandomSize();
	if (p.type == Type::OUTPOST && p.size == Size::SMALL) goto _RETRY;
	return p;
}