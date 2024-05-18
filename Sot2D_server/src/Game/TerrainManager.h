#pragma once

#include <vector>
#include <glm/glm.hpp>

#include "../../Sot2D_client/src/Game/Island.h" // HACK

static constexpr uint16_t	c_MaxIslandCount	 = 50;
static constexpr float		c_WorldSize			 = 5000.0f;
static constexpr float		c_MinDistBetwIslands = 500.0f;


class TerrainManager
{
public:
	TerrainManager() = default;
	~TerrainManager() = default;

	void Init();

	void Clear() { m_Islands.clear(); }

	const std::vector<Island>& GetInitialIslandsToSend() const { return m_Islands; }

private:
	std::vector<Island> m_Islands;

private:
	float DistToNearestIsland(const glm::vec2& pos);
};