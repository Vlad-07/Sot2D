#pragma once

#include <vector>
#include <glm/glm.hpp>
#include "Island.h"


constexpr float c_RenderDistance = 500.0f;
extern bool c_InfiniteRendering; // HACK: sus

class World
{
public:
	World() : m_WorldSize(), m_Islands(), m_TerrainMtx() {}
	~World() = default;

	// Add and init island
	void AddIsland(const Island& island);
	void Reset() { m_Islands.clear(); m_WorldSize = 0.0f; }
	void Reserve(uint32_t islandCount) { m_Islands.reserve(islandCount); }

	void RenderIslands(const glm::vec2& playerPos);

	float GetWorldSize() const { return m_WorldSize; }
	void SetWorldSize(float size) { m_WorldSize = size; }
	uint32_t GetIslandCount() const { return m_Islands.size(); }
	uint32_t GetIslandCap() const { return m_Islands.capacity(); }

public:
	std::mutex m_TerrainMtx;

private:
	float m_WorldSize;
	std::vector<Island> m_Islands;
};