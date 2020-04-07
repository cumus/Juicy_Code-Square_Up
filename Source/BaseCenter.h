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
	void OnDamage(int damage) override;
	void OnKill() override;

	void create_unit_bar() override;
	void update_health_ui();
	void update_upgrades_ui();

protected:

	Gameobject* main_base_bar_go;
	C_Button* main_base_bar;
	C_Image* main_base_portrait;
	C_Text* main_base_text;
	C_Image* main_base_healthbar;
	C_Image* main_base_health;

	float pos_y;
	int base_bar_text_id;

	int bc_lvl = 1;
	int bc_max_lvl = 5;
	
};

#endif // __BASECENTER_H__