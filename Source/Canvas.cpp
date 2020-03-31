#include "Canvas.h"
#include "Application.h"
#include "Map.h"
#include "Render.h"
#include "Gameobject.h"
#include "Transform.h"

C_Canvas* UI_Component::canvas = nullptr;

UI_Component::UI_Component(Gameobject* go, UI_Component* parent, ComponentType type) :
	Component(type, go),
	parent(parent),
	offset_x(0),
	offset_y(0),
	target({ 0.f, 0.f, 1.f, 1.f }),
	output({ 0, 0, 0, 0 })
{
	go->RemoveComponent(go->GetTransform());
}

UI_Component::~UI_Component()
{}

C_Canvas::C_Canvas(Gameobject* go) : UI_Component(go, go->GetUIParent(), UI_CANVAS)
{
	canvas = this;
}

C_Canvas::~C_Canvas()
{}

void C_Canvas::PostUpdate()
{
	output = App->render->GetCameraRect();

	if (!playing)
	{
		output.x = int(target.x * float(output.w));
		output.y = int(target.y * float(output.h));
		output.w = int(target.w * float(output.w));
		output.h = int(target.h * float(output.h));

	}
	else
	{
		output.x = output.y = 0;
	}

	App->render->DrawQuad(output, { 255, 255, 255, 40 }, true, HUD, false);
}

void C_Canvas::RecieveEvent(const Event& e)
{
	switch (e.type)
	{
	case ON_PLAY:
	{
		playing = true;
		break;
	}
	case ON_PAUSE:
	{
		playing = false;
		break;
	}
	case ON_STOP:
	{
		playing = false;
		break;
	}
	}
}

std::pair<float, float> C_Canvas::GetScale() const
{
	std::pair<float, float> ret[2] = { {target.w, target.h}, { 1.0f, 1.0f } };
	return ret[playing];
}

C_Image::C_Image(Gameobject* go) :
	UI_Component(go, go->GetUIParent(), UI_IMAGE)
{}

C_Image::~C_Image()
{}

void C_Image::PostUpdate()
{
	if (parent && canvas)
	{
		std::pair<float, float> res_ratio = Render::GetResRatio();
		std::pair<float, float> canvas_scale = canvas->GetScale();

		std::pair<float, float> total_scale = {
			target.w * res_ratio.first * canvas_scale.first,
			target.h * res_ratio.second * canvas_scale.second };

		SDL_Rect p_output = parent->output;
		output.x = p_output.x + int(float(p_output.w) * target.x) + int(offset_x * total_scale.first);
		output.y = p_output.y + int(float(p_output.h) * target.y) + int(offset_y * total_scale.second);
		output.w = int(float(section.w) * total_scale.first);
		output.h = int(float(section.h) * total_scale.second);

		App->render->Blit_Scale(tex_id, output.x, output.y, float(output.w) / float(section.w), float(output.h) / float(section.h), &section, HUD, false);
	}
}
