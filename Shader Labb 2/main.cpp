#define _DEBUG
#define DEBUG

#include <stdlib.h>
#include <crtdbg.h>

#include <windows.h>
#include <assert.h>
#include <FCollada.h>

//in project includes
#include "resource.h"
#include "Engine.h"
#include "Application.h"
#include "debugHeader.h"

#define _CRTDBG_MAP_ALLOC

LRESULT CALLBACK    WndProc( HWND, UINT, WPARAM, LPARAM );

//int wWinMain()
//{
//	fm::SetAllocationFunctions(malloc, free);
//}

//int WinMain()
//{
//	fm::SetAllocationFunctions(malloc, free);
//}

int WINAPI wWinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPWSTR lpCmdLine, int nCmdShow)
{
	fm::SetAllocationFunctions(malloc, free);

	MSG windowsMessage;
	HWND hWnd;
// 	DL::Debug::Create();
 	SetupInfo setup;
	setup.LoadFromFile();
// 	_chdir("data");
// 	FI_File::File file;
// 	file.Open("WindowInitScript.txt",1 << FI_File::READ);

	if (Engine::Create(nCmdShow, setup, WndProc, hInstance) == true)
	{
		Application application((*Engine::GetInstance()));
		application.Init();

		ShowCursor(true);
		while (1) 
		{
			// check for messages
			if ( PeekMessage( &windowsMessage, NULL, 0, 0, PM_REMOVE ) ) 
			{
				// handle or dispatch messages
				if ( windowsMessage.message == WM_QUIT )
				{
					break;
				}
				TranslateMessage( &windowsMessage );
				DispatchMessage( &windowsMessage );
			} 
			else
			{
				if(application.Update()==true)
				{
					application.Render();
				}
				else
				{
					break;
				}
			}
		}

		application.Destroy();
	}
	else
	{
		assert(false && "Could not create the engine!");
	}
	
	Engine::ShutDown();
	//DL::Debug::Destroy();
	// Clean up and shutdown

#ifdef DEBUG
	_CrtDumpMemoryLeaks();
#endif // DEBUG

	
	return 0;
}

LRESULT CALLBACK WndProc( HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam )
{
	fm::SetAllocationFunctions(malloc, free);
	
	PAINTSTRUCT ps;
	HDC hdc;

	switch( message )
	{
	case WM_PAINT:
		hdc = BeginPaint( hWnd, &ps );
		EndPaint( hWnd, &ps );
		break;

	case WM_DESTROY:
		PostQuitMessage( 0 );
		break;

	default:
		return DefWindowProc( hWnd, message, wParam, lParam );
	}

	return 0;
}

