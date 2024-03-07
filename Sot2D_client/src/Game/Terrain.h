#pragma once

#include <vector>
#include <glm/glm.hpp>
#include "Island.h"


class Terrain
{
public:
	Terrain(uint32_t islandCount, const Island* islands);
	~Terrain() = default;

	void AddIsland(const Island& island);

	void RenderIslands(const glm::vec2 playerPos);

	void GenerateIslands(uint32_t count, const glm::vec2& pos);

private:
	bool GetValidIslandPos(const glm::vec2& pos) const;
	bool IsNodePosValid(const glm::vec2& nodePos) const;

	struct GenerationNode
	{
		glm::vec2 pos;
		float radius;
	};

private:
	std::vector<Island> m_Islands;
	std::vector<GenerationNode> m_Nodes;

private: // constants
	const float c_DistBetwIsl = 200.0f;
	const float c_NodeDistance = 600.0f;
	const float c_NodeGenerationRadius = 600.0f;
};