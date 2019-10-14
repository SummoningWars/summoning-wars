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

#ifndef __SUMWARS_GUI_OPTIONSWINDOW_H__
#define __SUMWARS_GUI_OPTIONSWINDOW_H__

#include "window.h"
#include <OISKeyboard.h>

/**
 * \class OptionsWindow
 * \brief Fenster Optionen
 */
class OptionsWindow : public Window
{
	public:
	/**
	 * \fn OptionsWindow (Document* doc, OIS::Keyboard *keyboard)
	 * \param keyboard OIS keyboard
	 * \brief Konstruktor
	 */
		OptionsWindow (Document* doc, OIS::Keyboard *keyboard, const std::string& ceguiSkinName);
	
		/**
		 * \fn virtual void update()
		 * \brief aktualisiert den Inhalt des Fensters
		 */
		virtual void update();
		
		/**
		 * \fn virtual void updateTranslation
		 * \brief aktualisiert die Uebersetzungen
		 */
		virtual void updateTranslation();
		
		
		/**
		 * \fn bool requestsForKey()
		 * \brief Gibt an, ob das Fenster aktuell auf einen Tastendruck wartet
		 */
		bool requestsForKey()
		{
			return m_key_destination != NO_KEY;
		}
		
		/**
		 * \fn setzt den Keycode fuer das per Klick ausgewaehlte Label
		 * \param kc KeyCode
		 */
		void setKeyCode(KeyCode kc);
		
		/**
		 * \fn void reset()
		 * \brief Setzt das Fenster zurueck
		 */
		void reset();
		
	private:
		
		/**
		* \brief Aktion fuer die aktuell ein Shortkey erwartet wird
		 */
		ShortkeyDestination m_key_destination;
		
		/**
		 * Connect specific sound events for the widget.
		 */
		void connectWidgetSoundEvents (const std::string& widgetName, const std::string& widgetType);

		/**
		 * \fn bool onButtonItemClicked (const CEGUI::EventArgs& evt)
		 * \brief Handle the click of gui items.
		 */
		bool onGUIItemClicked (const CEGUI::EventArgs& evt);

		/**
		 * \fn bool onButtonItemHover(const CEGUI::EventArgs& evt)
		 * \brief Handle the hovering of gui items.
		 */
		bool onButtonItemHover (const CEGUI::EventArgs& evt);
		
		/**
		 * \fn bool onShortkeyLabelClicked(const CEGUI::EventArgs& evt)
		 * \brief Behandelt Klick auf ein Shortkey Label
		 */
		bool onShortkeyLabelClicked(const CEGUI::EventArgs& evt);
		
		/**
		 * \fn bool onShortkeyLabelClicked(const CEGUI::EventArgs& evt)
		 * \brief Behandelt Klick auf Button Ok
		 */
		bool onButtonOkClicked(const CEGUI::EventArgs& evt);

		/**
		 * \fn bool onButtonCancelClicked(const CEGUI::EventArgs& evt)
		 * \brief Handle the click on the "cancel" button. Also for any atomatically createrd.
		 */
		bool onButtonCancelClicked(const CEGUI::EventArgs& evt);
		
		/**
		 * \fn bool onAreaMouseButtonPressed(const CEGUI::EventArgs& evt);
		 * \brief Behandelt Druecken der linken Maustaste ueber auf dem Fenster
		 */
		bool onAreaMouseButtonPressed(const CEGUI::EventArgs& evt);
		
		/**
		 * \brief Handles change of difficulty
		 */
		bool onDifficultyChanged(const CEGUI::EventArgs& evt);
		
		/**
		 * \brief Handles change of text speed setting
		 */
		bool onTextSpeedChanged(const CEGUI::EventArgs& evt);
		
		/**
		 * \fn bool onSoundVolumeChanged(const CEGUI::EventArgs& evt);
		 * \brief Behandelt Aendern der Soundlautstarke
		 */
		bool onSoundVolumeChanged(const CEGUI::EventArgs& evt);
		
		/**
		 * \brief Behandelt Aendern der Musiklautstarke
		 */
		bool onMusicVolumeChanged(const CEGUI::EventArgs& evt);
		
		/**
		 * \brief Handles change of enemy highlight color setting
		 */
		bool onEnemyHighlightChanged(const CEGUI::EventArgs& evt);
		
		/**
		 * \brief Handles click on the reset graphics button
		 */
		//bool onResetGraphics(const CEGUI::EventArgs& evt);
		// TODO: get rid of obsolete code.

		/**
		 * \brief Handles the change of shadow mode
		 */
		bool onShadowModeSelected(const CEGUI::EventArgs& evt);

		/**
		 * \fn bool onLanguageSelected(const CEGUI::EventArgs& evt)
		 * \brief Behandelt Auswahl einer neuen Sprache
		 */
		bool onLanguageSelected(const CEGUI::EventArgs& evt);

		/**
		 * \fn bool onVideoDriverSelected(const CEGUI::EventArgs& evt)
		 * \brief React to the used renderer being changed.
		 */
		bool onVideoDriverSelected (const CEGUI::EventArgs& evt);
		
		/**
		 * \fn bool onDisplayModeSelected(const CEGUI::EventArgs& evt)
		 * \brief React to the display mode being changed.
		 */
		bool onDisplayModeSelected (const CEGUI::EventArgs& evt);

		/**
		 * \fn bool onGrabMouseChanged(const CEGUI::EventArgs& evt)
		 * \brief Handles changes to wheter the mouse will be grabbed or not in window mode
		 */
		bool onGrabMouseChanged(const CEGUI::EventArgs& evt);
		
		/**
		 * \brief returns index of highlight color
		 */
		unsigned int getColorSelectionIndex(std::string name);

		/**
		 * \var OIS::Keyboard *m_keyboard
		 * \brief Repraesentation der Tastatur
		 */
		OIS::Keyboard *m_keyboard;

		/**
		 * \brief The name of the CEGUI skin to use.
		 */
		std::string m_ceguiSkinName;
};

#endif // __SUMWARS_GUI_OPTIONSWINDOW_H__

