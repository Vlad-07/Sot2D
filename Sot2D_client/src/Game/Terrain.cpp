#include "Terrain.h"

#include <Eis.h>


void Terrain::AddIsland(const Island& island)
{
	m_Islands.push_back(island);
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
	for (const Island& i : m_Islands)
	{
	//	if (glm::distance(playerPos, i.GetCenterPos()) < c_RenderDistance)
			i.Render();
	}
}