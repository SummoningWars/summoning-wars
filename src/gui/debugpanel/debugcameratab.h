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

#ifndef DEBUGCAMERA_H
#define DEBUGCAMERA_H

#include "debugtab.h"

#ifdef CEGUI_07
#include <CEGUI/CEGUIWindow.h>
#else
#include "CEGUI/Window.h"
#endif

#include "OgreSceneNode.h"

class DebugCameraTab : public CEGUI::Window, public DebugTab
{

public:
    DebugCameraTab(const CEGUI::String& type, const CEGUI::String& name);
	
	virtual void update(OIS::Keyboard *keyboard, OIS::Mouse *mouse);
	
	virtual void initialiseComponents(void );

	/**
	 * \param evt CEGUI event arguments
	 * \brief Called upon clicking a mouse button down to the preview window
	 */
	bool onDebugWindowMouseDown(const CEGUI::EventArgs& evt);
	
	/**
	 * \param evt CEGUI event arguments
	 * \brief Called upon releasing a mouse button to the preview window
	 */
	bool onDebugWindowMouseUp(const CEGUI::EventArgs& evt);
	
protected:
	bool onPlayerCameraModeChanged(const CEGUI::EventArgs& evt);
	
private:
	CEGUI::Window *m_tabLayout;
	
	/**
	 * \brief Sets the debug camera and the Rendertarget up
	 */
	void setupCamera();
	
	/**
	 * \brief While true, the user rotates the camera in the preview window
	 */
	bool m_leftMouseDown;
	
	/**
	 * \brief While true, the user zooms the camera in the preview window
	 */
	bool m_rightMouseDown;
	
public:
	static CEGUI::String WidgetTypeName;
};

#endif // GUIDEBUGTAB_H
