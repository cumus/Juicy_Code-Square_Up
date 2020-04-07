#include "Audio.h"
#include "Render.h"
#include "Defs.h"
#include "Point.h"
#include "Log.h"
#include "Application.h"
#include "JuicyMath.h"

#include "optick-1.3.0.0/include/optick.h"
#include "SDL/include/SDL.h"

#include "SDL2_mixer-2.0.4/include/SDL_mixer.h"
#ifdef PLATFORMx86
#pragma comment( lib, "SDL2_mixer-2.0.4/lib/x86/SDL2_mixer.lib" )
#elif PLATFORMx64
#pragma comment( lib, "SDL2_mixer-2.0.4/lib/x64/SDL2_mixer.lib" )
#endif

Audio::Audio() : Module("audio")
{
	for (int i = 0; i < MAX_FX; ++i) fx[i] = nullptr;
}

Audio::~Audio()
{}

void Audio::LoadConfig(bool empty_config)
{
	pugi::xml_node config = FileManager::ConfigNode();

	if (empty_config)
	{
		pugi::xml_node codecs = config.append_child(name).append_child("codecs");
		codecs.append_attribute("FLAC").set_value(using_FLAC);
		codecs.append_attribute("MOD").set_value(using_MOD);
		codecs.append_attribute("MP3").set_value(using_MP3);
		codecs.append_attribute("OGG").set_value(using_OGG);
		codecs.append_attribute("MID").set_value(using_MID);
		codecs.append_attribute("OPUS").set_value(using_OPUS);
	}
	else
	{
		pugi::xml_node codecs = config.child(name).child("codecs");
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
	pugi::xml_node config = FileManager::ConfigNode();
	pugi::xml_node codecs = config.child(name).child("codecs");
	codecs.attribute("FLAC").set_value(using_FLAC);
	codecs.attribute("MOD").set_value(using_MOD);
	codecs.attribute("MP3").set_value(using_MP3);
	codecs.attribute("OGG").set_value(using_OGG);
	codecs.attribute("MID").set_value(using_MID);
	codecs.attribute("OPUS").set_value(using_OPUS);
}

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
			{
				total_channels = Mix_AllocateChannels(total_channels);
				LOG("SDL_Mixer opened correctly. Allocated %d channels.", total_channels);
			}
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

bool Audio::Update()
{
	/*

	TODO: Play/Pause Channels

	void  Mix_Pause(int channel);
	void  Mix_Resume(int channel);
	int Mix_Paused(int channel);


	
	TODO: Free channels

	if (sources.size() > Mix_Playing(-1) - Mix_Playing(0))
	{
		// Free sources map
		bool erased = false;
		for (std::map<double, SpatialData>::iterator it = sources.begin(); it != sources.end(); ++it)
		{
			if (erased)
			{
				erased = false;
				if (it != sources.begin())
					--it;
			}

			if (!Mix_Playing(it->second.channel))
			{
				Mix_UnregisterAllEffects(it->second.channel);
				sources.erase(it);
				erased = true;
			}
		}
	}
	*/

	return true;
}

bool Audio::CleanUp()
{
	LOG("Freeing sound FX, closing Mixer and Audio subsystem");

	UnloadFx();

	Mix_CloseAudio();
	if (music)
		Mix_FreeMusic(music);

	Mix_Quit();
	SDL_QuitSubSystem(SDL_INIT_AUDIO);

	return true;
}

void Audio::RecieveEvent(const Event& e)
{
	switch (e.type)
	{
	case TRANSFORM_MODIFIED:
	{
		std::map<double, SpatialData>::iterator it = sources.find(e.data1.AsDouble());
		if (it != sources.end())
		{
			std::pair<float, float> cam = App->render->GetCameraCenter();
			vec pos = e.data2.AsVec();

			Mix_SetPosition(it->second.channel,
				it->second.angle = JMath::HorizontalAxisAngle_F(cam, it->second.pos = { pos.x, pos.y }, -90.0f),
				it->second.distance = JMath::DistanceSquared(cam, it->second.pos));
		}
		break;
	}
	case CAMERA_MOVED:
	{
		std::pair<float, float> cam = App->render->GetCameraCenter();
		for (std::map<double, SpatialData>::iterator it = sources.begin(); it != sources.end(); ++it)
		{
			Mix_SetPosition(it->second.channel,
				it->second.angle = JMath::HorizontalAxisAngle_F(cam, it->second.pos, -90.0f),
				it->second.distance = JMath::DistanceSquared(cam, it->second.pos));
		}
		break;
	}
	default:
		break;
	}
}

// Play a music file
bool Audio::PlayMusic(const char* path, float fade_time)
{
	OPTICK_EVENT();

	bool ret = false;

	if(music)
	{
		if(fade_time > 0.0f)
			Mix_FadeOutMusic(int(fade_time * 1000.0f));
		else
			Mix_HaltMusic();

		// this call blocks until fade out is done
		Mix_FreeMusic(music);
	}
;
	if(music = Mix_LoadMUS_RW(App->files.LoadRWops(path), 1))
	{
		if (fade_time > 0.0f)
		{
			if (ret = (Mix_FadeInMusic(music, -1, (int)(fade_time * 1000.0f)) >= 0))
				LOG("Successfully playing %s", path);
			else
				LOG("Cannot fade in music %s. Mix_GetError(): %s", path, Mix_GetError());
		}
		else if (Mix_PlayMusic(music, -1) < 0)
			LOG("Cannot play in music %s. Mix_GetError(): %s", path, Mix_GetError());
	}
	else
		LOG("Cannot load music %s. Mix_GetError(): %s\n", path, Mix_GetError());

	return ret;
}

void Audio::PauseMusic(float fade_time) const
{
	if (music)
	{
		if (fade_time > 0.0f)
			Mix_FadeOutMusic(int(fade_time * 1000.0f));
		else
			Mix_HaltMusic();
	}
}

bool Audio::LoadFx(Audio_FX audio_fx)
{
	OPTICK_EVENT();

	bool ret;
	std::string audio_path = App->files.GetBasePath();

	switch (audio_fx)
	{
	case LOGO: audio_path += "audio/Effects/Intro/soda-open-and-pour-left-right.wav"; break;
	case HAMMER: audio_path += "audio/Effects/Buildings/Nails/HamerNail13Hits.wav"; break;
	case TITLE: audio_path += "audio/Effects/UI/Menu/Menu_Select.wav"; break;
	case SELECT: audio_path += "audio/Effects/UI/Menu/Flashpoint001a.wav"; break;
	case B_DESTROYED: audio_path += "audio/Effects/Buildings/Destroyed/PowerDown.wav"; break;
	case B_BUILDED: audio_path += "audio/Effects/Buildings/Nails/HamerNail4Hits.wav"; break;
	case UNIT_DIES: audio_path += "audio/Effects/Enemies/realistic_death_effect.wav"; break;
	default: break;
	}

	fx[audio_fx] = Mix_LoadWAV(audio_path.c_str());

	if (!(ret = (fx[audio_fx] != nullptr)))
		LOG("Cannot load %s. Mix_GetError(): %s", audio_path.c_str(), Mix_GetError());

	return ret;
}

void Audio::UnloadFx()
{
	for (unsigned int i = 0u; i < MAX_FX; ++i)
	{
		if (fx[i] != nullptr)
		{
			Mix_FreeChunk(fx[i]);
			fx[i] = nullptr;
		}
	}

	Mix_AllocateChannels(0);
	total_channels = Mix_AllocateChannels(1);
}

bool Audio::PlayFx(Audio_FX audio_fx, int repeat)
{
	if (!fx[audio_fx])
		LoadFx(audio_fx);

	return Mix_PlayChannel(0, fx[audio_fx], repeat) == 0;
}

bool Audio::PlaySpatialFx(Audio_FX audio_fx, double id, const std::pair<float, float> position, int repeat, int ticks, int fade_ms)
{
	OPTICK_EVENT();

	if (!fx[audio_fx])
		LoadFx(audio_fx);

	// Get channel to play. New sources will return -1 by default.
	int target_channel = sources[id].channel;
	sources[id].pos = position;
	if (target_channel < 0)
	{
		// Increase channel count on needing more channels
		if (total_channels - 1 < int(sources.size()))
			total_channels += Mix_AllocateChannels(total_channels + 1);

		// Search for empty channel
		for (target_channel = 1; target_channel < total_channels; ++target_channel)
		{
			if (!Mix_Playing(target_channel))
			{
				sources[id].channel = target_channel;
				break;
			}
		}
	}
	
	// Setup channel spatial properties and play
	std::pair<float, float> cam = App->render->GetCameraCenter();
	Mix_SetPosition(target_channel,
		sources[id].angle = JMath::HorizontalAxisAngle_F(cam, position, -90.0f),
		sources[id].distance = JMath::DistanceSquared(cam,  position));

	// Play fx
	return (fade_ms > 0) ?
		(Mix_FadeInChannelTimed(target_channel, fx[audio_fx], repeat, fade_ms, ticks) != -1) :
		(Mix_PlayChannelTimed(target_channel, fx[audio_fx], repeat, ticks) != -1);
}

bool Audio::StopFXChannel(double id, int ms, bool fade)
{
	bool ret = false;
	std::map<double, SpatialData>::iterator it = sources.find(id);
	if (it != sources.end())
	{
		if (fade)
			ret = (Mix_FadeOutChannel(it->second.channel, ms) != -1);
		else if (ms > 0)
			ret = (Mix_ExpireChannel(it->second.channel, ms) != -1);
		else
			ret = (Mix_HaltChannel(it->second.channel) != -1);
	}

	return ret;
}

Audio::SpatialData::SpatialData() :
	channel(-1),
	angle(-1.f),
	distance(-1.f),
	pos({ 0.f, 0.f })
{}

Audio::SpatialData::SpatialData(const SpatialData& copy) :
	channel(copy.channel),
	angle(copy.angle),
	distance(copy.distance),
	pos(copy.pos)
{}
