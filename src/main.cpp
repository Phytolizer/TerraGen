#include "viewport.hpp"
#include "world_gen.hpp"

extern "C"
{
#include <SDL.h>
}

int main(int /*argc*/, char* /*argv*/[])
{
    SDL_Init(SDL_INIT_VIDEO);
    constexpr int WORLD_WIDTH = 1750;
    constexpr int WORLD_HEIGHT = 900;
    Viewport viewport{0, 0, WORLD_WIDTH, WORLD_HEIGHT, 1};
    World world = WorldGen::Generate(WorldSize::Tiny);
    bool run = true;
    while (run)
    {
        SDL_Event e;
        while (SDL_PollEvent(&e) != 0)
        {
            switch (e.type)
            {
            case SDL_QUIT:
                run = false;
                break;
            case SDL_WINDOWEVENT:
                switch (e.window.event)
                {
                case SDL_WINDOWEVENT_CLOSE:
                    run = false;
                    break;
                }
                break;
            }
        }
        viewport.Render(world);
        SDL_RenderPresent(viewport.renderer);
    }
    SDL_Quit();
    return 0;
}
