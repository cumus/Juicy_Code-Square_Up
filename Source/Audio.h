#ifndef __AUDIO_H__
#define __AUDIO_H__

#include <vector>
#include "Module.h"
#include "Point.h"

#define DEFAULT_MUSIC_FADE_TIME 2.0f
#define MAX_DISTANCE 255
#define MAX_FX 100
#define RAD_TO_DEG 57.32f
#define SCALE 500

struct _Mix_Music;
struct Mix_Chunk;

enum FxPack
{
	SELECT = 0,
	NONE
};

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
	bool PauseMusic();

	// Load a WAV in memory
	unsigned int LoadFx(const char* path);

	// unload a wav from memory
	bool UnloadFx(unsigned int id);

	// Play a previously loaded WAV
	bool PlayFx(int channel, unsigned int fx, int repeat = 0);

	// Simulates 3D audio
	int PlaySpatialFx(unsigned int id, std::pair<int, int> position, int repeat);

	// Gets angleee of y axis in regards of cam position
	unsigned int GetAngle(iPoint cam_pos, iPoint source_pos);

	// distance between cam and audio source
	unsigned int GetDistance(iPoint cam_pos, iPoint source_pos);
private:

	_Mix_Music*					music = nullptr;
	std::vector<Mix_Chunk*>		fx;
	
	int				channels;

	// Mixer Codecs
	bool using_FLAC	= false;
	bool using_MOD	= false;
	bool using_MP3	= false;
	bool using_OGG	= true;
	bool using_MID	= false;
	bool using_OPUS = false;

public:

	int scale	= 0;
	int volume	= 0;

};

#endif // __AUDIO_H__