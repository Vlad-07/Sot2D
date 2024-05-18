#include "World.h"

#include <Eis.h>


bool c_InfiniteRendering = true;


void World::AddIsland(const Island& island)
{
	m_Islands.push_back(island);
	m_Islands.back().Init();
}

void World::RenderIslands(const glm::vec2& playerPos)
{
//	if (m_Islands.size() < m_Islands.capacity()) // might drop island packets without this check (but looks better)
//		return;
	if (!m_TerrainMtx.try_lock())
		return;

	for (uint32_t i = 0; i < m_Islands.size(); i++)
	{
		if (glm::distance(playerPos, m_Islands[i].GetCenterPos()) < c_RenderDistance || c_InfiniteRendering)
			m_Islands[i].Render();
	}
	m_TerrainMtx.unlock();
}