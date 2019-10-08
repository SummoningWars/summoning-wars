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

#ifndef ContentEditor_H
#define ContentEditor_H

#include <OgreSingleton.h>
#include "CEGUI/CEGUI.h"
#include "contenteditortab.h"
#include "OISKeyboard.h"
#include "debug.h"

#include <map>

#include "graphicobjectrenderinfo.h"
#include "graphicobject.h"
#include "fixedbase.h"

class ContentEditor : public Ogre::Singleton<ContentEditor>
{
public:
	/**
	* \fn void init(bool visible);
	* \brief Initialises the panel
	* \param visible Whether the panel is visible or not at the begining
	*/
	void init(bool visible);
	
	
	/**
	* \fn void toggleVisibility();
	* \brief Toggle the visibility
	*/
	void toggleVisibility();

	
	/**
	* \fn void update();
	* \brief Updates all registered tabs
	*/
	void update(OIS::Keyboard* keyboard, OIS::Mouse* mouse);
	
	/**
	 * \brief Tells the editor, that one component needs a full update
	 * \param name Name of the editor tab
	 */
	void fullUpdateComponent(std::string name);
	
	
	static ContentEditor& getSingleton(void);
	static ContentEditor* getSingletonPtr(void);

	/**
	 * \brief Returns the Tab with the supplied name
	 * \param name Name of the editor tab
	 */
	ContentEditorTab* getComponent(std::string name);
	
	/**
	 * \param evt CEGUI event arguments
	 * \brief Called upon clicking a mouse button down to the preview window
	 */
	bool onPreviewWindowMouseDown(const CEGUI::EventArgs& evt);
	
	/**
	 * \param evt CEGUI event arguments
	 * \brief Called upon releasing a mouse button to the preview window
	 */
	bool onPreviewWindowMouseUp(const CEGUI::EventArgs& evt);
	
	/**
	 * \param evt CEGUI event arguments
	 * \brief Called upon using the scrollwheel
	 */
	bool onPreviewWindowScrollWheel(const CEGUI::EventArgs& evt);
	
protected:
	
	/**
	 * \param evt CEGUI event arguments
	 * \brief Called upon click on close button
	 */
	bool onClose(const CEGUI::EventArgs& evt);


	

	
	/**
	 * \brief updates the Translation of the labels
	 */
	virtual void updateTranslation();
	
	/**
	* \fn handleCloseWindow(const CEGUI::EventArgs& e);
	* \brief Handles clicks on the "Close Window" button
	* \param e Event Args from CEGUI
	*/
	virtual bool handleCloseWindow(const CEGUI::EventArgs& e);
	
	
private:

	/**
	* \var 	CEGUI::Window *m_rootWindow;
	* \brief  Pointer to the root window of this panel
	*/
	CEGUI::Window *m_rootWindow;
	
	/**
	* \var 	float m_lastVisibilitySwitch;
	* \brief Buffers the last visibility toggle time
	*/
	long m_lastVisibilitySwitch;
		
	
	/**
	 * \brief While true, the user rotates the camera in the preview window
	 */
	bool m_leftMouseDown;
	
	/**
	 * \brief While true, the user zooms the camera in the preview window
	 */
	bool m_rightMouseDown;
	
	/**
	 * \brief data structure with all editor components
	 */
	std::map<std::string, ContentEditorTab*> m_components;
};

#endif // ContentEditor_H
