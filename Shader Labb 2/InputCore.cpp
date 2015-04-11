#include "InputCore.h"
#include <cassert>

Library2D::InputCore::InputCore(const HINSTANCE__ *aHInstance, const HWND__ *const aHWND)
	:myNumKeys(256)
{
	assert(aHInstance != NULL);
	assert(aHWND != NULL);

	myDirectInput = NULL;
	myKeyboardDevice = NULL;
	myMouseDevice = NULL;	
	myMousePosition = Vector2<float>(0, 0);

	for(unsigned int index = 0; index < myNumKeys; index++)
	{
		myKeyStates[index] = false;
	}

	DirectInput8Create(HINSTANCE(aHInstance), DIRECTINPUT_VERSION, IID_IDirectInput8, reinterpret_cast<LPVOID *>(&myDirectInput), NULL);

	myDirectInput->CreateDevice(GUID_SysKeyboard, &myKeyboardDevice, NULL);
	myDirectInput->CreateDevice(GUID_SysMouse, &myMouseDevice, NULL);

	myKeyboardDevice->SetDataFormat(&c_dfDIKeyboard);	
	const HWND windowHandle = HWND(aHWND);
	myKeyboardDevice->SetCooperativeLevel(windowHandle, DISCL_NONEXCLUSIVE | DISCL_FOREGROUND);
	
	myMouseDevice->SetDataFormat(&c_dfDIMouse);
	myMouseDevice->SetCooperativeLevel(windowHandle, DISCL_EXCLUSIVE | DISCL_FOREGROUND);
	
	//Below does not work
	//ShowCursor(aShowCursorFlag); 
}


bool Library2D::InputCore::IsKeyDown(const int aKey) const
{
	return (myKeyStates[aKey] & 0x80) != 0;
}


bool Library2D::InputCore::IsMouseDown(const int aMouseButton) const
{
	return (myMouseState.rgbButtons[aMouseButton] & 0x80) != 0;
}


Vector2<float> Library2D::InputCore::GetMousePosition() const
{
	return myMousePosition;
}


Vector2<float> Library2D::InputCore::GetMouseMovement() const
{
	return Vector2<float>(static_cast<float>(myMouseState.lX), static_cast<float>(myMouseState.lY));
}


float Library2D::InputCore::GetMouseWheelMovement() const
{
	return static_cast<float>(myMouseState.lZ);
}


void Library2D::InputCore::Update()
{	
	myKeyboardDevice->Acquire();
	myKeyboardDevice->GetDeviceState(myNumKeys, static_cast<LPVOID>(myKeyStates));
	
	myMouseDevice->Acquire();	
	myMouseDevice->GetDeviceState(sizeof(DIMOUSESTATE), static_cast<LPVOID>(&myMouseState));

	myMousePosition.x += myMouseState.lX;
	myMousePosition.y += myMouseState.lY;

	//myMousePosition.x = MAX(myMousePosition.x, 0);
	//myMousePosition.y = MAX(myMousePosition.y, 0);
	//myMousePosition.x = MIN(myMousePosition.x, 800);
	//myMousePosition.y = MIN(myMousePosition.y, 600);
}


Library2D::InputCore::~InputCore()
{
	if(myKeyboardDevice != NULL)
	{
		myKeyboardDevice->Unacquire();
	}

	if(myMouseDevice != NULL)
	{
		myMouseDevice->Unacquire();
	}

	if(myDirectInput != NULL)
	{
		myDirectInput->Release();		
	}
}