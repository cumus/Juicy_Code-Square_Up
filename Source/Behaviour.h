#ifndef __BEHAVIOUR_H__
#define __BEHAVIOUR_H__

#include "Component.h"
#include "Point.h"
#include "Canvas.h"
#include "Scene.h"
#include "Audio.h"

#include <vector>
#include <map>

#define RAYCAST_TIME 0.1

enum UnitType
{
	GATHERER,
	UNIT_MELEE,
	UNIT_RANGED,
	ENEMY_MELEE,
	ENEMY_RANGED,

	//Structures//
	BASE_CENTER,
	TOWER,
	EDGE,
};

enum UnitState
{
	IDLE,
	MOVING_N,
	MOVING_S,
	MOVING_W,
	MOVING_E,
	MOVING_NE,
	MOVING_NW,
	MOVING_SE,
	MOVING_SW,
	ATTACKING_N,
	ATTACKING_S,
	ATTACKING_W,
	ATTACKING_E,
	ATTACKING_NE,
	ATTACKING_NW,
	ATTACKING_SE,
	ATTACKING_SW,

	// Building
	BUILDING,
	FULL_LIFE,
	HALF_LIFE,
	DESTROYED,
};


class Sprite;
class AudioSource;

class Behaviour : public Component
{
public:
	Behaviour(Gameobject* go, UnitType type, UnitState starting_state, ComponentType comp_type = BEHAVIOUR);
	virtual ~Behaviour();

	void RecieveEvent(const Event& e) override;

	void Selected();
	void UnSelected();
	void OnDamage(int damage);
	void OnKill();
	void BuildGatherer(float x, float y);
	void BuildMelee(float x, float y);
	void BuildRanged(float x, float y);
	void BuildSuper(float x, float y);
	void BuildTower(float x, float y);
	void BuildCenter(float x, float y);
	void BuildWall(float x, float y);
	void BuildLab(float x, float y);
	void BuildBarrack(float x, float y);
	virtual void AfterDamageAction() {}
	virtual void OnRightClick(float x, float y) {}
	virtual void DoAttack() {}
	virtual void OnDestroy(){}
	virtual void OnGetImpulse(float x,float y) {}
	virtual void create_bar() {}
	virtual void update_health_ui() {}
	virtual void CreatePanel() {}
	virtual void UpdatePanel() {}

	UnitType GetType() const { return type; }
	UnitState* GetStatePtr() { return &current_state; }
	UnitState GetState() { return current_state; }

	//void QuickSort();
	unsigned int GetBehavioursInRange(vec pos, float dist, std::map<float, Behaviour*>& res) const;

public: 
	
	static std::map<double, Behaviour*> b_map;
	UnitState current_state;

protected:	

	UnitType type;
	
	// Stats
	int max_life, current_life, damage;
	float attack_range, vision_range,dieDelay;
	float rayCastTimer;
	bool shoot;
	Audio_FX deathFX;

	// Complementary components
	AudioSource* audio;
	Sprite* selection_highlight;

	float pos_y_HUD;
	int bar_text_id;
	Gameobject* selectionPanel;
	Gameobject* bar_go;
	C_Button* bar;
	C_Image* portrait;
	C_Text* text;
	C_Image* healthbar;
	C_Image* health;
};

class B_Unit : public Behaviour
{
public:

	B_Unit(Gameobject* go, UnitType type, UnitState starting_state, ComponentType comp_type = B_UNIT);

	void Update() override;
	void OnRightClick(float x, float y) override;
	void DoAttack() override;
	void OnDestroy() override;
	void OnGetImpulse(float x, float y) override;
	void create_bar() override;
	void update_health_ui() override;

protected:
	float speed;
	int damage;
	float atkDelay;
	Audio_FX attackFX;
	float msCount;
	bool inRange;
	Behaviour* attackObjective;
	vec attackPos;
	std::vector<iPoint>* path;
	iPoint nextTile;
	bool next;
	bool move;
	bool positiveX;
	bool positiveY;
	int dirX;
	int dirY;
	bool cornerNW;
	bool cornerNE;
	bool cornerSW;
	bool cornerSE;

	std::pair<int, int> atkObj;
	std::pair<int, int> shootPos;
};

#endif // __BEHAVIOUR_H_