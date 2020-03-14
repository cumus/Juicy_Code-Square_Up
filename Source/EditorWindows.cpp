#include "EditorWindows.h"
#include "Application.h"
#include "Input.h"
#include "Render.h"
#include "JuicyMath.h"

#include "Optick/include/optick.h"

EditorWindow::EditorWindow(const RectF rect) : rect(rect)
{
	color = { 250, 250, 250, 220 };
}

EditorWindow::~EditorWindow()
{}

bool EditorWindow::CheckIfEditing(float mouse_x, float mouse_y, KeyState mouse_left_button)
{
	if (!dragging)
	{
		// Check borders
		if (mouse_x >= rect.x - margin && mouse_x <= rect.x + rect.w + margin &&
			mouse_y >= rect.y - margin && mouse_y <= rect.y + rect.h + margin)
		{
			// Clicking inside window
			if (JMath::PointInsideRect(mouse_x, mouse_y, GetBorderN_Norm()))
			{
				if (JMath::PointInsideRect(mouse_x, mouse_y, GetBorderW_Norm())) hovering = CORNER_NW;
				else if (JMath::PointInsideRect(mouse_x, mouse_y, GetBorderE_Norm())) hovering = CORNER_NE;
				else hovering = SIDE_N;
			}
			else if (JMath::PointInsideRect(mouse_x, mouse_y, GetBorderS_Norm()))
			{
				if (JMath::PointInsideRect(mouse_x, mouse_y, GetBorderW_Norm())) hovering = CORNER_SW;
				else if (JMath::PointInsideRect(mouse_x, mouse_y, GetBorderE_Norm())) hovering = CORNER_SE;
				else hovering = SIDE_S;
			}
			else if (JMath::PointInsideRect(mouse_x, mouse_y, GetBorderW_Norm())) hovering = SIDE_W;
			else if (JMath::PointInsideRect(mouse_x, mouse_y, GetBorderE_Norm())) hovering = SIDE_E;

			if (hovering != SIDE_NONE && mouse_left_button == KEY_DOWN)
				dragging = true;
		}
		else
			hovering = SIDE_NONE;
	}
	else
	{
		// Editing window sizes
		switch (hovering)
		{
		case SIDE_N:
		{
			rect.h -= mouse_y - rect.y;
			rect.y = mouse_y;
			break;
		}
		case SIDE_W:
		{
			rect.w -= mouse_x - rect.x;
			rect.x = mouse_x;
			break;
		}
		case SIDE_E:
		{
			rect.w = mouse_x - rect.x;
			break;
		}
		case SIDE_S:
		{
			rect.h = mouse_y - rect.y;
			break;
		}
		case CORNER_NW:
		{
			rect.h -= mouse_y - rect.y;
			rect.y = mouse_y;
			rect.w -= mouse_x - rect.x;
			rect.x = mouse_x;
			break;
		}
		case CORNER_NE:
		{
			rect.h -= mouse_y - rect.y;
			rect.y = mouse_y;
			rect.w = mouse_x - rect.x;
			break;
		}
		case CORNER_SW:
		{
			rect.h = mouse_y - rect.y;
			rect.w -= mouse_x - rect.x;
			rect.x = mouse_x;
			break;
		}
		case CORNER_SE:
		{
			rect.h = mouse_y - rect.y;
			rect.w = mouse_x - rect.x;
			break;
		}
		default:
			break;
		}

		if (mouse_left_button != KEY_REPEAT)
			dragging = false;
	}

	return dragging;
}

bool EditorWindow::Update(float mouse_x, float mouse_y, KeyState mouse_left_button)
{
	if (mouse_x >= rect.x && mouse_x <= rect.x + rect.w
		&& mouse_y >= rect.y && mouse_y <= rect.y + rect.h)
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

	color.a = (mouse_inside ? 255 : 220);

	return mouse_inside;
}

void EditorWindow::Draw(bool draw_border) const
{
	// Draw background
	App->render->DrawQuadNormCoords(rect, color);

	// Draw contents
	DrawContent();

	// Draw Border
	if (draw_border)
		DrawBorders();
}

void EditorWindow::DrawBorders() const
{
	App->render->DrawQuadNormCoords(GetBorderN_Norm(), { 150, (hovering == SIDE_N || hovering == CORNER_NW || hovering == CORNER_NE ? 150u : 0), 0, 200 });
	App->render->DrawQuadNormCoords(GetBorderW_Norm(), { 150, (hovering == SIDE_W || hovering == CORNER_NW || hovering == CORNER_SW ? 150u : 0), 0, 200 });
	App->render->DrawQuadNormCoords(GetBorderE_Norm(), { 150, (hovering == SIDE_E || hovering == CORNER_SE || hovering == CORNER_NE ? 150u : 0), 0, 200 });
	App->render->DrawQuadNormCoords(GetBorderS_Norm(), { 150, (hovering == SIDE_S || hovering == CORNER_SE || hovering == CORNER_SW ? 150u : 0), 0, 200 });
}

RectF EditorWindow::GetBorderN_Norm() const
{
	return { rect.x - margin,
		rect.y - margin,
		rect.w + margin * 2.0f,
		margin * 2.0f };
}

RectF EditorWindow::GetBorderW_Norm() const
{
	return { rect.x - margin,
		rect.y - margin,
		margin * 2.0f,
		rect.h + margin * 2.0f };
}

RectF EditorWindow::GetBorderE_Norm() const
{
	return { rect.x + rect.w - margin,
		rect.y - margin,
		margin * 2.0f,
		rect.h + margin * 2.0f };
}

RectF EditorWindow::GetBorderS_Norm() const
{
	return { rect.x - margin,
		rect.y + rect.h - margin,
		rect.w + margin * 2.0f,
		margin * 2.0f };
}

void BarMenu::DrawContent() const
{
}

void PlayPauseWindow::DrawContent() const
{
}

void HeriarchyWindow::DrawContent() const
{
}

void PropertiesWindow::DrawContent() const
{
}

void ConsoleWindow::DrawContent() const
{
}

void ConfigWindow::DrawContent() const
{
}
