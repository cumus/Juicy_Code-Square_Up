#ifndef _UI_IMAGE_H_
#define _UI_IMAGE_H_
#include "UI_Elements.h"
#include "Point.h"
#include "SDL/include/SDL_rect.h"
#include "SDL2_image-2.0.5/include/SDL_image.h"
#include "Module.h"

class Module;

class UI_Image : public UI_Element
{
public:
	UI_Image();
	UI_Image(Module* callback);
	~UI_Image() {}

	void Init(iPoint position, SDL_Rect section);
	bool Update(float dt);
	bool CleanUp();
	bool Draw();

public:
	SDL_Rect section;
	int texture_id;
};

#endif // !_UI_IMAGE_H_

