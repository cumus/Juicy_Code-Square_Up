#include "HierarchyWindow.h"
#include "Application.h"
#include "Scene.h"
#include "Editor.h"
#include "UI_TextButton.h"

#include <stack>

HierarchyWindow::HierarchyWindow(const RectF rect) : EditorWindow(rect)
{
}

HierarchyWindow::~HierarchyWindow()
{
}

bool HierarchyWindow::Init()
{
	for (float i = 0.0f; i < 20.0f; ++i)
	{
		elements.push_back(new UI_TextButton(this, {0.0f, 0.05f * i, 0.5f, 0.05f}, " "));
	}

	root = App->scene->GetRoot();

	return !elements.empty();
}

void HierarchyWindow::RecieveEvent(const Event& e)
{
	int id = e.data1.AsInt();
	if (id >= 0 && id < gos.size() && e.type == MOUSE_UP)
	{
		Gameobject* go = gos[id].second;
		App->editor->selection = go;
		Event::Push(ON_SELECT, go);
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

	// Add 
	while (!stack.empty())
	{
		std::pair<float, Gameobject*> go = stack.top();
		stack.pop();

		gos.push_back(go);

		std::vector<Gameobject*> childs = go.second->GetChilds();
		for (std::vector<Gameobject*>::reverse_iterator it = childs.rbegin(); it != childs.rend(); ++it)
			stack.push({ go.first + 1.0f, *it });
	}

	for (int i = 0; i < 20; ++i)
	{
		if (i < gos.size())
		{
			elements[i]->rect = { gos[i].first * 0.05f, 0.05f * float(i), 0.5f, 0.05f };
			elements[i]->ToUiTextButton()->text = gos[i].second->GetName();
		}
		else
		{
			elements[i]->rect = { 0.0f, 0.05f, 0.5f, 0.05f };
			elements[i]->ToUiTextButton()->text = " ";
		}
	}
}
