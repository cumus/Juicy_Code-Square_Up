#ifndef __GATHERER_H__
#define __GATHERER_H__

#include "Behaviour.h"
#include "Gameobject.h"
#include "Component.h"
#include "Canvas.h"

class Gameobject;

class Gatherer : public B_Unit
{
public:
	Gatherer(Gameobject* go);
	~Gatherer();

	void CreatePanel() override;

public:

	float posY_panel;
	int panel_tex_ID;

	C_Image* panel;
	C_Image* gatherer_icon;
	C_Button* barracks_btn;
	C_Button* tower_btn;
	C_Button* labBtn;
};

#endif