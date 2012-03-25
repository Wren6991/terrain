/***************************************************************
 * Name:      terrainMain.cpp
 * Purpose:   Code for Application Frame
 * Author:    Luke Wren (wren6991@gmail.com)
 * Created:   2012-03-24
 * Copyright: Luke Wren ()
 * License:
 **************************************************************/

#include "terrainMain.h"
#include <wx/msgdlg.h>
#include <stdlib.h>
#include "vec3.h"
#include <iostream>

//(*InternalHeaders(terrainFrame)
#include <wx/intl.h>
#include <wx/string.h>
//*)

const double PI = 3.1415926535897932384626433832795028841971693993751058209749445923078164062862089986280348253421170679;

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
const long terrainFrame::idMenuQuit = wxNewId();
const long terrainFrame::idMenuAbout = wxNewId();
const long terrainFrame::ID_STATUSBAR1 = wxNewId();
const long terrainFrame::ID_TIMER1 = wxNewId();
//*)

BEGIN_EVENT_TABLE(terrainFrame,wxFrame)
    //(*EventTable(terrainFrame)
    //*)
END_EVENT_TABLE()

terrainFrame::terrainFrame(wxWindow* parent,wxWindowID id)
{
    //(*Initialize(terrainFrame)
    wxMenuItem* MenuItem2;
    wxMenuItem* MenuItem1;
    wxMenu* Menu1;
    wxBoxSizer* BoxSizer1;
    wxMenuBar* MenuBar1;
    wxMenu* Menu2;

    Create(parent, id, wxEmptyString, wxDefaultPosition, wxDefaultSize, wxDEFAULT_FRAME_STYLE, _T("id"));
    BoxSizer1 = new wxBoxSizer(wxHORIZONTAL);
    int GLCanvasAttributes_1[] = {
    	WX_GL_RGBA,
    	WX_GL_DOUBLEBUFFER,
    	WX_GL_DEPTH_SIZE,      16,
    	WX_GL_STENCIL_SIZE,    0,
    	0, 0 };
    GLCanvas1 = new wxGLCanvas(this, ID_GLCANVAS1, wxDefaultPosition, wxDefaultSize, 0, _T("ID_GLCANVAS1"), GLCanvasAttributes_1);
    GLCanvas1->SetMinSize(wxSize(480,480));
    BoxSizer1->Add(GLCanvas1, 1, wxALL|wxALIGN_CENTER_HORIZONTAL|wxALIGN_CENTER_VERTICAL, 5);
    SetSizer(BoxSizer1);
    MenuBar1 = new wxMenuBar();
    Menu1 = new wxMenu();
    MenuItem1 = new wxMenuItem(Menu1, idMenuQuit, _("Quit\tAlt-F4"), _("Quit the application"), wxITEM_NORMAL);
    Menu1->Append(MenuItem1);
    MenuBar1->Append(Menu1, _("&File"));
    Menu2 = new wxMenu();
    MenuItem2 = new wxMenuItem(Menu2, idMenuAbout, _("About\tF1"), _("Show info about this application"), wxITEM_NORMAL);
    Menu2->Append(MenuItem2);
    MenuBar1->Append(Menu2, _("Help"));
    SetMenuBar(MenuBar1);
    StatusBar1 = new wxStatusBar(this, ID_STATUSBAR1, 0, _T("ID_STATUSBAR1"));
    int __wxStatusBarWidths_1[1] = { -1 };
    int __wxStatusBarStyles_1[1] = { wxSB_NORMAL };
    StatusBar1->SetFieldsCount(1,__wxStatusBarWidths_1);
    StatusBar1->SetStatusStyles(1,__wxStatusBarStyles_1);
    SetStatusBar(StatusBar1);
    Timer1.SetOwner(this, ID_TIMER1);
    Timer1.Start(10, false);
    BoxSizer1->Fit(this);
    BoxSizer1->SetSizeHints(this);

    GLCanvas1->Connect(wxEVT_PAINT,(wxObjectEventFunction)&terrainFrame::OnGLCanvas1Paint,0,this);
    GLCanvas1->Connect(wxEVT_LEFT_DOWN,(wxObjectEventFunction)&terrainFrame::OnGLCanvas1LeftDown,0,this);
    GLCanvas1->Connect(wxEVT_LEFT_UP,(wxObjectEventFunction)&terrainFrame::OnGLCanvas1LeftUp,0,this);
    GLCanvas1->Connect(wxEVT_RIGHT_DOWN,(wxObjectEventFunction)&terrainFrame::OnGLCanvas1RightDown,0,this);
    GLCanvas1->Connect(wxEVT_RIGHT_UP,(wxObjectEventFunction)&terrainFrame::OnGLCanvas1RightUp,0,this);
    GLCanvas1->Connect(wxEVT_MOTION,(wxObjectEventFunction)&terrainFrame::OnGLCanvas1MouseMove,0,this);
    GLCanvas1->Connect(wxEVT_LEAVE_WINDOW,(wxObjectEventFunction)&terrainFrame::OnGLCanvas1MouseLeave,0,this);
    Connect(idMenuQuit,wxEVT_COMMAND_MENU_SELECTED,(wxObjectEventFunction)&terrainFrame::OnQuit);
    Connect(idMenuAbout,wxEVT_COMMAND_MENU_SELECTED,(wxObjectEventFunction)&terrainFrame::OnAbout);
    Connect(ID_TIMER1,wxEVT_TIMER,(wxObjectEventFunction)&terrainFrame::OnTimer1Trigger);
    //*)
    GLContext  = new wxGLContext(GLCanvas1);
    pitch = 0;
    yaw = 0;
    camx = 0;
    camy = 0;
    camz = 0;
    lastmousex = 0;
    lastmousey = 0;
}

inline double max(double a, double b)
{
    return a > b ? a : b;
}

double noise(double x)
{
    srand(pow(x, 1.12) * 61 - 1321);
    rand();
    return rand() / (double)RAND_MAX;
    //return noisetexture[int((x - floor(x / 100) * 100) * 100 + y - floor(y / 100) * 100)];
}
inline double noise(double x, double y)
{
    return noise(noise(x) * 1000 + noise(y * 12) * 877);
}

inline double lerp(double a, double b, double t)
{
    return a + t * (b - a);
}
inline double frac(double x)
{
    return x - floor(x);
}
inline double smoothnoise(double x, double y)
{
    return lerp(
                lerp(noise(floor(x), floor(y)    ), noise(floor(x) + 1, floor(y)    ), frac(x)),
                lerp(noise(floor(x), floor(y) + 1), noise(floor(x) + 1, floor(y) + 1), frac(x)),
                frac(y)
                );
}

double fbm(double x, double y, int octaves, double frequency, double persistence)
{
    if (octaves > 0)
        return smoothnoise(x, y) + persistence * fbm(x * frequency, y * frequency, octaves - 1, frequency, persistence);
    else
        return 0;
}

inline double voronoi(double x, double y, int gridsize, int npoints)
{
    srand(12345678);
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
    glHint(GL_PERSPECTIVE_CORRECTION_HINT, GL_NICEST);

    glEnable(GL_COLOR_MATERIAL);

    glViewport(0, 0, 480, 480);
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    glFrustum(-1, 1, -1, 1, 1, 1000);
    glRotatef(pitch * 180 / PI, 1, 0, 0);
    glRotatef(-yaw * 180 / PI, 0, 1, 0);
    glTranslatef(-camx, -camy, -camz);
    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();

    GLfloat mat_specular[] = { 0.9, 0.9, 1.0, 1.0 };
    GLfloat mat_shininess[] = { 50.0 };
    GLfloat light_position[] = { 1.0, 1.0, 1.0, 0.0 };

    glLightModeli(GL_LIGHT_MODEL_TWO_SIDE, GL_TRUE);
    glMaterialfv(GL_FRONT, GL_SPECULAR, mat_specular);
    glMaterialfv(GL_FRONT, GL_SHININESS, mat_shininess);
    glLightfv(GL_LIGHT0, GL_POSITION, light_position);

    glEnable(GL_LIGHTING);
    glEnable(GL_LIGHT0);
    glEnable(GL_COLOR_MATERIAL);

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
    glColor3f(1, 0, 0);
    glBegin(GL_POLYGON);
    glColor3f(1, 1, 0.8);
    glEnd();


    int gridsize = 70;
    double** grid = new double*[gridsize + 2];
    for (int i = 0; i < gridsize + 2; i++)
        grid[i] = new double[gridsize + 2];
    for (int i = 0; i < gridsize; i++)
    {
        for(int j = 0; j < gridsize; j++)
        {
//            grid[i][j] = max(voronoi(i, j, 100, 100) + fbm(i / 8.0, j / 8.0, 4, 2, 0.5) * 5 - 4, 5);
            grid[i][j] = voronoi(i, j, 100, 100);
        }
    }
    srand(123478);
    int nitrs = tick << 3;
    for (int i = 0; i < 10000; i++)
    {

        int x, y;
        x = rand() % gridsize + 1;
        y = rand() % gridsize + 1;
        while(true)
        {
            float mindiff = 0;
            float diff = grid[x + 1][y] - grid[x][y];
            int dir;
            if (diff < mindiff)
            {
                mindiff = diff;
                dir = 0;
            }
            diff = grid[x][y + 1] - grid[x][y];
            if (diff < mindiff)
            {
                mindiff = diff;
                dir = 1;
            }
            diff = grid[x - 1][y] - grid[x][y];
            if (diff < mindiff)
            {
                mindiff = diff;
                dir = 2;
            }
            diff = grid[x][y - 1] - grid[x][y];
            if (diff < mindiff)
            {
                mindiff = diff;
                dir = 3;
            }
            if (mindiff < 0)
            {
                grid[x][y] += max(mindiff, -0.1);
                switch(dir)
                {
                    case 0:
                        x++;
                        break;
                    case 1:
                        y++;
                        break;
                    case 2:
                        x--;
                        break;
                    case 3:
                        y--;
                        break;

                }
                x = x % gridsize + 1;
                y = y % gridsize + 1;
                //grid[x][y] -= mindiff * 0.1;
                //std::cout << "x: " << x << ", y: " << y << "\n";
                //std::cout << "<<<<  " << i << "  >>>>\n";
            }
            else
            {
                grid[x][y] +=  0.2;
                break;
            }
        }
    }
    for (int i = 0; i < gridsize + 2; i++)
    {
        for (int j = 0; j < gridsize + 2; j++)
        {
            grid[i][j] = max(grid[i][j], 5);
        }
    }
    vec3** normals = new vec3*[gridsize];
    for (int i = 0; i < gridsize; i++)
        normals[i] = new vec3[gridsize];
    for (int i = 0; i < gridsize; i++)
    {
        for (int j = 0; j < gridsize; j++)
        {
            normals[i][j] = vec3((grid[i + 2][j + 1] - grid[i][j + 1]) / 2, -1, (grid[i + 1][j + 2] - grid[i + 1][j]) / 2).normalise();
        }
    }
    for (int i = 0; i < gridsize - 1; i++)
    {
        for (int j = 0; j < gridsize - 1; j++)
        {
            glBegin(GL_POLYGON);
            glNormal3f(    normals[i    ][j    ].x, normals[i    ][j    ].y, normals[i    ][j    ].z);
            if (grid[i    ][j    ] <= 5)
                glColor3f(0.1, 0.1, 0.6);
            else
                glColor3f(0.1, 0.8, 0.1);
            glVertex3f(i    , grid[i    ][j    ], -j    );
            glNormal3f(    normals[i    ][j + 1].x, normals[i    ][j + 1].y, normals[i    ][j + 1].z);
            if (grid[i    ][j + 1] <= 5)
                glColor3f(0.1, 0.1, 0.6);
            else
                glColor3f(0.1, 0.8, 0.1);
            glVertex3f(i    , grid[i    ][j + 1], -j - 1);
            glNormal3f(    normals[i + 1][j + 1].x, normals[i + 1][j + 1].y, normals[i + 1][j + 1].z);
            if (grid[i + 1][j + 1] <= 5)
                glColor3f(0.1, 0.1, 0.6);
            else
                glColor3f(0.1, 0.8, 0.1);
            glVertex3f(i + 1, grid[i + 1][j + 1], -j - 1);
            glNormal3f(    normals[i + 1][j    ].x, normals[i + 1][j    ].y, normals[i + 1][j    ].z);
            if (grid[i + 1][j    ] <= 5)
                glColor3f(0.1, 0.1, 0.6);
            else
                glColor3f(0.1, 0.8, 0.1);
            glVertex3f(i + 1, grid[i + 1][j    ], -j    );
            glEnd();
        }
    }
    endgl();
    for (int i = 0; i < gridsize + 2; i++)
        delete grid[i];
    delete grid;
    for (int i = 0; i < gridsize; i++)
        delete normals[i];
    delete normals;
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
