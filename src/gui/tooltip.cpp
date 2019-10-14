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

#include "tooltip.h"
#include <iostream>

// Utility for CEGUI cross-version compatibility
#include "ceguiutility.h"

Tooltip::Tooltip ( CEGUI::Window* parent, std::string name, float fadeInTime, float fadeOutTime, float visibilityTime, float targetAlpha, TooltipType type)
{
    m_FadeInTime = fadeInTime;
    m_FadeOutTime = fadeOutTime;
    m_VisibilityTime = visibilityTime;
    m_Parent = parent;
    m_Name = name;
    m_FadingIn = true;
    m_FadingOut = false;
    m_Visible = false;
    m_Type = type;
    m_Child = NULL;

    m_CurrentFadeInTime = 0;
    m_CurrentVisibleTime = 0;
    m_CurrentFadeOutTime = 0;
    m_IsDead = false;

    if (targetAlpha == 0)
        m_Alpha = parent->getAlpha();
    else
        m_Alpha = targetAlpha;

    if (m_FadeInTime > 0.0f)
        m_FadeInStepPerMS = m_Alpha / m_FadeInTime;
    else
        m_FadeInStepPerMS = 0.0f;

    if (m_FadeOutTime > 0.0f)
        m_FadeOutStepPerMS = m_Alpha / m_FadeOutTime;
    else
        m_FadeOutStepPerMS = 0.0f;

}

void Tooltip::create(std::string msg, CEGUI::UVector2 position, CEGUI::UVector2 size, CEGUI::Font *font)
{
    using namespace CEGUI;
    FrameWindow* tt = static_cast<FrameWindow*>(WindowManager::getSingletonPtr()->createWindow( (CEGUI::utf8*)"SumwarsTooltip", (CEGUI::utf8*)m_Name.c_str() ));
    m_CEGUIWindow = tt;

    tt->setText((CEGUI::utf8*)msg.c_str());

    tt->setPosition( position );

	CEGUIUtility::setWidgetSize (tt, size);
    tt->setAlpha(0.0f);
    assert(m_Parent);
	CEGUIUtility::addChildWidget (m_Parent, tt);
}


void Tooltip::fadeOut()
{	
    m_Visible = false;
	m_FadingIn = false;
	m_FadingOut = true;
}


void Tooltip::update ( float timeSinceLastUpdate )
{
    if (m_FadingIn)
    {
        m_CurrentFadeInTime += timeSinceLastUpdate;

        float steps = timeSinceLastUpdate * m_FadeInStepPerMS;
        float newAlpha = m_CEGUIWindow->getAlpha() + steps;
        m_CEGUIWindow->setAlpha(newAlpha);

        if (m_CurrentFadeInTime > m_FadeInTime)
        {
            m_FadingIn = false;
            m_Visible = true;
        }

    }
    else if (m_Visible && m_VisibilityTime != 0)
    {
        m_CurrentVisibleTime += timeSinceLastUpdate;

        if (m_CurrentVisibleTime > m_VisibilityTime)
        {
            m_Visible = false;
            m_FadingOut = true;
        }
    }
    else if (m_FadingOut)
    {
        m_CurrentFadeOutTime += timeSinceLastUpdate;
        

        float steps = timeSinceLastUpdate * m_FadeOutStepPerMS;
        float newAlpha = m_CEGUIWindow->getAlpha() - steps;
		m_CEGUIWindow->setAlpha(newAlpha);
        if (m_FadeOutTime < m_CurrentFadeOutTime)
        {
			m_IsDead = true;
			CEGUI::Window* wndTemp = CEGUIUtility::getWindow (m_Name);
			CEGUIUtility::removeChildWidget (m_Parent, wndTemp);
            CEGUI::WindowManager::getSingleton().destroyWindow (wndTemp);
        }
    }
}
