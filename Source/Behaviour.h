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

enum UnitType : int
{
	GATHERER,
	UNIT_MELEE,
	UNIT_RANGED,
	UNIT_SUPER,
	UNIT_SPECIAL,

	ENEMY_MELEE,
	ENEMY_RANGED,
	ENEMY_SUPER,
	ENEMY_SPECIAL,

	//Structures//
	BASE_CENTER,
	TOWER,
	WALL,
	BARRACKS,
	LAB,
	EDGE,
	SPAWNER,

	MAX_UNIT_TYPES
};

enum UnitState : int
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
	FIRST_UPGRADE,
	SECOND_UPGRADE,
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
	void OnKill(const UnitType type);
	void BuildGatherer(float x, float y);
	void BuildMelee(float x, float y);
	void BuildRanged(float x, float y);
	void BuildSuper(float x, float y);
	void BuildTower(float x, float y);
	void BuildCenter(float x, float y);
	void BuildWall(float x, float y);
	void BuildLab(float x, float y);
	void BuildBarrack(float x, float y);
	virtual void UpdatePath(int x,int y) {}
	virtual void AfterDamageAction() {}
	virtual void OnRightClick(vec pos, vec modPos) {}
	virtual void DoAttack() {}
	virtual void OnDestroy(){}
	virtual void OnGetImpulse(float x,float y) {}
	virtual void create_bar() {}
	virtual void update_health_ui() {}
	virtual void CreatePanel() {}
	virtual void UpdatePanel() {}
	virtual void Upgrade() {}


	UnitType GetType() const { return type; }
	UnitState* GetStatePtr() { return &current_state; }
	UnitState GetState() { return current_state; }

	//void QuickSort();
	unsigned int GetBehavioursInRange(vec pos, float dist, std::map<float, Behaviour*>& res) const;

public: 
	
	static std::map<double, Behaviour*> b_map;
	UnitState current_state;
	int max_life, current_life, damage;

protected:	

	UnitType type;
	
	// Stats
	
	float attack_range, vision_range,dieDelay;
	float rayCastTimer;
	bool shoot;
	Audio_FX deathFX;

	// Complementary components
	AudioSource* audio;
	Sprite* selection_highlight;

	struct Lifebar
	{
		void Create(Gameobject* parent);
		void Show();
		void Hide();
		void Update(float life);

		Gameobject* go;
		Sprite* green_bar;
		vec offset;
		SDL_Rect starting_section;
	} mini_life_bar;

	float pos_y_HUD;
	int bar_text_id;
	Gameobject* selectionPanel = nullptr;
	Gameobject* bar_go = nullptr;
	C_Image* bar;
	C_Image* portrait;
	C_Text* text;
	C_Image* healthbar;
	C_Image* red_health;
	C_Image* health;
	C_Image* health_boarder;
	C_Image* upgrades;
};

class B_Unit : public Behaviour
{
public:

	B_Unit(Gameobject* go, UnitType type, UnitState starting_state, ComponentType comp_type = B_UNIT);

	void Update() override;
	void OnRightClick(vec pos, vec modPos) override;
	void DoAttack() override;
	void OnDestroy() override;
	void OnGetImpulse(float x, float y) override;
	virtual void UnitAttackType() {}
	virtual void IARangeCheck() {}

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
	bool arriveDestination;
	std::pair<float, float> atkObj;
	std::pair<float, float> shootPos;
	std::vector<iPoint> tilesVisited;
};

#endif // __BEHAVIOUR_H_