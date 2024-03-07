#include "Terrain.h"

#include <Eis.h>


Terrain::Terrain(uint32_t islandCount, const Island* islands) : m_Islands(islandCount)
{
	for (uint32_t i = 0; i < islandCount; i++)
		m_Islands[i] = islands[i];
}

void Terrain::AddIsland(const Island& island)
{
	m_Islands.push_back(island);
}


void Terrain::RenderIslands(const glm::vec2 playerPos)
{
	for (const Island& i : m_Islands)
	{
		if (glm::distance(playerPos, i.GetCenterPos()) > 100.0f)
			continue;

		// render island
	}
}

void Terrain::GenerateIslands(uint32_t count, const glm::vec2& nodePos)
{
	EIS_ASSERT(count, "Invalid island count to be generated!");
	if (IsNodePosValid(nodePos))
	{
		EIS_ERROR("Invalid node position!");
		return;
	}

	m_Nodes.push_back({ nodePos, c_NodeGenerationRadius });

	if (m_Islands.size() == 0) // generate main island
	{
		Island mainIsland(glm::vec2(0), { Island::Type::OUTPOST, Island::Size::MEDIUM });
		mainIsland.Init();
		m_Islands.push_back(mainIsland);
		count--;
	}

	for (uint32_t i = 0; i < count; i++)
	{
		//
	}
}

bool Terrain::GetValidIslandPos(const glm::vec2& pos) const
{
	return false;
}

bool Terrain::IsNodePosValid(const glm::vec2& pos) const
{
	for (const GenerationNode& n : m_Nodes)
		if (glm::distance(pos, n.pos) < c_NodeDistance)
			return false;
	return true;
}