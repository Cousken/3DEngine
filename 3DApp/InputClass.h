#pragma once
class InputClass
{
public:
	InputClass();
	InputClass(const InputClass&);
	~InputClass();

	void Initialize();
	bool IsKeyDown(unsigned int key) const;
	void SetKeyDown(unsigned int key);
	void SetKeyUp(unsigned int key);

private:
	bool m_keys[256];
};

