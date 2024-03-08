#include "Terrain.h"

#include <Eis.h>


void Terrain::AddIsland(const Island& island)
{
	m_Islands.push_back(island);
	m_Islands.back().Init();
}

void Terrain::UnloadFarIslands(const glm::vec2& playerPos)
{
	for (uint32_t i = 0; i < m_Islands.size(); i++)
	{
		if (glm::distance(m_Islands[i].GetCenterPos(), playerPos) > c_UnloadDistance)
			m_Islands.erase(m_Islands.begin() + i), i--;
	}
}


void Terrain::RenderIslands(const glm::vec2& playerPos)
{
	if (m_Islands.size() < m_Islands.capacity())
		return;
	if (!m_TerrainMtx.try_lock())
		return;

	for (uint32_t i = 0; i < m_Islands.size(); i++)
	{
		if (glm::distance(playerPos, m_Islands[i].GetCenterPos()) < c_RenderDistance)
			m_Islands[i].Render();
	}
	m_TerrainMtx.unlock();
}