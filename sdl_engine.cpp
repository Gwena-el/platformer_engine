#include <cstdint>
#include <SDL2/SDL.h>
#include <ctime>
#include <math.h>
#include <algorithm>

#include "sdl_engine.h"
#include "platformer.h"

#define internal static
#define local_persist static
#define global static

global SDL_Surface* font_surface;

// -----------------------------------------------------------------------------------------
float Get_Random_Number(float min, float max)
{
    static constexpr double fraction { 1.0 / (RAND_MAX + 1.0) };
    return min + static_cast<float>((max - min + 1) * (std::rand() * fraction));
}


// -----------------------------------------------------------------------------------------
void str_concat(char p[], char q[]) {
   int c, d;
   
   c = 0;
 
   while (p[c] != '\0') {
      c++;      
   }
 
   d = 0;
 
   while (q[d] != '\0') {
      p[c] = q[d];
      d++;
      c++;    
   }
 
   p[c] = '\0';
}


// -----------------------------------------------------------------------------------------
SDL_Texture* Load_Sprite_Texture(SDL_Renderer *renderer, char *path)
{
    SDL_Surface* sprite = SDL_LoadBMP(path);
    SDL_Texture* sprite_texture = SDL_CreateTextureFromSurface(renderer, sprite);
    SDL_FreeSurface(sprite);
    return sprite_texture;
}


// -----------------------------------------------------------------------------------------
void Draw_Line(SDL_Renderer *renderer, int x1, int y1, int x2, int y2,
               rgb_color color = { 255, 255, 255, 255 })
{
    SDL_SetRenderDrawColor(renderer, color.r, color.g, color.b, color.a);
    SDL_RenderDrawLine(renderer, x1, y1, x2, y2);
}


// -----------------------------------------------------------------------------------------
void Draw_Rect(SDL_Renderer *renderer, int x, int y, int w, int h,
               rgb_color color = { 255, 255, 255, 255 })
{
    SDL_Rect temp_rect = { x, y, w, h };

    SDL_SetRenderDrawColor(renderer, color.r, color.g, color.b, color.a);
    SDL_RenderDrawRect(renderer, &temp_rect);                
}

// -----------------------------------------------------------------------------------------
void Draw_Full_Rect(SDL_Renderer *renderer, int x, int y, int w, int h,
               rgb_color color = { 255, 255, 255, 255 })
{
    SDL_Rect test_rect = { x, y, w, h };

    SDL_SetRenderDrawColor(renderer, color.r, color.g, color.b, color.a);
    SDL_RenderFillRect(renderer, &test_rect);                
}


// -----------------------------------------------------------------------------------------
void Draw_Circle(SDL_Renderer *renderer, int xc, int yc, int x, int y, rgb_color color) 
{
    SDL_SetRenderDrawColor(renderer, color.r, color.g, color.b, color.a);
    SDL_RenderDrawPoint(renderer, xc+x, yc+y); 
    SDL_RenderDrawPoint(renderer, xc-x, yc+y); 
    SDL_RenderDrawPoint(renderer, xc+x, yc-y); 
    SDL_RenderDrawPoint(renderer, xc-x, yc-y); 
    SDL_RenderDrawPoint(renderer, xc+y, yc+x); 
    SDL_RenderDrawPoint(renderer, xc-y, yc+x); 
    SDL_RenderDrawPoint(renderer, xc+y, yc-x); 
    SDL_RenderDrawPoint(renderer, xc-y, yc-x);
    SDL_SetRenderDrawColor(renderer, 0, 0, 0, SDL_ALPHA_OPAQUE);
}


// -----------------------------------------------------------------------------------------
void Circle_Bres(SDL_Renderer *renderer, float xc, float yc, int r, rgb_color color) 
{ 
    int x = 0, y = r; 
    int d = 3 - 2 * r;
    int r_xc = round(xc);
    int r_yc = round(yc); 
    Draw_Circle(renderer, r_xc, r_yc, x, y, color); 
    while (y >= x) 
    { 
        // for each pixel we will 
        // draw all eight pixels 
          
        x++; 
  
        // check for decision parameter 
        // and correspondingly  
        // update d, x, y 
        if (d > 0) 
        { 
            y--;  
            d = d + 4 * (x - y) + 10; 
        } 
        else
            d = d + 4 * x + 6; 
        Draw_Circle(renderer, r_xc, r_yc, x, y, color); 
    } 
}

void Init_Text_Surface(char path[])
{
    font_surface = SDL_LoadBMP(path);
}

// -----------------------------------------------------------------------------------------
bool Create_Text_Texture(SDL_Renderer *renderer, SDL_Surface *font_surface,
                         text_texture *text_texture, char text[],
                         int px = 0, int py = 0, int color = 0)
{
    if(!font_surface)
    {
        printf("Font Surface isn't initialized.");
        return 0;
    }
    int t = 0;
    int max_width = 0;
    int num_lines = 1;
    int current_width = 0;
    
    while(text[t])
    {
        if(! ( (text[t] - '\n') == 0) ) 
            current_width++;
        else
        {
            num_lines++;
            if (current_width > max_width)
                max_width = current_width;
            
            current_width = 0;
        }        
        ++t;
    }

    if (current_width > max_width)
        max_width = current_width;

    int text_width = max_width * 16;
    int text_height = num_lines * 16;
    SDL_Surface* temp_surface = SDL_CreateRGBSurface(0, text_width, text_height, 32,
                                                      rmask, gmask, bmask, amask);

    // @NOTE: Change this if more color!
    int color_offset = (color == 0) ? 0 : 96;
    int x = 0; int y = 0;
    t = 0;
    while(text[t])
    {
        if(!( (text[t] - '\n') == 0 ))
        {
            int code = text[t] - 32;
            SDL_Rect src = { (code % 16) * 16, ((code / 16) * 16) + color_offset, 16, 16 };
            SDL_Rect dest = { x, y, 16, 16 }; 

            SDL_BlitSurface(font_surface, &src, temp_surface, &dest);
                                        
            x += 12; // NOTE: Should be every 16 pixel, but the letter are too spaced out.
                     // So 12 pixel looks much better.
       }
        else
        {
            x = 0;
            y += 16;
        }
        ++t;
    }

    text_texture->texture = SDL_CreateTextureFromSurface(renderer, temp_surface);    
    text_texture->width = text_width;
    text_texture->height = text_height;
    text_texture->px = px;
    text_texture->py = py;

    return 1;
}




// -----------------------------------------------------------------------------------------
// -----------------------------------------------------------------------------------------
struct Bitmap
{
    int width;
    int height;
    int bytes_per_pixel;
    void* pixels;
};

// -----------------------------------------------------------------------------------------
void Init_Bitmap(Bitmap *bitmap, int w, int h, int bytes_per_pixel)
{
    bitmap->width = w;
    bitmap->height = h;
    bitmap->bytes_per_pixel = bytes_per_pixel;
    bitmap->pixels = malloc(bitmap->width * bitmap->height * bitmap->bytes_per_pixel); 
}


// #define STB_TRUETYPE_IMPLEMENTATION 1
// #include "stb_truetype.h"

// // -----------------------------------------------------------------------------------------
// uint8_t* Get_Pixel(Bitmap *bitmap, int x, int y)
// {
//     uint8_t *pixel_index = (uint8_t *)((uint8_t *)bitmap->pixels + // pointer arithmetic
//                                  ( (y * bitmap->bytes_per_pixel) * bitmap->width) +
//                                  (x * bitmap->bytes_per_pixel) );

//     return pixel_index;
// }


// // -----------------------------------------------------------------------------------------
// FILE* Init_Font_File(uint8_t ttf_buffer[], char *filename)
// {
//     // @TODO: Need to assert if the file exist!
//     FILE* f = fopen(filename, "rb");
//     fread(ttf_buffer, 1, 1<<20, f);

//     return f;
// }


// //-----------------------------------------------------------------------------------------
// void Fill_Bitmap(Bitmap *bitmap, uint8_t ttf_buffer[], int x, int y, uint8_t *text)
// {
//     memset(bitmap->pixels, 0, bitmap->width * bitmap->height * bitmap->bytes_per_pixel);
    
//     stbtt_fontinfo font;
//     int s = 30;

//     stbtt_InitFont(&font, ttf_buffer, stbtt_GetFontOffsetForIndex(ttf_buffer, 0));

//     float scale = stbtt_ScaleForPixelHeight(&font, s);
//     int ascent = 0;
    
//     stbtt_GetFontVMetrics(&font, &ascent, 0, 0);
//     int baseline = (int)(ascent * scale);

//     int x_cursor = x;
//     while (*text)
//     {            
//         int advance, lsb, x0, y0, x1, y1;

//         int x_shift = floor(x_cursor);
    
//         stbtt_GetCodepointHMetrics(&font, *text, &advance, &lsb);
//         stbtt_GetCodepointBitmapBox(&font, *text, scale, scale, &x0, &y0, &x1, &y1);

//         if(text != (uint8_t *)" ")
//         {
//             uint8_t *pixel = Get_Pixel(bitmap, x_shift + x0, y + baseline + y0);
//             stbtt_MakeCodepointBitmap(&font, pixel,
//                                       bitmap->bytes_per_pixel * (x1-x0),
//                                       bitmap->bytes_per_pixel * (y1-y0),
//                                       bitmap->width * bitmap->bytes_per_pixel,
//                                       2 * scale, scale,
//                                       *text);
//         }

//         x_cursor += (advance * scale);
//         if (*(text + 1))
//             x_cursor += (scale * stbtt_GetCodepointKernAdvance(&font, *text, *(text + 1)));

//         text++;
//     }
// }


// -----------------------------------------------------------------------------------------
bool Does_Rays_Intersects(v2d p1, v2d p2, v2d n1, v2d n2)
{
    float u = (p1.y * n2.x + n2.y * p2.x - p2.y * n2.x - n2.y * p1.x) / (n1.x * n2.y - n1.y * n2.x);
    float v = (p1.x + n1.x * u - p2.x) / n2.x;

    return (u > 0 && v > 0);
}


// -----------------------------------------------------------------------------------------
v2d Get_v2d_Of_Intersection(v2d p1, v2d p2, v2d n1, v2d n2)
{
    v2d p1End = { p1.x + n1.x, p1.y + n1.y }; // another point in line p1->n1
    v2d p2End = { p2.x + n2.x, p2.y + n2.y }; // another point in line p2->n2

    float m1 = (p1End.y - p1.y) / (p1End.x - p1.x); // slope of line p1->n1
    float m2 = (p2End.y - p2.y) / (p2End.x - p2.x); // slope of line p2->n2

    float b1 = p1.y - m1 * p1.x; // y-intercept of line p1->n1
    float b2 = p2.y - m2 * p2.x; // y-intercept of line p2->n2

    float px = (b2 - b1) / (m1 - m2); // collision x
    float py = m1 * px + b1; // collision y

    return { px, py }; // return statement
}



// Returns 1 if the lines intersect, otherwise 0. In addition, if the lines 
// intersect the intersection point may be stored in the floats i_x and i_y.
char get_line_intersection(float p0_x, float p0_y, float p1_x, float p1_y, 
    float p2_x, float p2_y, float p3_x, float p3_y, float *i_x, float *i_y)
{
    float s1_x, s1_y, s2_x, s2_y;
    s1_x = p1_x - p0_x;     s1_y = p1_y - p0_y;
    s2_x = p3_x - p2_x;     s2_y = p3_y - p2_y;

    float s, t;
    s = (-s1_y * (p0_x - p2_x) + s1_x * (p0_y - p2_y)) / (-s2_x * s1_y + s1_x * s2_y);
    t = ( s2_x * (p0_y - p2_y) - s2_y * (p0_x - p2_x)) / (-s2_x * s1_y + s1_x * s2_y);

    if (s >= 0 && s <= 1 && t >= 0 && t <= 1)
    {
        // Collision detected
        if (i_x != NULL)
            *i_x = p0_x + (t * s1_x);
        if (i_y != NULL)
            *i_y = p0_y + (t * s1_y);
        return 1;
    }

    return 0; // No collision
}


// -----------------------------------------------------------------------------------------
struct Edge
{
    v2d start;
    v2d direction;
};

bool Rect_Collision(rect r1, rect r2)
{
    return ( (r1.x < r2.x + (float)r2.w) && (r1.x + (float)r1.w > r2.x) &&
             (r1.y < r2.y + (float)r2.h) && (r1.y + (float)r1.h > r2.y) );
}


bool v2d_Vs_Rect(v2d p, rect r)
{
    return ( (p.x >= r.x) && (p.y >= r.y) && (p.x < r.x + r.w) && (p.y < r.y + r.h) );
}

/* @NOTE: In the collision test, insure that t_hit_near is inferior to 1, 
          or collision will be detected before actual hit:

   if( Ray_Vs_Rect(&ray_ori, &ray_dir, &target,
                   contact_point, contact_normal, t_hit_near) && t_hit_near < 1.0f)
 */
bool Ray_Vs_Rect(SDL_Renderer *renderer,
                 v2d *ray_origin, v2d *ray_dir, rect *target,
                 v2d &contact_point, v2d &contact_normal, float &t_hit_near)
{
    contact_normal.x = 0;
    contact_normal.y = 0;
    contact_point.x = 0;
    contact_point.y = 0;

    // Cache division
    v2d invdir = {
        1.0f / ray_dir->x,
        1.0f / ray_dir->y
    };

    // Calculate intersections with rectangle bounding axes
    v2d t_near = {
        (target->x - ray_origin->x) * invdir.x,
        (target->y - ray_origin->y) * invdir.y
    };
    v2d t_far = {
        (target->x + target->w - ray_origin->x) * invdir.x,
        (target->y + target->h - ray_origin->y) * invdir.y
    };

    if (isnan(t_far.y) || isnan(t_far.x)) return false;
    if (isnan(t_near.y) || isnan(t_near.x)) return false;

    // Sort distances
    if (t_near.x > t_far.x) std::swap(t_near.x, t_far.x);
    if (t_near.y > t_far.y) std::swap(t_near.y, t_far.y);

    // Early rejection		
    if (t_near.x > t_far.y || t_near.y > t_far.x) return false;
    
    // Closest 'time' will be the first contact
    t_hit_near = (t_near.x >= t_near.y) ? t_near.x : t_near.y;

    // Furthest 'time' is contact on opposite side of target
    float t_hit_far = (t_far.x <= t_far.y) ? t_far.x : t_far.y;

    // Reject if ray direction is pointing away from object
    if (t_hit_far < 0)
        return false;

    // Contact point of collision from parametric line equation
    contact_point.x = ray_origin->x + t_hit_near * ray_dir->x;
    contact_point.y = ray_origin->y + t_hit_near * ray_dir->y;

    //Circle_Bres(renderer, contact_point.x, contact_point.y, 5, { 0, 0, 255, 255 });

    if (t_near.x > t_near.y)
    {        
        if (invdir.x < 0)
            contact_normal = { 1, 0 };
        else
            contact_normal = { -1, 0 };
    }
    else if (t_near.x < t_near.y)
    {       
        if (invdir.y < 0)
            contact_normal = { 0, 1 };
        else
            contact_normal = { 0, -1 };
    }

    // Note if t_near == t_far, collision is principly in a diagonal
    // so pointless to resolve. By returning a CN={0,0} even though its
    // considered a hit, the resolver wont change anything.
    return true;

}

bool Dyn_Rect_Vs_Rect(SDL_Renderer *renderer,
                      rect *dyn_rect, rect *fix_rect, float time_step,
                      v2d &contact_point, v2d &contact_normal, float &contact_time)
{
    // Check if dynamic rectangle is actually moving -
    // we assume rectangles are NOT in collision to start
    if (dyn_rect->vel.x == 0 && dyn_rect->vel.y == 0)
        return false;

    // Expand target rectangle by source dimensions
    rect expanded_target = {};
    expanded_target.x = fix_rect->x - dyn_rect->w / 2;
    expanded_target.y = fix_rect->y - dyn_rect->h / 2;
    expanded_target.w = fix_rect->w + dyn_rect->w;
    expanded_target.h = fix_rect->h + dyn_rect->h;

    v2d dy_ray_ori = {
        dyn_rect->x + dyn_rect->w / 2,
        dyn_rect->y + dyn_rect->h / 2
    };

    v2d dy_ray_dir = {
        dyn_rect->vel.x * time_step,
        dyn_rect->vel.y * time_step,
    };

    Draw_Line(renderer, dy_ray_ori.x, dy_ray_ori.y,
              dy_ray_ori.x + dy_ray_dir.x * 100.0f, dy_ray_ori.y + dy_ray_dir.y * 100.0f, { 255, 0, 0, 255 });

    if (Ray_Vs_Rect(renderer,
                    &dy_ray_ori, &dy_ray_dir, &expanded_target,
                  contact_point, contact_normal, contact_time))
         return (contact_time >= 0.0f && contact_time < 1.0f);
    else
        return false;
}


bool Resolve_Dyn_Rect_Vs_Rect(SDL_Renderer *renderer, rect *r_dynamic, rect *r_static,
                              v2d &contact_normal, float fTimeStep)
{
    v2d contact_point;
    float contact_time = 0.0f;
    if (Dyn_Rect_Vs_Rect(renderer,
                         r_dynamic, r_static,
                         fTimeStep,
                         contact_point, contact_normal, contact_time))
    {
        r_dynamic->vel.x += contact_normal.x * abs(r_dynamic->vel.x) * (1 - contact_time);
        r_dynamic->vel.y += contact_normal.y * abs(r_dynamic->vel.y) * (1 - contact_time);
        
        return true;
    }
    

    return false;
}




// -----------------------------------------------------------------------------------------
bool Creature_Vs_Rect(SDL_Renderer *renderer,
                      creature *creature, rect *fix_rect, float time_step,
                      v2d &contact_point, v2d &contact_normal, float &contact_time)
{
    // Check if dynamic rectangle is actually moving -
    // we assume rectangles are NOT in collision to start
    if (creature->vel.current.x == 0 && creature->vel.current.y == 0)
        return false;

    // Expand target rectangle by source dimensions
    rect expanded_target = {};
    expanded_target.x = fix_rect->x - creature->bound.w / 2;
    expanded_target.y = fix_rect->y - creature->bound.h / 2;
    expanded_target.w = fix_rect->w + creature->bound.w;
    expanded_target.h = fix_rect->h + creature->bound.h;

    v2d dy_ray_ori = {
        creature->bound.x + creature->bound.w / 2,
        creature->bound.y + creature->bound.h / 2
    };

    v2d dy_ray_dir = {
        creature->vel.current.x * time_step,
        creature->vel.current.y * time_step,
    };

    Draw_Line(renderer, dy_ray_ori.x, dy_ray_ori.y,
              dy_ray_ori.x + dy_ray_dir.x * 100.0f, dy_ray_ori.y + dy_ray_dir.y * 100.0f, { 255, 0, 0, 255 });

    if (Ray_Vs_Rect(renderer,
                    &dy_ray_ori, &dy_ray_dir, &expanded_target,
                  contact_point, contact_normal, contact_time))
         return (contact_time >= 0.0f && contact_time < 1.0f);
    else
        return false;
}


bool Resolve_Creature_Vs_Rect(SDL_Renderer *renderer, creature *creature, rect *r_static,
                              v2d &contact_normal, float fTimeStep)
{
    v2d contact_point;
    float contact_time = 0.0f;
    if (Creature_Vs_Rect(renderer,
                         creature, r_static,
                         fTimeStep,
                         contact_point, contact_normal, contact_time))
    {
        creature->vel.current.x += contact_normal.x * abs(creature->vel.current.x) * (1 - contact_time);
        creature->vel.current.y += contact_normal.y * abs(creature->vel.current.y) * (1 - contact_time);

        if(contact_normal.x == 0 && contact_normal.y == -1 && creature->is_jumping)
                creature->is_jumping = false;
        
        return true;
    }
    

    return false;
}
