#pragma once

#include <array>
#include <glm/glm.hpp>
#include <Eis.h>


enum class Tile : uint8_t
{
	NONE = 0, PENDING, WATER, DOCK, SAND, GRASS, FOREST, BUILDING
};

static constexpr float c_TileSize = 2.0f;
static constexpr int c_MaxTilemapSize = 102;
typedef std::array<std::array<Tile, c_MaxTilemapSize>, c_MaxTilemapSize> Tilemap;

class Island
{
public:
	// Values are probability cutoffs
	enum class Type : uint8_t
	{ NONE = 0, OUTPOST = 20, PLAIN = 100 };
	enum class Size : uint8_t
	{ NONE = 0, SMALL = 100, MEDIUM = 60, BIG = 30, HUGE = 10 };

	struct Props
	{
		Type type;
		Size size;
	};

public:
	Island() : m_CenterPos(0), m_Props({ Type::NONE, Size::NONE }), m_Inited(false), m_UsefulSurface(0), m_Tilemap() {}
	Island(const glm::vec2& pos, const Props& props = GetRandomProps());
	Island(const Island& i);
	Island(const Island&& i) noexcept;
	~Island() = default;

	// Server only:
	void Init();

	// Client only:
	void Render() const;

	glm::vec2 GetCenterPos() const { return m_CenterPos; }

	void operator =(const Island& i);

	static Type GetRandomType();
	static Size GetRandomSize();
	static Props GetRandomProps();

private:
	static void CreateIslandTilemap(Island& island);

private:
	glm::vec2 m_CenterPos;
	Props m_Props;
	bool m_Inited;

	Tilemap m_Tilemap;
	uint32_t m_UsefulSurface;

	Eis::Ref<Eis::Texture2D> m_Ground;
};