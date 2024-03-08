#pragma once

#include <vector>
#include <glm/glm.hpp>
#include "Island.h"


static constexpr float c_ChunkSize = 500.0f;
static constexpr float c_UnloadDistance = c_ChunkSize * 2 + 100.0f;
static constexpr float c_RenderDistance = c_ChunkSize;

class Terrain
{
public:
	Terrain() = default;
	~Terrain() = default;

	// Add and init island
	void AddIsland(const Island& island);
	void Clear() { m_Islands.clear(); }
	void Reserve(uint32_t islandCount) { m_Islands.reserve(islandCount); }

	void UnloadFarIslands(const glm::vec2& playerPos);

	void RenderIslands(const glm::vec2& playerPos);

public:
	std::mutex m_TerrainMtx;

private:
	std::vector<Island> m_Islands;
};