/*+===================================================================
  File:      MAIN.CPP

  Summary:   This application demonstrates creating a Direct3D 11 device

  Origin:    http://msdn.microsoft.com/en-us/library/windows/apps/ff729718.aspx

  Originally created by Microsoft Corporation under MIT License
  © 2022 Kyung Hee University
===================================================================+*/

#include "Common.h"

#include "Game/Game.h"

/*F+F+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
  Function: wWinMain

  Summary:  Entry point to the program. Initializes everything and 
            goes into a message processing loop. Idle time is used to 
            render the scene.

  Args:     HINSTANCE hInstance
              Handle to an instance.
            HINSTANCE hPrevInstance
              Has no meaning.
            LPWSTR lpCmdLine
              Contains the command-line arguments as a Unicode 
              string
            INT nCmdShow
              Flag that says whether the main application window 
              will be minimized, maximized, or shown normally

  Returns:  INT
              Status code.
-----------------------------------------------------------------F-F*/
INT WINAPI wWinMain(_In_ HINSTANCE hInstance, _In_opt_ HINSTANCE hPrevInstance, _In_ LPWSTR lpCmdLine, _In_ INT nCmdShow)
{
    //remove C4100 error
    UNREFERENCED_PARAMETER(lpCmdLine);
    UNREFERENCED_PARAMETER(hPrevInstance);
    
    //initialization
    HRESULT InitWindow(_In_ HINSTANCE hInstance, _In_ INT nCmdShow);
    HRESULT InitDevice();

    if (FAILED(library::InitWindow(hInstance, nCmdShow)))
    {
        return 0;
    }

    if (FAILED(library::InitDevice()))
    {
        library::CleanupDevice();
        return 0;
    }    

    MSG msg = { 0 };
    
    //main message loop
    while (WM_QUIT != msg.message)
    {
        if (PeekMessage(&msg, NULL, 0, 0, PM_REMOVE))
        {
            TranslateMessage(&msg);
            DispatchMessage(&msg);
        }
        else
        {
            library::Render();
        }
    }

    //destroy
    library::CleanupDevice();

    return static_cast<INT>(msg.wParam);
}
