#ifndef __PLAYPAUSE_WINDOW_H__
#define __PLAYPAUSE_WINDOW_H__

#include "EditorWindow.h"

class PlayPauseWindow : public EditorWindow
{
public:

	PlayPauseWindow(const RectF rect);
	~PlayPauseWindow();

	bool Init() override;
	void RecieveEvent(const Event& e) override;

private:

	void _Update() override;

private:

	enum Content
	{
		PLAY_PAUSE,
		TICK,
		STOP,
		TIMER
	};

	int icons_texture = -1;

	SDL_Rect sections[4];
};

#endif // __PLAYPAUSE_WINDOW_H__