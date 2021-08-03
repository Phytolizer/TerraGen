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

    std::size_t width = 0;
    std::size_t height = 0;
    auto generatorBuilder = Details::Generator::Builder{seed};
    switch (size)
    {
    case WorldSize::Tiny:
        generatorBuilder.WithSize(1280, 720);
        break;
    case WorldSize::Small:
        generatorBuilder.WithSize(8400, 2400);
        break;
    case WorldSize::Medium:
        generatorBuilder.WithSize(12800, 3600);
        break;
    case WorldSize::Large:
        generatorBuilder.WithSize(16800, 4800);
        break;
    }
    auto generator = generatorBuilder.Build();
    std::vector<Tile::Tile> tiles;
    tiles.resize(width * height);

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

    // Choose Cavern line (32-36% down)
    const int cavern_line = Random::GetInt(static_cast<int>(height * 0.32), static_cast<int>(height * 0.36), eng);
    // double cavernScale = 0.02;
    // double cavernAmplitude = 10;

    std::vector surface_heights(width, 0);
    std::vector cavern_heights(width, 0);

    // TWEAKABLES
    constexpr int spawn_area = 30;
    constexpr int spawn_area_amplitude = 1;
    constexpr int spawn_area_fallout = 60;
    constexpr double perlin_scale = 5;
    constexpr double perlin_amplitude_min = 0.5;
    constexpr double perlin_amplitude_max = 10;
    const int amplitude_time_min = width / 32;
    const int amplitude_time_max = width / 12;
    const int amplitude_terp_time = width / 50;
    // Other Variables
    double perlin_amplitude = 0;
    double prev_perlin_amplitude = 0;
    double next_perlin_amplitude = 0;
    int amplitude_time = 0;
    int amplitude_terp_time_elapsed = 0;
    for (int i = 0; i < width; ++i)
    {
        const int dist = std::abs(i - static_cast<int>(width) / 2);

        const double noise_scale_1_surface = n.GetNoise<double>(0, seed + i * perlin_scale);
        const double noise_scale_2_surface = n.GetNoise<double>(0, seed + i * perlin_scale / 2) / 2;
        const double noise_scale_4_surface = n.GetNoise<double>(0, seed + i * perlin_scale / 4) / 4;
        const double noise_scale_2_cavern = n.GetNoise<double>(1000, seed + i * perlin_scale / 2) / 2;
        const double noise_scale_4_cavern = n.GetNoise<double>(1000, seed + i * perlin_scale / 4) / 4;
        const double noise_scale_8_cavern = n.GetNoise<double>(1000, seed + i * perlin_scale / 8) / 8;

        const double noise_surface = noise_scale_1_surface + noise_scale_2_surface + noise_scale_4_surface;
        const double noise_cavern = noise_scale_2_cavern + noise_scale_4_cavern + noise_scale_8_cavern;

        double amplitude;
        if (dist <= spawn_area)
        {
            amplitude = spawn_area_amplitude;
        }
        else
        {
            if (--amplitude_time <= 0)
            {
                if (amplitude_terp_time_elapsed == 0)
                {
                    next_perlin_amplitude = Random::GetDouble(perlin_amplitude_min, perlin_amplitude_max, eng);
                    prev_perlin_amplitude = perlin_amplitude;
                    if (perlin_amplitude == 0)
                    {
                        // no interpolation
                        amplitude_terp_time_elapsed = amplitude_terp_time;
                        perlin_amplitude = next_perlin_amplitude;
                    }
                }
                ++amplitude_terp_time_elapsed;
                if (amplitude_terp_time_elapsed < amplitude_terp_time)
                {
                    double elapsed_prop = static_cast<double>(amplitude_terp_time_elapsed) / static_cast<double>(amplitude_terp_time);
                    perlin_amplitude = elapsed_prop * next_perlin_amplitude + (1 - elapsed_prop) * prev_perlin_amplitude;
                }
                else
                {
                    amplitude_terp_time_elapsed = 0;
                    amplitude_time = Random::GetInt(amplitude_time_min, amplitude_time_max, eng);
                    perlin_amplitude = next_perlin_amplitude;
                }
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
        const int pos = surface_line + static_cast<int>(noise_surface * amplitude);

        surface_heights[i] = pos;
        cavern_heights[i] = cavern_line + static_cast<int>(noise_cavern * amplitude);
    }

    for (int col = 0; col < width; ++col)
    {
        for (int row = 0; row < height; ++row)
        {
            if (row < surface_heights[col])
            {
                tiles[col + width * row].type = Tile::Type::Air;
            }
            else if (row == surface_heights[col])
            {
                tiles[col + width * row].type = Tile::Type::Grass;
            }
            else if (row < cavern_heights[col])
            {
                tiles[col + width * row].type = Tile::Type::Dirt;
            }
            else
            {
                tiles[col + width * row].type = Tile::Type::Stone;
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
    return World{std::move(tiles), width, height};
}
} // namespace WorldGen
