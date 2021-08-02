#include "viewport.hpp"
#include "world_gen.hpp"
#include <SDL.h>
#include <fmt/format.h>

int main(int argc, char *argv[])
{
    SDL_Init(SDL_INIT_VIDEO);
    Viewport viewport{0, 0, 1280, 720, 1};
    World world = WorldGen::Generate(WorldSize::Tiny);
    bool run = true;
    while (run)
    {
        SDL_Event e;
        while (SDL_PollEvent(&e))
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
