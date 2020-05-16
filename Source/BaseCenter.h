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
	void Update() override;
	void create_bar() override;
	void update_health_ui() override;
	void update_upgrades_ui();
	void CreatePanel() override;
	void UpdatePanel() override;
	void create_creation_bar() override;
	void update_creation_bar() override;
	void UpdateWalkabilityTiles() override;
	void FreeWalkabilityTiles() override;

public:

	static Gameobject* baseCenter;

protected:

	int lvl;
	int max_lvl;

	float posY_panel;
	int panel_tex_ID;
	C_Image* panel;
	C_Image* base_icon;
	C_Button* gatherer_btn;
	C_Button* meleeUnit_btn;
	C_Button* rangedUnit_btn;
	C_Button* superUnit_btn;
	C_Button* upgrade_btn;

	C_Text* gatherer_buildNum;
	//C_Text* melee_buildNum;
	//C_Text* ranged_buildNum;
	//C_Text* super_buildNum;
};

#endif // __BASECENTER_H__