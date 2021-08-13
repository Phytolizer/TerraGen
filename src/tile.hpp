#ifndef TILE_HPP_
#define TILE_HPP_

#include <SDL_video.h>
#include <cstdint>
#include <unordered_map>

class Tile
{
    public:
    enum class Type
    {
        Air,
        Dirt,
        Grass,
        Stone,
        Sand,
        Clay,
        Mud,
        Silt,
        Slush,
        Ash,
    };
    enum class Style
    {
        Full,
        HalfBrick,
        SlopeTopRight,
        SlopeTopLeft,
        SlopeBottomRight,
        SlopeBottomLeft,
    };
    enum class Liquid
    {
        None,
        Water,
        Lava,
        Honey,
    };
    enum class Wall
    {
        Air,
        Dirt,
        Grass,
    };

  public:
    Type m_type;
    Liquid m_liquid;
    double m_liquidLevel;
    Wall m_wall;
    
  public:
    Tile();
    void SetTile(Type type);
    void SetLiquid(Liquid liquid);
    void SetWall(Wall wall);
    SDL_Color GetColor();
};

extern std::unordered_map<Tile::Type, SDL_Color> t_Colors;
extern std::unordered_map<Tile::Liquid, SDL_Color> l_Colors;
extern std::unordered_map<Tile::Wall, SDL_Color> w_Colors;

#endif // TILE_HPP_
