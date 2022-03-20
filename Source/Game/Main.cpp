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
    //remove C4100 warning
    UNREFERENCED_PARAMETER(lpCmdLine);
    UNREFERENCED_PARAMETER(hPrevInstance);
    
    //initialize Window and Direct3D device
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

    // Main message loop
    MSG msg = { 0 };
   
    //Main Message Loop
    while (WM_QUIT != msg.message)
    {
        if (PeekMessage(&msg, NULL, 0, 0, PM_REMOVE))
        {
            TranslateMessage(&msg);
            DispatchMessage(&msg);
            //두 함수가 호출되면 WindowProc 호출된다.
        }
        else
        {
            library::Render();
        }
    }

    library::CleanupDevice();

    return static_cast<INT>(msg.wParam);
}
