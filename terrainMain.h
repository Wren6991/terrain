/***************************************************************
 * Name:      terrainMain.h
 * Purpose:   Defines Application Frame
 * Author:    Luke Wren (wren6991@gmail.com)
 * Created:   2012-03-24
 * Copyright: Luke Wren ()
 * License:
 **************************************************************/

#ifndef TERRAINMAIN_H
#define TERRAINMAIN_H

//(*Headers(terrainFrame)
#include <wx/sizer.h>
#include <wx/menu.h>
#include <wx/glcanvas.h>
#include <wx/frame.h>
#include <wx/timer.h>
#include <wx/statusbr.h>
//*)

class terrainFrame: public wxFrame
{
    public:

        terrainFrame(wxWindow* parent,wxWindowID id = -1);
        virtual ~terrainFrame();
        double pitch;
        double yaw;
        double camx, camy, camz;
        double lastmousex, lastmousey;
        bool mousedownl;
        bool mousedownr;

    private:

        void initgl();
        void endgl();

        //(*Handlers(terrainFrame)
        void OnQuit(wxCommandEvent& event);
        void OnAbout(wxCommandEvent& event);
        void OnGLCanvas1Paint(wxPaintEvent& event);
        void OnGLCanvas1LeftDown(wxMouseEvent& event);
        void OnTimer1Trigger(wxTimerEvent& event);
        void OnGLCanvas1LeftUp(wxMouseEvent& event);
        void OnGLCanvas1MouseMove(wxMouseEvent& event);
        void OnGLCanvas1MouseLeave(wxMouseEvent& event);
        void OnGLCanvas1KeyDown(wxKeyEvent& event);
        void OnGLCanvas1RightDown(wxMouseEvent& event);
        void OnGLCanvas1RightUp(wxMouseEvent& event);
        //*)

        //(*Identifiers(terrainFrame)
        static const long ID_GLCANVAS1;
        static const long idMenuQuit;
        static const long idMenuAbout;
        static const long ID_STATUSBAR1;
        static const long ID_TIMER1;
        //*)

        wxGLContext *GLContext;
        //(*Declarations(terrainFrame)
        wxGLCanvas* GLCanvas1;
        wxStatusBar* StatusBar1;
        wxTimer Timer1;
        //*)

        DECLARE_EVENT_TABLE()
};

#endif // TERRAINMAIN_H
