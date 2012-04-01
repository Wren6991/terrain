/***************************************************************
 * Name:      terrainMain.cpp
 * Purpose:   Code for Application Frame
 * Author:    Luke Wren (wren6991@gmail.com)
 * Created:   2012-03-24
 * Copyright: Luke Wren ()
 * License:
 **************************************************************/

#define GL_GLEXT_PROTOTYPES
#include "terrainMain.h"
#include <wx/msgdlg.h>
#include <stdlib.h>
#include "vec3.h"
#include "util.h"
#include <iostream>
#include <fstream>
#include <istream>

//(*InternalHeaders(terrainFrame)
#include <wx/intl.h>
#include <wx/string.h>
//*)

const double PI = 3.1415926535897932384626433832795028841971693993751058209749445923078164062862089986280348253421170679;

int gridsize = 300;

#define BUFFER_OFFSET(i) (reinterpret_cast<void*>(i))


//helper functions
enum wxbuildinfoformat {
    short_f, long_f };

wxString wxbuildinfo(wxbuildinfoformat format)
{
    wxString wxbuild(wxVERSION_STRING);

    if (format == long_f )
    {
#if defined(__WXMSW__)
        wxbuild << _T("-Windows");
#elif defined(__UNIX__)
        wxbuild << _T("-Linux");
#endif

#if wxUSE_UNICODE
        wxbuild << _T("-Unicode build");
#else
        wxbuild << _T("-ANSI build");
#endif // wxUSE_UNICODE
    }

    return wxbuild;
}

//(*IdInit(terrainFrame)
const long terrainFrame::ID_GLCANVAS1 = wxNewId();
const long terrainFrame::ExportTGA = wxNewId();
const long terrainFrame::idMenuQuit = wxNewId();
const long terrainFrame::idMenuAbout = wxNewId();
const long terrainFrame::ID_TIMER1 = wxNewId();
//*)

BEGIN_EVENT_TABLE(terrainFrame,wxFrame)
    //(*EventTable(terrainFrame)
    //*)
END_EVENT_TABLE()


inline double max(double a, double b)
{
    return a > b ? a : b;
}

double noise(double x, int seed)
{
    srand(pow(x, 1.12) * 61 - 1321 + seed);
    rand();
    return rand() / (double)RAND_MAX;
    //return noisetexture[int((x - floor(x / 100) * 100) * 100 + y - floor(y / 100) * 100)];
}
inline double noise(double x, double y, int seed)
{
    return noise(noise(x, seed) * 1000 + noise(y * 12, seed) * 877, seed);
}

inline double lerp(double a, double b, double t)
{
    return b - (cos(t * PI) * 0.5 + 0.5) * (b - a);     //actually cosine interpolation
}
inline double frac(double x)
{
    return x - floor(x);
}
inline double smoothnoise(double x, double y, int seed)
{
    return lerp(
                lerp(noise(floor(x), floor(y)    , seed), noise(floor(x) + 1, floor(y)    , seed), frac(x)),
                lerp(noise(floor(x), floor(y) + 1, seed), noise(floor(x) + 1, floor(y) + 1, seed), frac(x)),
                frac(y)
                );
}

double fbm(double x, double y, int octaves, double frequency, double persistence, int seed)
{
    if (octaves > 0)
        return smoothnoise(x, y, seed) + persistence * fbm(x * frequency, y * frequency, octaves - 1, frequency, persistence, seed);
    else
        return 0;
}

inline double voronoi(double x, double y, int gridsize, int npoints, int seed)
{
    srand(seed * seed);
    double minsqrdist = 100;
    double px, py, sqrdist;
    for (int i = 0; i < npoints; i++)
    {
        px = x - rand() * static_cast <double> (gridsize) / RAND_MAX;
        py = y - rand() * static_cast <double> (gridsize) / RAND_MAX;
        sqrdist = px * px + py * py;
        if (sqrdist < minsqrdist)
            minsqrdist = sqrdist;
    }
    return pow(minsqrdist, 0.5 * 1.1);
}
terrainFrame::~terrainFrame()
{
    //(*Destroy(terrainFrame)
    //*)
}

void terrainFrame::generateTerrain()
{
        double **grid = new double*[gridsize + 2];
    for (int i = 0; i < gridsize + 2; i++)
        grid[i] = new double[gridsize + 2];
    for (int i = 0; i < gridsize + 2; i++)
    {
        for(int j = 0; j < gridsize + 2; j++)
        {
            grid[i][j] =  + fbm(i / 32.0 + smoothnoise(i * 0.07, j * 0.07, 1338) * 0.2, j / 32.0 + smoothnoise(i * 0.07, j * 0.07, 1338 + 1) * 0.2, 6, 2, 0.45, 1338) * 15 - 5;
        }
    }

    vec3 **normals = new vec3*[gridsize];
    for (int i = 0; i < gridsize; i++)
        normals[i] = new vec3[gridsize];
    for (int i = 0; i < gridsize; i++)
    {
        for (int j = 0; j < gridsize; j++)
        {
            normals[i][j] = vec3(
                                 grid[i][j + 1] - grid[i + 2][j + 1],
                                 2,
                                 grid[i + 1][j] - grid[i + 1][j + 2]
                                 );                                     //cross product of tangent vectors
        }
    }

    vert *verts = new vert[gridsize * gridsize];
    for (int i = 0; i < gridsize; i++)
    {
        for (int j = 0; j < gridsize; j++)
        {
            vert& v = verts[i + j * gridsize];
            v.x = i;
            v.y = grid[i + 1][j + 1];
            v.z = j;
            v.nx = normals[i][j].x;
            v.ny = normals[i][j].y;
            v.nz = normals[i][j].z;
            v.u = 0;
            v.v = 0;
        }
    }

    GLContext->SetCurrent(*GLCanvas1);

    GLenum err = glewInit();

    if (GLEW_OK != err)
    {
        std::cout << "GLEW init failed: \"" << glewGetErrorString(err) << "\". Shutting down...\n";
        char c;
        std::cin >> c;
        this->Close();
        return;
    }
    else
    {
        std::cout << "GLEW OK! OGL version: " << GLEW_VERSION_MAJOR << "." << GLEW_VERSION_MINOR << "\n";
    }

    glGenBuffers(1, &resources.VBO);
    glBindBuffer(GL_ARRAY_BUFFER, resources.VBO);
    glBufferData(GL_ARRAY_BUFFER, gridsize * gridsize * sizeof(vert), verts, GL_STATIC_DRAW);

    GLuint *indices = new GLuint[(gridsize - 1) * (gridsize - 1) * 6];
    for (int j = 0; j < (gridsize - 1); j++)
    {
        bool flip = false;
        for (int i = 0; i < (gridsize - 1); i++)
        {
            if (flip)
            {
                indices[(i + j * (gridsize - 1)) * 6    ] = i + (j + 1) * gridsize;
                indices[(i + j * (gridsize - 1)) * 6 + 1] = i + 1 + (j + 1) * gridsize;
                indices[(i + j * (gridsize - 1)) * 6 + 2] = i + 1 + j * gridsize;
                indices[(i + j * (gridsize - 1)) * 6 + 3] = i + (j + 1) * gridsize;
                indices[(i + j * (gridsize - 1)) * 6 + 4] = i + 1 + j * gridsize;
                indices[(i + j * (gridsize - 1)) * 6 + 5] = i + j * gridsize;
            }
            else
            {
                indices[(i + j * (gridsize - 1)) * 6    ] = i + j * gridsize;                               //  0, 3 +----+ 1
                indices[(i + j * (gridsize - 1)) * 6 + 2] = i + 1 + j * gridsize;                           //       |\   |
                indices[(i + j * (gridsize - 1)) * 6 + 1] = i + 1 + (j + 1) * gridsize;                     //       |  \ |
                indices[(i + j * (gridsize - 1)) * 6 + 3] = i + j * gridsize;                               //     5 +----+ 2, 4
                indices[(i + j * (gridsize - 1)) * 6 + 5] = i + 1 + (j + 1) * gridsize;
                indices[(i + j * (gridsize - 1)) * 6 + 4] = i + (j + 1) * gridsize;                         //       +-> i
                //std::cout << (i + j * (gridsize - 1)) * 6 + 5 << "\n";                                    //       v   j
            }
            flip = !flip;
        }
    }/*
    for (int i = 0; i < (gridsize - 1) * (gridsize - 1); i++)
    {
        for (int j = 0; j < 6; j++)
            std::cout << indices[i * 6 + j] << ", ";
        std::cout << "\n";
    }*/

    //GLuint indices[] = {0, 1, 2, 1, 2, 3};

    glGenBuffers(1, &resources.IBO);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, resources.IBO);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, (gridsize - 1) * (gridsize - 1) * 6 * sizeof(GLuint), indices, GL_STATIC_DRAW);

    delete verts;
    delete indices;
}

void terrainFrame::makeShaders()
{
    resources.terrain.fshader = makeShader(GL_FRAGMENT_SHADER, "terrain.f.glsl");
    resources.terrain.vshader = makeShader(GL_VERTEX_SHADER, "terrain.v.glsl");
    resources.terrain.program = makeProgram(resources.terrain.vshader, resources.terrain.fshader);
    resources.attribute.pos = glGetAttribLocation(resources.terrainprogram, "pos");
    resources.attribute.v_normal = glGetAttribLocation(resources.terrainprogram, "v_normal");
    resources.sandtexture = makeTexture("sand.tga");
    resources.uniform.sand = glGetUniformLocation(resources.terrainprogram, "sand");
    resources.grasstexture = makeTexture("grass.tga");
    resources.uniform.grass = glGetUniformLocation(resources.terrainprogram, "grass");
    resources.rocktexture = makeTexture("rock.tga");
    resources.uniform.rock = glGetUniformLocation(resources.terrainprogram, "rock");
    resources.water.fshader = makeShader(GL_FRAGMENT_SHADER, "water.f.glsl");
    resources.water.vshader = makeShader(GL_VERTEX_SHADER, "water.v.glsl");
    resources.water.program = makeProgram(resources.watervertshader, resources.waterfragshader);
    resources.water.playerpos = glGetUniformLocation(resources.waterprogram, "playerpos");
}


terrainFrame::terrainFrame(wxWindow* parent,wxWindowID id)
{
    //(*Initialize(terrainFrame)
    wxMenuItem* MenuItem2;
    wxMenuItem* MenuItem1;
    wxMenu* Menu1;
    wxBoxSizer* BoxSizer1;
    wxMenuBar* MenuBar1;
    wxMenu* Menu2;

    Create(parent, id, _("Terrain!"), wxDefaultPosition, wxDefaultSize, wxDEFAULT_FRAME_STYLE, _T("id"));
    BoxSizer1 = new wxBoxSizer(wxHORIZONTAL);
    int GLCanvasAttributes_1[] = {
    	WX_GL_RGBA,
    	WX_GL_DOUBLEBUFFER,
    	WX_GL_DEPTH_SIZE,      16,
    	WX_GL_STENCIL_SIZE,    0,
    	0, 0 };
    GLCanvas1 = new wxGLCanvas(this, ID_GLCANVAS1, wxDefaultPosition, wxDefaultSize, 0, _T("ID_GLCANVAS1"), GLCanvasAttributes_1);
    GLCanvas1->SetMinSize(wxSize(640,640));
    BoxSizer1->Add(GLCanvas1, 1, wxALIGN_CENTER_HORIZONTAL|wxALIGN_CENTER_VERTICAL, 5);
    SetSizer(BoxSizer1);
    MenuBar1 = new wxMenuBar();
    Menu1 = new wxMenu();
    MenuItem3 = new wxMenuItem(Menu1, ExportTGA, _("&Export TGA"), wxEmptyString, wxITEM_NORMAL);
    Menu1->Append(MenuItem3);
    MenuItem1 = new wxMenuItem(Menu1, idMenuQuit, _("Quit\tAlt-F4"), _("Quit the application"), wxITEM_NORMAL);
    Menu1->Append(MenuItem1);
    MenuBar1->Append(Menu1, _("&File"));
    Menu2 = new wxMenu();
    MenuItem2 = new wxMenuItem(Menu2, idMenuAbout, _("About\tF1"), _("Show info about this application"), wxITEM_NORMAL);
    Menu2->Append(MenuItem2);
    MenuBar1->Append(Menu2, _("Help"));
    SetMenuBar(MenuBar1);
    Timer1.SetOwner(this, ID_TIMER1);
    Timer1.Start(10, false);
    BoxSizer1->Fit(this);
    BoxSizer1->SetSizeHints(this);

    GLCanvas1->Connect(wxEVT_PAINT,(wxObjectEventFunction)&terrainFrame::OnGLCanvas1Paint,0,this);
    GLCanvas1->Connect(wxEVT_LEFT_DOWN,(wxObjectEventFunction)&terrainFrame::OnGLCanvas1LeftDown,0,this);
    GLCanvas1->Connect(wxEVT_LEFT_UP,(wxObjectEventFunction)&terrainFrame::OnGLCanvas1LeftUp,0,this);
    GLCanvas1->Connect(wxEVT_RIGHT_DOWN,(wxObjectEventFunction)&terrainFrame::OnGLCanvas1RightDown,0,this);
    GLCanvas1->Connect(wxEVT_RIGHT_UP,(wxObjectEventFunction)&terrainFrame::OnGLCanvas1RightUp,0,this);
    GLCanvas1->Connect(wxEVT_RIGHT_DCLICK,(wxObjectEventFunction)&terrainFrame::OnGLCanvas1RightDClick,0,this);
    GLCanvas1->Connect(wxEVT_MOTION,(wxObjectEventFunction)&terrainFrame::OnGLCanvas1MouseMove,0,this);
    GLCanvas1->Connect(wxEVT_LEAVE_WINDOW,(wxObjectEventFunction)&terrainFrame::OnGLCanvas1MouseLeave,0,this);
    Connect(ExportTGA,wxEVT_COMMAND_MENU_SELECTED,(wxObjectEventFunction)&terrainFrame::OnMenuItem3Selected);
    Connect(idMenuQuit,wxEVT_COMMAND_MENU_SELECTED,(wxObjectEventFunction)&terrainFrame::OnQuit);
    Connect(idMenuAbout,wxEVT_COMMAND_MENU_SELECTED,(wxObjectEventFunction)&terrainFrame::OnAbout);
    Connect(ID_TIMER1,wxEVT_TIMER,(wxObjectEventFunction)&terrainFrame::OnTimer1Trigger);
    //*)
    GLContext  = new wxGLContext(GLCanvas1);

    pitch = 0;
    yaw = 0;
    camx = 10;
    camy = 20;
    camz = -10;
    lastmousex = 0;
    lastmousey = 0;

    generateTerrain();
    makeShaders();

    //std::cout << sizeof(vert);
    std::cout << (gridsize - 1) * (gridsize - 1) * 2;
}
void terrainFrame::OnQuit(wxCommandEvent& event)
{
    Close();
}

void terrainFrame::OnAbout(wxCommandEvent& event)
{
    wxString msg = wxbuildinfo(long_f);
    wxMessageBox(msg, _("Welcome to..."));
}

void terrainFrame::initgl()
{
    GLCanvas1->SetCurrent(*GLContext);
    glClearColor(0.53f, 0.81f, 0.92f, 1.0f);
    glClearDepth(1.0f);
    glClear(GL_COLOR_BUFFER_BIT|GL_DEPTH_BUFFER_BIT);
    glEnable(GL_DEPTH_TEST);
    glDepthFunc(GL_LEQUAL);
    glEnable(GL_CULL_FACE);

    glHint(GL_PERSPECTIVE_CORRECTION_HINT, GL_NICEST);

    glEnable(GL_COLOR_MATERIAL);
    glEnable (GL_BLEND);
    glBlendFunc (GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

    glViewport(0, 0, 640, 640);
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    glFrustum(-1, 1, -1, 1, 1, 1000);
    glRotatef(pitch * 180 / PI, 1, 0, 0);
    glRotatef(-yaw * 180 / PI, 0, 1, 0);
    glTranslatef(-camx, -camy, -camz);
    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();
/*
    GLfloat mat_specular[] = { 0.9, 0.9, 1.0, 1.0 };
    GLfloat mat_shininess[] = { 50.0 };
    GLfloat light_position[] = { 1.0, 1.0, 1.0, 0.0 };

    glLightModeli(GL_LIGHT_MODEL_TWO_SIDE, GL_TRUE);
    glMaterialfv(GL_FRONT, GL_SPECULAR, mat_specular);
    glMaterialfv(GL_FRONT, GL_SHININESS, mat_shininess);
    glLightfv(GL_LIGHT0, GL_POSITION, light_position);

    glEnable(GL_LIGHTING);
    glEnable(GL_LIGHT0);
    glEnable(GL_COLOR_MATERIAL);*/

}

void terrainFrame::endgl()
{
    glFlush();
    GLCanvas1->SwapBuffers();
}
void terrainFrame::OnGLCanvas1Paint(wxPaintEvent& event)
{
    static int tick = 0;
    tick++;
    initgl();
    /*glBegin(GL_POLYGON);
    glColor3f(1, 1, 0.8);
    glEnd();*/

    glUseProgram(resources.terrain.program);

    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, resources.sandtexture);
    glUniform1i(resources.uniform.sand, 0);

    glActiveTexture(GL_TEXTURE1);
    glBindTexture(GL_TEXTURE_2D, resources.grasstexture);
    glUniform1i(resources.uniform.grass, 1);

    glActiveTexture(GL_TEXTURE2);
    glBindTexture(GL_TEXTURE_2D, resources.rocktexture);
    glUniform1i(resources.uniform.rock, 2);


    glBindBuffer(GL_ARRAY_BUFFER, resources.VBO);

    glVertexAttribPointer(resources.attribute.pos, 3, GL_FLOAT, GL_FALSE, 32, BUFFER_OFFSET(0));
    glEnableVertexAttribArray(resources.attribute.pos);
    glVertexAttribPointer(resources.attribute.v_normal, 3, GL_FLOAT, GL_FALSE, 32, BUFFER_OFFSET(12));
    glEnableVertexAttribArray(resources.attribute.v_normal);


    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, resources.IBO);

    glDrawElements(GL_TRIANGLES, gridsize * gridsize * 6, GL_UNSIGNED_INT, BUFFER_OFFSET(0));

    glDisableVertexAttribArray(resources.attribute.pos);
    glDisableVertexAttribArray(resources.attribute.v_normal);

    glFlush();

    glUseProgram(resources.water.program);


     glUniform3f(resources.water.playerpos, camx, camy, camz);

    glBegin(GL_POLYGON);
    glColor4f(0, 0, 0.8, 0.7);
    glVertex3f(0, 5, 0);
    glVertex3f(0, 5 ,gridsize);
    glVertex3f(gridsize, 5, gridsize);
    glVertex3f(gridsize, 5, 0);
    glEnd();
    endgl();
}

void terrainFrame::OnTimer1Trigger(wxTimerEvent& event)
{
    wxPaintEvent evt;
    OnGLCanvas1Paint(evt);
}

void terrainFrame::OnGLCanvas1LeftDown(wxMouseEvent& event)
{
    mousedownl = true;
    lastmousex = event.GetX();
    lastmousey = event.GetY();
}

void terrainFrame::OnGLCanvas1LeftUp(wxMouseEvent& event)
{
    mousedownl = false;
}

void terrainFrame::OnGLCanvas1MouseMove(wxMouseEvent& event)
{
    double x, y;
    x = event.GetX();
    y = event.GetY();
    if (mousedownl)
    {
        pitch += (y - lastmousey) / 50.0;
        yaw   -= (x - lastmousex) / 50.0;
    }
    else if (mousedownr)
    {
        double dy, dx;
        dy = (y - lastmousey) / 50.0;
        dx = (x - lastmousex) / 50.0;
        camx += dy * cos(pitch) * sin(yaw);
        camy += dy * sin(pitch);
        camz += dy * cos(pitch) * cos(yaw);
    }
    lastmousex = x;
    lastmousey = y;
}

void terrainFrame::OnGLCanvas1MouseLeave(wxMouseEvent& event)
{
    mousedownl = false;
    mousedownr = false;
}



void terrainFrame::OnGLCanvas1RightDown(wxMouseEvent& event)
{
    mousedownr = true;
}

void terrainFrame::OnGLCanvas1RightUp(wxMouseEvent& event)
{
    mousedownr = false;
}

void terrainFrame::OnGLCanvas1RightDClick(wxMouseEvent& event)
{

}

void terrainFrame::OnMenuItem3Selected(wxCommandEvent& event)
{
    double** grid = new double*[gridsize + 2];
    for (int i = 0; i < gridsize + 2; i++)
        grid[i] = new double[gridsize + 2];
    for (int i = 0; i < gridsize; i++)
    {
        for(int j = 0; j < gridsize; j++)
        {
            grid[i][j] =  + fbm(i / 16.0 + smoothnoise(i * 0.2, j * 0.2, 1338) * 0.8, j / 16.0 + smoothnoise(i * 0.2, j * 0.2, 1338 + 1) * 0.8, 5, 2, 0.4, 1338) * 10 - 4;
        }
    }
    std::ofstream ofile;
    ofile.open("C:\\Users\\Owner\\Documents\\heightmap.tga", std::ios::binary | std::ios::out);
    unsigned char c = 0;
    ofile << c;
    ofile << c;
    c = 2;
    ofile << c;
    c = 0;
    ofile << c << c;
    ofile << c << c;
    ofile << c;
    ofile << c << c;
    ofile << c << c;
    c = gridsize & 0x00ff;
    ofile << c;
    c = (gridsize & 0xff00) >> 8;
    ofile << c;
    c = gridsize & 0x00ff;
    ofile << c;
    c = (gridsize & 0xff00) >> 8;
    ofile << c;
    c = 24;
    ofile << c;
    c = 0;
    ofile << c;
    for (int i = 0; i < gridsize; i++)
    {
        for (int j = 0; j < gridsize; j++)
        {
            c = grid[i][j] * 16 + 64;
            ofile << c << c << c;
        }
    }
    ofile << std::flush;
    ofile.close();
    wxMessageBox("LOL", _("Welcome to..."));
}
