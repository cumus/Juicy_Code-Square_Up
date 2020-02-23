#ifndef __SCENE_H__
#define __SCENE_H__

#include "Module.h"

struct SDL_Texture;

class Scene : public Module
{
public:

	Scene();
	~Scene();

	bool Awake(pugi::xml_node& config) override;
	bool Start() override;
	bool PreUpdate() override;
	bool Update() override;
	bool PostUpdate() override;
	bool CleanUp() override;

private:
	int levelNum;
	int count;

	float cameraSpeedx = 0;
	float cameraSpeedy = 0;
};

#endif // __j1SCENE_H__