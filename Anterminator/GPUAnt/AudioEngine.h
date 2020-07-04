#pragma once
#include <SFML/Audio.hpp>
#include <array>
#include <string>
#include "Camera.h"
#include "WaterEngineGPU.h"	
#include <iostream>
class AudioType {
private:
public:
	sf::SoundBuffer buffer;
	AudioType() {
	}
	AudioType(std::string fileloc) {
		if(!buffer.loadFromFile(fileloc))
		{
			std::cout << "Load failed\n";
		}
	}
};
struct AudioSource {
	sf::Sound snd = sf::Sound();
	bool Free = true;
};
class AudioEngine
{
	const std::string AudioFolder = "./Audio/";
public:
	float Loudness = 1;
	static constexpr int MaxAudioTypes = 10;
	std::array<AudioType, MaxAudioTypes> AudioTypeArray;
	static constexpr int MaxAudioProducers = 200;
	std::array<AudioSource, MaxAudioProducers> SourceList;
	float MusicVolume = 0;
	sf::Music CurrentMusicTrack;
	int MusicOffCounter = 0;
	int Track = 0;
	sf::Listener MainListener;
	std::vector<std::string> MusicOptions = {
	};
	AudioEngine();
	void LoadFiles();
	void UpdateGame(WaterEngineGPU& entityengine, Camera& cam);
	void PlaySound(int type, float loudness, float positionx, float positiony);
	int GetFreeSource();
};

