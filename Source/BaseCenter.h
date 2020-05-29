#ifndef __BASECENTER_H__
#define __BASECENTER_H__

#include "Behaviour.h"
#include "Canvas.h"

#define BASE_UPGRADE_COST 100

class Base_Center : public BuildingWithQueue
{
public:
	Base_Center(Gameobject* go);
	~Base_Center();

	void Upgrade() override;
	void Update() override;
	void create_bar() override;
	void update_health_ui() override;
	void CreatePanel() override;
	void FreeWalkabilityTiles() override;

public:

	static Gameobject* baseCenter;

protected:

	float posY_panel;
	int panel_tex_ID;
	C_Image* panel;
	C_Image* base_icon;
	C_Button* gatherer_btn;
	C_Button* capsule_button;
	C_Button* meleeUnit_btn;
	C_Button* rangedUnit_btn;
	C_Button* superUnit_btn;
	C_Button* upgrade_btn;
};

#endif // __BASECENTER_H__