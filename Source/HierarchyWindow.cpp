#include "HierarchyWindow.h"
#include "Application.h"
#include "Scene.h"
#include "Editor.h"
#include "UI_TextButton.h"

#include <stack>

HierarchyWindow::HierarchyWindow(const RectF rect, int total_elements) : EditorWindow(rect), total_elements(total_elements)
{}

HierarchyWindow::~HierarchyWindow()
{}

bool HierarchyWindow::Init()
{
	for (float i = 0.0f; i < total_elements; ++i)
		elements.push_back(new UI_TextButton(this, {0.0f, 0.05f * i, 0.5f, 0.05f}, " "));

	root = App->scene->GetRoot();

	return !elements.empty();
}

void HierarchyWindow::RecieveEvent(const Event& e)
{
	if (e.type == MOUSE_UP)
	{
		int id = e.data1.AsInt();
		if (id >= 0 && id < int(gos.size()))
			App->editor->SetSelection(gos[id].second);
	}
}

void HierarchyWindow::_Update()
{
	gos.clear();
	std::stack<std::pair<float, Gameobject*>> stack;

	// Add root childs to stack
	std::vector<Gameobject*> childs = root->GetChilds();
	for (std::vector<Gameobject*>::reverse_iterator it = childs.rbegin(); it != childs.rend(); ++it)
		stack.push({ 0.0f, *it });

	// Fill gos vector from stack
	while (!stack.empty())
	{
		// Add stacked gameobject to vector
		std::pair<float, Gameobject*> go = stack.top();
		gos.push_back(go);
		stack.pop();

		// Add childs to stack in reverse order
		std::vector<Gameobject*> childs = go.second->GetChilds();
		for (std::vector<Gameobject*>::reverse_iterator it = childs.rbegin(); it != childs.rend(); ++it)
			stack.push({ go.first + 1.0f, *it });
	}

	// Set UI_Elements from gos vector
	for (unsigned int i = 0; i < total_elements; ++i)
	{
		if (i < gos.size())
		{
			elements[i]->rect = { gos[i].first * 0.05f, 0.05f * float(i), 0.5f, 0.05f };
			elements[i]->ToUiTextButton()->text->SetText(gos[i].second->GetName());
		}
		else
		{
			elements[i]->rect = { 0.0f, 0.05f, 0.5f, 0.05f };
			elements[i]->ToUiTextButton()->text->SetText(" ");
		}
	}
}
