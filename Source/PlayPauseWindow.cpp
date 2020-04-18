#include "PlayPauseWindow.h"
#include "Application.h"
#include "TextureManager.h"
#include "UI_ButtonImage.h"
#include "UI_Text.h"
#include <string>

PlayPauseWindow::PlayPauseWindow(const RectF rect) : EditorWindow(rect)
{
	sections[0] = {   1,   1, 100, 100 };
	sections[1] = { 122,   0, 100, 100 };
	sections[2] = { 244,   0, 100, 100 };
	sections[3] = {   1, 116, 100, 100 };
}

PlayPauseWindow::~PlayPauseWindow()
{
}

bool PlayPauseWindow::Init()
{
	color.a = 80;
	icons_texture = App->tex.Load("Assets/textures/icons.png");

	if (icons_texture >= 0)
	{
		elements.push_back(new UI_ButtonImage(this, { 0.0f,  0.0f, 0.15f, 1.0f }, icons_texture, {   1,   1, 100, 100 }));
		elements.push_back(new UI_ButtonImage(this, { 0.17f, 0.0f, 0.15f, 1.0f }, icons_texture, {   1, 116, 100, 100 }));
		elements.push_back(new UI_ButtonImage(this, { 0.34f, 0.0f, 0.15f, 1.0f }, icons_texture, { 244,   0, 100, 100 }));
		elements.push_back(new UI_Text(this, { 0.5f, 0.1f, 0.45f, 0.8f }, "time"));
	}

	return !elements.empty();
}


void PlayPauseWindow::RecieveEvent(const Event& e)
{
	int id = e.data1.AsInt();
	if (id >= 0 && e.type == MOUSE_UP)
	{
		switch (Content(id))
		{
		case PlayPauseWindow::PLAY_PAUSE:
		{
			bool is_pausing = App->GetState() == PLAYING;
			elements[PLAY_PAUSE]->ToUiButtonImage()->section = sections[!is_pausing];
			Event::Push(is_pausing ? SCENE_PAUSE : SCENE_PLAY, App);
			break; 
		}
		case PlayPauseWindow::TICK:
		{
			if (App->GetState() != PLAYING)
				Event::Push(SCENE_TICK, App);
			break;
		}
		case PlayPauseWindow::STOP:
		{
			elements[PLAY_PAUSE]->ToUiButtonImage()->section = sections[0];
			Event::Push(SCENE_STOP, App);
			break;
		}
		default:
			break;
		}
	}
}

void PlayPauseWindow::_Update()
{
	elements[TIMER]->ToUiText()->text->SetText(std::to_string(App->time.GetGameTimer()).c_str());
}
