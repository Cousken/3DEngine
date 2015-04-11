#include "Input.h"
#include "InputCore.h"
#include <cassert>

Library2D::Input::Input(const HINSTANCE__ *const aHInstance, const HWND__ *const aHWND)
{
	assert(aHInstance != NULL);
	assert(aHWND != NULL);
	myInputCore = new InputCore(aHInstance, aHWND);
}

Library2D::Input::~Input()
{
	if(myInputCore)
	{
		delete myInputCore;
		myInputCore = NULL;
	}
}

bool Library2D::Input::IsKeyDown(const int aKey) const
{
	assert(myInputCore != NULL);
	return myInputCore->IsKeyDown(aKey);
}

bool Library2D::Input::IsMouseDown(const int aMouseButton) const
{
	assert(myInputCore != NULL);
	return myInputCore->IsMouseDown(aMouseButton);
}

Vector2<float> Library2D::Input::GetMousePosition() const
{
	assert(myInputCore != NULL);
	return myInputCore->GetMousePosition();
}

Vector2<float> Library2D::Input::GetMouseMovement() const
{
	assert(myInputCore != NULL);
	return myInputCore->GetMouseMovement();
}

float Library2D::Input::GetMouseWheelMovement() const
{
	assert(myInputCore != NULL);
	return myInputCore->GetMouseWheelMovement();
}

void Library2D::Input::Update()
{
	assert(myInputCore != NULL);
	myInputCore->Update();
}
