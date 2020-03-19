#ifndef __AUDIO_H__
#define __AUDIO_H__

#include <vector>
#include "Module.h"
#include "Point.h"

#define DEFAULT_MUSIC_FADE_TIME 2.0f

struct _Mix_Music;
struct Mix_Chunk;

class Audio : public Module
{
public:

	Audio();
	~Audio();

	void LoadConfig(bool empty_config) override;
	void SaveConfig() const override;

	bool Init();
	bool CleanUp();

	// Play a music file
	bool PlayMusic(const char* path, float fade_time = DEFAULT_MUSIC_FADE_TIME);

	// Load a WAV in memory
	unsigned int LoadFx(const char* path);

	// unload a wav from memory
	bool UnloadFx(unsigned int id);

	// Play a previously loaded WAV
	bool PlayFx(unsigned int fx, int repeat = 0);

private:

	_Mix_Music*				music = nullptr;
	std::vector<Mix_Chunk*>	fx;

	// Mixer Codecs
	bool using_FLAC = false;
	bool using_MOD = false;
	bool using_MP3 = false;
	bool using_OGG = true;
	bool using_MID = false;
	bool using_OPUS = false;
};

#endif // __AUDIO_H__