#ifndef __SUPERUNIT_H__
#define __SUPERUNIT_H__

#include "Behaviour.h"
#include "Gameobject.h"
#include "Component.h"

class Gameobject;

class SuperUnit : public B_Unit
{
public:
	SuperUnit(Gameobject* go);
	~SuperUnit();
	void UnitAttackType() override;
	void CreatePanel() override;

public:
	float posY_panel;
	int panel_tex_ID;
	C_Image* panel;
	C_Image* super_icon;
};

#endif#