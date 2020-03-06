#ifdef __APPLE__
#include <GLUT/glut.h>
#else
#include <GL/glut.h>
#endif
#include <vector>
#include <fstream>
#include <stdio.h>
#include <stdlib.h>
#include <iostream>
#include <string>
#include <windows.h>

using namespace std;
float camx=-40, camy=-15, camz=11;
float xdeg=43, ydeg=324, zdeg=0;
char cond='m';
void correct()
{
    if (xdeg >= 360)
        xdeg -=360;
    if (xdeg <= 0)
        xdeg +=360;
    if (ydeg >= 360)
        ydeg -=360;
    if (ydeg <= 0)
        ydeg +=360;
    if (zdeg >= 360)
        zdeg -=360;
    if (zdeg <= 0)
        zdeg +=360;
}
class objLoad
{
public:
    ifstream in;
    objLoad(char* filename)
    {
        in.open(filename);
    }
    int spaces(char *line)
    {
        int sp=0;
        string l = string(line);
        for(unsigned int i=0;i<l.size();i++)
           {
                if(line[i]==' ')
                {
                    sp++;
                }
           }
           return sp;
    }
    class coordinate
    {
    public:
        float x,y,z;
        coordinate(float xx, float yy, float zz=0)
        {
            x = xx;
            y = yy;
            z = zz;
        }
    };
    class face{
        public:
            int facenum;
            bool four;
            int faces[4];
            float d[3], a[3], s[3];
            face(int facen,int f1,int f2,int f3,float ar,float ag,float ab,float dr,float dg,float db,float sr,float sg,float sb) : facenum(facen){  //constructor for triangle
                    faces[0]=f1;faces[1]=f2;faces[2]=f3;
                    a[0] = ar;a[1] = ag;a[2] = ab;
                    d[0] = dr;d[1] = dg;d[2] = db;
                    s[0] = sr;s[1] = sg;s[2] = sb;
                    four=false;
            }
           face(int facen,int f1,int f2,int f3,int f4,float ar,float ag,float ab,float dr,float dg,float db,float sr,float sg,float sb) : facenum(facen){ //overloaded constructor for quad
                    faces[0]=f1;    faces[1]=f2;    faces[2]=f3;    faces[3]=f4;
                    a[0] = ar;  a[1] = ag;  a[2] = ab;
                    d[0] = dr;  d[1] = dg;  d[2] = db;
                    s[0] = sr;  s[1] = sg;  s[2] = sb;
                    four=true;
            }
    };
    vector <coordinate> vertex;
    vector <coordinate> normals;
    vector <coordinate> texture;
    vector <face> faces;
    void read()
    {
        if(!in.is_open())
        {
            cout << "Not oepened" << endl;
        }
        float x,y,z;
        char a[256];
        while(!in.eof())
        {
            float ared, agreen, ablue;
            float dred, dgreen, dblue;
            float sred, sgreen, sblue;
            in.getline(a,256);
            if(a[0] == 'K' && a[1] == 'a')
            {
                sscanf(a, "Ka %f %f %f", &ared, &agreen, &ablue);
            }
            else if(a[0] == 'K' && a[1] == 'd')
            {
                sscanf(a, "Kd %f %f %f", &dred, &dgreen, &dblue);
            }
            else if(a[0] == 'K' && a[1] == 's')
            {
                sscanf(a, "Ks %f %f %f", &sred, &sgreen, &sblue);
            }
            if(a[0] == 'v' && a[1] == ' ')
            {
                sscanf(a,"v %f %f %f",&x,&y,&z);
                vertex.push_back(coordinate(x,y,z));
            }
            else if(a[0] == 'v' && a[1] == 'n')
            {
                sscanf(a,"vn %f %f %f",&x,&y,&z);
                normals.push_back(coordinate(x,y,z));
            }
            else if(a[0] == 'v' && a[1] == 't')
            {
                sscanf(a,"vt %f %f",&x,&y);
                texture.push_back(coordinate(x,y,z));
            }
            else if(a[0] == 'f')
            {
                int v1,v2,v3,v4,n,t;
                if(spaces(a)==3)
                {
                    sscanf(a,"f %d/%d/%d %d/%d/%d %d/%d/%d",&v1,&t,&n, &v2,&t,&n, &v3,&t,&n);

                    faces.push_back(face(n,v1,v2,v3,ared,agreen,ablue,dred,dgreen,dblue,sred,sgreen,sblue));
                }
                else if(spaces(a) == 4)
                {
                    sscanf(a,"f %d/%d/%d %d/%d/%d %d/%d/%d %d/%d/%d", &v1,&t,&n, &v2,&t,&n, &v3,&t,&n, &v4,&t,&n);
                    faces.push_back(face(n,v1,v2,v3,v4,ared,agreen,ablue,dred,dgreen,dblue,sred,sgreen,sblue));
                }
                else
                    continue;
            }
            else
                continue;
        }
    }

    void draw()
    {
        for(unsigned int i=0;i<faces.size();i++)
        {
            const GLfloat mat_ambient[]    = { faces[i].a[0],faces[i].a[1],faces[i].a[2], 1.0f };
            const GLfloat mat_diffuse[]    = { faces[i].d[0],faces[i].d[1],faces[i].d[2], 1.0f };
            const GLfloat mat_specular[]   = { faces[i].s[0],faces[i].s[1],faces[i].s[2], 1.0f };
                glMaterialfv(GL_FRONT, GL_AMBIENT,   mat_ambient);
                glMaterialfv(GL_FRONT, GL_DIFFUSE,   mat_diffuse);
                glMaterialfv(GL_FRONT, GL_SPECULAR,  mat_specular);
                glBegin(GL_TRIANGLES);
                glColor3f(faces[i].d[0],faces[i].d[1],faces[i].d[2]);

                    glNormal3f(normals[faces[i].facenum-1].x,normals[faces[i].facenum-1].y,normals[faces[i].facenum-1].z);
                    glVertex3f(vertex[faces[i].faces[0]-1].x,vertex[faces[i].faces[0]-1].y,vertex[faces[i].faces[0]-1].z);
                    glVertex3f(vertex[faces[i].faces[1]-1].x,vertex[faces[i].faces[1]-1].y,vertex[faces[i].faces[1]-1].z);
                    glVertex3f(vertex[faces[i].faces[2]-1].x,vertex[faces[i].faces[2]-1].y,vertex[faces[i].faces[2]-1].z);

                glEnd();
        }
    }
};


inline void Translated(int x,int y,int z)
{
    glTranslated(x,y-15,-z-60);
};

inline void moved(float x1,float z1,float t1,float x2,float z2,float t2,double t)
{
    Translated(x2+(t-t2)*((x2-x1)/(t2-t1)),0,z2+(t-t2)*((z2-z1)/(t2-t1)));
};

inline void rotation(float Thita1,float t1,float Thita2,float t2,double t)
{
    int angle=(Thita2+(t-t2)*((Thita2-Thita1)/(t2-t1)))/10;
    glRotated(angle*10,0, 1, 0);
};

objLoad car1("D:\\406_434_439_444_Traffic_Simulation\\Blender and Object files\\Cybertruck.obj");
objLoad car2("D:\\406_434_439_444_Traffic_Simulation\\Blender and Object files\\Cybertruck.obj");
objLoad car3("D:\\406_434_439_444_Traffic_Simulation\\Blender and Object files\\Cybertruck.obj");
objLoad car4("D:\\406_434_439_444_Traffic_Simulation\\Blender and Object files\\Cybertruck.obj");
objLoad car5("D:\\406_434_439_444_Traffic_Simulation\\Blender and Object files\\Cybertruck.obj");
objLoad car6("D:\\406_434_439_444_Traffic_Simulation\\Blender and Object files\\Cybertruck.obj");
objLoad car7("D:\\406_434_439_444_Traffic_Simulation\\Blender and Object files\\Cybertruck.obj");
objLoad car8("D:\\406_434_439_444_Traffic_Simulation\\Blender and Object files\\Cybertruck.obj");

objLoad Environment("D:\\406_434_439_444_Traffic_Simulation\\Blender and Object files\\Environment.obj");


/* GLUT callback Handlers */

static void resize(int width, int height)
{
    const float ar = (float) width / (float) height;

    glViewport(0, 0, width, height);
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    glFrustum(-ar, ar, -1.0, 1.0, 2.0, 100.0);

    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity() ;
}

static void display(void)
{
    const double t = glutGet(GLUT_ELAPSED_TIME)% 15000;
    glLoadIdentity();
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    float light_ambient[4];
    float light_diffuse[4];
    float light_specular[4];
    float pos[]={0,15,0,1.0};
     float bulb_ambient[] = {0.5,0.5,0.5,1.0};
    float bulb_diffuse[] = {0.5,0.5,0.5,1.0};
    float bulb_specular[] = {1.0,1.0,1.0,1.0};
    float posn1[] = {-3.507413 ,3.699298 ,3.134908+5};
    float posn2[] = {5.574394, 3.699298, 8.882782+5};
    float posn3[] = {2.105798, 3.699298, -11.899135+5};

    if(cond == 'm')
    {
        light_ambient[0] = 1.3;light_ambient[1] = 1.3;light_ambient[2] = 1.3;light_ambient[3] = 1.0;
        light_diffuse[0] = 1.7;light_diffuse[1] = 1.7;light_diffuse[2] = 1.7;light_diffuse[3] = 1.0;
        light_specular[0] = 1.0;light_specular[1] = 1.0;light_specular[2] = 1.0;light_specular[3] = 1.0;
        glLightfv(GL_LIGHT0, GL_AMBIENT,  light_ambient);
        glLightfv(GL_LIGHT0, GL_DIFFUSE,  light_diffuse);
        glLightfv(GL_LIGHT0, GL_SPECULAR, light_specular);
        glLightfv(GL_LIGHT0,GL_POSITION,pos);
        glDisable(GL_LIGHT1);glDisable(GL_LIGHT2);glDisable(GL_LIGHT3);
    }
    if (cond == 'n')
    {
        light_ambient[0] = 0.1;light_ambient[1] = 0.1;light_ambient[2] = 0.1;light_ambient[3] = 1.0;
        light_diffuse[0] = 0.1;light_diffuse[1] = 0.1;light_diffuse[2] = 0.1;light_diffuse[3] = 1.0;
        light_specular[0] = 0.2;light_specular[1] = 0.2;light_specular[2] = 0.2;light_specular[3] = 1.0;

        glLightfv(GL_LIGHT0, GL_AMBIENT,  light_ambient);
        glLightfv(GL_LIGHT0, GL_DIFFUSE,  light_diffuse);
        glLightfv(GL_LIGHT0, GL_SPECULAR, light_specular);
        glLightfv(GL_LIGHT0,GL_POSITION,pos);
        glLightfv(GL_LIGHT1, GL_AMBIENT,  bulb_ambient);
        glLightfv(GL_LIGHT1, GL_DIFFUSE,  bulb_diffuse);
        glLightfv(GL_LIGHT1, GL_SPECULAR, bulb_specular);
        glLightfv(GL_LIGHT1,GL_POSITION,posn1);
        glLightfv(GL_LIGHT2, GL_AMBIENT,  bulb_ambient);
        glLightfv(GL_LIGHT2, GL_DIFFUSE,  bulb_diffuse);
        glLightfv(GL_LIGHT2, GL_SPECULAR, bulb_specular);
        glLightfv(GL_LIGHT2,GL_POSITION,posn2);
        glLightfv(GL_LIGHT3, GL_AMBIENT,  bulb_ambient);
        glLightfv(GL_LIGHT3, GL_DIFFUSE,  bulb_diffuse);
        glLightfv(GL_LIGHT3, GL_SPECULAR, bulb_specular);
        glLightfv(GL_LIGHT3,GL_POSITION,posn3);
    }



    glTranslated(camx,camy,camz);
    glRotatef(xdeg,1,0,0);
    glRotatef(ydeg,0,1,0);
    glRotatef(zdeg,0,0,1);
    glPushMatrix();
        if(t<4000)
        {
            moved(7,-45,0,9,-6,4000,t);
            glRotated(-90,0, 1, 0);
        }
        else if(t<5000)
        {
            moved(9,-6,4000,4,-2,5000,t);
            rotation(-90,4000,0,5000,t);
        }
         else if(t<6000)
        {
            moved(4,-2,5000,-20,-2,6000,t);
            glRotated(0,0, 1, 0);
        }
        else if(t<8000)
        {
            moved(-20,-2,6000,-65,-2,8000,t);
            glRotated(0,0, 1, 0);
        }
        else
           Translated(-1000,-1000,-1000);
        car1.draw();
    glPopMatrix();
    glPushMatrix();
        if(t<3000)
        {
            moved(1,50,0,1,3,3000,t);
            glRotated(90,0, 1, 0);
        }
        else if(t<4000)
        {
            moved(1,3,3000,-7,-2,4000,t);
            rotation(90,3000,0,4000,t);
        }
         else if(t<4500)
        {
            moved(-7,-2,4000,-20,-2,4500,t);
            glRotated(0,0, 1, 0);
        }
        else if(t<6000)
        {
            moved(-20,-2,4500,-65,-2,6000,t);
            glRotated(0,0, 1, 0);
        }
        else
           Translated(-1000,-1000,-1000);
        car2.draw();
    glPopMatrix();
    glPushMatrix();
        if(t<3000)
            Translated(100,100,100);
       else if(t<6000)
        {
            moved(1,75,3000,1,3,6000,t);
            glRotated(90,0, 1, 0);
        }
        else if(t<7000)
        {
            moved(1,3,6000,4,0,7000,t);
            rotation(90,6000,180,7000,t);
        }
         else if(t<8000)
        {
            moved(4,0,7000,10,-7,8000,t);
            rotation(180,7000,90,8000,t);
        }
        else if(t<11000)
        {
            moved(10,-7,8000,10,-65,11000,t);
            glRotated(90,0, 1, 0);
        }
        else
           Translated(-1000,-1000,-1000);
        car3.draw();
    glPopMatrix();
    glPushMatrix();
        if(t<1500)
            Translated(4,0,-2);
        else if(t<2500)
        {
            moved(4,-2,1500,-2,4,2500,t);
            rotation(0,1500,-90,2500,t);
        }
       else if(t<5500)
        {
            moved(-2,4,2500,-2,75,5500,t);
            glRotated(-90,0, 1, 0);
        }
        else if(t<10000)
            Translated(100,100,100);
       else if(t<14000)
        {
            moved(7,-45,10000,9,-6,14000,t);
            glRotated(-90,0, 1, 0);
        }
        else if(t<15000)
        {
            moved(9,-6,14000,4,-2,15000,t);
            rotation(-90,14000,0,15000,t);
        }
        else
           Translated(-1000,-1000,-1000);
        car4.draw();
    glPopMatrix();
    glPushMatrix();
       if(t<1000)
        {
            moved(-7,0,0,-2,4,1000,t);
            rotation(180,0,270,1000,t);
        }
       else if(t<4000)
        {
            moved(-2,4,1000,-2,75,4000,t);
            glRotated(-90,0, 1, 0);
        }
        else if(t>13000)
        {
            moved(-65,0,13000,-7,0,15000,t);
            glRotated(180,0, 1, 0);
        }
        else
           Translated(-1000,-1000,-1000);
        car5.draw();
    glPopMatrix();
    glPushMatrix();
            if(t<3500)
            Translated(100,100,100);
       else if(t<7500)
        {
            moved(1,75,3500,1,3,7500,t);
            glRotated(90,0, 1, 0);
        }
        else if(t<8500)
        {
            moved(1,3,7500,4,0,8500,t);
            rotation(90,7500,180,8500,t);
        }
         else if(t<9500)
        {
            moved(4,0,8500,10,-7,9500,t);
            rotation(180,8500,90,9500,t);
        }
        else if(t<12500)
        {
            moved(10,-7,9500,10,-65,12500,t);
            glRotated(90,0, 1, 0);
        }
        else
           Translated(-1000,-1000,-1000);
        car6.draw();
    glPopMatrix();
    glPushMatrix();
        if(t<3000)
            Translated(100,100,100);
        else if(t<6000)
        {
            moved(-65,0,3000,-7,0,6000,t);
            glRotated(180,0, 1, 0);
        }
        else if(t<7000)
        {
            moved(-7,0,6000,-2,4,7000,t);
            rotation(180,6000,270,7000,t);
        }
       else if(t<10000)
        {
            moved(-2,4,7000,-2,75,10000,t);
            glRotated(-90,0, 1, 0);
        }
        else
           Translated(-1000,-1000,-1000);
        car7.draw();
    glPopMatrix();
    glPushMatrix();
        if(t<7500)
            Translated(100,100,100);
        else if(t<10000)
        {
            moved(-65,0,7500,4,0,10000,t);
            glRotated(180,0, 1, 0);
        }else if(t<11000)
        {
            moved(4,0,10000,10,-7,11000,t);
            rotation(180,10000,90,11000,t);
        }
        else if(t<14000)
        {
            moved(10,-7,11000,10,-65,14000,t);
            glRotated(90,0, 1, 0);
        }
        else
           Translated(-1000,-1000,-1000);
        car8.draw();
    glPopMatrix();

    glPushMatrix();
        Translated(0,0,5);
        glRotated(0,0, 1, 0);
        glRotated(0,0,0,0);
        Environment.draw();
    glPopMatrix();

    glutSwapBuffers();
}


static void idle(void)
{
    glutPostRedisplay();
}


void wasd(unsigned char key, int x, int y)
{
    switch(key)
    {
    case 'w':
        camz = camz + 1;break;
    case 's':
        camz = camz - 1;break;
    case 'a':
        camx = camx - 1;break;
    case 'd':
        camx = camx + 1;break;
    case 'q':
        camy = camy + 1 ;break;
    case 'e':
        camy = camy - 1;break;
    case 'u':
        xdeg += 1;
        correct();break;
    case 'j':
        xdeg -= 1;
        correct();break;
    case 'i':
        ydeg += 1;
        correct();break;
    case 'k':
        ydeg -= 1;
        correct();break;
    case 'o':
        zdeg += 1;
        correct();break;
    case 'l':
        zdeg -= 1;
        correct();break;
    case 'n':
        cond = 'n';break;
    case 'm':
        cond = 'm';break;
    }
}

/* Program entry point */

int main(int argc, char *argv[])
{
    car1.read();
    car2.read();
    car3.read();
    car4.read();
    car5.read();
    car6.read();
    car7.read();
    car8.read();

    Environment.read();
    char key;
    int x=0,y=0;

    glutInit(&argc, argv);
    glutInitWindowSize(1920,1080);
    glutInitWindowPosition(10,10);
    glutInitDisplayMode(GLUT_RGB | GLUT_DOUBLE | GLUT_DEPTH);

    glutCreateWindow("GLUT Shapes");

    glutReshapeFunc(resize);
    glutDisplayFunc(display);
    glutIdleFunc(idle);

    glutKeyboardFunc(wasd);

    glClearColor(0,0,0,1);
    glEnable(GL_CULL_FACE);
    glCullFace(GL_BACK);

    glEnable(GL_DEPTH_TEST);
    glDepthFunc(GL_LESS);

    glEnable(GL_LIGHT0);
    glEnable(GL_LIGHT1);
    glEnable(GL_LIGHT2);
    glEnable(GL_LIGHT3);
    glEnable(GL_NORMALIZE);
    glEnable(GL_COLOR_MATERIAL);
    glEnable(GL_LIGHTING);

    glutMainLoop();

    return EXIT_SUCCESS;
}
