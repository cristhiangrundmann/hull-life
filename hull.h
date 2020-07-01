#ifndef __HULL__
#define __HULL__

#include <glm/glm.hpp>
#include <vector>

using namespace glm;
using std::vector;

#define LUMP_ENTITIES      0
#define LUMP_PLANES        1
#define LUMP_TEXTURES      2
#define LUMP_VERTICES      3
#define LUMP_VISIBILITY    4
#define LUMP_NODES         5
#define LUMP_TEXINFO       6
#define LUMP_FACES         7
#define LUMP_LIGHTING      8
#define LUMP_CLIPNODES     9
#define LUMP_LEAVES       10
#define LUMP_MARKSURFACES 11
#define LUMP_EDGES        12
#define LUMP_SURFEDGES    13
#define LUMP_MODELS       14
#define HEADER_LUMPS      15

#define CONTENTS_EMPTY        -1
#define CONTENTS_SOLID        -2
#define CONTENTS_WATER        -3
#define CONTENTS_SLIME        -4
#define CONTENTS_LAVA         -5
#define CONTENTS_SKY          -6
#define CONTENTS_ORIGIN       -7
#define CONTENTS_CLIP         -8
#define CONTENTS_CURRENT_0    -9
#define CONTENTS_CURRENT_90   -10
#define CONTENTS_CURRENT_180  -11
#define CONTENTS_CURRENT_270  -12
#define CONTENTS_CURRENT_UP   -13
#define CONTENTS_CURRENT_DOWN -14
#define CONTENTS_TRANSLUCENT  -15

#define MAX_MAP_HULLS 4

#define PLANE_X 0
#define PLANE_Y 1
#define PLANE_Z 2
#define PLANE_ANYX 3
#define PLANE_ANYY 4
#define PLANE_ANYZ 5

struct LUMP
{
    int32_t pos;
    int32_t size;
};

struct HEADER
{
    int32_t version;
    LUMP lump[HEADER_LUMPS];
};

struct CLIPNODE
{
    int32_t plane;
    int16_t next[2];
};

struct MODEL
{
    vec3 min, max;
    vec3 org;
    int32_t hull[MAX_MAP_HULLS];
    int32_t visleafs;
    int32_t firstface, faces;
};

struct LINE
{
    vec3 pos, dir;
    float t0, t1;
    union
    {
        bool bad;
        struct
        {
            bool invalid : 1;
            bool empty : 1;
        };
    };
};

struct PLANE
{
    vec3 normal;
    float dist;
    int32_t type; //not used?
};

struct TRIANGLE
{
    vec3 a, b, c;
    PLANE p;
};

void view(int, char**);

#define FAR (100000000)
#define PARALLEL (0.00000001)
#define MAX_DEPTH 256
#define LINEAT(x,y) (((x)*((x)-1)/2)+(y))

extern PLANE stack[];
extern LINE lines[];
extern int top;

extern int planes_num;
extern PLANE *planes;

extern int clipnodes_num;
extern CLIPNODE *clipnodes;

extern int models_num;
extern MODEL *models;

extern float fno_tolerance;

extern vector<TRIANGLE> solid_tris[3];
extern vector<TRIANGLE> empty_tris[3];

void push(PLANE& plane);
void pop(void);
bool brush(void);
void proc(void);

#endif