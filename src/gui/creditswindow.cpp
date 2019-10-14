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

#include "creditswindow.h"
#include "ceguiutility.h"
#include "OgreConfigFile.h"
#include "OgreDataStream.h"
#include "sumwarshelper.h"

#include "config.h"

#include "creditswindow_content.inc"

CreditsWindow::CreditsWindow(Document* doc, const std::string& ceguiSkinName)
	: Window (doc)
	, m_ceguiSkinName (ceguiSkinName)
{
	CEGUI::FrameWindow* creditsframe = static_cast <CEGUI::FrameWindow*> (CEGUIUtility::loadLayoutFromFile ("creditsscreen.layout"));
	if (!creditsframe)
	{
		WARNING ("WARNING: Failed to load [%s]", "creditsscreen.layout");
	}
	CEGUI::Window* credits_frame_holder = CEGUIUtility::loadLayoutFromFile ("creditsscreen_holder.layout");
	if (!credits_frame_holder)
	{
		WARNING ("WARNING: Failed to load [%s]", "creditsscreen_holder.layout");
	}
	
	CEGUI::Window* wndHolder = CEGUIUtility::getWindowForLoadedLayoutEx (credits_frame_holder, "CreditsWindow_Holder");
	CEGUI::Window* wndCharInfo = CEGUIUtility::getWindowForLoadedLayoutEx (creditsframe, "CreditsWindow");
	CEGUIUtility::addChildWidget (wndHolder, wndCharInfo);
	m_window = credits_frame_holder;

	CEGUI::Window* credits = CEGUIUtility::getWindowForLoadedLayoutEx (creditsframe, "CreditsPane/CreditsText");

	Ogre::DataStreamPtr mem_stream(OGRE_NEW Ogre::MemoryDataStream((void*)authors_content.c_str(), authors_content.length(), false, true));
	mem_stream->seek(0);

	Ogre::ConfigFile cf;
	cf.load(mem_stream, "=", false);

	// Go through all sections & settings in the file
	Ogre::ConfigFile::SectionIterator seci = cf.getSectionIterator();

	std::list<std::string> content;
	CEGUI::String outputText;

	std::string secName, typeName, archName;
	while (seci.hasMoreElements())
	{
		secName = seci.peekNextKey();
		secName = secName.erase(0,2) + LINE_ENDING;
		content.push_back(CEGUIUtility::getColourizedString(CEGUIUtility::Red, secName, CEGUIUtility::White).c_str());
		Ogre::ConfigFile::SettingsMultiMap *settings = seci.getNext();
		Ogre::ConfigFile::SettingsMultiMap::iterator i;
		for (i = settings->begin(); i != settings->end(); ++i)
		{
			typeName = i->first;
			archName = i->second;
			content.push_back(archName + LINE_ENDING);
		}
		content.push_back(std::string(" ") + LINE_ENDING);

	}

	for (std::list<std::string>::iterator it = content.begin (); it != content.end (); ++it)
	{
		outputText.append ((CEGUI::utf8*)it->c_str ());
		outputText.append ((CEGUI::utf8*)LINE_ENDING);
	}
	credits->setText (outputText);
	credits->setFont("DejaVuSerif-12");
	
	updateTranslation();
	
	m_shown_timer.start();
}

void CreditsWindow::updateTranslation()
{
	// no translation for credits (at least not yet).
}

void CreditsWindow::update()
{
	if (!m_window->isVisible())
	{
		m_shown_timer.start();
	}
	else
	{
		float starttime = 2000;	// time before scrolling starts
		float alltime = 40000;	// time for scrolling
		float pos = (m_shown_timer.getTime()-starttime)/alltime;
		
		if (pos > 1.0)
			pos = 1.0;
		
		if (pos < 0.0)
			pos = 0.0;
		
		// credits scrolling
		CEGUI::ScrollablePane* pane  = static_cast<CEGUI::ScrollablePane*>(CEGUIUtility::getWindowForLoadedLayoutEx (m_window, "CreditsWindow/CreditsPane/CreditsText"));
		pane->setVerticalScrollPosition(pos);
	}
}

