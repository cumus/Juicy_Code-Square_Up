#include "EditorWindows.h"
#include "Application.h"
#include "Input.h"
#include "Render.h"

EditorWindow::EditorWindow(float x, float y, float w, float h) : x(x), y(y), w(w), h(h)
{
	r = g = b = 250;
	a = 220;
}

bool EditorWindow::Update(float mouse_x, float mouse_y, KeyState mouse_left_button, bool sizing)
{
	float margin = 0.002f;

	if (sizing)
	{
		if (!dragging)
		{
			// Check borders
			if (mouse_x >= x - margin && mouse_x <= x + w + margin &&
				mouse_y >= y - margin && mouse_y <= y + h + margin)
			{
				// Clicking inside window
				if (InsideRect({ mouse_x, mouse_y }, GetBorderN_Normalized(margin)))
				{
					if (InsideRect({ mouse_x, mouse_y }, GetBorderW_Normalized(margin))) hovering = CORNER_NW;
					else if (InsideRect({ mouse_x, mouse_y }, GetBorderE_Normalized(margin))) hovering = CORNER_NE;
					else hovering = SIDE_N;
				}
				else if (InsideRect({ mouse_x, mouse_y }, GetBorderS_Normalized(margin)))
				{
					if (InsideRect({ mouse_x, mouse_y }, GetBorderW_Normalized(margin))) hovering = CORNER_SW;
					else if (InsideRect({ mouse_x, mouse_y }, GetBorderE_Normalized(margin))) hovering = CORNER_SE;
					else hovering = SIDE_S;
				}
				else if (InsideRect({ mouse_x, mouse_y }, GetBorderW_Normalized(margin))) hovering = SIDE_W;
				else if (InsideRect({ mouse_x, mouse_y }, GetBorderE_Normalized(margin))) hovering = SIDE_E;

				if (hovering != NONE && mouse_left_button == KEY_DOWN)
					dragging = true;
			}
			else
				hovering = NONE;
		}
		else
		{
			// Editing window sizes
			switch (hovering)
			{
			case SIDE_N:
			{
				h -= mouse_y - y;
				y = mouse_y;
				break;
			}
			case SIDE_W:
			{
				w -= mouse_x - x;
				x = mouse_x;
				break;
			}
			case SIDE_E:
			{
				w = mouse_x - x;
				break;
			}
			case SIDE_S:
			{
				h = mouse_y - y;
				break;
			}
			case CORNER_NW:
			{
				h -= mouse_y - y;
				y = mouse_y;
				w -= mouse_x - x;
				x = mouse_x;
				break;
			}
			case CORNER_NE:
			{
				h -= mouse_y - y;
				y = mouse_y;
				w = mouse_x - x;
				break;
			}
			case CORNER_SW:
			{
				h = mouse_y - y;
				w -= mouse_x - x;
				x = mouse_x;
				break;
			}
			case CORNER_SE:
			{
				h = mouse_y - y;
				w = mouse_x - x;
				break;
			}
			default:
				break;
			}

			if (mouse_left_button != KEY_REPEAT)
				dragging = false;
		}
	}

	if (mouse_x >= x && mouse_x <= x + w
		&& mouse_y >= y && mouse_y <= y + h)
	{
		// Mouse inside window
		mouse_inside = true;

		/*if (mouse_left_button == KEY_DOWN || mouse_left_button == KEY_REPEAT)
		{
			// Mouse Click content
			for (std::vector<UI_Element*>::iterator it = elements.begin(); it != elements.end(); ++it)
			{
				if (InsideRect({ mouse_x - x, mouse_y - y }, (*it)->rect))
			}
		}*/
	}
	else
	{
		mouse_inside = false;
	}

	a = (mouse_inside ? 250 : 220);

	return mouse_inside;
}

void EditorWindow::Draw(float width, float height, bool draw_border) const
{
	// Calculate area
	SDL_Rect area = { int(x * width), int(y * height), int(w * width), int(h * height) };

	// Draw background
	App->render->DrawQuad(area, r, g, b, a, true, false);

	// Draw contents
	DrawContent(area);

	// Draw Border
	if (draw_border)
		DrawBorders(width, height);
}

void EditorWindow::DrawBorders(float width, float height, float margin) const
{
	App->render->DrawQuad(GetBorderN(width, height, margin), 150, (hovering == SIDE_N || hovering == CORNER_NW || hovering == CORNER_NE ? 150 : 0), 0, 200, true, false);
	App->render->DrawQuad(GetBorderW(width, height, margin), 150, (hovering == SIDE_W || hovering == CORNER_NW || hovering == CORNER_SW ? 150 : 0), 0, 200, true, false);
	App->render->DrawQuad(GetBorderE(width, height, margin), 150, (hovering == SIDE_E || hovering == CORNER_SE || hovering == CORNER_NE ? 150 : 0), 0, 200, true, false);
	App->render->DrawQuad(GetBorderS(width, height, margin), 150, (hovering == SIDE_S || hovering == CORNER_SE || hovering == CORNER_SW ? 150 : 0), 0, 200, true, false);
}

SDL_Rect EditorWindow::GetBorderN(float width, float height, float margin) const
{
	return { int((x - margin) * width),
		int((y - margin) * height),
		int((w + margin * 2.0f) * width),
		int(margin * 2.0f * height) };
}

SDL_Rect EditorWindow::GetBorderW(float width, float height, float margin) const
{
	return { int((x - margin) * width),
		int((y - margin) * height),
		int((margin * 2.0f) * width),
		int((h + margin * 2.0f) * height) };
}

SDL_Rect EditorWindow::GetBorderE(float width, float height, float margin) const
{
	return { int((x + w - margin) * width),
		int((y - margin) * height),
		int((margin * 2.0f) * width),
		int((h + margin * 2.0f) * height) };
}

SDL_Rect EditorWindow::GetBorderS(float width, float height, float margin) const
{
	return { int((x - margin) * width),
		int((y + h - margin) * height),
		int((w + margin * 2.0f) * width),
		int(margin * 2.0f * height) };
}

RectF EditorWindow::GetBorderN_Normalized(float margin) const
{
	return { x - margin,
		y - margin,
		w + margin * 2.0f,
		margin * 2.0f };
}

RectF EditorWindow::GetBorderW_Normalized(float margin) const
{
	return { x - margin,
		y - margin,
		margin * 2.0f,
		h + margin * 2.0f };
}

RectF EditorWindow::GetBorderE_Normalized(float margin) const
{
	return { x + w - margin,
		y - margin,
		margin * 2.0f,
		h + margin * 2.0f };
}

RectF EditorWindow::GetBorderS_Normalized(float margin) const
{
	return { x - margin,
		y + h - margin,
		w + margin * 2.0f,
		margin * 2.0f };
}

bool EditorWindow::InsideRect(std::pair<float, float> p, RectF rect)
{
	return
		  (p.first >= rect.x
		&& p.first <= rect.x + rect.w
		&& p.second >= rect.y
		&& p.second <= rect.y + rect.h);
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
