/***************************************************************
 * Name:      terrainApp.cpp
 * Purpose:   Code for Application Class
 * Author:    Luke Wren (wren6991@gmail.com)
 * Created:   2012-03-24
 * Copyright: Luke Wren ()
 * License:
 **************************************************************/

#include "terrainApp.h"

//(*AppHeaders
#include "terrainMain.h"
#include <wx/image.h>
//*)

IMPLEMENT_APP(terrainApp);

bool terrainApp::OnInit()
{
    //(*AppInitialize
    bool wxsOK = true;
    wxInitAllImageHandlers();
    if ( wxsOK )
    {
    	terrainFrame* Frame = new terrainFrame(0);
    	Frame->Show();
    	SetTopWindow(Frame);
    }
    //*)
    return wxsOK;

}
