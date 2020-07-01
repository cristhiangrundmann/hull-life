#include "hull.h"
#include <stdio.h>

PLANE stack[MAX_DEPTH];
LINE lines[LINEAT(MAX_DEPTH, 0)];
int top;

void push(PLANE& plane)
{
    if(top >= MAX_DEPTH) return;

    for(int32_t i = 0; i < top; i++)
    {
        PLANE& p = stack[i];
		LINE& line = lines[LINEAT(top, i)];

        line.bad = false;
        line.t0 = -FAR;
        line.t1 = +FAR;

        line.dir = cross(p.normal, plane.normal);

		float c = dot(p.normal, plane.normal);

		float div = c*c - 1;

		if(abs(div) <= PARALLEL)
		{
			line.invalid = true;
			continue;
		}

		float a0 = c*plane.dist - p.dist;
		float a1 = c*p.dist - plane.dist;

        line.pos = (p.normal * a0 + plane.normal * a1) / div;

        float k = abs(p.dist - dot(line.pos, p.normal));

		line.dir = normalize(line.dir) * 10.0f;
    }

    stack[top++] = plane;
}

void pop()
{
    if(top > 0) top--;
}

float fno_tolerance = 0;

int line_count;

void trim(int lo, int hi, int p)
{
    LINE& line = lines[LINEAT(hi, lo)];
    if(line.bad) return;

    PLANE& plane = stack[p];

	float nd = dot(plane.normal, line.dir);
	float fno = plane.dist - dot(plane.normal, line.pos);

	if(nd == 0)
	{
		if(fno > fno_tolerance)
        {
            line_count--;
            line.empty = true;
        }
		return;
	}

	float t = fno / nd;

	if(nd > 0) { if(t > line.t0) line.t0 = t; }
    else
    if(nd < 0) { if(t < line.t1) line.t1 = t; }

	if(line.t0 > line.t1) 
    {
        line_count--;
        line.empty = true;
    }
}


bool brush()
{
    int lc = LINEAT(top, 0);
    line_count = lc;
    for(int i = 0; i < lc; i++)
    {
        LINE& line = lines[i];
        if(line.invalid) 
        {
            line_count--;
            continue;
        }
        line.empty = false;
        line.t0 = -FAR;
        line.t1 = FAR;
    }

    for(int p2 = top-1; p2 >= 2; p2--)
    for(int p1 = p2-1; p1 >= 1; p1--)
    for(int p0 = p1-1; p0 >= 0; p0--)
    {
        trim(p0, p1, p2); if(line_count == 0) return false;
        trim(p0, p2, p1); if(line_count == 0) return false;
        trim(p1, p2, p0); if(line_count == 0) return false;
    }
    return true;
}