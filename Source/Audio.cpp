#include "Audio.h"
#include "Defs.h"
#include "Log.h"
#include "Application.h"

#include "optick-1.3.0.0/include/optick.h"
#include "SDL/include/SDL.h"

#include "SDL2_mixer-2.0.4/include/SDL_mixer.h"
#ifdef PLATFORMx86
#pragma comment( lib, "SDL2_mixer-2.0.4/lib/x86/SDL2_mixer.lib" )
#elif PLATFORMx64
#pragma comment( lib, "SDL2_mixer-2.0.4/lib/x64/SDL2_mixer.lib" )
#endif


Audio::Audio() : Module("audio")
{}

// Destructor
Audio::~Audio()
{}

void Audio::LoadConfig(bool empty_config)
{
	if (empty_config)
	{
		pugi::xml_node codecs = FileManager::ConfigNode().append_child(name).append_child("codecs");
		codecs.append_attribute("FLAC").as_bool(using_FLAC);
		codecs.append_attribute("MOD").as_bool(using_MOD);
		codecs.append_attribute("MP3").as_bool(using_MP3);
		codecs.append_attribute("OGG").as_bool(using_OGG);
		codecs.append_attribute("MID").as_bool(using_MID);
		codecs.append_attribute("OPUS").as_bool(using_OPUS);
	}
	else
	{
		pugi::xml_node codecs = FileManager::ConfigNode().child(name).child("codecs");
		using_FLAC = codecs.attribute("FLAC").as_bool(using_FLAC);
		using_MOD = codecs.attribute("MOD").as_bool(using_MOD);
		using_MP3 = codecs.attribute("MP3").as_bool(using_MP3);
		using_OGG = codecs.attribute("OGG").as_bool(using_OGG);
		using_MID = codecs.attribute("MID").as_bool(using_MID);
		using_OPUS = codecs.attribute("OPUS").as_bool(using_OPUS);
	}
}

void Audio::SaveConfig() const
{
	pugi::xml_node codecs = FileManager::ConfigNode().child(name).child("codecs");
	codecs.attribute("FLAC").set_value(using_FLAC);
	codecs.attribute("MOD").set_value(using_MOD);
	codecs.attribute("MP3").set_value(using_MP3);
	codecs.attribute("OGG").set_value(using_OGG);
	codecs.attribute("MID").set_value(using_MID);
	codecs.attribute("OPUS").set_value(using_OPUS);
}

// Called before render is available
bool Audio::Init()
{
	OPTICK_EVENT();

	bool ret = (SDL_InitSubSystem(SDL_INIT_AUDIO) == 0);

	if (ret)
	{
		LOG("SDL_AUDIO initialized.");

		// Initialize SDL_mixer
		int flags = 0;
		if (using_FLAC) flags |= MIX_INIT_FLAC;
		if (using_MOD) flags |= MIX_INIT_MOD;
		if (using_MP3) flags |= MIX_INIT_MP3;
		if (using_OGG) flags |= MIX_INIT_OGG;
		if (using_MID) flags |= MIX_INIT_MID;
		if (using_OPUS) flags |= MIX_INIT_OPUS;

		int res_flags = Mix_Init(flags);

		if ((res_flags & flags) == flags)
		{
			// Initialize SDL_mixer with default frequecy & format
			if (Mix_OpenAudio(MIX_DEFAULT_FREQUENCY, MIX_DEFAULT_FORMAT, 2, 2048) == 0)
				LOG("SDL_Mixer opened correctly.");
			else
				LOG("SDL_mixer could not initialize! SDL_mixer Error: %s\n", Mix_GetError());
		}
		else
			LOG("Could not initialize Mixer lib. Mix_Init: %s", Mix_GetError());
	}
	else
		LOG("SDL_AUDIO could not initialize! SDL_Error: %s\n", SDL_GetError());

	return ret;
}

// Called before quitting
bool Audio::CleanUp()
{
	if(!active)
		return true;

	LOG("Freeing sound FX, closing Mixer and Audio subsystem");

	if(music) Mix_FreeMusic(music);

	for(std::vector<Mix_Chunk*>::iterator it = fx.begin(); it != fx.end(); ++it)
		Mix_FreeChunk(*it);

	fx.clear();

	Mix_CloseAudio();
	Mix_Quit();
	SDL_QuitSubSystem(SDL_INIT_AUDIO);

	return true;
}

// Play a music file
bool Audio::PlayMusic(const char* path, float fade_time)
{
	OPTICK_EVENT();

	bool ret = true;

	if(!active)
		return false;

	if(music)
	{
		if(fade_time > 0.0f)
			Mix_FadeOutMusic(int(fade_time * 1000.0f));
		else
			Mix_HaltMusic();

		// this call blocks until fade out is done
		Mix_FreeMusic(music);
	}

	music = Mix_LoadMUS_RW(App->files.LoadRWops(path), 1);

	if(music)
	{
		if (fade_time > 0.0f)
		{
			if (Mix_FadeInMusic(music, -1, (int)(fade_time * 1000.0f)) < 0)
			{
				LOG("Cannot fade in music %s. Mix_GetError(): %s", path, Mix_GetError());
				ret = false;
			}
		}
		else
		{
			if (Mix_PlayMusic(music, -1) < 0)
			{
				LOG("Cannot play in music %s. Mix_GetError(): %s", path, Mix_GetError());
				ret = false;
			}
		}
	}
	else
	{
		LOG("Cannot load music %s. Mix_GetError(): %s\n", path, Mix_GetError());
		ret = false;
	}

	LOG("Successfully playing %s", path);
	return ret;
}

// Load WAV
unsigned int Audio::LoadFx(const char* path)
{
	OPTICK_EVENT();

	unsigned int ret = 0;

	if(!active)
		return 0;

	std::string audio_path = App->files.GetBasePath();
	audio_path += path;
	Mix_Chunk* chunk = Mix_LoadWAV(audio_path.c_str());

	if(chunk)
	{
		fx.push_back(chunk);
		ret = fx.size();
	}
	else
	{
		LOG("Cannot load wav %s. Mix_GetError(): %s", audio_path.c_str(), Mix_GetError());
	}

	return ret;
}

// Play WAV
bool Audio::PlayFx(unsigned int id, int repeat)
{
	OPTICK_EVENT();

	bool ret = (active && id > 0 && id <= fx.size());

	if (ret) Mix_PlayChannel(-1, fx[id - 1], repeat);

	return ret;
}