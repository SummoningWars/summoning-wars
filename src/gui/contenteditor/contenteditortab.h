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

#ifndef CONTENTEDITOR_TAB_H
#define CONTENTEDITOR_TAB_H

#include <OgreMeshManager.h>
#include "CEGUI/CEGUI.h"
#include "debug.h"
#include "mathhelper.h"

/**
 * \brief Base class for all content editor components.
 */
class ContentEditorTab
{
	public:
		
		/**
		 * \brief Initialises the panel
		 * \param parent parent CEGUI window
		 */
		virtual void init(CEGUI::Window* parent)
		{
			m_no_cegui_events = false;
      m_rootWindow = parent;
		}
		
		/**
		* \brief Updates the content of the tab
		* Function is called once each tick.
		*/
		virtual void update()
		{
		}
		
		/**
		 * \brief tells the object to update underlying game data
		 */
		void setUpdateBaseContent()
		{
			m_update_base_content = true;
		}
		
	protected:
		/**
		 * \brief Sets the text of a CEGUI window
		 * \param windowname Name of the window
		 * \param text new text of the window
		 */
		void setWindowText(std::string windowname, std::string text);
		
		/**
		 * \brief returns the text of a CEGUI window
		 * \param windowname Name of the window
		 * \return text of the window
		 */
		std::string getWindowText(std::string windowname, std::string def="");
		
		/**
		 * \brief Sets the value of a CEGUI spinner
		 * \param windowname Name of the window
		 * \param value new value
		 */
		void setSpinnerValue(std::string windowname, double value);
		
		/**
		 * \brief Returns the value of a CEGUI spinner
		 * \param windowname Name of the window
		 * \return current spinner value
		 */
		double getSpinnerValue(std::string windowname, double def=0);
		
		/**
		 * \brief Sets the state of a CEGUI Checkbox
		 * \param windowname Name of the window
		 * \param state new state
		 */
		void setCheckboxSelected(std::string windowname, bool state);
		
		/**
		 * \brief Returns the state of a CEGUI Checkbox
		 * \param windowname Name of the window
		 * \return state
		 */
		bool getCheckboxSelected(std::string windowname);
		
		/**
		 * \brief returns the selected Item of a combobox
		 * \param windowname Name of the window
		 * \param def default string returned if no item is selected
		 * \return text of the selected item
		 */
		std::string getComboboxSelection(std::string windowname, std::string def="");
		
		/**
		 * \brief set the selection of a CEGUI combobox
		 * \param windowname Name of the window
		 * \param selection name of the selected item
		 */
		void setComboboxSelection(std::string windowname, std::string selection);
		
		/**
		 * \brief Sets the cursor in a CEGUI MultiLineEditbox at the specified position
		 * \param windowname Name of the window
		 * \param row cursor row 
		 * \param col cursor column
		 */
		void setMultiLineEditboxCursor(std::string windowname, int row, int col);
		
		void getNodeBounds(const Ogre::SceneNode* node, Ogre::Vector3& minimum, Ogre::Vector3& maximum, int level=1);
		

		
	protected:
		/**
		 * \brief while set to true, all CEGUI events will be ignored
		 * This allows setting some GUI elements in event handlers without getting infinite loops due.
		 */
		bool m_no_cegui_events;
		
		/**
		 * \brief marks that underlying game data should be reloaded
		 */
		bool m_update_base_content;

	  /**
	  * \var 	CEGUI::Window *m_rootWindow;
	  * \brief  Pointer to the root window of this panel
	  */
	  CEGUI::Window* m_rootWindow;

		
};


#endif