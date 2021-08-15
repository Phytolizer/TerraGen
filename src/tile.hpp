#ifndef TILE_HPP_
#define TILE_HPP_

#include <SDL_video.h>
#include <cstdint>
#include <unordered_map>

struct Tile
{
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
        Copper,
        Iron,
        Silver,
        Gold,
        Web,
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
    enum class Depth
    {
        Space,
        Overworld,
        Underground,
        Cavern,
        Underworld,
    };

    Type m_type;
    Liquid m_liquid;
    double m_liquidLevel;
    Wall m_wall;
    Depth m_depth;

    Tile();
    void SetType(Type type);
    void SetLiquid(Liquid liquid);
    void SetWall(Wall wall);
    void SetDepth(Depth depth);
    SDL_Color GetColor();
};

extern std::unordered_map<Tile::Type, SDL_Color> TileTypeColors;
extern std::unordered_map<Tile::Liquid, SDL_Color> LiquidColors;
extern std::unordered_map<Tile::Wall, SDL_Color> WallColors;
extern std::unordered_map<Tile::Depth, SDL_Color> DepthColors;

#endif // TILE_HPP_
