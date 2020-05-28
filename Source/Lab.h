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
	C_Image* lab_icon;
	C_Button* gatherer_btn;
	C_Button* meleeUnit_btn;
	C_Button* rangedUnit_btn;
	C_Button* superUnit_btn;
};

#endif // __LAB_H__