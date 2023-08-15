#include <SFML/Audio.hpp>
#include <map>
#include "log.h"
#pragma once

class SFXLoader {
	std::map<std::string, std::string> soundsPaths = { {"walk", "assets/sounds/walk.wav"},{"jump", "assets/sounds/jump.wav"} };
	std::map<std::string, float> soundVolume = { {"walk", 40.0f}, {"jump", 90.0f} };
	std::map < std::string, std::pair<sf::SoundBuffer*, sf::Sound*> > soundObjects;

public:
	static SFXLoader* objectPtr;

	SFXLoader() {
		objectPtr = NULL;
	};

	void init() {
		for (auto i = soundsPaths.begin(); i != soundsPaths.end(); i++)
		{
			sf::SoundBuffer* buffer = new sf::SoundBuffer;
			sf::Sound* snd = new sf::Sound;
			log(INFO, "Loading SFX '" + i->first + "'...", "");
			if (!buffer->loadFromFile(i->second)) {
				log(ERROR, "Sound '" + i->second + "' not found!");
			}
			log(NONE, "[DONE] (" + std::to_string(buffer->getSampleCount()) + " samples)");
			snd->setBuffer(*buffer);
			snd->setVolume(soundVolume[i->first]);
			soundObjects[i->first] = std::pair<sf::SoundBuffer*, sf::Sound*>{ buffer, snd };
		}
		objectPtr = this;
	}

	sf::Sound* getSound(std::string key) {
		if (soundObjects.count(key) > 0) {
			return soundObjects[key].second;
		}
		log(ERROR, "Cannot load sound '" + key + "'");
		return nullptr;
	}

	~SFXLoader()
	{
		for (auto i = soundObjects.begin(); i != soundObjects.end(); i++)
		{
			delete soundObjects[i->first].first;
			delete soundObjects[i->first].second;
		}
	}
};

class MusicLoader {
	std::map<std::string, std::string> musicPaths = { {"calm", "assets/music/calm.ogg"}, 
		{"main", "assets/music/epic1.ogg"},
		{"battle1", "assets/music/battle1.ogg"}};
	std::map<std::string, float> musicVolume = { {"calm", 20.0f}, {"main", 80.0f}, {"battle1", 95.0f}};
	std::map < std::string, sf::Music* > musicObjects;

public:
	static MusicLoader* objectPtr;

	MusicLoader() {
		objectPtr = NULL;
	};

	void init() {
		for (auto i = musicPaths.begin(); i != musicPaths.end(); i++)
		{
			sf::Music* music = new sf::Music;

			log(INFO, "Loading music '" + i->first + "'...", "");
			if (!music->openFromFile(i->second)) {
				log(ERROR, "Music file '" + i->second + "' not found!");
			}
			log(NONE, "[DONE] (" + std::to_string(music->getDuration().asMilliseconds() / 1000) + " seconds)");
			music->setVolume(musicVolume[i->first]);
			music->setLoop(true);
			musicObjects[i->first] = music;
		}
		objectPtr = this;
	}

	sf::Music* getMusic(std::string key) {
		if (musicObjects.count(key) > 0) {
			return musicObjects[key];
		}
		log(ERROR, "Cannot load music '" + key + "'");
		return nullptr;
	}

	~MusicLoader()
	{
		for (auto i = musicObjects.begin(); i != musicObjects.end(); i++)
		{
			delete musicObjects[i->first];
		}
	}
};

SFXLoader* SFXLoader::objectPtr;
MusicLoader* MusicLoader::objectPtr;
