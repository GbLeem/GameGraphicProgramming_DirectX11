/*+===================================================================
  File:      MAIN.CPP

  Summary:   This application demonstrates creating a Direct3D 11 device

  Origin:    http://msdn.microsoft.com/en-us/library/windows/apps/ff729718.aspx

  Originally created by Microsoft Corporation under MIT License
  � 2022 Kyung Hee University
===================================================================+*/

#include "Common.h"

#include <memory>

#include "Game/Game.h"
#include "Cube/YourCube.h"
#include "Cube/SmallCube.h"
#include "Cube/innerCube.h"
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

#ifdef _DEBUG
    _CrtSetDbgFlag(_CRTDBG_ALLOC_MEM_DF | _CRTDBG_LEAK_CHECK_DF);
#endif

    UNREFERENCED_PARAMETER(hPrevInstance);
    UNREFERENCED_PARAMETER(lpCmdLine);

    std::unique_ptr<library::Game> game = std::make_unique<library::Game>(L"Game Graphics Programming Lab 04: 3D Spaces and Transformations");

    std::shared_ptr<library::VertexShader> vertexShader = std::make_shared<library::VertexShader>(L"Shaders/Shaders.fxh", "VS", "vs_5_0");
    if (FAILED(game->GetRenderer()->AddVertexShader(L"MainShader", vertexShader)))
    {
        return 0;
    }
    
    std::shared_ptr<library::PixelShader> pixelShader = std::make_shared<library::PixelShader>(L"Shaders/Shaders.fxh", "PS", "ps_5_0");
    if (FAILED(game->GetRenderer()->AddPixelShader(L"MainShader", pixelShader)))
    {
        return 0;
    }

    if (FAILED(game->GetRenderer()->AddRenderable(L"YourCube", std::make_shared<YourCube>())))
    {
        return 0;
    }
    if (FAILED(game->GetRenderer()->SetVertexShaderOfRenderable(L"YourCube", L"MainShader")))
    {
        return 0;
    }
    if (FAILED(game->GetRenderer()->SetPixelShaderOfRenderable(L"YourCube", L"MainShader")))
    {
        return 0;
    }

    if (FAILED(game->GetRenderer()->AddRenderable(L"SmallCube", std::make_shared<SmallCube>())))
    {
        return 0;
    }
    if (FAILED(game->GetRenderer()->SetVertexShaderOfRenderable(L"SmallCube", L"MainShader")))
    {
        return 0;
    }
    if (FAILED(game->GetRenderer()->SetPixelShaderOfRenderable(L"SmallCube", L"MainShader")))
    {
        return 0;
    }

    if (FAILED(game->GetRenderer()->AddRenderable(L"innerCube", std::make_shared<innerCube>())))
    {
        return 0;
    }
    if (FAILED(game->GetRenderer()->SetVertexShaderOfRenderable(L"innerCube", L"MainShader")))
    {
        return 0;
    }
    if (FAILED(game->GetRenderer()->SetPixelShaderOfRenderable(L"innerCube", L"MainShader")))
    {
        return 0;
    }

    if (FAILED(game->Initialize(hInstance, nCmdShow)))
    {
        return 0;
    }

    return game->Run();
}