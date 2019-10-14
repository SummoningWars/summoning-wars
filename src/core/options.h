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
#include <vector>
#include <map>
#include <set>

/**
 * TODO: should rewrite class to allow easier extensions (maybe using mapping?) Because in the current form, as soon
 * as an option is added, the entire project is recompiled.
 * Also, an important piece of information needs to be added for each option:
 * Does changing it take effect immediately? Or is a game restart required? Because if it is, the user will need to be
 * informed.
 */

/**
 * Code fuer eine Taste
 */
typedef int KeyCode;

/**
 * \enum ShortkeyDestination
 * \brief List of Actions that may be triggered by a shortkey
 */
enum ShortkeyDestination
{
	NO_KEY=0,
	SHOW_INVENTORY =1,
	SHOW_CHARINFO =2,
	SHOW_SKILLTREE =3,
	SHOW_PARTYMENU=4,
	SHOW_CHATBOX =5,
	SHOW_CHATBOX_NO_TOGGLE =6,
	SHOW_QUESTINFO =7,
	SHOW_MINIMAP =8,
	SHOW_ITEMLABELS=9,
	CLOSE_ALL=19,
	SWAP_EQUIP=20,
	SHOW_OPTIONS = 21,
	SWAP_RIGHT_ACTION=22,
	USE_POTION = 30,
	USE_SKILL_LEFT=100,
	USE_SKILL_RIGHT=300,
	CHEAT = 1000,
};


/**
 * \enum DisplayModes
 *
 */
enum DisplayModes
{
	FULLSCREEN_EX = 0,
	WINDOWED_WITH_BORDER = 1,
	WINDOWED_FULLSCREEN = 2
};




/**
 * Maps Keycodes to the Actions that are triggered by the keycode
 */
typedef std::map<KeyCode,ShortkeyDestination> ShortkeyMap;

/**
 * \brief Stores all Options that may be changed in the Options Menu
 * Manages all Options that may be changed in the Options Menu. In addition, offers some functions store / save them to XML. Some option values are not stored inside the Object itself. 
 */
class Options
{
	public:

		/**
		 * \brief Enumeration of shadow options
		 */
		enum ShadowMode
		{
			SM_NONE = 0,
			SM_SM_SIMPLE = 1, // SHADOWTYPE_STENCIL_MODULATIVE
			SM_SA_SIMPLE = 2, // SHADOWTYPE_STENCIL_ADDITIVE
			SM_TA_SIMPLE = 3, // SHADOWTYPE_TEXTURE_ADDITIVE
			SM_TM_SIMPLE = 4, // SHADOWTYPE_TEXTURE_MODULATIVE
			SM_COUNT // The count of shadow modes (2); must always use a value smaller than this one!
		};

		/**
		 * \brief Enumeration of Difficulty settings
		 */
		enum Difficulty
		{
			EASY = 1,
			NORMAL = 2,
			HARD = 3,
			INSANE = 4,
		};
		
		/**
		 * \brief Returns the Instance of the Object (Singleton pattern)
		 */
		static Options* getInstance();
		
		/**
		 * \brief Initializes data structures
		 */
		void init();
		
		/**
		 * Destructor
		 */
		~Options()
		{
			
		};
		
		/**
		 * \brief Sets all options to the default values
		 */
		void setToDefaultOptions();
		
		/**
		 * \brief Writes all Option values to XML file
		 * \return true if successful, else false
		 */
		bool readFromFile(const std::string& filename);
		
		/**
		 * \brief Reads all option values from XML file
		 * \return true if operation was successful, else false
		 */
		bool writeToFile(const std::string& filename);
		
		/**
		 * \fn static void setSoundVolume(float vol)
		 * \brief set the sound volume
		 * \param vol sound volume (0-1)
		 */
		void setSoundVolume(float vol);

		/**
		 * \fn static float getSoundVolume()
		 * \brief get the sound volume
		 * \return sound volume
		 */
		float getSoundVolume();
		
		/**
		 * \brief set music volume
		 * \param vol music volume (0-1)
		 */
		void setMusicVolume(float vol);
		
		/**
		 * \brief returns the music volume
		 * \param vol music volume
		 */
		float getMusicVolume();
		
		/**
		 * \brief Checks if a keycode is a special key
		 * \param kc keycode
		 * Special keys (as Escape and Enter) may not be used as shortkeys
		 * \return true, if \a kc is a special key, false otherwise
		 */
		bool isSpecialKey(KeyCode kc)
		{
			return  m_special_keys.count(kc) > 0;
		}
		
		/**
		 * \brief Returns the locale for internationalization
		 */
		std::string getLocale();

	
		/**
		 * \brief Sets the locale for internationalization
		 * \param locale locale string, en_US for instance
		 */
		void setLocale(const std::string& locale);


		/**
		 * \fn bool installShortkey(KeyCode key,ShortkeyDestination dest, bool check_special=true)
		 * \brief Binds keycode \a key to the action specified by \a dest
		 * \param key keycode
		 * \param dest action to be triggered by the key
		 * \return true if the shortkey was successfully set, false otherwise
		 */
		bool setShortkey(KeyCode key,ShortkeyDestination dest);
		
		/**
		 * \fn KeyCode getMappedKey(ShortkeyDestination sd)
		 * \brief Returns the keycode, that triggers the action specified by \a sd
		 * Returns NO_KEY if there is not keycode triggering the action
		 */
		KeyCode getMappedKey(ShortkeyDestination sd);
		
		/**
		 * \brief Returns the action that is mapped to the given key
		 * \param key keycode
		 */
		ShortkeyDestination getMappedDestination(KeyCode key);
		
		/**
		 * \brief Sets the port for network communication
		 * \param port port number
		 */
		void setPort(int port)
		{
			m_port = port;
		}
		
		/**
		 * \brief Returns the port number currently used for network communication
		 */
		int getPort()
		{
			return m_port;
		}
		
		/**
		 * \brief Sets the name of the server host
		 * \param host hostname
		 */
		void setServerHost(const std::string& host)
		{
			m_server_host = host;
		}
		
		/**
		 * \brief Return the name of the server host
		 * \return hostname
		 */
		const std::string& getServerHost()
		{
			return m_server_host;
		}
		
		/**
		 * \brief Sets the maximal number of players that can participate in a game
		 * \param max_nr player limit
		 */
		void setMaxNumberPlayers(int max_nr)
		{
			m_max_players = max_nr;
		}
		
		/**
		 * \brief Returns the maximal number of players that can participate in a game
		 */
		int getMaxNumberPlayers()
		{
			return m_max_players;
		}
		
		/**
		 * \brief Sets the new difficulty
		 * \param difficulty new difficulty setting
		 */
		void setDifficulty(Difficulty difficulty)
		{
			m_difficulty = difficulty;
		}
		
		/**
		 * \brief returns the current difficulty
		 */
		Difficulty getDifficulty()
		{
			return m_difficulty;
		}
		
		/**
		 * \brief Returns the text speed factor
		 */
		float getTextSpeed()
		{
			return m_text_speed;
		}
		
		/**
		 * \brief Sets the text speed factor (factor of 1 is standard speed)
		 * \param text_speed text speed
		 */
		void setTextSpeed(float text_speed)
		{
			m_text_speed = text_speed;
		}
		
		/**
		 * \brief Returns enemy highlight color
		 */
		std::string getEnemyHighlightColor()
		{
			return m_enemy_highlight_color;
		}

		/**
		 * \brief Sets the highlight color for enemies
		 * \param color color (i.e. red, blue, green)
		 */
		void setEnemyHighlightColor(std::string color)
		{
			m_enemy_highlight_color = color;
		}
		
		/**
		 * \brief Sets the value of a debug option
		 * \param valname Name of the option
		 * \param value value as string
		 */
		void setDebugOption(std::string valname, std::string value)
		{
			m_debug_options[valname] = value;
		}
		
		/**
		 * \brief Returns the value of a debug option
		 * Returns the default, if the string is not set
		 * \param valname Name of the option
		 * \param defaultval value that is returned if no value is set
		 */
		std::string getDebugOption(std::string valname, std::string defaultval ="")
		{
			if (m_debug_options.count(valname) == 0)
				return defaultval;
			
			return m_debug_options[valname];
		}
		
		/**
		 * \brief Returns grab mouse setting
		 */
		bool getGrabMouseInWindowedMode()
		{
			return m_grab_mouse;
		}

		/**
		 * \brief Sets the grab mouse setting
		 * \param bool grabMouse True / False
		 */
		void setGrabMouseInWindowedMode(bool grabMouse)
		{
			m_grab_mouse = grabMouse;
		}
		

		/**
		 * \brief Returns the windowed fullscreen mode support.
		 */
		bool isWindowedFullscreenModeSupported ()
		{
			return m_windowed_fullscreen_mode_supported;
		}


		/**
		 * \brief Sets the windowed fullscreen mode support.
		 * \param supported Specify whether the setting is supported or not.
		 */
		void setWindowedFullscreenModeSupported (bool supported)
		{
			m_windowed_fullscreen_mode_supported = supported;
		}


		/**
		 * \brief Returns the used display mode.
		 */
		DisplayModes getUsedDisplayMode ()
		{
			return m_used_display_mode;
		}

		/**
		 * \brief Sets the display mode to use.
		 */
		void setUsedDisplayMode (DisplayModes newMode)
		{
			m_used_display_mode = newMode;
		}


		/**
		 * \brief Get the list of video drivers that can be used. Note: you can edit the list (the ref is retrieved).
		 */
		std::vector <std::string>& getEditableAvailableVideoDrivers ()
		{
			return m_available_video_drivers;
		}

		/**
		 * \brief Obtain access to a mapping of lists of resolutions.
		 */
		std::map <std::string, std::vector <std::string> >& getEditableResolutionsMapping ()
		{
			return m_available_resolutions;
		}

		/**
		 * \brief Obtain the used screen resolution.
		 */
		const std::string& getUsedResolution () const
		{
			return m_used_resolution;
		}


		/**
		 * \brief Set the resolution to use.
		 */
		void setUsedResolution (const std::string& newResolution)
		{
			m_used_resolution = newResolution;
		}

		/**
		 * \brief Get the used video driver.
		 */
		const std::string& getUsedVideoDriver () const
		{
			return m_used_video_driver;
		}


		/**
		 * \brief Set the used video driver.
		 */
		void setUsedVideoDriver (const std::string& newDriverName)
		{
			m_used_video_driver = newDriverName;
		}


		/**
		 * \brief Find out if the user chose to show or hide the console window.
		 * (Windows specific).
		 * \return true if the console window is to be shown, false otherwise.
		 */
		bool getShowConsoleWindow () const
		{
			return m_show_console_window;
		}


		/**
		 * \brief Set the user's preferrnce to show or hide the console window.
		 * \param newValue The new value to set.
		 */
		void setShowConsoleWindow (bool newValue)
		{
			m_show_console_window = newValue;
		}


		/**
		 * \brief Get the shadow mode to use.
		 */
		ShadowMode getShadowMode () const
		{
			return m_shadow_mode;
		}


		/**
		 * Set the shadow mode to use.
		 */
		void setShadowMode (ShadowMode newMode)
		{
			m_shadow_mode = newMode;
		}
		
		/**
		 * \brief Get the savegame chosen at startup
		 */
		std::string getDefaultSavegame() const
		{
			return m_default_savegame;
		}
		
		/**
		 * \brief Set the savegame chosen at startup
		 */
		void setDefaultSavegame(std::string savegame)
		{
			m_default_savegame = savegame;
		}
		
		/**
		 * \brief Pushes the named value on the lua stack
		 * \param valname name of the value
		 * \return number of values pushed to the stack
		 */
		int getValue(std::string valname);
		

		/**
		 * \brief Set the named member, value is taken from lua stack
		 * \param valname of the value
		 * \param bool true if successful, else false
		 */
		bool setValue(std::string valname);


		/**
		 * \brief Getter for the CEGUI skin to use for the UI.
		 * \author Augustin Preda.
		 */
		const std::string& getCeguiSkin () const
		{
			return m_cegui_skin;
		}

		/**
		 * \brief Setter for the CEGUI skin to use for the UI.
		 * \author Augustin Preda.
		 */
		void setCeguiSkin (const std::string& skinName)
		{
			m_cegui_skin = skinName;
		}

		/**
		 * \brief Getter for the CEGUI skin to use for the UI.
		 * \author Augustin Preda.
		 */
		const std::string& getCeguiCursorSkin () const
		{
			return m_cegui_cursor_skin;
		}

		/**
		 * \brief Setter for the CEGUI skin to use for the UI.
		 * \author Augustin Preda.
		 */
		void setCeguiCursorSkin (const std::string& skinName)
		{
			m_cegui_cursor_skin = skinName;
		}


	private:
		/**
		 * Constructor
		 */
		Options();
		
		/**
		 * \var std::set<KeyCode> m_special_keys
		 * \brief Set of special key, that may not be used as shortkeys
		 */
		std::set<KeyCode> m_special_keys;
		
		/**
		 * \brief Stores the mapping of keys to actions
		 */
		ShortkeyMap m_shortkey_map;
		
		/**
		 * \var std::string m_server_host
		 * \brief IP des Servers
		 */
		std::string m_server_host;

		/**
		 * \var int m_port
		 * \brief Port ueber den der Netzverkehr laeuft
		 */
		int m_port;

		/**
		 * \var int m_max_players
		 * \brief maximale Anzahl Spieler
		 */
		int m_max_players;
		
		
		/**
		 * \brief difficulty setting
		 */
		Difficulty m_difficulty;
		
		/**
		 * \brief Factor the influences text display times
		 */
		float m_text_speed;
		
		/**
		 *\brief Enemy highlight color
		 */
		std::string m_enemy_highlight_color;
		
		/**
		 * \brief Collection of additional options used for debugging
		 */
		std::map<std::string, std::string> m_debug_options;
		
		/**
		 * \brief Grab mouse in windowed mode setting
		 */
		bool m_grab_mouse;

		/**
		 * \brief Is the windowed fullscreen mode supported?
		 */
		bool m_windowed_fullscreen_mode_supported;

		/**
		 * \brief User preferrence to show or hide the console window.
		 */
		bool m_show_console_window;

		/**
		 * \brief The preferred shadow mode.
		 */
		ShadowMode m_shadow_mode;

		/**
		 * \brief The currently selected display mode.
		 */
		DisplayModes m_used_display_mode;

		/**
		 * \brief The currently loaded video driver.
		 */
		std::string m_used_video_driver;

		/**
		 * \brief The used screen/window displat resolution.
		 */
		std::string m_used_resolution;

		/**
		 * \brief The available video drivers.
		 */
		std::vector <std::string> m_available_video_drivers;

		/**
		 * \brief The available display resolutions.
		 */
		std::map <std::string, std::vector <std::string> > m_available_resolutions;
		
		/**
		 * \brief default savegame chosen at the start
		 */
		std::string m_default_savegame;

		/**
		 * \brief Specify the CEGUI skin to use.
		 */
		std::string m_cegui_skin;

		/**
		 * \brief Specify the CEGUI cursor skin to use.
		 */
		std::string m_cegui_cursor_skin;

};
