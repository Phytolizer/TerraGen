#include "tile.hpp"

Tile::Tile()
{
    m_type = Type::Air;
    m_liquid = Liquid::None;
    m_liquidLevel = 0;
    m_wall = Wall::Air;
    m_depth = Depth::Overworld;
}

void Tile::SetType(Type type)
{
    m_type = type;
}
void Tile::SetLiquid(Liquid liquid)
{
    m_liquid = liquid;
    m_liquidLevel = liquid == Liquid::None ? 0 : 1;
}
void Tile::SetWall(Wall wall)
{
    m_wall = wall;
}
void Tile::SetDepth(Depth depth)
{
    m_depth = depth;
}

SDL_Color Tile::GetColor()
{
    if (m_type != Type::Air)
    {
        return TileTypeColors[m_type];
    }
    else if (m_liquid != Liquid::None)
    {
        return LiquidColors[m_liquid];
    }
    else if (m_wall != Wall::Air)
    {
        return WallColors[m_wall];
    }
    else
    {
        return DepthColors[m_depth];
    }
}

std::unordered_map<Tile::Type, SDL_Color> TileTypeColors{
    {Tile::Type::Air, {0, 0, 0, 0}},
    {Tile::Type::Dirt, {151, 107, 75, 255}},
    {Tile::Type::Grass, {40, 182, 80, 255}},
    {Tile::Type::Stone, {128, 128, 128, 255}},
    {Tile::Type::Sand, {190, 170, 80, 255}},
    {Tile::Type::Clay, {146, 81, 68, 255}},
    {Tile::Type::Mud, {255, 100, 100, 255}},
    {Tile::Type::Silt, {146, 81, 68, 255}},
    {Tile::Type::Slush, {146, 81, 68, 255}},
    {Tile::Type::Ash, {79, 65, 64, 255}},
    {Tile::Type::Copper, {184, 115, 51, 255}},
    {Tile::Type::Iron, {210, 210, 210, 255}},
    {Tile::Type::Silver, {190, 190, 190, 255}},
    {Tile::Type::Gold, {255, 215, 20, 255}},
    {Tile::Type::Web, {250, 250, 250, 255}},
};

std::unordered_map<Tile::Liquid, SDL_Color> LiquidColors{
    {Tile::Liquid::None, {0, 0, 0, 0}},
    {Tile::Liquid::Water, {9, 61, 255, 255}},
    {Tile::Liquid::Lava, {253, 32, 3, 255}},
    {Tile::Liquid::Honey, {253, 32, 3, 255}},
};

std::unordered_map<Tile::Wall, SDL_Color> WallColors{
    {Tile::Wall::Air, {0, 0, 0, 0}},
    {Tile::Wall::Dirt, {111, 67, 35, 255}},
    {Tile::Wall::Grass, {0, 142, 40, 255}},
};

std::unordered_map<Tile::Depth, SDL_Color> DepthColors{
    {Tile::Depth::Space, {65, 64, 255, 255}},      {Tile::Depth::Overworld, {123, 152, 254, 255}},
    {Tile::Depth::Underground, {88, 61, 46, 255}}, {Tile::Depth::Cavern, {74, 67, 60, 255}},
    {Tile::Depth::Underworld, {50, 44, 38, 255}},
};