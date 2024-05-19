#include "WorldManager.h"

#include <glm/gtc/random.hpp>
#include <Eis/Core/Random.h>

#include "../../Sot2D_client/src/Game/Island.cpp" // HACK: compile them files


void WorldManager::Init()
{
	EIS_PROFILE_FUNCTION();

	m_Islands.clear();
	m_Islands.reserve(50);

	m_Islands.emplace_back(glm::vec2(0.0f), Island::Props(Island::Type::OUTPOST, Island::Size::MEDIUM));
	m_Islands.back().Init();

	for (uint32_t i = 1; i < c_MaxIslandCount; i++)
	{
		glm::vec2 pos;
	_RETRY:
		pos = glm::diskRand(c_WorldSize);
		if (DistToNearestIsland(pos) < c_MinDistBetwIslands)
			goto _RETRY;

		m_Islands.emplace_back(pos);
		m_Islands.back().Init();
	}
}

float WorldManager::DistToNearestIsland(const glm::vec2& pos)
{
	EIS_PROFILE_FUNCTION();

	float minDistance = std::numeric_limits<float>::max();
	for (const Island& i : m_Islands)
	{
		const float d = glm::distance(pos, i.GetCenterPos());
		if (d < minDistance)
			minDistance = d;
	}

	return minDistance;
}