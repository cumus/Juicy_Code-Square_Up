#ifndef __AUDIO_SOURCE_H__
#define __AUDIO_SOURCE_H__

#include "Component.h"
#include "Audio.h"
#include <vector>

class Gameobject;

class AudioSource : public Component
{
public:

	AudioSource(Gameobject* go, ComponentType type = AUDIO_SOURCE);
	~AudioSource();

public: 
	unsigned int test_sound = 0u;
};

class AS_Object : public AudioSource
{
public:
	AS_Object(Gameobject*go, ComponentType type=AS_OBJECT):AudioSource(go, type){}
	virtual ~AS_Object() {}

	void RecieveEvent(const Event& e)override;

public:
	vec pos;
};





#endif // !__AUDIO_SOURCE_H__
