#include "sdl_platform_layer.h"
#include "sdl_engine.cpp"
#include "platformer.cpp"

#include <SDL2/SDL.h>
#include <x86intrin.h>
#include <vector>

// -----------------------------------------------------------------------------------------
internal Size SDL_Get_Window_Size(SDL_Window *window)
{
    int width;
    int height;
    SDL_GetWindowSize(window, &width, &height);

    return { width, height };
}


// -----------------------------------------------------------------------------------------
void Update_Map_Size(SDL_Window *window, Size *previous_window_size, map *map1, creature *player)
{
    Size window_size = SDL_Get_Window_Size(window);
    printf("window_size: %i\n", window_size.width);
    map1->bloc_w = window_size.width / map1->w;
    map1->bloc_h = window_size.height / map1->h;

    player->bound.x = (player->bound.x * (float)window_size.width) / (float)previous_window_size->width;
    player->bound.y = (player->bound.y * (float)window_size.height) / (float)previous_window_size->height;
    player->bound.w = map1->bloc_w / 2.0f;
    player->bound.h = map1->bloc_h / 2.0f;
    previous_window_size->width = window_size.width;
    previous_window_size->height = window_size.height;
}


// -----------------------------------------------------------------------------------------
internal int SDL_Get_Window_Refresh_Rate(SDL_Window *window)
{
    SDL_DisplayMode mode;
    int display_index = SDL_GetWindowDisplayIndex(window);
    // If we can't find the refresh rate, we'll return this:
    int default_refresh_rate = 30;
    if (SDL_GetDesktopDisplayMode(display_index, &mode) != 0)
    {
        return default_refresh_rate;
    }
    if (mode.refresh_rate == 0)
    {
        return default_refresh_rate;
    }
    return mode.refresh_rate;
}


// -----------------------------------------------------------------------------------------
internal float SDL_Get_Seconds_Elapsed(uint64_t old_counter, uint64_t current_counter)
{
    return ((float)(current_counter - old_counter) / (float)(SDL_GetPerformanceFrequency()));
}


// -----------------------------------------------------------------------------------------
internal bool Handle_Event(SDL_Event *event, Info_Frame_PS *info_frame, Size *current_window_size,
                           map *map1, creature *player)
{
    bool should_quit = false;
    switch (event->type) {
        case SDL_QUIT:
        {
            should_quit = true;
            printf("SDL_QUIT\n");
        }break;
                    
        case SDL_WINDOWEVENT:
        {
            
            SDL_Window *window = SDL_GetWindowFromID(event->window.windowID);
            switch(event->window.event)
            {
                case SDL_WINDOWEVENT_SIZE_CHANGED:
                {
                    // SDL_Renderer *renderer = SDL_GetRenderer(window);                    
                    printf("SDL_WINDOWEVENT_SIZE_CHANGED\n");
                    printf("current_window_size: %i\n", current_window_size->width);
                    Update_Map_Size(window, current_window_size, map1, player);
                } break;
                            
                case SDL_WINDOWEVENT_FOCUS_GAINED:
                {
                    printf("SDL_WINDOWEVENT_FOCUS_GAINED\n");
                } break;
                            
                case SDL_WINDOWEVENT_EXPOSED:
                {
                    printf("SDL_WINDOWEVENT_EXPOSED\n");
                } break;                            
            }
        } break;

        case SDL_KEYDOWN:
        case SDL_KEYUP:
        {
            SDL_Keycode keycode = event->key.keysym.sym;
            bool is_down = (event->key.state == SDL_PRESSED);
            // bool was_down = false;

            // if (event->key.state == SDL_RELEASED)
            // {
            //     was_down = true;
            // }
            // else if (event->key.repeat != 0)
            // {
            //     was_down = true;
            // }
        } break;
        
    }
    return should_quit;
}

// -----------------------------------------------------------------------------------------
int main(int argc, char *argv[])
{
    SDL_Init(SDL_INIT_VIDEO);
    srand(static_cast<uint32_t>(std::time(nullptr)));
    uint64_t perf_count_frequency = SDL_GetPerformanceFrequency();
    Info_Frame_PS frame_info = {};

    // Window position and dimension
    int window_width = 1280;
    int window_height = 960;
    int x;
    int y;
    
    // enumerate displays
    int displays = SDL_GetNumVideoDisplays();

    if(displays > 1)
    {
        SDL_Rect display_bounds[displays];
        for(int i = 0; i < displays; ++i)
            SDL_GetDisplayBounds(i, &display_bounds[i]);   

        // To center on second display
        x = display_bounds[1].x + ( (display_bounds[1].w / 2) - (window_width / 2) );
        y = display_bounds[1].y + ( (display_bounds[1].h / 2) - (window_height / 2) );
    }
    else
    {
        x = SDL_WINDOWPOS_UNDEFINED;
        y = SDL_WINDOWPOS_UNDEFINED;   
    }

    SDL_Window* window = SDL_CreateWindow("Platformer Project",
                                          x,
                                          y,
                                          window_width,
                                          window_height,
                                          SDL_WINDOW_RESIZABLE);

    if(window)
    {
        SDL_Renderer* renderer = SDL_CreateRenderer(window, -1, 0);

        printf("Refresh rate is %d Hz\n", SDL_Get_Window_Refresh_Rate(window));
        int game_update_hz = SDL_Get_Window_Refresh_Rate(window);
        float target_second_per_frame = 1.0f / (float)game_update_hz;

        if(renderer)
        {
            Init_Text_Surface((char *)"../data/font_16x16.bmp");
            bool running = true;
            SDL_Event event;

            // ----------------------------------------------------------------
#define map_width 16
#define map_height 12

            map map1;
            int map_elements[] = {
                1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,
                1,0,0,0,0,0,0,0,0,0,0,0,0,0,0,1,
                1,0,0,0,0,0,0,0,1,1,1,1,0,0,0,1,
                1,0,0,0,0,0,0,0,0,0,0,0,0,0,0,1,
                1,1,1,1,1,1,1,1,0,0,0,0,0,0,0,1,
                1,0,0,0,0,0,0,0,1,1,1,1,1,0,0,1,
                1,0,0,0,0,0,0,0,0,0,0,0,0,0,1,1,
                1,1,1,0,0,0,0,0,0,0,0,0,0,0,0,1,
                1,0,0,0,0,0,0,0,0,0,0,1,1,1,1,1,
                1,0,0,0,0,1,1,1,1,1,1,0,0,0,0,1,
                1,0,0,0,0,0,0,0,0,0,0,0,0,0,0,1,
                1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,
            };
            map1.w = 16;
            map1.h = 12;
            Size window_size = SDL_Get_Window_Size(window);
            map1.bloc_w = window_size.width / map1.w;
            map1.bloc_h = window_size.height / map1.h;

            // std::vector<rect> rectangle_list;
            // for(int y = 0; y < map1.h; ++y)
            //     for(int x = 0; x < map1.w; ++x)
            //     {
            //         int i = y * map1.w + x;
            //         if(map_elements[i])
            //         {
            //             rect temp = {
            //                 (float)x * map1.bloc_w,
            //                 (float)y * map1.bloc_h,
            //                 map1.bloc_w,
            //                 map1.bloc_h,
            //             };
            //             rectangle_list.push_back(temp);
            //         }
            //     }

            creature player = {};
            player.bound = { 1.0f * map1.bloc_w, 10.0f * map1.bloc_h, map1.bloc_w/2.0f, map1.bloc_h/2.0f };
            player.max_velocity = 100.0f;
            player.in_contact = false;
            player.is_jumping = false;

            // @TODO: Currently inside the main game loop function,
            // Don't forget to pull it out once we start refactoring stuff.
            float gravity = 1.0f;
            
            
            // ----------------------------------------------------------------            
            uint64_t last_counter = SDL_GetPerformanceCounter();
            uint64_t last_cycle_count = _rdtsc();
            Size current_window_size = {1280, 960};
            while(running)
            {
                while (SDL_PollEvent(&event))
                {
                    // @NOTE: Instead of returning a bool, we could return a struct with a bit more
                    // information about what happened during event poll...
                    if(Handle_Event(&event, &frame_info, &current_window_size, &map1, &player))
                    {
                        running = false;
                    }
                }
                // const Uint8 *state = SDL_GetKeyboardState(NULL);
                const Uint8 *state = SDL_GetKeyboardState(NULL);
                info_contact contact = {};

                
                std::vector<rect> rectangle_list;
                for(int y = 0; y < map1.h; ++y)
                    for(int x = 0; x < map1.w; ++x)
                    {
                        int i = y * map1.w + x;
                        if(map_elements[i])
                        {
                            rect temp = {
                                (float)x * map1.bloc_w,
                                (float)y * map1.bloc_h,
                                map1.bloc_w,
                                map1.bloc_h,
                            };
                            rectangle_list.push_back(temp);
                        }
                    }
                
                if(player.in_contact)
                {
                    player.vel.current.x = 0.0f;
                    player.vel.current.y = 0.0f;
                }
                player.vel.nouvel.x = 0.0f;
                player.vel.nouvel.y = 0.0f;
                

                Game_Loop(renderer, &map1, rectangle_list,
                          &player, target_second_per_frame, &contact);
                

                Draw_Map(renderer, &map1, map_elements);
                Draw_Rect(renderer, player.bound.x, player.bound.y,
                          player.bound.w, player.bound.h, {0, 255, 0, 255});
                

                
#if 0
#define gravity 100.0f
                v2d contact_point;
                v2d contact_normal;
                float contact_time;
                rgb_color fix_color;

                player.vel.y = 0.0f;
                player.vel.x = 0.0f;

                float step_time = 0.0f;
                if(state[SDL_SCANCODE_RIGHT] && player.vel.x < 100.0f && step_time < 0.5f)
                {
                    player.vel.x += 50.0f;
                    ++step_time;
                }
                if(state[SDL_SCANCODE_LEFT] && player.vel.x > -100.0f && step_time < 0.5f)
                {
                    player.vel.x -= 50.0f;
                    ++step_time;
                }
                if(state[SDL_SCANCODE_DOWN])
                {
                    player.vel.y += 1.0f;
                }                
                if((state[SDL_SCANCODE_UP]) && (player.in_contact == true))
                {
                    player.vel.y -= 200.0f;
                    player.in_contact = false;
                }               
                player.vel.y += gravity;

                
                std::vector<std::pair<int, float>> z; 
                for (size_t i = 0; i < rectangle_list.size(); i++)
                {
                    if (Dyn_Rect_Vs_Rect(renderer,
                                         &player, &rectangle_list[i],
                                         target_second_per_frame,
                                         contact_point, contact_normal, contact_time))
                    {
                        z.push_back({ i, contact_time });
                    }
                }


                // Do the sort
                std::sort(z.begin(), z.end(), [](const std::pair<int, float>& a, const std::pair<int, float>& b)
                {
                    return a.second < b.second;
                });

                if(z.size() == 0)
                    player.in_contact = false;
                
                // Now resolve the collision in correct order 
                for (auto j : z)
                {
                    Resolve_Dyn_Rect_Vs_Rect(renderer,
                                             &player, &rectangle_list[j.first],
                                             contact_normal,
                                             target_second_per_frame);
                }

                player.x += player.vel.x * target_second_per_frame;
                player.y += player.vel.y * target_second_per_frame;
                
                
                for(size_t i = 0; i < rectangle_list.size(); ++i)
                {
                    Draw_Rect(renderer, rectangle_list[i].x, rectangle_list[i].y,
                              rectangle_list[i].w, rectangle_list[i].h);
                }
                Draw_Rect(renderer, player.x, player.y, player.w, player.h, {0, 255, 0, 255});
#endif

                // ---------------------------------------------------------------
                // Print fps
                if(state[SDL_SCANCODE_P])
                {
                    text_texture fps_texture;
                    char fps[12];
                    sprintf(fps, "%.2f", frame_info.FPS);
                    //sprintf(fps, "%.2f", frame_info.ms_per_frame);
                    Create_Text_Texture(renderer, font_surface, &fps_texture,
                                        fps, 5, 0, WHITE);
                    SDL_Rect fps_rect = {
                        fps_texture.px, fps_texture.py, fps_texture.width, fps_texture.height
                    };
                    SDL_RenderCopy(renderer, fps_texture.texture, NULL, &fps_rect);                    
                }
                
                // -----------------------------------------------------------------
                SDL_RenderPresent(renderer);
                SDL_SetRenderDrawColor(renderer, 0,0,0, SDL_ALPHA_OPAQUE);                
                SDL_RenderClear(renderer);
               
                // -----------------------------------------------------------------               
                uint64_t end_cycle_count = _rdtsc();
                uint64_t end_counter = SDL_GetPerformanceCounter();
                uint64_t counter_elapsed = end_counter - last_counter;
                uint64_t cycles_elapsed = end_cycle_count - last_cycle_count;

                frame_info.ms_per_frame = (((1000.0f * (double)counter_elapsed) / (double)perf_count_frequency));
                frame_info.fTimeElapsed = ((((double)counter_elapsed) / (double)perf_count_frequency));
                frame_info.FPS = (double)perf_count_frequency / (double)counter_elapsed;                
                frame_info.MCPF = ((double)cycles_elapsed / (1000.0f * 1000.0f));

                // -----------------------------------------------------------------               
                // @NOTE: We use target_second_per_frame to update are physics, and "wait" for the
                // elapsed time to match our targeted frame rate.

                // @TODO:
                // @TODO:
                // @TODO: LOTS TO CHANGE!!!!!!!!!!!!!
                // @TODO: Check ScreenShot Handmade Hero 018 for implementation about 40 min in.
                // Need to display the frame after waiting!
                // Why wait after the frame is already renderer! ;)
                if (SDL_Get_Seconds_Elapsed(last_counter,
                                            SDL_GetPerformanceCounter()) < target_second_per_frame)
                {
                    int32_t time_to_sleep =
                        ((target_second_per_frame - SDL_Get_Seconds_Elapsed(last_counter, SDL_GetPerformanceCounter())) * 1000) - 1;
                    if (time_to_sleep > 0)
                    {
                        SDL_Delay(time_to_sleep);
                    }
                    // @TODO: I do not have a Assert! And I am not confortable putting
                    // the assertion macro from handmade hero...
                    // Look at std::assert ??
                    // Assert(SDL_Get_Seconds_Elapsed(LastCounter, SDL_GetPerformanceCounter()) < TargetSecondsPerFrame)

                    float elapsed_time = SDL_Get_Seconds_Elapsed(last_counter, SDL_GetPerformanceCounter());
                    while (elapsed_time < target_second_per_frame)
                    {
                        // Waiting...
                        elapsed_time = SDL_Get_Seconds_Elapsed(last_counter,
                                                               SDL_GetPerformanceCounter());
                    }
                }

                last_cycle_count = end_cycle_count;
                last_counter = end_counter;
            }
        }
        else
        {
            // @TODO: logging
        }
    }
    else
    {
        // @TODO: logging
    }
    
    return 0;
}
