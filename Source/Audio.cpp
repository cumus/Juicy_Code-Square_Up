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
	fx_volume = music_volume = 1.0f;
	fade_duration = 1.0f;

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
	if (state == PAUSING)
	{
		if ((fade_timer -= App->time.GetDeltaTime()) <= 0.0f)
		{
			fade_timer = 0.0f;
			state = PAUSED;

			for (std::map<double, SpatialData>::iterator it = sources.begin(); it != sources.end(); ++it)
				it->second.Pause();
		}

		SetFadeVolume(fade_timer / fade_duration);
	}
	else if (state == RESUMING)
	{
		if ((fade_timer += App->time.GetDeltaTime()) >= fade_duration)
		{
			fade_timer = fade_duration;
			state = NO_FADE;

			for (std::map<double, SpatialData>::iterator it = sources.begin(); it != sources.end(); ++it)
				it->second.RePlay();
		}

		SetFadeVolume(fade_timer / fade_duration);
	}

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
	case PLAY_FX:
	{
		PlayFx(Audio_FX(e.data1.AsInt()), e.data2.AsInt());
		break;
	}
	case HALT_FX:
	{
		std::map<double, SpatialData>::iterator it = sources.find(e.data1.AsDouble());
		if (it != sources.end())
			it->second.Halt();

		break;
	}
	case TRANSFORM_MODIFIED:
	{
		std::map<double, SpatialData>::iterator it = sources.find(e.data1.AsDouble());
		if (it != sources.end())
		{
			const vec global_pos = e.data2.AsVec();
			it->second.Update(App->render->GetCameraCenter(), { global_pos.x, global_pos.y }, fx_volume);
		}
		break;
	}
	case CAMERA_MOVED:
	{
		std::pair<float, float> cam = App->render->GetCameraCenter();
		for (std::map<double, SpatialData>::iterator it = sources.begin(); it != sources.end(); ++it)
			it->second.Update(cam, it->second.pos, fx_volume);

		break;
	}
	case SCENE_PLAY:
	{
		if (state == PAUSING || state == PAUSED)
		{
			state = RESUMING;
			fade_timer = 0.f;
		}

		break;
	}
	case SCENE_PAUSE:
	{
		state = PAUSING;
		fade_timer = fade_duration;

		break;
	}
	case SCENE_STOP:
	{
		for (std::map<double, SpatialData>::iterator it = sources.begin(); it != sources.end(); ++it)
			it->second.Halt();

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

	music_is_playing = false;

	if(music)
	{
		StopMusic(fade_time);
		Mix_FreeMusic(music);
	}
;
	if(music = Mix_LoadMUS_RW(App->files.LoadRWops(path), 1))
	{
		if (fade_time > 0.0f)
		{
			if (music_is_playing = (Mix_FadeInMusic(music, -1, (int)(fade_time * 1000.0f)) >= 0))
				LOG("Successfully playing %s", path);
			else
				LOG("Cannot fade in music %s. Mix_GetError(): %s", path, Mix_GetError());
		}
		else if (Mix_PlayMusic(music, -1) < 0)
			LOG("Cannot play in music %s. Mix_GetError(): %s", path, Mix_GetError());
	}
	else
		LOG("Cannot load music %s. Mix_GetError(): %s\n", path, Mix_GetError());

	return music_is_playing;
}

void Audio::StopMusic(float fade_time)
{
	if (music)
	{
		if (fade_time > 0.0f)
			Mix_FadeOutMusic(int(fade_time * 1000.0f));
		else
			Mix_HaltMusic();
	}

	music_is_playing = false;
}

bool Audio::MusicIsPlaying()
{
	return music_is_playing;
}

bool Audio::LoadFx(Audio_FX audio_fx)
{
	OPTICK_EVENT();

	bool ret;
	std::string audio_path = App->files.GetBasePath();

	switch (audio_fx)
	{
	case LOGO: audio_path += "Assets/audio/Effects/Intro/soda-open-and-pour-left-right.wav"; break;///////////// USING
	//case HAMMER: audio_path += "Assets/audio/Effects/Buildings/Nails/HamerNail13Hits.wav"; break;
	case TITLE: audio_path += "Assets/audio/Effects/UI/Menu/Menu_Select.wav"; break;///////////// USING
	case SELECT: audio_path += "Assets/audio/Effects/UI/Menu/Flashpoint001a.wav"; break;///////////// USING
	case B_DESTROYED: audio_path += "Assets/audio/Effects/Attack_sounds/Laser_05.wav"; break;///////////// USING
	case B_BUILDED: audio_path += "Assets/audio/Effects/Buildings/Nails/HamerNail4Hits.wav"; break;///////////// USING
	case UNIT_DIES: audio_path += "Assets/audio/Effects/Buildings/Destroyed/PowerDown.wav"; break;///////////// USING
	case UNIT_MOVES: audio_path += "Assets/audio/Effects/Footsteps/footsteps.wav"; break;///////////// USING
	case EDGE_FX: audio_path += "Assets/audio/Effects/Buildings/Destroyed/minimize_006.ogg"; break;///////////// USING
	//case B_SELECT: audio_path += "Assets/audio/Effects/Buildings/Select/Select.wav"; break;
	case SPAWNER_FX: audio_path += "Assets/audio/Effects/Allies/Spawn/Spawn.wav"; break;///////////// USING
	//case UI_BTON_CLICK: audio_path += "Assets/audio/Effects/UI/Ingame/itempick1.wav"; break;
	//case ALARM: audio_path += "Assets/audio/Effects/Notification/Scifi_Alarm16.wav"; break;
	//case BASE_CENTER_FX: audio_path += "Assets/audio/Effects/Buildings/Scanner1.ogg"; break;
	case RANGED_ATK_FX: audio_path += "Assets/audio/Effects/Attack_sounds/Laser_09.wav"; break;///////////// USING
	case SUPER_ATK_FX: audio_path += "Assets/audio/Effects/Attack_sounds/Laser_05.wav"; break;///////////// USING
	case MELEE_ATK_FX: audio_path += "Assets/audio/Effects/Attack_sounds/Laser_05.wav"; break;///////////// USING
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
	bool ret = false;

	if (fx[audio_fx] || LoadFx(audio_fx))
		ret = Mix_PlayChannel(0, fx[audio_fx], repeat) == 0;

	return ret;
}

bool Audio::PlaySpatialFx(Audio_FX audio_fx, double id, const std::pair<float, float> position, int repeat, int ticks, int fade_ms)
{
	OPTICK_EVENT();

	bool ret = false;

	if (fx[audio_fx] || LoadFx(audio_fx))
	{
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

		sources[id].chunk = int(audio_fx);

		// Setup channel spatial properties and play
		std::pair<float, float> cam = App->render->GetCameraCenter();
		Mix_SetPosition(target_channel,
			sources[id].angle = JMath::HorizontalAxisAngle_F(cam, position, -90.0f),
			sources[id].distance = JMath::DistanceSquared(cam, position));

		// Play fx
		ret = (fade_ms > 0) ?
			(Mix_FadeInChannelTimed(target_channel, fx[audio_fx], repeat, fade_ms, ticks) != -1) :
			(Mix_PlayChannelTimed(target_channel, fx[audio_fx], repeat, ticks) != -1);
	}

	return ret;
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

void Audio::SetMusicVolume(float vol)
{
	SDL_assert(vol >= 0.0f && vol <= 1.0f);

	if (music_volume != vol)
	{
		music_volume = vol;
		Mix_VolumeMusic(int(music_volume * 128.0f));
	}
}

void Audio::SetFXVolume(float vol)
{
	SDL_assert(vol >= 0.0f && vol <= 1.0f);

	if (fx_volume != vol)
	{
		fx_volume = vol;
		int target_vol = int(fx_volume * 128.0f);
		Mix_Volume(0, target_vol);
		for (std::map<double, SpatialData>::iterator it = sources.begin(); it != sources.end(); ++it)
			Mix_Volume(it->second.channel, target_vol);
	}
}

inline void Audio::SetFadeVolume(float fade_percent)
{
	Mix_VolumeMusic(int(music_volume * ((fade_percent * 112.0f) + 16.0f)));

	int target_vol = int(fade_percent * fx_volume * 128.0f);
	for (std::map<double, SpatialData>::iterator it = sources.begin(); it != sources.end(); ++it)
		Mix_Volume(it->second.channel, target_vol);
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

inline void Audio::SpatialData::Update(const std::pair<float, float> cam, const std::pair<float, float> position, float fx_volume)
{
	Mix_SetPosition(channel,
		angle = JMath::HorizontalAxisAngle_F(cam, pos = position, -90.0f),
		distance = JMath::Distance(cam, position));
	
	Mix_Volume(channel, (inside_cam = App->render->InsideCam(position.first, position.second)) ? int(fx_volume * 128.0f) : 0);
}

inline void Audio::SpatialData::Pause()
{
	if (!halted && !paused)
	{
		paused = true;
		Mix_Pause(channel);
	}
}

inline void Audio::SpatialData::RePlay()
{
	if (!halted && paused)
	{
		paused = false;
		Mix_Resume(channel);
	}
}

inline void Audio::SpatialData::Halt()
{
	if (!halted)
	{
		halted = true;
		Mix_HaltChannel(channel);
	}
}
