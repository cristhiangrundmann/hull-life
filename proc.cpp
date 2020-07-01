#include "hull.h"
#include <stdio.h>

vector<TRIANGLE> solid_tris[3];
vector<TRIANGLE> empty_tris[3];

int hull = 0;

void insert(vector<TRIANGLE>& list)
{
    for(int i = 0; i < top; i++)
    {
        bool b = true;
        vec3 p;
        for(int j = 0; j < top; j++)
        {
            if(i == j) continue;
            int lo = i, hi = j;
            if(j < i)
            {
                hi = i;
                lo = j;
            }

            LINE& line = lines[LINEAT(hi, lo)];
            if(line.bad) continue;

            if(b)
            {
                if(line.t0 != -FAR) p = line.pos + line.t0*line.dir;
                else if(line.t1 != FAR) p = line.pos + line.t1*line.dir;
                else continue;
                b = false;
                //continue;
            }

            vec3 a = line.pos + line.t0*line.dir;
            vec3 b = line.pos + line.t1*line.dir;

            TRIANGLE t;
            t.a = a;
            t.b = b;
            t.c = p;
            t.p = stack[i];
            list.push_back(t);
        }
    }
}

float smallest = FAR;

bool small()
{
    float vol = 0;

    vec3 p1;
    bool bp1 = true;

    for(int i = 0; i < top; i++)
    {
        vec3 p2;
        bool bp2 = true;
        for(int j = 0; j < top; j++)
        {
            if(i == j) continue;
            int lo = i, hi = j;
            if(j < i)
            {
                hi = i;
                lo = j;
            }

            LINE& line = lines[LINEAT(hi, lo)];
            if(line.bad) continue;

            if(bp1)
            {
                if(line.t0 != -FAR) p1 = line.pos + line.t0*line.dir;
                else if(line.t1 != FAR) p1 = line.pos + line.t1*line.dir;
                else continue;
                bp1 = false;
                continue;
            }

            if(bp2)
            {
                if(line.t0 != -FAR) p2 = line.pos + line.t0*line.dir;
                else if(line.t1 != FAR) p2 = line.pos + line.t1*line.dir;
                else continue;
                bp2 = false;
                continue;
            }

            vec3 a = line.pos + line.t0*line.dir;
            vec3 b = line.pos + line.t1*line.dir;

            vol += abs( dot( p2 - p1, cross( a - p2, b - p2 ) ) );
        }
    }

    if(vol < smallest) smallest = vol;
    if(vol < 64) return true;

    return false;
}

bool traverse2(int16_t c)
{
    if(top >= MAX_DEPTH) return false;
    CLIPNODE& clipnode = clipnodes[c];

    PLANE& plane = stack[top];
    push(planes[clipnode.plane]);

    for(int i = 0; i < 2; i++)
    {
        if(clipnode.next[i] >= 0) 
        {
            if(traverse2(clipnode.next[i])) 
            {
                pop();
                return true;
            }
        }
        else 
        {
            switch(clipnode.next[i])
            {
                case CONTENTS_SOLID:
                    fno_tolerance = 0;
                    if(!brush()) break;
                    
                    for(int p1 = 1; p1 < top; p1++)
                    for(int p2 = 0; p2 < p1; p2++)
                    {
                        LINE& line = lines[LINEAT(p1, p2)];
                        if(line.bad) continue;
                        if(line.t0 == -FAR) continue;
                        if(line.t1 == FAR) continue;
                        pop();
                        return true;
                    }

                    break;
                case CONTENTS_EMPTY:
                    break;
            }
        }

        plane.normal *= -1;
        plane.dist *= -1;
    }

    pop();
    return false;
}
int16_t root;

void traverse(int16_t c)
{
    if(top >= MAX_DEPTH) return;
    CLIPNODE& clipnode = clipnodes[c];

    PLANE& plane = stack[top];
    push(planes[clipnode.plane]);

    for(int i = 0; i < 2; i++)
    {
        if(clipnode.next[i] >= 0) traverse(clipnode.next[i]);
        else 
        {
            switch(clipnode.next[i])
            {
                case CONTENTS_SOLID:
                    fno_tolerance = 0;
                    brush();
                    insert(solid_tris[hull]);
                    break;
                default:
                    fno_tolerance = 0;
                    brush();
                    if(small())
                    if(traverse2(root))
                    {
                        brush();
                        insert(empty_tris[hull]);
                    }
                    break;
            }
        }

        plane.normal *= -1;
        plane.dist *= -1;
    }

    pop();
}

#define THICKNESS (0.00)

void proc()
{
    for(int m = 0; m < models_num; m++)
    {
        MODEL& mdl = models[m];
        
        //printf("org: (%f, %f, %f)\n", mdl.org.x, mdl.org.y, mdl.org.z);
        //printf("min: (%f, %f, %f)\n", mdl.min.x, mdl.min.y, mdl.min.z);
        //printf("max: (%f, %f, %f)\n", mdl.max.x, mdl.max.y, mdl.max.z);

        //INSERT BOUNDING BOX
        
        top = 0;
      
        PLANE b;

        b.type = PLANE_X;
        b.normal = vec3(1, 0, 0); b.dist = mdl.min.x-THICKNESS; push(b);
        b.normal = vec3(-1, 0, 0); b.dist = -mdl.max.x-THICKNESS; push(b);

        b.type = PLANE_Y;
      
        b.normal = vec3(0, 1, 0); b.dist = mdl.min.y-THICKNESS; push(b);
        b.normal = vec3(0, -1, 0); b.dist = -mdl.max.y-THICKNESS; push(b);

        b.type = PLANE_Z;
        b.normal = vec3(0, 0, 1); b.dist = mdl.min.z-THICKNESS; push(b);
        b.normal = vec3(0, 0, -1); b.dist = -mdl.max.z-THICKNESS; push(b);

        for(hull = 0; hull < 3; hull++)
        {
            root = mdl.hull[hull+1];
            traverse(root);
        }
        //break;
    }
    printf("smallest: %f\n", smallest);
}
