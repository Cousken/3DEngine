#pragma once

#include <windows.h>

/////////////
// GLOBALS //
/////////////
const bool FULL_SCREEN = false;
const bool VSYNC_ENABLED = true;
const float SCREEN_DEPTH = 1000.0f;
const float SCREEN_NEAR = 0.1f;

class GraphicsClass
{

public:
	bool Initialize(int screenHeight, int screenWidth, HWND hwnd);
	void Shutdown() const;
	bool Frame() const;

private:
	bool Render();
};

