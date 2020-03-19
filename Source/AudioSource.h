#ifndef __AUDIO_SOURCE_H__
#define __AUDIO_SOURCE_H__

#include "Component.h"
#include "Audio.h"
#include <vector>

class Gameobject;

class AudioSource : public Component
{
public:

	AudioSource(Gameobject* go, int id = -1);
	~AudioSource();

	bool Play() const;

	void RecieveEvent(const Event& e) override;

public: 

	int fx_id;
};





#endif // !__AUDIO_SOURCE_H__
