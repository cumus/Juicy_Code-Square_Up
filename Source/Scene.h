#ifndef __SCENE_H__
#define __SCENE_H__

#include "Module.h"
#include "Map.h"
#include "Gameobject.h"

struct SDL_Texture;

class Scene : public Module
{
public:

	Scene();
	~Scene();

	bool PreUpdate() override;
	bool Update() override;
	bool PostUpdate() override;
	bool CleanUp() override;

	void RecieveEvent(const Event& e) override;

	bool LoadTestScene();

	Gameobject* GetRoot();
	const Gameobject* GetRoot() const;
	Gameobject* AddGameobject(const char* name, Gameobject* parent = nullptr);
	Gameobject* MouseClickSelect(int mouse_x, int mouse_y);

public:

	int id_mouse_tex;
	int image_text_id;

private:

	Map map;
	Gameobject root;

	float time = 0;
	bool x_t, y_t;
};

#endif // __SCENE_H__