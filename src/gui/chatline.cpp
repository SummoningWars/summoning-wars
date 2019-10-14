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

#include "chatline.h"
#include "ceguiutility.h"

ChatLine::ChatLine (Document* doc, const std::string& ceguiSkinName)
	: Window(doc)
	, m_ceguiSkinName (ceguiSkinName)
{
	m_history_line = 0;
	
	CEGUI::WindowManager& win_mgr = CEGUI::WindowManager::getSingleton();
	
	// Rahmen fuer CharInfo Fenster
	
	CEGUI::Editbox* chatline;
	
	chatline = static_cast<CEGUI::Editbox*>(win_mgr.createWindow (CEGUIUtility::getWidgetWithSkin (m_ceguiSkinName, "Editbox"), "Chatline"));
 	chatline->setPosition(CEGUI::UVector2(cegui_reldim(0.07f), cegui_reldim( 0.83f)));
	CEGUIUtility::setWidgetSizeRel (chatline, 0.43f, 0.05f);

	chatline->setWantsMultiClickEvents(false);
	chatline->subscribeEvent(CEGUI::Editbox::EventTextAccepted, CEGUI::Event::Subscriber(&ChatLine::onSendMessage,  this));
	chatline->subscribeEvent(CEGUI::Window::EventKeyDown, CEGUI::Event::Subscriber(&ChatLine::onKeyPress,  this));
	
	chatline->setText("");
	chatline->setAlwaysOnTop(true);
	if (chatline->isPropertyPresent ("BackgroundColour"))
	{
		chatline->setProperty("BackgroundColour", "B2000000"); 
	}

	m_window = chatline;
	
	updateTranslation();
	
}

void ChatLine::update()
{
	/*
	CEGUI::WindowManager& win_mgr = CEGUI::WindowManager::getSingleton();
	Player* pl = m_document->getLocalPlayer();
	
	CEGUI::MultiLineEditbox* chat_content;
	chat_content =  static_cast<CEGUI::MultiLineEditbox*>(win_mgr.getWindow("ChatContent"));
	
	if (chat_content->getText() != (CEGUI::utf8*) pl->getMessages().c_str())
	{
		chat_content->setText((CEGUI::utf8*) pl->getMessages().c_str());
		CEGUI::Scrollbar* sb = chat_content->getVertScrollbar();
		if (sb !=0)
		{
			sb->setScrollPosition(sb->getDocumentSize());
		}
	}
	*/
}

void ChatLine::updateTranslation()
{

}

void ChatLine::setHistoryLine()
{
	CEGUI::Editbox* line = static_cast<CEGUI::Editbox*>(m_window);
	
	if (m_history_line == 0)
	{
		line->setText("");
	}
	else
	{
		// Zeile aus der History suchen
		std::list<std::string>::iterator it;
		it = m_history.begin();
		for (int i=1; i<m_history_line && it != m_history.end(); ++i)
		{
			++it;
		}
		
		if (it != m_history.end())
		{
			line->setText((CEGUI::utf8*) it->c_str());
		}
		else
		{
			line->setText((CEGUI::utf8*) m_history.back().c_str());
		}
	}
}

bool ChatLine::onSendMessage(const CEGUI::EventArgs& evt)
{
	const CEGUI::MouseEventArgs& we =
			static_cast<const CEGUI::MouseEventArgs&>(evt);
	
	CEGUI::Editbox* line = static_cast<CEGUI::Editbox*>(we.window);
	
	m_document->sendChatMessage(line->getText().c_str());
	
	// einfuegen in History
	m_history.push_front(line->getText().c_str());
	if (m_history.size() >10)
	{
		m_history.pop_back();
	}
	m_history_line = 0;
	
	line->setText("");
	
	return true;
}

bool ChatLine::onKeyPress(const CEGUI::EventArgs& evt)
{
	const CEGUI::KeyEventArgs &kevt = static_cast<const CEGUI::KeyEventArgs&>(evt);
	
	if (kevt.scancode == CEGUI::Key::ArrowUp)
	{
		if (m_history_line<(int) m_history.size())
		{
			m_history_line++;
		}
		setHistoryLine();
		return true;
	}
	else if (kevt.scancode == CEGUI::Key::ArrowDown)
	{
		
		if (m_history_line>0)
		{
			m_history_line--;
		}
		setHistoryLine();
		return true;
	}
	
	return false;
}
