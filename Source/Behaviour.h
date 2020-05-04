#ifndef __BEHAVIOUR_H__
#define __BEHAVIOUR_H__

#include "Component.h"
#include "Point.h"
#include "Canvas.h"
#include "Scene.h"
#include "Audio.h"
#include "Collider.h"

#include <vector>
#include <list>
#include <map>

#define RAYCAST_TIME 0.1
#define CREATION_TIME 1.0

enum UnitType : int
{
	GATHERER,
	UNIT_MELEE,
	UNIT_RANGED,
	UNIT_SUPER,

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
	NO_UPGRADE,
	DESTROYED,
	FIRST_UPGRADE,
	SECOND_UPGRADE
};

enum IAState
{
	IDLE_IA,
	BASE_IA,
	CHASING_IA,
	ATTACKING_IA,
};


class Sprite;
class AnimatedSprite;
class AudioSource;


class Behaviour : public Component
{
public:
	Behaviour(Gameobject* go, UnitType type, UnitState starting_state, ComponentType comp_type = BEHAVIOUR);
	virtual ~Behaviour();

	void RecieveEvent(const Event& e) override;
	void PreUpdate() override;

	void Selected();
	void UnSelected();
	void OnDamage(int damage);
	void OnKill(const UnitType type);
	void ActivateSprites();
	void DesactivateSprites();
	void CheckFoWMap(bool debug=false);
	std::vector<iPoint> GetTilesInsideRadius();
	void ApplyMaskToTiles(std::vector<iPoint>tilesAffected);
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
	virtual void create_creation_bar() {}
	virtual void update_creation_bar() {}
	virtual void Upgrade() {}
	virtual void UpdateWalkabilityTiles() {}
	virtual void FreeWalkabilityTiles() {}
	virtual void OnCollisionEnter(Collider selfCol, Collider col) {}
	virtual void OnCollisionStay(Collider selfCol, Collider col) {}
	virtual void OnCollisionExit(Collider selfCol, Collider col) {}

	static bool IsHidden(double id) { return b_map[id]->visible; }

	UnitType GetType() const { return type; }
	UnitState* GetStatePtr() { return &current_state; }
	UnitState GetState() { return current_state; }

	//void QuickSort();
	unsigned int GetBehavioursInRange(vec pos, float dist, std::map<float, Behaviour*>& res) const;

protected:

	virtual void AddUnitToQueue(UnitType type, vec pos = vec(), float time = -1) {}

public: 
	
	static std::map<double, Behaviour*> b_map;
	static std::vector<double> enemiesInSight;
	UnitState current_state;
	vec pos;
	int max_life, current_life, damage;
	std::vector<iPoint> tilesVisited;
	AnimatedSprite* characteR = nullptr;
	bool providesVisibility,visible;

protected:	

	// Stats
	UnitType type;
	float attack_range, vision_range,dieDelay;
	float rayCastTimer;
	bool shoot,drawRanges;
	Audio_FX deathFX;
	std::pair<float, float> visionRange;
	std::pair<float, float> atkRange;
	Behaviour* attackObjective;
	std::vector<iPoint> lastFog;

	// Complementary components
	AudioSource* audio;
	Sprite* selection_highlight;

	// Mini Life Bars
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

	//Bulding Units Components
	float pos_y_HUD;
	int bar_text_id;
	Gameobject* selectionPanel = nullptr;
	Gameobject* btnTower = nullptr;
	Gameobject* btnBarrack = nullptr;
	Gameobject* btnBaseCenter = nullptr;
	Gameobject* bar_go = nullptr;
	Gameobject* creation_bar_go = nullptr;
	C_Image* bar;
	C_Image* portrait;
	C_Text* text;
	C_Image* healthbar;
	C_Image* red_health;
	C_Image* health;
	C_Image* health_boarder;
	C_Image* upgrades;
};

class BuildingWithQueue : public Behaviour
{
public:

	BuildingWithQueue(Gameobject* go, UnitType type, UnitState starting_state, ComponentType comp_type = BEHAVIOUR);

	void Update() override;
	void AddUnitToQueue(UnitType type, vec pos = vec(), float time = -1) override;

protected:

	struct QueuedUnit
	{
		QueuedUnit(UnitType type = MAX_UNIT_TYPES, Gameobject* go = nullptr, vec pos = vec(), float time = -1);
		QueuedUnit(const QueuedUnit& copy);
		float Update();

		UnitType type;
		vec pos;
		float time;
		float current_time;
		Transform* transform;
	};

	vec spawnPoint;
	Sprite* progress_bar;
	static SDL_Rect bar_section;
	std::list<QueuedUnit> build_queue;
};

class B_Unit : public Behaviour
{
public:

	B_Unit(Gameobject* go, UnitType type, UnitState starting_state, ComponentType comp_type = B_MELEE_UNIT);

	void Update() override;
	void OnRightClick(vec pos, vec modPos) override;
	void DoAttack() override;
	void OnDestroy() override;
	void OnGetImpulse(float x, float y) override;
	void CheckAtkRange();
	void CheckPathTiles();
	void CheckCollision();
	void ChangeState();
	void CheckDirection(fPoint actualPos);
	void ShootRaycast();
	void DrawRanges();
	virtual void UnitAttackType() {}
	virtual void IARangeCheck() {}

protected:
	float speed;
	int damage;
	float atkDelay;
	Audio_FX attackFX;
	float msCount;
	bool inRange;
	vec attackPos;
	std::vector<iPoint>* path;
	iPoint nextTile;
	bool next;
	bool move;
	bool gotTile;
	bool positiveX;
	bool positiveY;
	int dirX;
	int dirY;
	bool arriveDestination;
	std::pair<float, float> atkObj;
	std::pair<float, float> shootPos;
};

#endif // __BEHAVIOUR_H_