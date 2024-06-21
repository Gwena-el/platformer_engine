#if !defined(PLAFORMER)

struct map
{    
    int w;
    int h;
    
    float bloc_w;
    float bloc_h;
};


struct creature_velocity
{
    v2d current;
    v2d nouvel;
};

struct creature
{
    rect bound;
    float max_velocity;
    creature_velocity vel;
    bool in_contact;
    bool is_jumping;
};


struct info_contact
{
    float time;
    v2d   point;
    v2d   normal;      
};

#define PLATFORMER
#endif
