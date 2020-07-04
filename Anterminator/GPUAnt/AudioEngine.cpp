#include "AudioEngine.h"
#include <iostream>
#include <SFML/Audio.hpp>
#include <SFML/System.hpp>
#include <filesystem>
#include "Camera.h"
AudioEngine::AudioEngine() {
	std::cout << "Init audio engine\n";
//	CurrentMusicTrack.setLoop(false);
//	CurrentMusicTrack.setRelativeToListener(true);
//	CurrentMusicTrack.setPosition(0, 0, 0);
	std::filesystem::directory_iterator start(AudioFolder + "Music/");
	std::filesystem::directory_iterator end;
	std::transform(start, end, std::back_inserter(MusicOptions), [](const std::filesystem::directory_entry& entry) {return entry.path().filename().string(); });
	LoadFiles();
}
void AudioEngine::LoadFiles() {
	std::cout << "Loading audio files\n";
	std::vector<std::string> files = {
	"walk.wav",
	"born.wav",
	"die.wav",
	"bite.wav"
	};
	for (int i = 0; i < files.size(); ++i)
	{
		std::cout << "Loading file:" << files[i] << "\n";
		this->AudioTypeArray[i] = AudioType(AudioFolder + files[i]);
	}
	std::cout << "Finished\n";
	if (CurrentMusicTrack.openFromFile(AudioFolder + "Music/" + MusicOptions[Track])) {
		std::cout << "Music loaded fine\n";
		CurrentMusicTrack.setAttenuation(0);
		CurrentMusicTrack.setVolume(MusicVolume);
		CurrentMusicTrack.play();
		CurrentMusicTrack.stop();
	}
	else {
		std::cout << "Music load failed\n";
	}
}
void AudioEngine::UpdateGame(WaterEngineGPU& entityengine,Camera & cam)
{
	if (CurrentMusicTrack.getStatus() == sf::Music::Stopped)
	{
		if (MusicOffCounter-- == 0)
		{
			MusicOffCounter =  1000 + (rand() % 4000);
			Track = ++Track % MusicOptions.size();
			CurrentMusicTrack.openFromFile(AudioFolder + "Music/" + MusicOptions[Track]);
			CurrentMusicTrack.play();
		}
	}
	auto end = CurrentMusicTrack.getDuration().asSeconds();
	auto current = CurrentMusicTrack.getPlayingOffset().asSeconds();
	if (current < 10) {
		auto t = current / 10;
		CurrentMusicTrack.setVolume(MusicVolume * t);
	}
	else if (end - current < 10) {
		auto t = (end - current)/10;
		CurrentMusicTrack.setVolume(MusicVolume * (t));
	}
	else {
		CurrentMusicTrack.setVolume(MusicVolume);
	}

	MainListener.setPosition(cam.Position.x, cam.Position.y, cam.Position.z);
	MainListener.setDirection(sf::Vector3f(0, 0, -1));
	MainListener.setUpVector(sf::Vector3f(0, 0, 1));
	for (int i = 0; i < entityengine.MaxAudioEvents; ++i)
	{
		GPUAudioEvent& au = entityengine.GPUAudioEventList[i];
		if (au.Type != 0) {
			int Offset = au.Type - 1;
			PlaySound(Offset, au.Loudness, au.PositionX, au.PositionY);
			au = GPUAudioEvent();
		}
		else {
			break;
		}
	}
	for (int id = 0; id < MaxAudioProducers; ++id)
	{
		if (!SourceList[id].Free)
		{
			SourceList[id].Free = SourceList[id].snd.getStatus() == sf::Sound::Stopped;
		}
	}
}
void AudioEngine::PlaySound(int type, float loudness, float positionx, float positiony)
{
	auto pos = MainListener.getPosition();
	sf::Vector3f sndpos = sf::Vector3f(positionx,positiony,0);
	auto diff = pos - sndpos;
	float distsqrd = (diff.x * diff.x) + (diff.y * diff.y) + (diff.z * diff.z);
	if (distsqrd < 500 * 500)
	{
		int id = GetFreeSource();
		if (id != -1)
		{
			sf::Sound& snd = SourceList[id].snd;
			snd.setBuffer(this->AudioTypeArray[type].buffer);
			snd.setPosition(positionx, positiony, 0);
			snd.setRelativeToListener(false);
			snd.setAttenuation(0.1);
			snd.setVolume(loudness * Loudness * 100);
			snd.setLoop(false);
			snd.play();
			SourceList[id].Free = false;
		}
	}
}
int AudioEngine::GetFreeSource() 
{
	for (int i = 0; i < MaxAudioProducers; ++i)
	{
		if (SourceList[i].Free) 
		{
			return i;
		}
	}
	return -1;
}
