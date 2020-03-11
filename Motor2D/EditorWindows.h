#ifndef __EDITOR_WINDOWS_H__
#define __EDITOR_WINDOWS_H__

#include "SDL/include/SDL_rect.h"

class EditorWindow
{
public:

	EditorWindow(float x, float y, float w, float h);
	virtual ~EditorWindow() {}

	void Draw(float width, float height) const;

	virtual void DrawContent(SDL_Rect area) const {}

protected:

	float x, y, w, h;
	unsigned int r, g, b, a;
};

class BarMenu : public EditorWindow
{
public:

	BarMenu(float x, float y, float w, float h) : EditorWindow(x, y, w, h) {}
	~BarMenu() {}

	void DrawContent(SDL_Rect area) const override;
};

class PlayPauseWindow : public EditorWindow
{
public:

	PlayPauseWindow(float x, float y, float w, float h) : EditorWindow(x, y, w, h) {}
	~PlayPauseWindow() {}

	void DrawContent(SDL_Rect area) const override;
};

class HeriarchyWindow : public EditorWindow
{
public:

	HeriarchyWindow(float x, float y, float w, float h) : EditorWindow(x, y, w, h) {}
	~HeriarchyWindow() {}

	void DrawContent(SDL_Rect area) const override;
};

class PropertiesWindow : public EditorWindow
{
public:

	PropertiesWindow(float x, float y, float w, float h) : EditorWindow(x, y, w, h) {}
	~PropertiesWindow() {}

	void DrawContent(SDL_Rect area) const override;
};

class ConsoleWindow : public EditorWindow
{
public:

	ConsoleWindow(float x, float y, float w, float h) : EditorWindow(x, y, w, h) {}
	~ConsoleWindow() {}

	void DrawContent(SDL_Rect area) const override;
};

class ConfigWindow : public EditorWindow
{
public:

	ConfigWindow(float x, float y, float w, float h) : EditorWindow(x, y, w, h) {}
	~ConfigWindow() {}

	void DrawContent(SDL_Rect area) const override;
};

#endif // __EDITOR_WINDOWS_H__