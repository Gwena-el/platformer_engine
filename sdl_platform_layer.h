#if !defined(SDL_PLATFORM_LAYER)

#include <cstdint>

// -----------------------------------------------------------------------------------------
struct Info_Frame_PS
{
    double ms_per_frame;
    double fTimeElapsed;
    double FPS;
    double MCPF;
};
 

// -----------------------------------------------------------------------------------------
struct rgb_color
{
    uint8_t r;
    uint8_t g;
    uint8_t b;
    uint8_t a;
};

#define SDL_PLATFORM_LAYER
#endif
