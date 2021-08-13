#include "tile.hpp"

Tile::Tile()
{
    m_type = Type::Air;
    m_liquid = Liquid::None;
    m_liquidLevel = 0;
    m_wall = Wall::Air;
}


void Tile::SetTile(Type type)
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

SDL_Color Tile::GetColor()
{
    if (m_type != Type::Air)
    {
        return t_Colors[m_type];
    }
    else if (m_liquid != Liquid::None)
    {
        return l_Colors[m_liquid];
    }
    else
    {
        return w_Colors[m_wall];
    }
}

std::unordered_map<Tile::Type, SDL_Color> t_Colors{
    {Tile::Type::Air, {0, 0, 0, 0}},
    {Tile::Type::Dirt, {151, 107, 75, 255}},
    {Tile::Type::Grass, {40, 182, 80, 255}},
    {Tile::Type::Sand, {190, 170, 80, 255}},
    {Tile::Type::Clay, {146, 81, 68, 255}},
    {Tile::Type::Stone, {128, 128, 128, 255}},
    {Tile::Type::Ash, {79, 65, 64, 255}},
};

std::unordered_map<Tile::Liquid, SDL_Color> l_Colors{
    {Tile::Liquid::None, {0, 0, 0, 0}},
    {Tile::Liquid::Water, {9, 61, 255, 255}},
    {Tile::Liquid::Lava, {253, 32, 3, 255}},
    {Tile::Liquid::Honey, {253, 32, 3, 255}},
};

std::unordered_map<Tile::Wall, SDL_Color> w_Colors{
    {Tile::Wall::Air, {123, 152, 254, 255}},
    {Tile::Wall::Dirt, {111, 67, 35, 255}},
    {Tile::Wall::Grass, {0, 142, 40, 255}},
};