#ifndef SDL_ENGINE

enum text_color{
    WHITE,
    BLACK,
    MAX_TEXT_COLOR
};


/* SDL interprets each pixel as a 32-bit number, so our masks must depend
   on the endianness (byte order) of the machine */
#if SDL_BYTEORDER == SDL_BIG_ENDIAN
    #define rmask 0xff000000
    #define gmask 0x00ff0000
    #define bmask 0x0000ff00
    #define amask 0x000000ff
#else
    #define rmask 0x000000ff
    #define gmask 0x0000ff00
    #define bmask 0x00ff0000
    #define amask 0xff000000
#endif



struct text_texture
{
    SDL_Texture* texture;

    int width;
    int height;
    // Pre_define position.
    int px;
    int py;
};

struct Size
{
    int width;
    int height;
};


struct v2d
{
    float x;
    float y;
};

struct rect
{
    float x;
    float y;
    float w;
    float h;

    v2d vel;
};


#define SDL_ENGINE
#endif
