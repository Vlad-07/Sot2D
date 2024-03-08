#pragma once

#include <vector>
#include <glm/glm.hpp>

#include "../../Sot2D_client/src/Game/Island.h" // HACK


static constexpr float c_MinDistBetwIslands = 250.0f;
static constexpr float c_ChunkSize = 500.0f;
static constexpr uint8_t c_MaxIslandsPerChunk = 5;
static constexpr int32_t c_ChunksToLoad = 1; // Excluding the chunk you are in


class TerrainManager
{
public:
	TerrainManager() = default;
	~TerrainManager() = default;

	void InitStartingArea();
	void Clear();

	uint32_t GetInitialIslandsNrToSend();
	std::vector<Island>& GetInitialIslandsToSend();

private:
	std::vector<Island> m_Islands;

private:
	float DistToNearestIsland(const glm::vec2& pos);
};