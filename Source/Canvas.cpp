#include "Canvas.h"
#include "Application.h"
#include "Input.h"
#include "Map.h"
#include "Render.h"
#include "FontManager.h"
#include "Gameobject.h"
#include "Transform.h"
#include "JuicyMath.h"

C_Canvas* UI_Component::canvas = nullptr;

UI_Component::UI_Component(Gameobject* go, UI_Component* parent, ComponentType type) :
	Component(type, go),
	parent(parent),
	offset({ 0.f, 0.f }),
	target({ 0.f, 0.f, 1.f, 1.f }),
	output({ 0, 0, 0, 0 })
{
	go->RemoveComponent(go->GetTransform());
}

UI_Component::~UI_Component()
{}

void UI_Component::ComputeOutputRect(float width, float height)
{
	if (canvas && parent)
	{
		std::pair<float, float> res_ratio = Render::GetResRatio();
		std::pair<float, float> canvas_scale = canvas->GetScale();

		std::pair<float, float> total_scale = {
			target.w * res_ratio.first * canvas_scale.first,
			target.h * res_ratio.second * canvas_scale.second };

		SDL_Rect p_output = parent->output;
		output.x = p_output.x + int(float(p_output.w) * target.x + (offset.first * total_scale.first));
		output.y = p_output.y + int(float(p_output.h) * target.y + (offset.second * total_scale.second));
		output.w = int(width * total_scale.first);
		output.h = int(height * total_scale.second);
	}
	else
	{
		output = { 0, 0, 1, 1 };
	}
}

bool UI_Component::PointInsideOutputRect(int x, int y) const
{
	return JMath::PointInsideRect(x, y, output);
}

C_Canvas::C_Canvas(Gameobject* go) : UI_Component(go, go->GetUIParent(), UI_CANVAS)
{
	canvas = this;
}

C_Canvas::~C_Canvas()
{
	canvas = nullptr;
}

void C_Canvas::PreUpdate()
{
	hovered_childs = 0u;

	int x, y;
	App->input->GetMousePosition(x, y);
	const std::vector<Gameobject*> childs = game_object->GetChilds();
	for (std::vector<Gameobject*>::const_iterator it = childs.cbegin(); it != childs.cend(); ++it)
	{
		const UI_Component* comp = (*it)->GetUI();
		if (comp && comp->PointInsideOutputRect(x, y))
			hovered_childs++;
	}
}

void C_Canvas::PostUpdate()
{
	output = App->render->GetCameraRect();
	if (!playing)
	{
		output.x = int(target.x * float(output.w));
		output.y = int(target.y * float(output.h));
		output.w = int(target.w * float(output.w));
		output.h = int(target.h * float(output.h));

		App->render->DrawQuad(output, { 255, 255, 255, 40 }, true, HUD, false);
	}
	else
	{
		output.x = output.y = 0;
	}
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

bool C_Canvas::MouseOnUI()
{
	return canvas && canvas->hovered_childs > 0;
}

bool C_Canvas::IsPlaying()
{
	return canvas && canvas->playing;
}

Gameobject* C_Canvas::GameObject()
{
	return (canvas && canvas->game_object) ? canvas->game_object : nullptr;
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
	ComputeOutputRect(float(section.w), float(section.h));
	App->render->Blit_Scale(tex_id, output.x, output.y, float(output.w) / float(section.w), float(output.h) / float(section.h), &section, HUD, false);
}

C_Text::C_Text(Gameobject* go, const char* t, int font_id) :
	UI_Component(go, go->GetUIParent(), UI_TEXT)
{
	text = new RenderedText(t, font_id);
}

C_Text::~C_Text()
{
	DEL(text);
}

void C_Text::PostUpdate()
{
	if (text)
	{
		int width, height;
		text->GetSize(width, height);
		ComputeOutputRect(float(width), float(height));

		if (scale_to_fit)
			App->render->Blit_Text(text, output.x, output.y, HUD, false);
		else
			App->render->Blit_TextSized(text, output, HUD, false);
	}
}

C_Button::C_Button(Gameobject* go, const Event& e) :
	UI_Component(go, go->GetUIParent(), UI_BUTTON),
	event_triggered(e)
{}

C_Button::~C_Button()
{
}

void C_Button::PostUpdate()
{
	ComputeOutputRect(float(section.w), float(section.h));

	int x, y;
	App->input->GetMousePosition(x, y);
	if ((mouse_inside = PointInsideOutputRect(x, y)) && event_triggered.listener)
	{
		KeyState mouse_click = App->input->GetMouseButtonDown(0);
		if (mouse_click == KEY_DOWN || (trigger_while_pressed && mouse_click == KEY_REPEAT))
		{
			if (C_Canvas::IsPlaying())
				Event::Push(event_triggered);
		}
	}

	if (tex_id >= 0)
		App->render->Blit_Scale(tex_id, output.x, output.y, float(output.w) / float(section.w), float(output.h) / float(section.h), &section, HUD, false);
	else
		App->render->DrawQuad(output, { 0, 0, 0, 255 }, true, HUD, false);
}
