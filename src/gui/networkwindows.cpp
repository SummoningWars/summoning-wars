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

#include "networkwindows.h"

// Utility for CEGUI cross-version compatibility
#include "ceguiutility.h"

HostGameWindow::HostGameWindow (Document* doc)
	:Window(doc)
{
	SW_DEBUG ("HostGameWindow being created");
	CEGUI::PushButton* btn;
	CEGUI::Editbox* box;

	// The host game window and holder
	CEGUI::FrameWindow* host_game = (CEGUI::FrameWindow*) CEGUIUtility::loadLayoutFromFile ("hostgamewindow.layout");
	if (!host_game)
	{
		SW_DEBUG ("WARNING: Failed to load [%s]", "hostgamewindow.layout");
	}

	CEGUI::Window* host_game_holder = CEGUIUtility::loadLayoutFromFile ("hostgamewindow_holder.layout");
	if (!host_game_holder)
	{
		SW_DEBUG ("WARNING: Failed to load [%s]", "hostgamewindow_holder.layout");
	}
	
	CEGUI::Window* wndHolder = CEGUIUtility::getWindowForLoadedLayoutEx (host_game_holder, "HostGameWindow_Holder");
	CEGUI::Window* wndCharInfo = CEGUIUtility::getWindowForLoadedLayoutEx (host_game, "HostGameWindow");
	if (wndHolder && wndCharInfo)
	{
		CEGUIUtility::addChildWidget (wndHolder, wndCharInfo);
	}
	else
	{
		if (!wndHolder) SW_DEBUG ("ERROR: Unable to get the window holder for char screen.");
		if (!wndCharInfo) SW_DEBUG ("ERROR: Unable to get the window for char screen.");
	}

	m_window = host_game_holder;
	
	btn = static_cast<CEGUI::PushButton*>(CEGUIUtility::getWindowForLoadedLayoutEx (m_window, "HostGameWindow/HostGameStartButton"));
	btn->subscribeEvent(CEGUI::PushButton::EventClicked, CEGUI::Event::Subscriber(&HostGameWindow ::onStartHostGame, this));
		
	box = static_cast<CEGUI::Editbox*>(CEGUIUtility::getWindowForLoadedLayoutEx (m_window, "HostGameWindow/PlayerNumberBox"));
	box->setWantsMultiClickEvents(false);
	box->setMaxTextLength(31);
	
	box = static_cast<CEGUI::Editbox*>(CEGUIUtility::getWindowForLoadedLayoutEx (m_window, "HostGameWindow/HostPortBox"));
	box->setWantsMultiClickEvents(false);
	box->setMaxTextLength(31);
	
	// Connect the Cancel button to the cancel event.
	btn = static_cast<CEGUI::PushButton*>(CEGUIUtility::getWindowForLoadedLayoutEx (m_window, "HostGameWindow/HostGameCancelButton"));
	btn->setID(5);
	btn->subscribeEvent(CEGUI::PushButton::EventClicked, CEGUI::Event::Subscriber(&HostGameWindow::onCancelHostGame, this));

	// Connect the window close button to the cancel event.
	CEGUI::Window* autoCloseButton;
	autoCloseButton = CEGUIUtility::getWindowForLoadedLayoutEx (m_window, "HostGameWindow/__auto_closebutton__");
	if (autoCloseButton)
	{
		autoCloseButton->subscribeEvent (CEGUI::Window::EventMouseClick, CEGUI::Event::Subscriber (&HostGameWindow::onCancelHostGame, this));
	}

	updateTranslation();
}

void HostGameWindow::update()
{
	CEGUI::Editbox* box;
	
	Options* options = Options::getInstance();
	
	int port = options->getPort();
	int max_nr = options->getMaxNumberPlayers();
	
	std::ostringstream stream;
	stream.str("");
	stream << max_nr;
	
	box = static_cast<CEGUI::Editbox*>(CEGUIUtility::getWindowForLoadedLayoutEx (m_window, "HostGameWindow/PlayerNumberBox"));
	if (box->getText() != stream.str())
	{
		box->setText(stream.str());
	}
	
	stream.str("");
	stream << port;
	
	box = static_cast<CEGUI::Editbox*>(CEGUIUtility::getWindowForLoadedLayoutEx (m_window, "HostGameWindow/HostPortBox"));
	if (box->getText() != stream.str())
	{
		box->setText(stream.str());
	}

}

void HostGameWindow::updateTranslation()
{
	CEGUI::Window* label;
	
	CEGUI::PushButton* btn = static_cast<CEGUI::PushButton*>(CEGUIUtility::getWindowForLoadedLayoutEx (m_window, "HostGameWindow/HostGameStartButton"));
	btn->setText((CEGUI::utf8*) gettext("Ok"));

	btn = static_cast<CEGUI::PushButton*>(CEGUIUtility::getWindowForLoadedLayoutEx (m_window, "HostGameWindow/HostGameCancelButton"));
	btn->setText((CEGUI::utf8*) gettext("Cancel"));
	
	label = CEGUIUtility::getWindowForLoadedLayoutEx (m_window, "HostGameWindow");
	label->setText((CEGUI::utf8*) gettext("Host_game"));
	
	label = CEGUIUtility::getWindowForLoadedLayoutEx (m_window, "HostGameWindow/PlayerNumberLabel");
	label->setText((CEGUI::utf8*) gettext("Max. number of players"));
	
	label = CEGUIUtility::getWindowForLoadedLayoutEx (m_window, "HostGameWindow/HostPortLabel");
	label->setText((CEGUI::utf8*) gettext("Port"));
}


bool HostGameWindow::onStartHostGame(const CEGUI::EventArgs& evt)
{
	CEGUI::Editbox* box;
	
	Options* options = Options::getInstance();
	
	int port = options->getPort();
	int max_nr = options->getMaxNumberPlayers();
	
	std::stringstream stream,stream2;
	
	box = static_cast<CEGUI::Editbox*>(CEGUIUtility::getWindowForLoadedLayoutEx (m_window, "HostGameWindow/HostPortBox"));
	stream.str(box->getText().c_str());
	stream >> port;
	
	box = static_cast<CEGUI::Editbox*>(CEGUIUtility::getWindowForLoadedLayoutEx (m_window, "HostGameWindow/PlayerNumberBox"));
	stream2.str(box->getText().c_str());
	stream2 >> max_nr;
	
	options->setPort(port);
	options->setMaxNumberPlayers(max_nr);
	
	m_document->onButtonStartHostGame();
	return true;
}


bool HostGameWindow::onCancelHostGame (const CEGUI::EventArgs& evt)
{
	m_document->getGUIState()->m_shown_windows = Document::START_MENU;
	m_document->setModified(Document::WINDOWS_MODIFIED);

	return true;
}



JoinGameWindow::JoinGameWindow (Document* doc)
	:Window(doc)
{
	SW_DEBUG ("JoinGameWindow being created");
	CEGUI::PushButton* btn;
	CEGUI::Editbox* box;

	// The join game window and holder.
	CEGUI::FrameWindow* join_game = (CEGUI::FrameWindow*) CEGUIUtility::loadLayoutFromFile ("joingamewindow.layout");
	if (!join_game)
	{
		SW_DEBUG ("WARNING: Failed to load [%s]", "joingamewindow.layout");
	}

	CEGUI::Window* join_game_holder = CEGUIUtility::loadLayoutFromFile ("joingamewindow_holder.layout");
	if (!join_game_holder)
	{
		SW_DEBUG ("WARNING: Failed to load [%s]", "joingamewindow_holder.layout");
	}
	
	CEGUI::Window* wndHolder = CEGUIUtility::getWindowForLoadedLayoutEx (join_game_holder, "JoinGameWindow_Holder");
	CEGUI::Window* wndCharInfo = CEGUIUtility::getWindowForLoadedLayoutEx (join_game, "JoinGameWindow");
	if (wndHolder && wndCharInfo)
	{
		CEGUIUtility::addChildWidget (wndHolder, wndCharInfo);
	}
	else
	{
		if (!wndHolder)
    {
      SW_DEBUG ("ERROR: Unable to get the window holder for char screen.");
    }
		if (!wndCharInfo)
    {
      SW_DEBUG ("ERROR: Unable to get the window for char screen.");
    }
	}

	m_window = join_game_holder;
		
	btn = static_cast<CEGUI::PushButton*>(CEGUIUtility::getWindowForLoadedLayoutEx (m_window, "JoinGameWindow/JoinGameStartButton"));
	btn->subscribeEvent(CEGUI::PushButton::EventClicked, CEGUI::Event::Subscriber(&JoinGameWindow ::onStartJoinGame, this));
	
	box = static_cast<CEGUI::Editbox*>(CEGUIUtility::getWindowForLoadedLayoutEx (m_window, "JoinGameWindow/HostnameBox"));
	box->setWantsMultiClickEvents(false);
	box->setMaxTextLength(31);
	
	box = static_cast<CEGUI::Editbox*>(CEGUIUtility::getWindowForLoadedLayoutEx (m_window, "JoinGameWindow/PortBox"));
	box->setWantsMultiClickEvents(false);
	box->setMaxTextLength(31);

	// Connect the Cancel button to the cancel event.
	btn = static_cast<CEGUI::PushButton*>(CEGUIUtility::getWindowForLoadedLayoutEx (m_window, "JoinGameWindow/JoinGameCancelButton"));
	btn->setID(5);
	btn->subscribeEvent(CEGUI::PushButton::EventClicked, CEGUI::Event::Subscriber(&JoinGameWindow::onCancelJoinGame, this));

	// Connect the window close button to the cancel event.
	CEGUI::Window* autoCloseButton;
	autoCloseButton = CEGUIUtility::getWindowForLoadedLayoutEx (m_window, "JoinGameWindow/__auto_closebutton__");
	if (autoCloseButton)
	{
		autoCloseButton->subscribeEvent (CEGUI::Window::EventMouseClick, CEGUI::Event::Subscriber (&JoinGameWindow::onCancelJoinGame, this));
	}
	
	updateTranslation();
}

void JoinGameWindow::update()
{
	CEGUI::Editbox* box;
	
	Options* options = Options::getInstance();
	
	int port = options->getPort();
	std::ostringstream stream;
	std::string hostname = options->getServerHost();
	
	box = static_cast<CEGUI::Editbox*>(CEGUIUtility::getWindowForLoadedLayoutEx (m_window, "JoinGameWindow/HostnameBox"));
	if (box->getText() != hostname)
	{
		box->setText(hostname);
	}
	
	stream.str("");
	stream << port;
	
	box = static_cast<CEGUI::Editbox*>(CEGUIUtility::getWindowForLoadedLayoutEx (m_window, "JoinGameWindow/PortBox"));
	if (box->getText() != stream.str())
	{
		box->setText(stream.str());
	}

}

void JoinGameWindow::updateTranslation()
{
	CEGUI::Window* label;
	
	CEGUI::PushButton* btn = static_cast<CEGUI::PushButton*>(CEGUIUtility::getWindowForLoadedLayoutEx (m_window, "JoinGameWindow/JoinGameStartButton"));
	btn->setText((CEGUI::utf8*) gettext("Ok"));
	
	btn = static_cast<CEGUI::PushButton*>(CEGUIUtility::getWindowForLoadedLayoutEx (m_window, "JoinGameWindow/JoinGameCancelButton"));
	btn->setText((CEGUI::utf8*) gettext("Cancel"));

	label = CEGUIUtility::getWindowForLoadedLayoutEx (m_window, "JoinGameWindow");
	label->setText((CEGUI::utf8*) gettext("Join_game"));
	
	label = CEGUIUtility::getWindowForLoadedLayoutEx (m_window, "JoinGameWindow/HostnameLabel");
	label->setText((CEGUI::utf8*) gettext("Host"));
	
	label = CEGUIUtility::getWindowForLoadedLayoutEx (m_window, "JoinGameWindow/PortLabel");
	label->setText((CEGUI::utf8*) gettext("Port"));
}


bool JoinGameWindow::onStartJoinGame(const CEGUI::EventArgs& evt)
{
	CEGUI::Editbox* box;
	
	Options* options = Options::getInstance();
	
	int port = options->getPort();
	std::stringstream stream;
	std::string hostname = options->getServerHost();
	
	box = static_cast<CEGUI::Editbox*>(CEGUIUtility::getWindowForLoadedLayoutEx (m_window, "JoinGameWindow/HostnameBox"));
	hostname = box->getText().c_str();
	
	box = static_cast<CEGUI::Editbox*>(CEGUIUtility::getWindowForLoadedLayoutEx (m_window, "JoinGameWindow/PortBox"));
	stream.str(box->getText().c_str());
	stream >> port;
	
	SW_DEBUG("join game %i %s",port,hostname.c_str());
	options->setPort(port);
	options->setServerHost(hostname);
	
	m_document->onButtonStartJoinGame();
	return true;
}


bool JoinGameWindow::onCancelJoinGame (const CEGUI::EventArgs& evt)
{
	m_document->getGUIState()->m_shown_windows = Document::START_MENU;
    m_document->setState(Document::INACTIVE);
	m_document->setModified(Document::WINDOWS_MODIFIED);

	return true;
}

