/*
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 */

#ifndef SOUND_H
#define SOUND_H

#ifdef WIN32
	#include <al.h>
#else
	#include <AL/al.h>
#endif

#include <AL/alut.h>

#include <stdlib.h>
#include <stdio.h>
#include <math.h>
#include <fstream>
#include <iostream>
#include <map>
#include <queue>
#include <set>

#include "geometry.h"
#include <tinyxml.h>

/**
 * \brief Name eines Sounds
 */
typedef std::string SoundName;

/**
 * \brief Name einer Situation in der ein Sound abgespielt wird
 */
typedef std::string SoundTarget;

/**
 * \brief OpenAL ID eines Sounds
 */
typedef ALuint Sound;

class SoundObject;

/**
 * \class SoundSystem
 * \brief Basisklasse fuer Sound
 */
class SoundSystem
{
	public:
        /**
         * \brief Structure for a Set of sounds that might be played on a specific occasion
         */
        struct SoundSet
        {
            /**
             * \brief probability, that a sound it played at all
             */
            float m_probability;
            
            /**
             * \brief Number of weights for the different sounds
             * The relative values of the weights determine the probability which sound is played, if a sound is played
             */
            std::vector<float> m_weights;
            
            /**
             * \brief AL handles of the sounds
             */
            std::vector<Sound> m_sounds;
        };
        
		/**
		 * \fn static void init()
		 * \brief initialisiert das Soundsystem
		 */
		static void init();

		/**
		 * \fn static Sound getSound(SoundName sname)
		 * \brief Gibt zu einem gegebenen Name einen Sound aus
		 * \param sname Name des Sounds
		 */
		static Sound getSound(SoundName sname);

		/**
		 * \brief Laedt ein Soundfile und registriert den Sound unter einen gegebenen Name
		 */
		static void loadSoundFile(std::string file, SoundName sname, float weight);

		/**
		 * \fn static void loadSoundData(const char* pFilename)
		 * \brief Laedt Daten zu Sounds aus einer XML Datei
		 * \param pFilename Dateiname
		 */
		static void loadSoundData(const char* pFilename);

		/**
		 * \fn static void setListenerPosition(Vector pos)
		 * \brief Setzt die Position an der der Schall gehoehrt wird
		 */
		static void setListenerPosition(Vector pos);
		
		static bool checkErrors();

		/**
		 * \brief updates the object
		 */
		static void update();
		
		/**
		 * \fn static void cleanup()
		 * \brief gibt Speicher frei
		 */
		static void cleanup();
		
		/**
		 * \brief Deletes the buffers only and prepares for reloading
		 */
		static void cleanupBuffers();
		
		/**
		 * \fn static void clearObjects()
		 * \brief Loescht alle SoundObjekte
		 */
		static void clearObjects();

		/**
		 * \fn SoundObject* getSoundObject(std::string name)
		 * \brief Gibt das SoundObjekt mit dem angegebenen Name aus
		 * \param name Name
		 */
		static SoundObject* getSoundObject(std::string name);

		/**
		 * \fn static void createSoundObject(std::string name)
		 * \brief Erzeugt ein SoundObjekt mit dem angegebenen Name
		 * \param name Name
		 */
		static SoundObject* createSoundObject(std::string name);

		/**
		 * \fn static void deleteSoundObject(std::string name)
		 * \brief Loescht ein SoundObjekt mit dem angegebenen Name
		 */
		static void deleteSoundObject(std::string name);

		/**
		 * \fn static void deleteSoundObject(SoundObject* object)
		 * \brief Loescht das angegebene SoundObjekt
		 * \param object Soundobjekt
		 */
		static void deleteSoundObject(SoundObject* object);

		/**
		 * \brief tries to create a valid OpenAL source handle
		 * \param handle return parameter, containing the handle
		 * \return true if the handle is valid
		 */
		static bool getSourceHandle(ALuint &handle);
		
		/**
		 * \fn static void setSoundVolume(float vol)
		 * \brief Stellt die Lautstaerke ein
		 * \param vol Lautstaerke (0-1)
		 */
		static void setSoundVolume(float vol);

		/**
		 * \fn static float getSoundVolume()
		 * \brief Gibt die Lautstaerke des Sounds aus
		 */
		static float getSoundVolume()
		{
			return m_sound_volume;
		}
		
		/**
		 * \brief Plays an ambient sound
		 * \param sname Name of the sound
		 * \param volume Volume of the sound
		 * \param position Position of the sound
		 * If position is NULL, the sound is attached to the listener.
		 */
		static void playAmbientSound(SoundName sname, float volume = 1.0, Vector* position=0); 

	private:

		static void loadSoundInfos(TiXmlNode* node);

		/**
		 * \brief Bildet die Name der Sounds auf die OpenAL IDs ab
		 */
		static std::map<SoundName, SoundSet> m_sounds;

		/**
		 * \var static std::map<std::string, SoundObject*> m_sound_objects
		 * \brief Schallquellen sortiert nach Name
		 */
		static std::map<std::string, SoundObject*> m_sound_objects;

		/**
		 * \var static float m_sound_volume
		 * \brief Lautstaerke
		 */
		static float m_sound_volume;
		
		/**
		 * \brief Sound Device
		 */
		static ALCdevice * m_device;
		
		/**
		 * \brief AL Sound Kontext
		 */
		static ALCcontext* m_context;
		
		/**
		 * \brief List of background sounds played without specific object
		 */
		static std::list<SoundObject*> m_ambient_sounds;

};

/**
 * \class SoundObject
 * \brief Schallquelle
 */
class SoundObject
{
	public:
		/**
		 * \fn SoundObject(std::string name, Vector pos = Vector(0,0))
		 * \brief Konstruktor
		 * \param pos Position
		 * \param name Name
		 */
		SoundObject(std::string name, Vector pos = Vector(0,0));

		/**
		 * \fn ~SoundObject()
		 * \brief Destruktor
		 */
		~SoundObject();

		/**
		 * \fn void setPosition(Vector pos)
		 * \brief Setzt die Position der Schallquelle
		 * \param pos neue Position
		 */
		void setPosition(Vector pos);
		
		/**
		 * \brief Sets the position relative to the listener
		 * \param pos relative Position
		 **/
		void setPositionRelative(Vector pos);

		/**
		 * \fn Vector getPosition()
		 * \brief Gibt die Position der Schallquelle aus
		 */
		Vector getPosition();

		/**
		 * \fn void setSound(Sound snd)
		 * \brief Setzt den abgespielten Sound der Schallquelle
		 * \param snd Sound ID
		 */
		void setSound(Sound snd);

		/**
		 * \fn void setSound(SoundName sname)
		 * \brief Setzt den abgespielten Sound
		 * \param sname Name des Sounds
		 */
		void setSound(SoundName sname);

		/**
		 * \fn void setVolume(float vol)
		 * \brief Setzt die Lautstaerke der Schallquelle
		 * \param vol Lautstaerke (0-1)
		 */
		void setVolume(float vol)
		{
			if (m_handle_valid) 
				alSourcef(m_handle, AL_GAIN, vol*m_global_volume);
			m_volume = vol;
		}
		
		/**
		 * \fn void setGlobalVolume(float vol)
		 * \brief Setzt den globalen Faktor der Lautstaerke
		 * \param vol Lautstaerke (0-1)
		 */
		void setGlobalVolume(float vol)
		{
			if (m_handle_valid) 
				alSourcef(m_handle, AL_GAIN, m_volume*vol);
			m_global_volume = vol;
		}

		/**
		 * \fn void play()
		 * \brief Startet das Abspielen des eingestellten Sounds
		 */
		void play();

		/**
		 * \fn void setPlayOffset(float ms, float tol)
		 * \brief Setzt den Zeitpunkt des Sounds der gerade abgespielt wird
		 * \param ms Zeitpunkt in ms seit Beginn
		 * \param tol Toleranzwert - wenn der aktuelle Zustand weniger als um tol abweicht wird nichts geaendert
		 */
		void setPlayOffset(float ms, float tol);

		/**
		 * \fn void stop()
		 * \brief Stoppt das Abspielen des eingestellten Sounds
		 */
		void stop();

		/**
		 * \fn void rewind()
		 * \brief Spult den abgespielten Sound zurueck
		 */
		void rewind();

		/**
		 * \fn SoundName getSoundName()
		 * \brief Gibt den Name des aktuell abgespielten Sounds aus
		 */
		SoundName getSoundName()
		{
			return m_sound_name;
		}

		/**
		 * \fn void update()
		 * \brief aktualisiert internen Status
		 */
		void update();


		/**
		 * \fn std::string getName()
		 * \brief gibt den Name aus
		 */
		std::string getName()
		{
			return m_name;
		}
		
		/**
		 * \brief indicates if the sound source is playing sound right now
		 */
		bool isPlaying();

	private:
		/**
		 * \var ALuint m_handle
		 * \brief OpenAL handle fuer die Soundquelle
		 **/
		ALuint m_handle;
		
		/**
		 * \brief true, if the handle belongs to a valid OpenAL Source
		 */
		bool m_handle_valid;

		/**
		 * \var SoundName m_sound_name
		 * \brief Name des aktuellen Sounds
		 */
		SoundName m_sound_name;

		/**
		 * \var std::string m_name
		 * \brief Name des Objektes
		 */
		std::string m_name;
		
		/**
		 * \brief Zusatzfaktor zum eigenen Lautstaerkewert
		 */
		float m_global_volume;
		
		/**
		 * \brief Lautstarke
		 */
		float m_volume;
		
		/**
		 * \brief position of the sound source
		 */
		Vector m_position;
};

#endif

