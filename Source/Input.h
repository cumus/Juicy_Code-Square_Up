#ifndef __INPUT_H__
#define __INPUT_H__

#include "Module.h"

#define NUM_MOUSE_BUTTONS 5

struct SDL_Rect;

enum KeyState
{
	KEY_IDLE = 0,
	KEY_DOWN,
	KEY_REPEAT,
	KEY_UP
};

class Input : public Module
{
public:

	Input();
	~Input();

	bool Init() override;
	bool Start() override;
	bool PreUpdate() override;
	bool CleanUp() override;

	inline KeyState GetKey(int id) const
	{
		return keyboard[id];
	}

	inline KeyState GetMouseButtonDown(int id) const
	{
		return mouse_buttons[id];
	}

	void GetMousePosition(int &x, int &y) const;
	void GetMouseMotion(int& x, int& y) const;
	int GetMouseWheelMotion() const;

private:

	KeyState keyboard[MAX_KEYS];
	KeyState mouse_buttons[NUM_MOUSE_BUTTONS];

	int mouse_motion_x = 0;
	int mouse_motion_y = 0;
	int mouse_x = 0;
	int mouse_y = 0;
	int mouse_wheel_motion = 0;
};

#endif // __INPUT_H__