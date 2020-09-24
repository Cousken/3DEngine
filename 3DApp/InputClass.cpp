#include "InputClass.h"

InputClass::InputClass()
{

}

InputClass::InputClass(const InputClass&)
{

}

InputClass::~InputClass()
{

}

void InputClass::Initialize() 
{
	int i;

	// Initialize all the keys to being released and not pressed.
	for (i = 0; i < 256; i++)
	{
		m_keys[i] = false;
	}

	return;
}

bool InputClass::IsKeyDown(unsigned int key) const
{
	// Return what state the key is in (pressed/not pressed).
	return m_keys[key];
}

void InputClass::SetKeyDown(unsigned int key)
{
	// If a key is pressed then save that state in the key array.
	m_keys[key] = true;
	return;
}

void InputClass::SetKeyUp(unsigned int key)
{
	// If a key is released then clear that state in the key array.
	m_keys[key] = false;
	return;
}
