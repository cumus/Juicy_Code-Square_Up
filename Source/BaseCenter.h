#ifndef __BASECENTER_H__
#define __BASECENTER_H__

#include "Behaviour.h"
#include "Canvas.h"

class Base_Center : public BuildingWithQueue
{
public:
	Base_Center(Gameobject* go);
	~Base_Center();

	void Upgrade() override;
	void AfterDamageAction() override;

	void create_bar() override;
	void update_health_ui() override;
	void update_upgrades_ui();
	void CreatePanel() override;
	void UpdatePanel() override;
	void create_creation_bar() override;
	void update_creation_bar() override;
	void UpdateWalkabilityTiles() override;

public:

	static Gameobject* baseCenter;

protected:

	int bc_lvl;
	int bc_max_lvl;

	float posY_panel;
	int panel_tex_ID;
	C_Image* panel;
	C_Button* gatherer_btn;
	C_Button* meleeUnit_btn;
	C_Button* rangedUnit_btn;
	C_Button* superUnit_btn;
	C_Text* gatherer_buildNum;
	//C_Text* melee_buildNum;
	//C_Text* ranged_buildNum;
	//C_Text* super_buildNum;

	// Creation Bars

	C_Image* creation_bar;
	C_Image* gatherer_icon;
	C_Image* melee_icon;
	C_Image* ranged_icon;
	C_Image* gahterer_creation_bar;
	C_Image* gatherer_creation_bar_completed;
	C_Image* gatherer_creation_bar_boarder;
	C_Image* melee_creation_bar;
	C_Image* melee_creation_bar_completed;
	C_Image* melee_creation_bar_boarder;
	C_Image* ranged_creation_bar;
	C_Image* ranged_creation_bar_completed;
	C_Image* ranged_creation_bar_boarder;
};

#endif // __BASECENTER_H__