#ifndef _INPUT_CORE_H_
#define _INPUT_CORE_H_

#include "Vector.h"
#include "Keys.h"
#include "MouseButtons.h"
#include <dinput.h>
class HGE;

namespace Library2D
{
	class InputCore
	{
	public:
		InputCore(const HINSTANCE__ *const aHInstance, const HWND__ *const aHWND);
		~InputCore(void);

		bool IsKeyDown(const int aKey) const;
		bool IsMouseDown(const int aMouseButton) const;
		Vector2<float> GetMousePosition() const;
		Vector2<float> GetMouseMovement() const;
		float GetMouseWheelMovement() const;
		void Update();

	private:
		const unsigned int myNumKeys;
		LPDIRECTINPUT8 myDirectInput;
		LPDIRECTINPUTDEVICE8 myKeyboardDevice;
		LPDIRECTINPUTDEVICE8 myMouseDevice;
		DIMOUSESTATE myMouseState;
		BYTE myKeyStates[256];    
		Vector2<float> myMousePosition;

		InputCore operator=(const InputCore &anInputCore);
	};
}



#endif // _INPUT_CORE_H_