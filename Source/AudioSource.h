#ifndef __AUDIO_SOURCE_H__
#define __AUDIO_SOURCE_H__

#include "Component.h"
#include "Audio.h"

class AudioSource : public Component
{
public:

	AudioSource(Gameobject* go);
	~AudioSource();

	bool Play(Audio_FX fx, int loops = 0);
	void Halt();

	void RecieveEvent(const Event& e) override;
};

#endif // !__AUDIO_SOURCE_H__
