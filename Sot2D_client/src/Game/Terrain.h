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

	void AddIsland(const Island& island);
	void UnloadFarIslands(const glm::vec2& playerPos);

	void RenderIslands(const glm::vec2& playerPos);

	void Clear() { m_Islands.clear(); }

private:
	std::vector<Island> m_Islands;
};