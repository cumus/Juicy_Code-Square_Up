#ifndef __RANGEDUNIT_H__
#define __RANGEDUNIT_H__

#include "Behaviour.h"
#include "Gameobject.h"
#include "Component.h"

class Gameobject;

class RangedUnit : public B_Unit
{
public:
	RangedUnit(Gameobject* go);
	~RangedUnit();
	void UnitAttackType() override;
	void CreatePanel() override;

public:
	float posY_panel;
	int panel_tex_ID;
	C_Image* panel;
	C_Image* ranged_icon;
};

#endif#