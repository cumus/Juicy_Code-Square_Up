#include "HierarchyWindow.h"
#include "Application.h"
#include "Scene.h"
#include "Editor.h"
#include "UI_TextButton.h"

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
	if (id >= 0 && e.type == MOUSE_UP)
	{
		Gameobject* go = gos[id].second;
		App->editor->selection = go;
		Event::Push(ON_SELECT, go);
	}
}

void HierarchyWindow::_Update()
{
	gos.clear();
	root->RecursiveFillHierarchy(0, gos);

	int i = 0;
	for (std::vector<std::pair<float, Gameobject*>>::iterator it = gos.begin(); it != gos.end(); ++it)
	{
		if (i < 20)
		{
			elements[i]->rect = { it->first * 0.05f, 0.05f * float(i), 0.5f, 0.05f };
			elements[i]->ToUiTextButton()->text = it->second->GetName();
		}

		++i;
	}
}
