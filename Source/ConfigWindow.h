#ifndef __CONFIG_WINDOW_H__
#define __CONFIG_WINDOW_H__

#include "EditorWindow.h"

class ConfigWindow : public EditorWindow
{
public:

	ConfigWindow(const RectF rect);
	~ConfigWindow();

	bool Init() override;
	void RecieveEvent(const Event& e) override;

private:

	void _Update() override;

private:

	int tex_id;
	int tex_in;
	int tex_out;
	int tex_down;
	int tex_repeat;
	int tex_up;

	// Test Slider

	int SliderBar_tex_id;
	RectF SliderBar_Rect;
	RectF SliderButton_Rect;

};

#endif // __CONFIG_WINDOW_H__