#ifndef __TOWER_H__
#define __TOWER_H__

#include "Behaviour.h"
#include "Canvas.h"

#define TOWER_UPGRADE_COST 70

class Tower : public Behaviour
{
public:
	Tower(Gameobject* go);
	~Tower();

	void Upgrade() override;
	void DoAttack() override;
	void Update() override;
	void create_bar() override;
	void update_health_ui();
	void update_upgrades_ui();
	void CreatePanel() override;
	void FreeWalkabilityTiles() override;
	void OnCollision(Collider selfCol, Collider col) override;

public:

	float posY_panel;
	int panel_tex_ID;
	
	C_Image* panel;
	C_Image* tower_icon;
	C_Button* upgrade_btn;

protected:

	int lvl = 1;
	int max_lvl = 5;
	int attack_speed = 1;
	std::pair<float, float> localPos;
	std::pair<float, float> atkPos;
	float atkDelay;
	Audio_FX attackFX;
	float ms_count;
};

#endif // __TOWER_H__