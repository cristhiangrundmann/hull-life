#include "hull.h"
#include <GL/gl.h>
#include <GL/freeglut.h>
#include <GL/glu.h>

const float PI = 3.141592653589793238463;

float keys[256]; int nkeys = 0;
float delta, time;

vec3 front, right, up;

vec3 campos;
float scampos = 2.0;

vec2 camrot;
float scamrot = 2.0;

bool showedges = true, showlines = false, showsolid = true, wallhack = false;

int hull_view = 0;

void setcam()
{
    if(camrot.y < 0) camrot.y = 0;
    if(camrot.y > PI) camrot.y = PI;

    front = vec3(sin(camrot.y)*cos(camrot.x), sin(camrot.y)*sin(camrot.x), cos(camrot.y));
    right = vec3(sin(camrot.x), -cos(camrot.x), 0);
    up = cross(right, front);

    glLoadIdentity();

    gluLookAt(campos.x, campos.y, campos.z, campos.x+front.x, campos.y+front.y, campos.z+front.z, up.x, up.y, up.z);    
}

void display()
{
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    if(showsolid)
    for(TRIANGLE t : solid_tris[hull_view])
    {
        //BACKFACE CULLING
        float k = dot(campos, t.p.normal) - t.p.dist;
        if(k >= 0) continue;

        glColor4f(abs(t.p.normal.x), abs(t.p.normal.y), abs(t.p.normal.z), 1);
        glBegin(GL_TRIANGLES);
        glVertex3f(t.a.x, t.a.y, t.a.z);
        glVertex3f(t.b.x, t.b.y, t.b.z);
        glVertex3f(t.c.x, t.c.y, t.c.z);
        glEnd();

        if(!showedges) continue;

        glLineWidth(2);
        glBegin(GL_LINES);
        glColor4f(1, 1, 1, 1);
        glVertex3f(t.a.x, t.a.y, t.a.z);
        glVertex3f(t.b.x, t.b.y, t.b.z);

        if(showlines)
        {
            glVertex3f(t.a.x, t.a.y, t.a.z);
            glVertex3f(t.c.x, t.c.y, t.c.z);
            glVertex3f(t.c.x, t.c.y, t.c.z);
            glVertex3f(t.b.x, t.b.y, t.b.z);
        }
        glEnd();
    }

    for(TRIANGLE t : empty_tris[hull_view])
    {
        //BACKFACE CULLING
        float k = dot(campos, t.p.normal) - t.p.dist;
        if(k >= 0) continue;

        float color = (sin(time*25)+1) / 2;
        glColor4f(color, color, color, 1);
        glBegin(GL_TRIANGLES);
        glVertex3f(t.a.x, t.a.y, t.a.z);
        glVertex3f(t.b.x, t.b.y, t.b.z);
        glVertex3f(t.c.x, t.c.y, t.c.z);
        glEnd();

        glLineWidth(5);
        if(wallhack) glDisable(GL_DEPTH_TEST);
        glBegin(GL_LINES);
        glColor4f(color, color, color, 1);
        glVertex3f(t.a.x, t.a.y, t.a.z);
        glVertex3f(t.b.x, t.b.y, t.b.z);
        glEnd();
        glEnable(GL_DEPTH_TEST);

        if(showsolid) continue;

        glLineWidth(1);
        glBegin(GL_LINES);
        glColor4f(color, 1, 1-color, 0.1);
        glVertex3f(0, 0, 0);
        glColor4f(color, 0, 1-color, 0.3);
        glVertex3f(t.b.x, t.b.y, t.b.z);
        glEnd();

    }

    glutSwapBuffers();
}

void idle()
{
    float newtime = (float)glutGet(GLUT_ELAPSED_TIME)/1000;
    delta = newtime - time;
    time = newtime;
    
    //if(nkeys == 0) return;

    campos += delta*scampos*((keys['w']-keys['s'])*front  + (keys['d']-keys['a'])*right);
    camrot += delta*scamrot*vec2(keys['g']-keys['j'], keys['h']-keys['y']);
    
    setcam();
    glutPostRedisplay();
}

void reshape(int w, int h)
{
    if(h == 0) h == 1;
    float aspect = (float)w/h;
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    glViewport(0, 0, w, h);
    gluPerspective(90, aspect, 1, 10000);
    glMatrixMode(GL_MODELVIEW);
    setcam();
}

void keyboard(unsigned char key, int x, int y)
{
    if(keys[key] == 0) nkeys++; else return;
    keys[key] = 1;
}

void keyboardup(unsigned char key, int x, int y)
{
    if(keys[key] == 1) nkeys--; else return;
    keys[key] = 0;

    switch(key)
    {
        case '1': hull_view = 0; break;
        case '2': hull_view = 1; break;
        case '3': hull_view = 2; break;
        case 'q': scampos *= 2; break;
        case 'e': scampos /= 2; break;
        case 't': scamrot *= 2; break;
        case 'u': scamrot /= 2; break;
        case 'p': showedges = !showedges; break;
        case 'o': showlines = !showlines; break;
        case '+': showsolid = !showsolid; break;
        case '/': wallhack = !wallhack; break;
        default: return;
    }
    glutPostRedisplay();
}

void motion(int x, int y)
{
    static int lastx = -1, lasty = 0;

    if(lastx == -1)
    {
        lastx = x;
        lasty = y;
        return;
    }

    vec2 d = vec2((float)-x + lastx, (float)y - lasty);

    camrot += d*0.01f*scamrot;
    setcam();
    glutPostRedisplay();

    lastx = x;
    lasty = y;
}

void view(int argc, char ** argv)
{
    glutInit(&argc, argv);
    glutInitDisplayMode(GLUT_DEPTH | GLUT_DOUBLE | GLUT_RGBA | GLUT_MULTISAMPLE);
    glutInitWindowPosition(500, 500);
    glutInitWindowSize(800, 800);
    glutCreateWindow("Half-Life Hull");



    glutDisplayFunc(display);
    glutIdleFunc(idle);
    glutReshapeFunc(reshape);
    glutKeyboardFunc(keyboard);
    glutKeyboardUpFunc(keyboardup);
    glutMotionFunc(motion);

    glutSetOption(GLUT_MULTISAMPLE, 8);
    glEnable(GL_MULTISAMPLE);
    glHint(GL_MULTISAMPLE_FILTER_HINT_NV, GL_NICEST);

    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    glEnable(GL_DEPTH_TEST);

    hull_view = 0;
    campos = vec3(-1, -1, 1);
    camrot = vec2(PI/4, 4*PI/6);

    glutMainLoop();
}