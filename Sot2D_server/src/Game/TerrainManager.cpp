#include "TerrainManager.h"

#include <glm/gtc/random.hpp>
#include <Eis/Core/Random.h>

#include "../../Sot2D_client/src/Game/Island.cpp" // HACK: compile them files


void TerrainManager::InitStartingArea()
{
	EIS_PROFILE_FUNCTION();

	m_Islands.clear();
	m_Islands.reserve(50);

	Island startIsland(glm::vec2(0.0f), { Island::Type::OUTPOST, Island::Size::MEDIUM });
	startIsland.Init();
	m_Islands.push_back(startIsland);

	for (uint32_t i = 0; i < 49; i++)
	{
		glm::vec2 pos;
		_RETRY:
		pos = glm::vec2(Eis::Random::Float(-c_ChunkSize * 10, c_ChunkSize * 10), Eis::Random::Float(-c_ChunkSize * 10, c_ChunkSize * 10));
		if (DistToNearestIsland(pos) < c_MinDistBetwIslands)
			goto _RETRY;

		Island is(pos);
		is.Init();
		m_Islands.push_back(is);
	}
}

void TerrainManager::Clear()
{
	m_Islands.clear();
}

uint32_t TerrainManager::GetInitialIslandsNrToSend()
{
	return (uint32_t)m_Islands.size();
}

std::vector<Island>& TerrainManager::GetInitialIslandsToSend()
{
	return m_Islands;
}


float TerrainManager::DistToNearestIsland(const glm::vec2& pos)
{
	EIS_PROFILE_FUNCTION();

	float minDistance = std::numeric_limits<float>::max();
	for (const Island& i : m_Islands)
	{
		float d = glm::distance(pos, i.GetCenterPos());
		if (d < minDistance)
			minDistance = d;
	}

	return minDistance;
}