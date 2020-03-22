#include "ConfigWindow.h"
#include "Application.h"
#include "TextureManager.h"
#include "UI_ButtonImage.h"
#include "UI_TextButton.h"

ConfigWindow::ConfigWindow(const RectF rect) : EditorWindow(rect)
{}

ConfigWindow::~ConfigWindow()
{}

bool ConfigWindow::Init()
{
	tex_id = App->tex.Load("textures/background.png");
	tex_in = App->tex.Load("textures/joseph.png");
	tex_out = App->tex.Load("textures/goku.png");
	tex_down = App->tex.Load("textures/fullmetal.png");
	tex_repeat = App->tex.Load("textures/naruto.png");
	tex_up = App->tex.Load("textures/onepiece.png");

	if (tex_id >= 0)
		// elements.push_back(new UI_Image(this, { 0.5f, 0.4f, 0.4f, 0.5f }, tex_id));

		elements.push_back(new UI_ButtonImage(this, { 0.25f, 0.6f, 0.5f, 0.25f }, tex_out));

	// elements.push_back(new UI_Button(this, { 0.0f, 0.0f, 0.5f, 0.25f }));

	// elements.push_back(new UI_Text(this, { 0.25f, 0.58f,  0.45f, 0.25f }, 1, "Hello"));

	elements.push_back(new UI_TextButton(this, { 0.0f, 0.0f, 0.5f, 0.25f }, "Press Here"));

	return !elements.empty();
}

void ConfigWindow::RecieveEvent(const Event& e)
{

	switch (e.data1.AsInt())
	{
	case 0:
	{
		switch (e.type)
		{
		case HOVER_IN:
		{
			// elements[e.data1.AsInt()]->ToUiButton()->color = { 255, 0, 0, 255 };
			// elements[e.data1.AsInt()]->ToUiTextButton()->color = { 255, 0, 0, 255 };
			elements[e.data1.AsInt()]->ToUiButtonImage()->texture_id = tex_in;

			break;
		}
		case HOVER_OUT:
		{
			// elements[e.data1.AsInt()]->ToUiButton()->color = { 0, 0, 0, 255 };
			// elements[e.data1.AsInt()]->ToUiTextButton()->color = { 0, 0, 0, 255 };
			elements[e.data1.AsInt()]->ToUiButtonImage()->texture_id = tex_out;

			break;
		}
		case MOUSE_DOWN:
		{
			// elements[e.data1.AsInt()]->ToUiButton()->color = { 0, 255, 0, 255 };
			// elements[e.data1.AsInt()]->ToUiTextButton()->color = { 0, 255, 0, 255 };
			elements[e.data1.AsInt()]->ToUiButtonImage()->texture_id = tex_down;

			break;
		}
		case MOUSE_REPEAT:
		{
			// elements[e.data1.AsInt()]->ToUiButton()->color = { 0, 255, 255, 255 };
			// elements[e.data1.AsInt()]->ToUiTextButton()->color = { 0, 255, 255, 255 };
			elements[e.data1.AsInt()]->ToUiButtonImage()->texture_id = tex_repeat;

			break;
		}
		case MOUSE_UP:
		{
			// elements[e.data1.AsInt()]->ToUiButton()->color = { 255, 255, 0, 255 };
			// elements[e.data1.AsInt()]->ToUiTextButton()->color = { 255, 255, 0, 255 };
			elements[e.data1.AsInt()]->ToUiButtonImage()->texture_id = tex_up;

			break;
		}
		}

		break;
	}
	}
}

void ConfigWindow::_Update()
{
	color.a = (state.mouse_inside ? 255 : 220);
}