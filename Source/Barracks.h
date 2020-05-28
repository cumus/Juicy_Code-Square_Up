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

	void create_bar() override;
	void CreatePanel() override;
	void FreeWalkabilityTiles() override;
	

protected:

	int buildQueue;
	float spawnPointX;
	float spawnPointY;

	float posY_panel;
	int panel_tex_ID;
	C_Image* panel;
	C_Image* barrack_icon;
	C_Button* meleeUnit_btn;
	C_Button* rangedUnit_btn;
	C_Button* superUnit_btn;
	C_Button* upgrade_btn;
};

#endif // __BARRACKS_H__