#include "Island.h"

#include <Eis.h>


Island::Island(const glm::vec2& pos, const Props& props) : m_CenterPos(pos), m_Props(props), m_Col(0), m_Radius(0.0f)
{
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

void Island::Render()
{
	Eis::Renderer2D::DrawCircle(m_CenterPos, glm::vec2(m_Radius), m_Col);
}