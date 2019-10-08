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

#include "tooltipmanager.h"
#include "ceguiutility.h"

template<> TooltipManager* Ogre::Singleton<TooltipManager>::ms_Singleton = 0;

TooltipManager::TooltipManager ()
{
    m_toolTipsCreatedCount = 0;
    m_CEGUIWinMgr = CEGUI::WindowManager::getSingletonPtr();
    m_CEGUISystem = CEGUI::System::getSingletonPtr();
    m_DefaultFont = m_CEGUISystem->getDefaultFont();
}

void TooltipManager::createTooltip ( CEGUI::Window* win, std::list< std::string > list, float timeVisible, CEGUI::Font* font, Tooltip::TooltipType type )
{
	CEGUI::Window *gamescreen = CEGUI::WindowManager::getSingleton().getWindow("GameScreen");
    std::string msg;
    CEGUI::UVector2 size;
    std::ostringstream windowName;
    windowName << "Tooltip__" << m_toolTipsCreatedCount;
    CEGUI::Font *tempFont = m_DefaultFont;

    if ( font )
    {
        size = CEGUIUtility::getWindowSizeForText ( list, font, msg );
        tempFont = font;
    }
    else
        size = CEGUIUtility::getWindowSizeForText ( list, m_DefaultFont, msg );

    CEGUI::Vector2 mousePos = CEGUI::MouseCursor::getSingleton().getPosition();
    CEGUI::Renderer *rend = m_CEGUISystem->getRenderer();
    CEGUI::UVector2 position = CEGUI::UVector2 ( CEGUI::UDim ( 0, 20 ), CEGUI::UDim ( 0, 20 ) );

    if ( mousePos.d_y < ( rend->getDisplaySize().d_height / 2 ) )
        position = CEGUI::UVector2 ( CEGUI::UDim ( 0, 20 ), CEGUI::UDim ( 0, rend->getDisplaySize().d_height-110-size.d_y.d_scale*rend->getDisplaySize().d_height ) );

    if ( type == Tooltip::Main )
    {
		Tooltip *tt = new Tooltip ( gamescreen, windowName.str(), m_fadeInTime, m_fadeOutTime, m_timeVisible, 0.7f );
        fadeAllOut();
        m_CurrentMain = tt;
        tt->create ( msg, position, size, tempFont );
        win->subscribeEvent ( CEGUI::Window::EventMouseLeaves, CEGUI::Event::Subscriber ( &TooltipManager::handleMouseLeave, this ) );
        m_Tooltips[windowName.str() ] = tt;
        m_toolTipsCreatedCount++;
    }
    else
    {
        Tooltip *temp = m_CurrentMain->getChild();
        if ( temp )
        {
            while ( temp )
            {
                if ( !temp->getChild() )
                {
					Tooltip *tt = new Tooltip ( gamescreen, windowName.str(), m_fadeInTime, m_fadeOutTime, m_timeVisible, 0.7f );
                    CEGUI::UVector2 pos = temp->getCEGUIWindow()->getPosition();
                    pos.d_x += m_CurrentMain->getCEGUIWindow()->getWidth();
                    tt->create ( msg, pos, size, tempFont );
                    temp->setChild ( tt );
                    m_Tooltips[windowName.str() ] = tt;
                    m_toolTipsCreatedCount++;
                    break;

                }
                temp = temp->getChild();

            }
        }
        else
        {
			Tooltip *tt = new Tooltip ( gamescreen, windowName.str(), m_fadeInTime, m_fadeOutTime, m_timeVisible, 0.7f );
            CEGUI::UVector2 pos = m_CurrentMain->getCEGUIWindow()->getPosition();
            pos.d_x += m_CurrentMain->getCEGUIWindow()->getWidth();
            tt->create ( msg, pos, size, tempFont );
            m_CurrentMain->setChild ( tt );
            m_Tooltips[windowName.str() ] = tt;
            m_toolTipsCreatedCount++;
        }
    }
}

void TooltipManager::createTooltip(CEGUI::Window* win, std::string text, float timeVisible, CEGUI::Font* font, Tooltip::TooltipType type)
{
	createTooltip(win, CEGUIUtility::getTextAsList(text), timeVisible, font, type);
}


void TooltipManager::fadeAllOut()
{
    std::map<std::string, Tooltip*>::iterator iter;
    for ( iter = m_Tooltips.begin(); iter != m_Tooltips.end(); iter++ )
    {
        Tooltip *tt = iter->second;

        if ( !tt->isDead() )
            tt->fadeOut();
    }
}

bool TooltipManager::handleMouseLeave ( const CEGUI::EventArgs& e )
{
    fadeAllOut();
    return true;
}


void TooltipManager::setVisibleTime ( float timeVisible )
{
    m_timeVisible = timeVisible;
}

float TooltipManager::getVisibleTime()
{
    return m_timeVisible;
}

void TooltipManager::setFadeInTime ( float timeToFadeIn )
{
    m_fadeInTime = timeToFadeIn;
}

float TooltipManager::getFadeInTime()
{
    return m_fadeInTime;
}

void TooltipManager::setFadeOutTime ( float timeToFadeOut )
{
    m_fadeOutTime = timeToFadeOut;
}

float TooltipManager::getFadeOutTime()
{
    return m_fadeOutTime;
}

void TooltipManager::update ( float timeSinceLastUpdate )
{
    std::map<std::string, Tooltip*>::iterator iter;
    for ( iter = m_Tooltips.begin(); iter != m_Tooltips.end();)
    {
        Tooltip *tt = iter->second;

        if ( tt->isDead() )
        {
            CEGUI::WindowManager::getSingleton().destroyWindow ( tt->getName() );
            m_Tooltips.erase ( iter++ );
            delete tt;
        }
        else
		{
            tt->update ( timeSinceLastUpdate );
			++iter;
		}
    }
}

TooltipManager* TooltipManager::getSingletonPtr ( void )
{
    return ms_Singleton;
}

TooltipManager& TooltipManager::getSingleton ( void )
{
    assert ( ms_Singleton );
    return ( *ms_Singleton );
}
