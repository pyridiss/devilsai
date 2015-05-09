
/*
    Devilsai - A game written using the SFML library
    Copyright (C) 2009-2015  Quentin Henriet

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

#include <SFML/Audio.hpp>

#include "Bibliotheque.h"
#include "Constantes.h"

/** Ce fichier implémente le gestionnaire d'images du jeu **/

class Basic_Sound
{
	SoundBuffer buffer;
	Sound sound;

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

	Basic_Sound sound;
	const auto& result = sounds.insert(SoundClass::value_type(soundID, sound));

	bool success = result.first->second.loadFromFile(INSTALL_DIR + "sound/" + soundID + ".ogg");

	if (success) MESSAGE("Sound " + soundID + " loaded", MUSIC)
	else Erreur("This sound does not exist:", soundID);
}

void addMusic(string musicID)
{
	if (musics.find(musicID) != musics.end()) return;

	Basic_Music music;
	const auto& result = musics.insert(MusicClass::value_type(musicID, music));

	bool success = result.first->second.openFromFile(INSTALL_DIR + "music/" + musicID + ".ogg");

	if (success) MESSAGE("Music " + musicID + " opened", MUSIC)
	else Erreur("This music does not exist:", musicID);
}

void playSound(string soundID)
{
	SoundClass::iterator i = sounds.find(soundID);

	if (i != sounds.end())
		sounds[soundID].start();
	else Erreur("This sound has not been loaded:", soundID);
}

void playMusic(string musicID)
{
	//First, we check if no other music is running
	for (auto& i : musics)
	{
		if (i.second.getMusicState() == Basic_Music::Running) i.second.fadeOut();
	}

	MusicClass::iterator i = musics.find(musicID);

	if (i != musics.end())
		musics[musicID].fadeIn();
	else Erreur("This music has not been loaded:", musicID);
}

void stopMusic(string musicID)
{
	MusicClass::iterator i = musics.find(musicID);

	if (i != musics.end())
		musics[musicID].fadeOut();
	else Erreur("This music has not been loaded:", musicID);
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
{
	music = nullptr;
	currentVolume = 100;
	musicState = Stopped;
}

Basic_Music::~Basic_Music()
{
	if (music != nullptr) delete music;
	music = nullptr;
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

	currentVolume += I(1);

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

	currentVolume -= I(1);

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


void deleteMusics()
{
	musics.clear();
	sounds.clear();

	MESSAGE("All sounds and musics are deleted.", MUSIC)
}