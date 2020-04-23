#ifndef __BARRACKS_H__
#define __BARRACKS_H__

#include "Behaviour.h"
#include "Canvas.h"

class Barracks : public BuildingWithQueue
{
public:
	Barracks(Gameobject* go);
	~Barracks();

	void Upgrade() override;
	void AfterDamageAction() override;

	void create_bar() override;
	void update_health_ui();
	void update_upgrades_ui();
	void CreatePanel() override;
	void UpdatePanel() override;
	void UpdateWalkabilityTiles() override;
	

protected:

	int bc_lvl;
	int bc_max_lvl;
	int buildQueue;
	float spawnPointX;
	float spawnPointY;

	float posY_panel;
	int panel_tex_ID;
	C_Image* panel;
	C_Button* meleeUnit_btn;
	C_Button* rangedUnit_btn;
	C_Button* superUnit_btn;
	//C_Text* melee_buildNum;
	//C_Text* ranged_buildNum;
	//C_Text* super_buildNum;

};

#endif // __BARRACKS_H__