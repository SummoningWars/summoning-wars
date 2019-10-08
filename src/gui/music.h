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

#pragma once

#include <string>

#include "ogg.h"

class MusicManager
{
	public:
		/**
		 * \brief Konstruktor
		 */
		MusicManager()
		{
			m_musicfile = "";
			m_is_playing = false;
			m_music_volume = 1;
		}
		
		/**
		 * \brief Destruktor
		 */
		~MusicManager()
		{
			if (m_is_playing)
			{
				stop();
			}
		}
		
		/**
		 * \brief Spielt eine Musikdatei im .ogg Format ab
		 * \param file Musikdatei
		 * Spielt eine Musikdatei im .ogg Format ab. Die evtl zuvor laufende Musik wird gestoppt
		 */
		void play(std::string file);
		
		/**
		 * \brief Stoppt das Abspielen der Musik
		 */
		void stop();
		
		
		/**
		 * \brief Aktualisierung, muss regelmaessig aufgerufen werden
		 */
		void update();
			
		/**
		 * \brief Gibt die einzige Instanz der Klasse aus
		 */
		static MusicManager& instance()
		{
			static MusicManager inst;
			return inst;
		}
		
		/**
		 * \brief Gibt aus, ob gerade Musik gespielt wird
		 */
		bool isPlaying()
		{
			return m_is_playing;
		}
		
		/**
		 * \fn void setMusicVolume(float vol)
		 * \brief Stellt die Lautstaerke ein
		 * \param vol Lautstaerke (0-1)
		 */
		void setMusicVolume(float vol)
		{
			m_music_volume = vol;
			m_ogg_stream.setVolume(vol);
		}

		/**
		 * \fn float getMusicVolume()
		 * \brief Gibt die Lautstaerke des Sounds aus
		 */
		float getMusicVolume()
		{
			return m_music_volume;
		}
		
	private:
		
		/**
		 * \brief aktuell abgespielete Musikdatei
		 */
		std::string m_musicfile;
		
		/**
		 * \brief Gibt an, ob gerade eine Datei abgespielt wird
		 */
		bool m_is_playing;
		
		/**
		 * \brief Hilfsklasse fuer das Abspielen von ogg Dateien
		 */
		ogg_stream m_ogg_stream;
		
		/**
		 * \brief Lautstaerke
		 */
		float m_music_volume;
	
};

