
/*
    Devilsai - A game written using the SFML library
    Copyright (C) 2009-2017  Quentin Henriet

    This program is free software: you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation, either version 3 of the License, or
    (at your option) any later version.

    This program is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with this program.  If not, see <http://www.gnu.org/licenses/>.
*/

#include <map>

#include <SFML/Audio.hpp>

#include "tools/debug.h"
#include "tools/filesystem.h"
#include "tools/timeManager.h"
#include "musicManager/musicManager.h"

using namespace sf;

namespace musicManager{

class Basic_Sound
{
	SoundBuffer buffer;
	Sound sound;

    public:
        Basic_Sound()
          : buffer(),
            sound()
        {
        }

	public:
		bool loadFromFile(string file);
		void start();
};

class Basic_Music
{
	public:
		enum musicStates {Running, Stopped, FadingIn, FadingOut};

	public:
		Music* music;
		musicStates musicState;
		float currentVolume;

	public:
		Basic_Music();
        Basic_Music(const Basic_Music&) = delete;
        Basic_Music& operator=(const Basic_Music&) = delete;
		~Basic_Music();

	public:
		bool openFromFile(string file);
		void fadeIn();
		void fadeOut();
		musicStates getMusicState();
};

typedef map < string, Basic_Sound >	SoundClass;
typedef map < string, Basic_Music >	MusicClass;

SoundClass sounds;
MusicClass musics;


void addSound(string soundID)
{
    if (sounds.find(soundID) != sounds.end()) return;

    bool success = sounds[soundID].loadFromFile(tools::filesystem::dataDirectory() + "sound/" + soundID + ".ogg");

    if (success) tools::debug::message("Sound " + soundID + " has been loaded", "musics", __FILENAME__, __LINE__);
    else tools::debug::error("Failed to load sound: " + soundID, "files", __FILENAME__, __LINE__);
}

void addMusic(string musicID)
{
    if (musics.find(musicID) != musics.end()) return;

    bool success = musics[musicID].openFromFile(tools::filesystem::dataDirectory() + "music/" + musicID + ".ogg");

    if (success) tools::debug::message("Music " + musicID + " has been loaded", "musics", __FILENAME__, __LINE__);
    else tools::debug::error("Failed to load music: " + musicID, "files", __FILENAME__, __LINE__);
}

void playSound(string soundID)
{
	SoundClass::iterator i = sounds.find(soundID);

	if (i != sounds.end())
		sounds[soundID].start();
	else tools::debug::error("This sound has not been loaded yet: " + soundID, "files", __FILENAME__, __LINE__);
}

void playMusic(string musicID)
{
	//First, we check if no other music is running
	for (auto& i : musics)
	{
		if (i.second.getMusicState() == Basic_Music::Running)
        {
            if (i.first == musicID) return; //Music is already running
            i.second.fadeOut();
        }
	}

	MusicClass::iterator i = musics.find(musicID);

	if (i != musics.end())
		musics[musicID].fadeIn();
	else tools::debug::error("This music has not been loaded yet: " + musicID, "files", __FILENAME__, __LINE__);
}

void stopMusic(string musicID)
{
	MusicClass::iterator i = musics.find(musicID);

	if (i != musics.end())
		musics[musicID].fadeOut();
	else tools::debug::error("This music has not been loaded yet: " + musicID, "files", __FILENAME__, __LINE__);
}

void manageRunningMusics()
{
	bool aMusicIsBeingPlayed = false;

	for (auto& i : musics)
	{
		if (i.second.getMusicState() == Basic_Music::Running) aMusicIsBeingPlayed = true;
		if (i.second.getMusicState() == Basic_Music::FadingOut)
		{
			i.second.fadeOut();
			aMusicIsBeingPlayed = true;
		}
	}

	if (!aMusicIsBeingPlayed)
		for (auto& i : musics)
		{
			if (i.second.getMusicState() == Basic_Music::FadingIn)
			{
				i.second.fadeIn();
			}
		}
}

/* 'Basic_Sound' functions */

bool Basic_Sound::loadFromFile(string file)
{
	bool result = buffer.loadFromFile(file);
	if (result) sound.setBuffer(buffer);
	return result;
}

void Basic_Sound::start()
{
	if (sound.getStatus() == Sound::Stopped)
		sound.play();
}


/* 'Basic_Music' functions */

Basic_Music::Basic_Music()
  : music(nullptr),
    musicState(Stopped),
    currentVolume(100)
{
}

Basic_Music::~Basic_Music()
{
	if (music != nullptr) delete music;
}

bool Basic_Music::openFromFile(string file)
{
	if (music == nullptr) music = new Music;
	bool result = music->openFromFile(file);
	music->setLoop(true);
	return result;
}

void Basic_Music::fadeIn()
{
	if (musicState == Stopped || musicState == FadingOut)
	{
		music->play();
		musicState = FadingIn;
		currentVolume = 0;
	}

    currentVolume += tools::timeManager::I(1);

	if (currentVolume >= 100)
	{
		musicState = Running;
		currentVolume = 100;
	}

	music->setVolume(currentVolume);
}

void Basic_Music::fadeOut()
{
	if (musicState == Running || musicState == FadingIn)
	{
		musicState = FadingOut;
		currentVolume = 100;
	}

    currentVolume -= tools::timeManager::I(1);

	if (currentVolume <= 0)
	{
		music->stop();
		musicState = Stopped;
		currentVolume = 0;
	}

	music->setVolume(currentVolume);
}

Basic_Music::musicStates Basic_Music::getMusicState()
{
	return musicState;
}

} //namespace musicManager
