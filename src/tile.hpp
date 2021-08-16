#ifndef TILE_HPP_
#define TILE_HPP_

#include <cstring>
extern "C"
{
#include <SDL_video.h>
}
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

    Type m_type{Type::Air};
    Liquid m_liquid{Liquid::None};
    double m_liquidLevel{0};
    Wall m_wall{Wall::Air};
    Depth m_depth{Depth::Overworld};

    void SetType(Type type);
    void SetLiquid(Liquid liquid);
    void SetWall(Wall wall);
    void SetDepth(Depth depth);
    [[nodiscard]] SDL_Color GetColor() const;
};

extern const std::unordered_map<Tile::Type, SDL_Color> TILE_TYPE_COLORS;
extern const std::unordered_map<Tile::Liquid, SDL_Color> LIQUID_COLORS;
extern const std::unordered_map<Tile::Wall, SDL_Color> WALL_COLORS;
extern const std::unordered_map<Tile::Depth, SDL_Color> DEPTH_COLORS;

#endif    // TILE_HPP_
