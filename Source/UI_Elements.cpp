#include "UI_Elements.h"
#include "EditorWindows.h"
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
