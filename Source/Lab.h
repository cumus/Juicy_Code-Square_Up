#ifndef __LAB_H__
#define __LAB_H__

#include "Behaviour.h"
#include "Canvas.h"

class Lab : public Behaviour
{
public:
	Lab(Gameobject* go);
	~Lab();

	void Upgrade() override;
	void Update() override;
	void create_bar() override;
	void update_health_ui() override;
	void CreatePanel() override;
	void FreeWalkabilityTiles() override;


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

	C_Text* gatherer_buildNum;
	//C_Text* melee_buildNum;
	//C_Text* ranged_buildNum;
	//C_Text* super_buildNum;
};

#endif // __LAB_H__