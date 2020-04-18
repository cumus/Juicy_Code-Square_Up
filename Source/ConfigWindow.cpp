#include "ConfigWindow.h"
#include "Application.h"
#include "TextureManager.h"
#include "UI_ButtonImage.h"
#include "UI_TextButton.h"
#include "UI_Slider.h"
#include "Input.h"
#include "Render.h"

ConfigWindow::ConfigWindow(const RectF rect) : EditorWindow(rect)
{}

ConfigWindow::~ConfigWindow()
{}

bool ConfigWindow::Init()
{
	// Test UI_Elements
	/*
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

	*/

	// Test UI_Slider

	int SliderBar_tex_id = App->tex.Load("Assets/textures/slider_bar.png");

	SliderBar_Rect = { 0.2f, 0.1f, 0.4f, 0.1f };

	SliderButton_Rect = { 1.00f, 0.98f, 0.15f, 1.4f };

	elements.push_back(new UI_Slider(this, SliderBar_Rect, SliderButton_Rect, SliderBar_tex_id));

	return !elements.empty();
	
}

void ConfigWindow::RecieveEvent(const Event& e)
{
	int mouse_x, mouse_y;

	RectF camera;

	App->input->GetMousePosition(mouse_x, mouse_y);

	camera = App->render->GetCameraRectF();

	float x = ((float)(mouse_x) / (camera.w)) / (rect.x + SliderBar_Rect.x * rect.w);

    SliderButton_Rect.x = x;

	switch (e.data1.AsInt())
	{
	case 0:
	{
		switch (e.type)
		{
		case HOVER_IN:
		{
			// Test UI_Elements
			
			// elements[e.data1.AsInt()]->ToUiButton()->color = { 255, 0, 0, 255 };
			// elements[e.data1.AsInt()]->ToUiTextButton()->color = { 255, 0, 0, 255 };
			// elements[e.data1.AsInt()]->ToUiButtonImage()->texture_id = tex_in;

			// Test UI_Slider

			elements[e.data1.AsInt()]->ToUiSlider()->button_color = { 255, 0, 0, 255 };
			elements[e.data1.AsInt()]->ToUiSlider()->Set_Value();

			break;
		}
		case HOVER_OUT:
		{
			// Test UI_Elements

			// elements[e.data1.AsInt()]->ToUiButton()->color = { 0, 0, 0, 255 };
			// elements[e.data1.AsInt()]->ToUiTextButton()->color = { 0, 0, 0, 255 };
			// elements[e.data1.AsInt()]->ToUiButtonImage()->texture_id = tex_out;

			// Test UI_Slider

			elements[e.data1.AsInt()]->ToUiSlider()->button_color = { 0, 0, 0, 255 };
			elements[e.data1.AsInt()]->ToUiSlider()->Set_Value();

			break;
		}
		case MOUSE_DOWN:
		{
			// Test UI_Elements

			// elements[e.data1.AsInt()]->ToUiButton()->color = { 0, 255, 0, 255 };
			// elements[e.data1.AsInt()]->ToUiTextButton()->color = { 0, 255, 0, 255 };
			// elements[e.data1.AsInt()]->ToUiButtonImage()->texture_id = tex_down;



			// Test UI_Slider

			elements[e.data1.AsInt()]->ToUiSlider()->button_rect = SliderButton_Rect;
			elements[e.data1.AsInt()]->ToUiSlider()->button_color = { 0, 255, 0, 255 };
			elements[e.data1.AsInt()]->ToUiSlider()->Set_Value();

			break;
		}
		case MOUSE_REPEAT:
		{
			// Test UI_Elements

			// elements[e.data1.AsInt()]->ToUiButton()->color = { 0, 255, 255, 255 };
			// elements[e.data1.AsInt()]->ToUiTextButton()->color = { 0, 255, 255, 255 };
			// elements[e.data1.AsInt()]->ToUiButtonImage()->texture_id = tex_repeat;

			// Test UI_Slider


			elements[e.data1.AsInt()]->ToUiSlider()->button_rect = SliderButton_Rect;
			elements[e.data1.AsInt()]->ToUiSlider()->button_color = { 0, 255, 255, 255 };
			elements[e.data1.AsInt()]->ToUiSlider()->Set_Value();

			break;
		}
		case MOUSE_UP:
		{


			// elements[e.data1.AsInt()]->ToUiButton()->color = { 255, 255, 0, 255 };
			// elements[e.data1.AsInt()]->ToUiTextButton()->color = { 255, 255, 0, 255 };
			// elements[e.data1.AsInt()]->ToUiButtonImage()->texture_id = tex_up;

			// Test UI_Slider

			elements[e.data1.AsInt()]->ToUiSlider()->button_color = { 255, 255, 0, 255 };
			elements[e.data1.AsInt()]->ToUiSlider()->Set_Button(75);
			

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