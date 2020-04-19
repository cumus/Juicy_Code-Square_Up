#ifndef __BASECENTER_H__
#define __BASECENTER_H__

#include "Behaviour.h"
#include "Canvas.h"

class Base_Center : public Behaviour
{
public:
	Base_Center(Gameobject* go);
	~Base_Center();

	void Upgrade() override;
	void AfterDamageAction() override;

	void create_bar() override;
	void update_health_ui();
	void update_upgrades_ui();
	void CreatePanel() override;
	void UpdatePanel() override;

public:
	static Gameobject* baseCenter;
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
	C_Text* gatherer_buildNum;
	//C_Text* melee_buildNum;
	//C_Text* ranged_buildNum;
	//C_Text* super_buildNum;
	
};

#endif // __BASECENTER_H__