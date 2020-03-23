#include "UI_Elements.h"
#include "EditorWindow.h"
#include "Application.h"
#include "Render.h"

UI_Element::UI_Element(EditorWindow* window, UI_Type type, RectF rect)
	: window(window), type(type), rect(rect)
{}

UI_Element::~UI_Element()
{}

bool UI_Element::Draw() const
{
	return App->render->DrawQuadNormCoords(GetTargetNormRect(), { 0, 0, 0, 255 });
}

UI_Button* UI_Element::ToUiButton()
{
	return nullptr;
}

UI_Image* UI_Element::ToUiImage()
{
	return nullptr;
}

UI_Text* UI_Element::ToUiText()
{
	return nullptr;
}

UI_TextButton* UI_Element::ToUiTextButton()
{
	return nullptr;
}

UI_ButtonImage* UI_Element::ToUiButtonImage()
{
	return nullptr;
}

UI_SubMenu* UI_Element::ToUiSubMenu()
{
	return nullptr;
}



RectF UI_Element::GetTargetNormRect() const
{
	RectF ret;

	if (window != nullptr)
	{
		RectF parent = window->rect;
		
		ret = {
		parent.x + (rect.x * parent.w),
		parent.y + (rect.y * parent.h),
		parent.w - ((1.0f - rect.w) * parent.w),
		parent.h - ((1.0f - rect.h) * parent.h) };
	}

	return ret;
}

SDL_Rect UI_Element::GetTargetRect() const
{
	SDL_Rect ret;

	if (window != nullptr)
	{
		RectF parent = window->rect;
		RectF cam = App->render->GetCameraRectF();

		ret = {
		int((parent.x + (rect.x * parent.w)) * cam.w),
		int((parent.y + (rect.y * parent.h)) * cam.h),
		int((parent.w - ((1.0f - rect.w) * parent.w)) * cam.w),
		int((parent.h - ((1.0f - rect.h) * parent.h)) * cam.h) };
	}

	return ret;
}
