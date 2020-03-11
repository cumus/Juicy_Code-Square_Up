#include "EditorWindows.h"
#include "Application.h"
#include "Render.h"

EditorWindow::EditorWindow(float x, float y, float w, float h) : x(x), y(y), w(w), h(h)
{
	r = g = b = 180;
	a = 220;
}

void EditorWindow::Draw(float width, float height) const
{
	// Calculate area
	SDL_Rect area = { int(x * width), int(y * height), int(w * width), int(h * height) };

	// Draw background
	App->render->DrawQuad(area, r, g, b, a, true, false);

	// Draw contents
	DrawContent(area);
}

void BarMenu::DrawContent(SDL_Rect area) const
{
}

void PlayPauseWindow::DrawContent(SDL_Rect area) const
{
}

void HeriarchyWindow::DrawContent(SDL_Rect area) const
{
}

void PropertiesWindow::DrawContent(SDL_Rect area) const
{
}

void ConsoleWindow::DrawContent(SDL_Rect area) const
{
}

void ConfigWindow::DrawContent(SDL_Rect area) const
{
}
