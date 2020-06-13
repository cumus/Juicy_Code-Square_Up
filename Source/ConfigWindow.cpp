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
	return true;
}

void ConfigWindow::RecieveEvent(const Event& e)
{
	/*int mouse_x, mouse_y;
	App->input->GetMousePosition(mouse_x, mouse_y);
	RectF camera = App->render->GetCameraRectF();
	switch (e.type)*/
}

void ConfigWindow::_Update()
{
	color.a = (state.mouse_inside ? 255 : 220);
}