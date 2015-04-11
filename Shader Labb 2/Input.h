#ifndef _INPUT_H_
#define _INPUT_H_

#include "Vector.h"
#include "Keys.h"
#include "MouseButtons.h"
class hgeSprite;
struct HINSTANCE__;
struct HWND__;

namespace Library2D
{
	class InputCore;
}

namespace Library2D
{
	class Input
	{
	public:
		Input(const HINSTANCE__ *const aHInstance, const HWND__ *const aHWND);
		~Input();

		bool IsKeyDown(const int aKey) const;
		bool IsMouseDown(const int aMouseButton) const;
		Vector2<float> GetMousePosition() const;
		Vector2<float> GetMouseMovement() const;
		float GetMouseWheelMovement() const;

		void Update();

	private:
		InputCore* myInputCore;
	};
}

#endif // _INPUT_H_