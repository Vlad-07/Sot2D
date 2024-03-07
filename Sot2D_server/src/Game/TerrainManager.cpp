#include "TerrainManager.h"

#include <glm/gtc/random.hpp>
#include <Eis/Core/Random.h>

#include "../../Sot2D_client/src/Game/Island.cpp" // HACK: compile them files


void TerrainManager::InitStartingArea()
{
	m_Islands.clear();

	Island startIsland(glm::vec2(0.0f), { Island::Type::OUTPOST, Island::Size::MEDIUM });
	startIsland.Init();
	m_Islands.push_back(startIsland);

	for (uint32_t i = 0; i < 100; i++)
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

uint32_t TerrainManager::GetInitialIslandsNrToSend()
{
	return m_Islands.size();
}

Island* TerrainManager::GetInitialIslandsToSend()
{
	return m_Islands.data();
}


float TerrainManager::DistToNearestIsland(const glm::vec2& pos)
{
	float minDistance = std::numeric_limits<float>::max();
	for (const Island& i : m_Islands)
	{
		float d = glm::distance(pos, i.GetCenterPos());
		if (d < minDistance)
			minDistance = d;
	}

	return minDistance;
}