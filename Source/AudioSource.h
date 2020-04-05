#ifndef __AUDIO_SOURCE_H__
#define __AUDIO_SOURCE_H__

#include "Component.h"
#include "Audio.h"

class AudioSource : public Component
{
public:

	AudioSource(Gameobject* go, Audio_FX fx, int loops = 0);
	~AudioSource();

	bool Play();
	void RecieveEvent(const Event& e) override;

public:

	Audio_FX fx;
	int loops;
};

#endif // !__AUDIO_SOURCE_H__
