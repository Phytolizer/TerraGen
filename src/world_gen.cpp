#include "world_gen.hpp"
#include "randomness.hpp"
#include <FastNoiseLite.h>
#include <chrono>

namespace WorldGen
{
World Generate(const WorldSize size)
{
    const auto start = std::chrono::high_resolution_clock::now();
    std::random_device _rd;
    std::default_random_engine eng{_rd()};
    constexpr int seed = 100;
    eng.seed(seed);
    FastNoiseLite n;

    std::vector<std::vector<Tile::Tile>> tiles;
    int width = 0;
    int height = 0;
    switch (size)
    {
    case WorldSize::Tiny:
        width = 1280;
        height = 720;
        break;
    case WorldSize::Small:
        width = 8400;
        height = 2400;
        break;
    case WorldSize::Medium:
        width = 12800;
        height = 3600;
        break;
    case WorldSize::Large:
        width = 16800;
        height = 4800;
        break;
    }
    tiles.reserve(width);
    for (int i = 0; i < width; ++i)
    {
        tiles.emplace_back(height);
    }

    /* LAYERS
     * 06% Space
     * 18% Surface
     *      - 12% Air
     *      - 6% Ground
     * 10% Underground
     * 35% Cavern (Water)
     * 20% Cavern (Lava)
     * 11% Underworld
     */

    // Choose Surface line (16-20% down)
    const int surface_line = Random::GetInt(static_cast<int>(height * 0.16), static_cast<int>(height * 0.20), eng);
    // double surfaceScale = 0.06;
    // double surfaceAmplitude;
    // int surfaceAmplitudeTime = 0;

    std::vector surface_heights(width, 0);
    // VARIABLES TO SET
    constexpr int spawn_area = 30;
    constexpr int spawn_area_amplitude = 1;
    constexpr int spawn_area_fallout = 60;
    constexpr double perlin_scale = 5;
    constexpr double perlin_amplitude_min = 0.5;
    constexpr double perlin_amplitude_max = 10;
    const int amplitude_time_min = width / 32;
    const int amplitude_time_max = width / 12;
    // Other Variables
    double perlin_amplitude = 0;
    int amplitude_time = 0;
    for (int i = 0; i < width; ++i)
    {
        const int dist = std::abs(i - width / 2);

        const double noise_scale_1 = n.GetNoise<double>(0, i * perlin_scale);
        const double noise_scale_2 = n.GetNoise<double>(0, i * perlin_scale / 2) / 2;
        const double noise_scale_4 = n.GetNoise<double>(0, i * perlin_scale / 4) / 4;

        const double noise = noise_scale_1 + noise_scale_2 + noise_scale_4;

        double amplitude;
        if (dist <= spawn_area)
        {
            amplitude = spawn_area_amplitude;
        }
        else
        {
            if (--amplitude_time <= 0)
            {
                amplitude_time = Random::GetInt(amplitude_time_min, amplitude_time_max, eng);
                perlin_amplitude = Random::GetDouble(perlin_amplitude_min, perlin_amplitude_max, eng);
            }
            if (dist <= spawn_area + spawn_area_fallout)
            {
                const double amp_switch = (dist - spawn_area) / spawn_area_fallout;
                amplitude = spawn_area_amplitude * (1 - amp_switch) + perlin_amplitude * amp_switch;
            }
            else
            {
                amplitude = perlin_amplitude; // * (1 + (width / 2) * static_cast<double>(dist) / width / 2);
            }
        }
        const int pos = surface_line + static_cast<int>(noise * amplitude);

        surface_heights[i] = pos;
    }

    // Choose Cavern line (32-36% down)
    const int cavern_line = Random::GetInt(static_cast<int>(height * 0.32), static_cast<int>(height * 0.36), eng);
    // double cavernScale = 0.02;
    // double cavernAmplitude = 10;

    std::vector cavern_heights(width, 0);
    for (int i = 0; i < width; ++i)
    {
        cavern_heights[i] = cavern_line;
    }

    for (int col = 0; col < width; ++col)
    {
        for (int row = 0; row < height; ++row)
        {
            if (row < surface_heights[col])
            {
                tiles[col][row].type = Tile::Type::Air;
            }
            else if (row < cavern_heights[col])
            {
                tiles[col][row].type = Tile::Type::Dirt;
            }
            else
            {
                tiles[col][row].type = Tile::Type::Stone;
            }
        }
    }

    /* BIOMES
     * Forest
     * Corruption/Crimson (mutually exclusive)
     * Ocean
     * Desert
     * Big desert (+caves)
     */

    const auto end = std::chrono::high_resolution_clock::now();
    const auto elapsed = std::chrono::duration_cast<std::chrono::milliseconds>(end - start);
    fmt::print("Took {} ms\n", elapsed.count());
    return World{std::move(tiles)};
}
} // namespace WorldGen
