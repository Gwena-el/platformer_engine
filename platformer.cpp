//#include "platformer.h"
#include <vector>

// -----------------------------------------------------------------------------------------
void Draw_Map(SDL_Renderer *renderer, map *map, int map_elements[])
{
    for(int y = 0; y < map->h; ++y)
    {
        for(int x = 0; x < map->w; ++x)
        {
            int i = y * map->w + x;
            if(map_elements[i] == 1 )
            {
                Draw_Rect(renderer, x * map->bloc_w, y * map->bloc_h, map->bloc_w, map->bloc_h);
            }
        }
    }
}

// -----------------------------------------------------------------------------------------
void Keyboard_State(creature *player)
{
    const Uint8 *state = SDL_GetKeyboardState(NULL);

    float speed_modif;
    if(player->in_contact)
        speed_modif = 100.0f;
    else
        speed_modif = 1.0f;
    
    if(state[SDL_SCANCODE_RIGHT] && player->vel.current.x < player->max_velocity)
    {
        player->vel.nouvel.x += 1.0f * speed_modif;
    }
    if(state[SDL_SCANCODE_LEFT] && player->vel.current.x > -player->max_velocity)
    {
        player->vel.nouvel.x -= 1.0f * speed_modif;
    }
    if(state[SDL_SCANCODE_UP] && player->vel.current.y > -200.0f)
    {
        if(!player->is_jumping)
        {
            player->vel.nouvel.y -= 250.0f;
            player->is_jumping = true;
        }
    }
    if(state[SDL_SCANCODE_DOWN] && player->vel.current.y < player->max_velocity)
    {
        player->vel.nouvel.y += 1.0f * speed_modif;
    }
}

void Get_Current_Player_Velocity(creature *player)
{
    player->vel.current.x += player->vel.nouvel.x;
    player->vel.current.y += player->vel.nouvel.y;
}

// -----------------------------------------------------------------------------------------
void Game_Loop(SDL_Renderer *renderer, map *map,
               std::vector<rect> &rectangle_list,
               creature *player,
               float target_second_per_frame, info_contact *contact)
{    
    float gravity = 3.0f;
    player->vel.nouvel.y += gravity;
    
    Keyboard_State(player);

    Get_Current_Player_Velocity(player);   

    std::vector<std::pair<int, float>> z; 
    for (size_t i = 0; i < rectangle_list.size(); i++)
    {       
        if (Creature_Vs_Rect(renderer,
                             player, &rectangle_list[i],
                             target_second_per_frame,
                             contact->point, contact->normal, contact->time))
        {
            z.push_back({ i, contact->time });
        }
    }


    // Do the sort
    std::sort(z.begin(), z.end(), [](const std::pair<int, float>& a, const std::pair<int, float>& b)
    {
        return a.second < b.second;
    });


    // Now resolve the collision in correct order
    bool current_contact = false;
    for (auto j : z)
    {
        if(Resolve_Creature_Vs_Rect(renderer,
                                     player, &rectangle_list[j.first],
                                     contact->normal,
                                     target_second_per_frame))
        {
            current_contact = true;
        }
    }
    if(player->in_contact)
    {
        if(!current_contact)
            player->in_contact = false;
    }               
    else
    {
        if(current_contact)
        {
            player->vel.current.x = 0.0f;
            player->vel.current.y = 0.0f;
            player->in_contact = true;
        }
    }

    player->bound.x += player->vel.current.x * target_second_per_frame;
    player->bound.y += player->vel.current.y * target_second_per_frame;    

}
