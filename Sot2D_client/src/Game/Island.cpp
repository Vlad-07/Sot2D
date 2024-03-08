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

//	Eis::Renderer2D::DrawCircle(m_CenterPos, glm::vec2(m_Radius), m_Col);

	for (int yOff = 0; yOff < m_UsefulSurface; yOff++)
	for (int xOff = 0; xOff < m_UsefulSurface; xOff++)
	{
		glm::vec4 color(1.0f, 0.0f, 1.0f, 1.0f);
		switch (m_Tilemap[yOff][xOff])
		{
		case Tile::NONE: break;
		case Tile::WATER: color = glm::vec4(0.1f, 0.1f, 1.0f, 1.0f); break;
		case Tile::DOCK: color = glm::vec4(0.0f, 0.0f, 0.0f, 1.0f); break;
		case Tile::SAND: color = glm::vec4(1.0f, 1.0f, 0.0f, 1.0f); break;
		case Tile::GRASS: color = glm::vec4(0.0f, 1.0f, 0.0f, 1.0f); break;
		case Tile::FOREST: color = glm::vec4(0.0f, 0.3f, 0.0f, 1.0f); break;
		case Tile::BUILDING: color = glm::vec4(0.6f, 0.3f, 0.0f, 1.0f); break;
		}
		Eis::Renderer2D::DrawQuad(m_CenterPos + glm::vec2((float)xOff, (float)yOff) * c_TileSize - glm::vec2(m_UsefulSurface / 2.0f), glm::vec2(c_TileSize), color);
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


void Island::CreateIslandTilemap(Island& island)
{
	EIS_PROFILE_FUNCTION();

	switch (island.m_Props.size)
	{
	case Island::Size::SMALL: island.m_UsefulSurface = 10 + 2; break;
	case Island::Size::MEDIUM: island.m_UsefulSurface = 26 + 2; break;
	case Island::Size::BIG: island.m_UsefulSurface = 50 + 2; break;
	case Island::Size::HUGE: island.m_UsefulSurface = c_MaxTilemapSize; break;
	}

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

	std::queue<glm::ivec2> q;
	q.push(glm::ivec2(island.m_UsefulSurface / 2, island.m_UsefulSurface / 2));
	while (!q.empty())
	{
		static int dx[]{ -1, -1, -1,  0,  1, 1, 1, 0 },
				   dy[]{  1,  0, -1, -1, -1, 0, 1, 1 };
		glm::ivec2& pos = q.front();

		static std::vector<Tile> invalidTiles;

		// neighbout stuff
		for (int k = 0; k < 8; k++)
		{
			glm::ivec2 neighbour(pos.x + dx[k], pos.y + dy[k]);
			if (neighbour.x >= 0 && neighbour.x < island.m_UsefulSurface
				&& neighbour.y >= 0 && neighbour.y < island.m_UsefulSurface)
			{
				if (island.m_Tilemap[neighbour.y][neighbour.x] == Tile::NONE)
				{
					island.m_Tilemap[neighbour.y][neighbour.x] = Tile::PENDING;
					q.push(neighbour);
				}
				else // limit current tile
				{
					switch (island.m_Tilemap[neighbour.y][neighbour.x])
					{
					case Tile::NONE:
					case Tile::PENDING:
					case Tile::GRASS:
						break;
					case Tile::WATER:
						invalidTiles.push_back(Tile::BUILDING);
						invalidTiles.push_back(Tile::FOREST);
						break;
					case Tile::DOCK:
						invalidTiles.push_back(Tile::DOCK);
						invalidTiles.push_back(Tile::BUILDING);
						invalidTiles.push_back(Tile::FOREST);
						break;
					case Tile::BUILDING:
					case Tile::FOREST:
						invalidTiles.push_back(Tile::WATER);
						invalidTiles.push_back(Tile::DOCK);
						break;
					}
				}
			}
		}

	_RETRY:
		island.m_Tilemap[pos.y][pos.x] = (Tile)Eis::Random::UInt(2, 7);
		for (Tile t : invalidTiles) if (island.m_Tilemap[pos.y][pos.x] == t) goto _RETRY;

		q.pop();
		invalidTiles.clear();
	}
}