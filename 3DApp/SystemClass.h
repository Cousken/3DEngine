#pragma once
#include <windows.h>
#include <memory>

#include "inputclass.h"
#include "graphicsclass.h"

class SystemClass
{
public:
	SystemClass();
	SystemClass(const SystemClass&);
	~SystemClass();

	bool Initialize();
	void Shutdown();
	void Run();

	LRESULT CALLBACK MessageHandler(HWND, UINT, WPARAM, LPARAM);
	HINSTANCE GetAppHandle();


	static void CreateInstance();
	static std::unique_ptr<SystemClass>& GetInstance();
	static INT_PTR About(HWND hDlg, UINT message, WPARAM wParam, LPARAM lParam);


protected:


private:
	bool Frame();
	void InitializeWindows(int&, int&);
	void ShutdownWindows();


	LPCWSTR m_applicationName;
	HINSTANCE m_hinstance;
	HWND m_hwnd;

	std::unique_ptr<InputClass> m_Input;
	std::unique_ptr<GraphicsClass> m_Graphics;

};

/////////////////////////
// FUNCTION PROTOTYPES //
/////////////////////////
static LRESULT CALLBACK WndProc(HWND, UINT, WPARAM, LPARAM);


/////////////
// GLOBALS //
/////////////
extern std::unique_ptr<SystemClass> g_ApplicationHandle;


