#include "Island.h"

#include <queue>
#include <Eis.h>


Island::Island(const glm::vec2& pos, const Props& props)
	: m_CenterPos(pos), m_Props(props), m_Inited(false),
	  m_UsefulSurface(0), m_Tilemap()
{}
Island::Island(const Island& i)
	: m_CenterPos(i.m_CenterPos), m_Props(i.m_Props), m_Inited(false),
	  m_UsefulSurface(i.m_UsefulSurface), m_Tilemap(i.m_Tilemap)
{}
Island::Island(const Island&& i) noexcept
	: m_CenterPos(i.m_CenterPos), m_Props(i.m_Props), m_Inited(false),
	  m_UsefulSurface(i.m_UsefulSurface), m_Tilemap(i.m_Tilemap)
{}

void Island::operator=(const Island& i)
{
	m_CenterPos = i.m_CenterPos;
	m_Props = i.m_Props;
	m_UsefulSurface = i.m_UsefulSurface;
	m_Tilemap = i.m_Tilemap;
}

void Island::Init()
{
	/*
	switch (m_Props.type)
	{
	case Type::NONE:
		EIS_ASSERT(false, "Invalid island type! (NONE)");
		break;
	case Type::PLAIN:
		m_Col = glm::vec4(0.1f, 0.9f, 0.1f, 1.0f);
		break;
	case Type::OUTPOST:
		m_Col = glm::vec4(0.5f, 0.5f, 0.1f, 1.0f);
		break;
	}

	switch (m_Props.size)
	{
	case Size::NONE:
		EIS_ASSERT(false, "Invalid island size! (NONE)");
		break;
	case Size::SMALL:
		m_Radius = 10.0f;
		break;
	case Size::MEDIUM:
		m_Radius = 25.0f;
		break;
	case Size::BIG:
		m_Radius = 50.0f;
		break;
	case Size::HUGE:
		m_Radius = 70.0f;
		break;
	}//*/

	EIS_PROFILE_FUNCTION();

	EIS_ASSERT(m_Props.type != Type::NONE, "Invalid island type! (NONE)");
	EIS_ASSERT(m_Props.size != Size::NONE, "Invalid island size! (NONE)");

	if (m_UsefulSurface == 0)
		CreateIslandTilemap(*this);

	m_Inited = true;
}


void Island::Render() const
{
	EIS_PROFILE_FUNCTION();

	EIS_ASSERT(m_Inited, "Island not initialized!");

	for (uint32_t yOff = 0; yOff < m_UsefulSurface; yOff++)
	for (uint32_t xOff = 0; xOff < m_UsefulSurface; xOff++)
	{
		glm::vec4 color(1, 0, 1, 1);
		switch (m_Tilemap[yOff][xOff])
		{
		case Tile::NONE: break;
		case Tile::WATER: color = glm::vec4(0, 0, 1, 1); break;
		case Tile::OCEAN: color = glm::vec4(0, 0, 0.5f, 1); continue;
		case Tile::SAND: color = glm::vec4(1, 1, 0, 1); break;
		case Tile::GRASS: color = glm::vec4(0, 1, 0, 1); break;
		}
		Eis::Renderer2D::DrawQuad(m_CenterPos + glm::vec2((float)xOff, (float)yOff) * c_TileSize - glm::vec2((m_UsefulSurface + 1) / 2.0f), glm::vec2(c_TileSize), color);
	}
}



Island::Type Island::GetRandomType()
{
	EIS_PROFILE_FUNCTION();

	uint8_t roll = Eis::Random::UInt(0, 100);
	if (roll <= (uint8_t)Type::OUTPOST) return Type::OUTPOST;
	if (roll <= (uint8_t)Type::PLAIN) return Type::PLAIN;
	EIS_ASSERT(false, "Probabilities not set correctly!");
	return Type::NONE;
}
Island::Size Island::GetRandomSize()
{
	EIS_PROFILE_FUNCTION();

	uint8_t roll = Eis::Random::UInt(0, 100);
	if (roll <= (uint8_t)Size::HUGE) return Size::HUGE;
	if (roll <= (uint8_t)Size::BIG) return Size::BIG;
	if (roll <= (uint8_t)Size::MEDIUM) return Size::MEDIUM;
	if (roll <= (uint8_t)Size::SMALL) return Size::SMALL;
	EIS_ASSERT(false, "Probabilities not set correctly!");
	return Size::NONE;
}
Island::Props Island::GetRandomProps()
{
	EIS_PROFILE_FUNCTION();

	Props p;
	p.type = GetRandomType();
	_RETRY:
	p.size = GetRandomSize();
	if (p.type == Type::OUTPOST && p.size == Size::SMALL) goto _RETRY;
	return p;
}



static constexpr int dx[]{ -1, -1, -1,  0,  1, 1, 1, 0 },
					 dy[]{  1,  0, -1, -1, -1, 0, 1, 1 };

void Island::CreateIslandTilemap(Island& island)
{
	EIS_PROFILE_FUNCTION();

	switch (island.m_Props.size)
	{
	case Island::Size::SMALL: island.m_UsefulSurface = 15; break;
	case Island::Size::MEDIUM: island.m_UsefulSurface = 35; break;
	case Island::Size::BIG: island.m_UsefulSurface = 55; break;
	case Island::Size::HUGE: island.m_UsefulSurface = c_MaxTilemapSize; break;
	}

	EIS_ASSERT(island.m_UsefulSurface % 2 == 1, "Islands must have a center tile!");

	bool spawnBuildings;
	switch (island.m_Props.type)
	{
	case Island::Type::PLAIN: spawnBuildings = false; break;
	case Island::Type::OUTPOST: spawnBuildings = true; break;
	}

	for (uint32_t k = 0; k < island.m_UsefulSurface; k++)
	{
		island.m_Tilemap[k][0] = island.m_Tilemap[k][island.m_UsefulSurface - 1] =
			island.m_Tilemap[0][k] = island.m_Tilemap[island.m_UsefulSurface - 1][k] = Tile::WATER;
	}


	std::queue<glm::uvec2> q;
	q.push(glm::uvec2((island.m_UsefulSurface + 1) / 2, (island.m_UsefulSurface + 1) / 2));
	while (!q.empty())
	{
		glm::uvec2& pos = q.front();

	//	static std::vector<Tile> invalidTiles;

		// neighbout stuff
		for (int k = 0; k < 8; k++)
		{
			glm::uvec2 neighbour(pos.x + dx[k], pos.y + dy[k]);

			if (neighbour.x >= island.m_UsefulSurface || neighbour.y >= island.m_UsefulSurface)
				continue;

			if (island.m_Tilemap[neighbour.y][neighbour.x] == Tile::NONE)
			{
				island.m_Tilemap[neighbour.y][neighbour.x] = Tile::PENDING;
				q.push(neighbour);
			}
			else // limit current tile
			{
			}
		}

//	_RETRY:
		float distToCenter = ((pos.x - (island.m_UsefulSurface + 1) / 2.0f) * (pos.x - (island.m_UsefulSurface + 1) / 2.0f)
							+ (pos.y - (island.m_UsefulSurface + 1) / 2.0f) * (pos.y - (island.m_UsefulSurface + 1) / 2.0f)) / (island.m_UsefulSurface * island.m_UsefulSurface / 4);
		if (distToCenter + Eis::Random::Float(-0.02f, 0.02f) < 0.3f)
			island.m_Tilemap[pos.y][pos.x] = Tile::GRASS;
		else if (distToCenter + Eis::Random::Float(-0.1f, 0.1f) < 0.7f)
			island.m_Tilemap[pos.y][pos.x] = Tile::SAND;
		else
			island.m_Tilemap[pos.y][pos.x] = Tile::WATER;


	//	for (const Tile& t : invalidTiles) if (island.m_Tilemap[pos.y][pos.x] == t) goto _RETRY;

		q.pop();
	//	invalidTiles.clear();
	}


	// Process water
	q.push(glm::uvec2(0, 0));
	while (!q.empty())
	{
		glm::uvec2& pos = q.front();

		if (island.m_Tilemap[pos.y][pos.x] == Tile::OCEAN)
		{
			q.pop();
			continue;
		}

		if (island.m_Tilemap[pos.y][pos.x] == Tile::WATER)
			island.m_Tilemap[pos.y][pos.x] = Tile::OCEAN;

		for (int k = 0; k < 8; k++)
		{
			glm::uvec2 neighbour(pos.x + dx[k], pos.y + dy[k]);

			if (neighbour.x >= island.m_UsefulSurface || neighbour.y >= island.m_UsefulSurface)
				continue;

			if (island.m_Tilemap[neighbour.y][neighbour.x] == Tile::WATER)
				q.push(neighbour);
		}

		island.m_Tilemap[pos.y][pos.x] = Tile::OCEAN;

		q.pop();
	}
}