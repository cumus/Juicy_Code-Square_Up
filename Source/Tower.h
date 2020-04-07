#ifndef __TOWER_H__
#define __TOWER_H__

#include "Behaviour.h"
#include "Canvas.h"

class Tower : public Behaviour
{
public:
	Tower(Gameobject* go);
	~Tower();

	void OnRightClick(float x, float y) override;
	void Upgrade();
	void OnDamage(int damage) override;
	void OnKill() override;
	void virtual DoAttack(vec pos) override;

	void create_unit_bar() override;
	void update_health_ui();
	void update_upgrades_ui();

protected:

	int t_lvl = 1;
	int t_max_lvl = 5;
	int t_damage = 5;
	int attack_speed = 1;
};

#endif // __TOWER_H__