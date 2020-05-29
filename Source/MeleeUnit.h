#ifndef __MELEEUNIT_H__
#define __MELEEUNIT_H__

#include "Behaviour.h"
#include "Gameobject.h"
#include "Component.h"

class Gameobject;

class MeleeUnit : public B_Unit
{
public:
	MeleeUnit(Gameobject* go);
	~MeleeUnit();

	void CreatePanel() override;

public:
	float posY_panel;
	int panel_tex_ID;
	C_Image* panel;
	C_Image* melee_icon;
};

#endif

