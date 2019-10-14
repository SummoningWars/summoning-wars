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

#ifndef GAME_INFO_TAB_H
#define GAME_INFO_TAB_H

#include "contenteditortab.h"

class GameObject;

/**
 * \brief Game info class. Enables content creators to inspect whats going on on the screen.
 */
class GameInfoTab : public ContentEditorTab
{
public:
	/**
		 * \brief Initialises the panel
		 * \param parent parent CEGUI window
		 */
	virtual void init(CEGUI::Window* parent);

	/**
		 * \brief Updates the content of the tab
		 * Function is called once each tick.
		 */
	virtual void update();

	/**
	 * @brief sets the id of the clicked object
	 * @param id of the object
	 */
	virtual void setClickedObjectID(int object);

private:
	/**
	 * @brief Retrieves all object infos
	 */
	virtual void getObjectInfos();

	/**
	 * @brief Translates the layer id to a readable string
	 * @param A game object
	 * @return The string in readable form
	 */
	virtual CEGUI::String gameObjectLayerToString(GameObject *go);

	/**
	 * @brief Translates the state id to a readable string
	 * @param A game object
	 * @return The string in readable form
	 */
	virtual CEGUI::String gameObjectStateToString(GameObject *go);

	/**
	 * @brief Translates the base type to a readable string
	 * @param A game object
	 * @return The string in readable form
	 */
	virtual CEGUI::String gameObjectBaseTypeToString(GameObject *go);

	/**
	 * @brief The window where all informations are displayed
	 */
	CEGUI::MultiLineEditbox *m_ShowBox;

	/**
	 * @brief stores current tracked object id
	 */
	int m_ClickedObjectID;

	/**
	 * @brief holds the object info as string
	 */
	std::string m_object_info;

	/**
	 * @brief holds the world info as string
	 */
	std::string m_world_info;

    /**
     * \brief The root window to use when accessing the widgets.
     */
    CEGUI::Window* m_rootWindow;
};


#endif
