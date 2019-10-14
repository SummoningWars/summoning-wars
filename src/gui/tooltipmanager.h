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

#ifndef __SUMWARS_GUI_TOOLTIPMANAGER_H__
#define __SUMWARS_GUI_TOOLTIPMANAGER_H__

#include <list>
#include <map>

#include "CEGUI/CEGUI.h"
#include "OgreSingleton.h"
#include "tooltip.h"


/**
 * \class TooltipManager
 * \brief Manages Tooltips 
 */
class TooltipManager : public Ogre::Singleton<TooltipManager>
{
public:
	/**
	 * \fn TooltipManager()
	 * \brief Konstruktor
	 */
	TooltipManager ( );
	
	/**
	 * \fn void createTooltip(CEGUI::Window *win, std::list<std::string> list, float timeVisible = 0.0f, const CEGUI::Font *font = 0, Tooltip::TooltipType type = Tooltip::Main)
	 * \brief Creates a Tooltip
	 * \param win parent CEGUI window 
	 * \param list Text contents for the window as std::list
	 * \param timeVisible Time before the tooltip fades
	 * \param font CEGUI font for the Tooltip
	 * \param type the Tooltip type
	 */
	void createTooltip (CEGUI::Window *win, std::list<std::string> list, float timeVisible = 0.0f, const CEGUI::Font *font = 0, Tooltip::TooltipType type = Tooltip::Main);
	
	/**
	 * \fn void createTooltip(CEGUI::Window *win, std::string text, float timeVisible = 0.0f, CEGUI::Font *font = 0, Tooltip::TooltipType type = Tooltip::Main)
	 * \brief Creates a Tooltip
	 * \param win parent CEGUI window 
	 * \param text Text contents for the window as std::list
	 * \param timeVisible Time before the tooltip fades
	 * \param font CEGUI font for the Tooltip
	 * \param type the Tooltip type
	 */
	void createTooltip (CEGUI::Window *win, std::string text, float timeVisible = 0.0f, CEGUI::Font *font = 0, Tooltip::TooltipType type = Tooltip::Main);
	
	/**
	 * \fn void fadeAllOut()
	 * \brief Fades all Tooltips out
	 */
	void fadeAllOut();
	
	/**
	 * \fn void handleMouseLeave()
	 * \brief Handles all Mouse Leave events
	 * \param e CEGUI MouseLeave event
	 */
	bool handleMouseLeave( const CEGUI::EventArgs &e );
	
	/**
	 * \fn void setVisibleTime ( float timeVisible )
	 * \brief Sets visible time for all Tooltips
	 * \param timeVisible Time visible in milliseconds
	 */
	void setVisibleTime ( float timeVisible );
	float getVisibleTime();

	/**
	 * \fn void setVisibleTime ( float timeToFadeIn )
	 * \brief Sets fade out time
	 * \param timeToFadeIn Time to fade in in milliseconds
	 */
	void setFadeInTime ( float timeToFadeIn );
	float getFadeInTime();

	/**
	 * \fn void handleMouseLeave()
	 * \brief Handles all Mouse Leave events
	 * \param e CEGUI MouseLeave event
	 */
	void setFadeOutTime ( float timeToFadeOut );
	float getFadeOutTime();

	/**
	 * \fn void update ( float timeSinceLastUpdate )
	 * \brief Updates all tooltips
	 * \param timeSinceLastUpdate Time since last frame im milliseconds
	 */
	void update(float timeSinceLastUpdate);

	static TooltipManager& getSingleton(void);
	static TooltipManager* getSingletonPtr(void);
	
private:
	
	/**
	 * \var CEGUI::WindowManager *m_CEGUIWinMgr
	 * \brief Pointer to CEGUI::WindowManager 
	 */
	CEGUI::WindowManager *m_CEGUIWinMgr;
	
	/**
	 * \var CEGUI::System *m_CEGUISystem
	 * \brief Pointer to CEGUI::System 
	 */
	CEGUI::System *m_CEGUISystem;
	
	/**
	 * \var Tooltip *m_CurrentMain
	 * \brief Current Main Tooltip
	 */
	Tooltip *m_CurrentMain;
	
	/**
	 * \var std::map<std::string, Tooltip*> m_Tooltips
	 * \brief List of all current Tooltips
	 */
	std::map<std::string, Tooltip*> m_Tooltips;
	
	/**
	 * \var float m_timeVisible
	 * \brief Visible time for all Tooltips
	 */
	float m_timeVisible;
	
	/**
	 * \var float m_fadeInTime
	 * \brief Fade in time for all Tooltips
	 */
	float m_fadeInTime;
	
	/**
	 * \var float m_fadeOutTime
	 * \brief Fade out time for all Tooltips
	 */
	float m_fadeOutTime;
	
	/**
	 * \var int m_toolTipsCreatedCount
	 * \brief Counts how many Tooltips have been created
	 */
	int m_toolTipsCreatedCount;
	
	/**
	 * \var CEGUI::Font* m_DefaultFont
	 * \brief The default font
	 */
    CEGUI::Font* m_DefaultFont;
};

#endif // __SUMWARS_GUI_TOOLTIPMANAGER_H__

