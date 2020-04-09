#ifndef __BASECENTER_H__
#define __BASECENTER_H__

#include "Behaviour.h"
#include "Canvas.h"

class Base_Center : public Behaviour
{
public:
	Base_Center(Gameobject* go);
	~Base_Center();

	void SpawnUnit(float x, float y);
	void OnRightClick(float x, float y) override;
	void Upgrade();
	void AfterDamageAction() override;

	void create_bar() override;
	void update_health_ui();
	void update_upgrades_ui();
	void CreatePanel() override;
	void UpdatePanel() override;

protected:

	int bc_lvl;
	int bc_max_lvl;
	int buildQueue;
	float spawnPointX;
	float spawnPointY;

	float posY_panel;
	int panel_tex_ID;
	C_Image* panel;
	C_Button* gatherer_btn;
	C_Button* meleeUnit_btn;
	C_Button* rangedUnit_btn;
	C_Button* superUnit_btn;
	//C_Image* gatherer_img;
	//C_Image* melee_img;
	//C_Image* ranged_img;
	//C_Image* super_img;
	C_Text* gatherer_buildNum;
	//C_Text* melee_buildNum;
	//C_Text* ranged_buildNum;
	//C_Text* super_buildNum;
	
};

#endif // __BASECENTER_H__