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

#include "tradewindow.h"

// Utility for CEGUI cross-version compatibility
#include "ceguiutility.h"


TradeWindow::TradeWindow (Document* doc, const std::string& ceguiSkinName)
	: ItemWindow(doc)
	, m_ceguiSkinName (ceguiSkinName)
{
	DEBUGX("setup trade");

	m_big_sheet =0;
	m_medium_sheet =0;
	m_small_sheet =0;
	
	
	CEGUI::WindowManager& win_mgr = CEGUI::WindowManager::getSingleton();

	// Rahmen fuer Inventar Fenster
	CEGUI::FrameWindow* trade = (CEGUI::FrameWindow*) CEGUIUtility::loadLayoutFromFile ("tradewindow.layout");
	if (!trade)
	{
		SW_DEBUG ("WARNING: Failed to load [%s]", "tradewindow.layout");
	}
	CEGUI::Window* trade_holder = CEGUIUtility::loadLayoutFromFile ("tradewindow_holder.layout");
	if (!trade_holder)
	{
		SW_DEBUG ("WARNING: Failed to load [%s]", "tradewindow_holder.layout");
	}

	CEGUI::Window* wndHolder = CEGUIUtility::getWindowForLoadedLayoutEx (trade_holder, "TradeWindow_Holder");
	CEGUI::Window* wndTrade = CEGUIUtility::getWindowForLoadedLayoutEx (trade, "TradeWindow");
	if (wndHolder && wndTrade)
	{
		CEGUIUtility::addChildWidget (wndHolder, wndTrade);
	}
	else
	{
		if (!wndHolder) SW_DEBUG ("ERROR: Unable to get the window holder for trade.");
		if (!wndTrade) SW_DEBUG ("ERROR: Unable to get the window for trade.");
	}

	m_window = trade_holder;

	CEGUI::Window* tradeContent = 0;
	tradeContent = CEGUIUtility::getWindowForLoadedLayoutEx (m_window, "TradeWindow/tradewindow_aux");
	
	trade->subscribeEvent(CEGUI::Window::EventMouseButtonDown, CEGUI::Event::Subscriber(&TradeWindow::onTradeAreaMouseButtonPressed, this));
	trade->setWantsMultiClickEvents(false);

	// Bestandteile des Charakterfensters hinzufuegen
	CEGUI::PushButton* btn;
	CEGUI::Window* label;

	int i,j;

	// Label fuer grosse Items
	std::ostringstream outStream;

	// Store the WIDTH specific dividers for the various items.
	double ratioOfInventory = 0.95;
	double dividerForBigItems = ratioOfInventory / 5;
	double dividerForMedItems = ratioOfInventory / 7;
	double dividerForSmallItems = ratioOfInventory / 10;
	
	for (j=0;j<3;j++)
	{
		for (i=0;i<5;i++)
		{
			outStream.str("");
			outStream << "TraderBigItem" << j*5+i<< "Label";
			label = win_mgr.createWindow (CEGUIUtility::getWidgetWithSkin (m_ceguiSkinName, "BackgroundButton"), outStream.str());
			CEGUIUtility::addChildWidget (tradeContent, label);
			label->setPosition (CEGUI::UVector2 (cegui_reldim (i * dividerForBigItems), cegui_reldim (j * 0.13f)));
			CEGUIUtility::setWidgetSizeRel (label, dividerForBigItems, 0.13f);

			label->setID(Equipement::BIG_ITEMS+j*5+i);
			label->subscribeEvent(CEGUI::Window::EventMouseButtonDown, CEGUI::Event::Subscriber(&TradeWindow::onTradeItemMouseButtonPressed, this));
			label->subscribeEvent(CEGUI::Window::EventMouseButtonUp, CEGUI::Event::Subscriber(&TradeWindow::onTradeItemMouseButtonReleased,  this));
			label->subscribeEvent(CEGUIUtility::EventMouseEntersWindowArea (), CEGUI::Event::Subscriber(&TradeWindow::onTradeItemHover,  this));
			label->setWantsMultiClickEvents(false);
		}
	}
	
	btn = static_cast<CEGUI::PushButton*>(CEGUIUtility::getWindowForLoadedLayoutEx (m_window, "TradeWindow/tradewindow_aux/TradeBigPrevButton"));
	btn->setID(0);
	btn->subscribeEvent(CEGUI::PushButton::EventClicked, CEGUI::Event::Subscriber(&TradeWindow::onTradeNextItems, this));
	
	btn = static_cast<CEGUI::PushButton*>(CEGUIUtility::getWindowForLoadedLayoutEx (m_window, "TradeWindow/tradewindow_aux/TradeBigNextButton"));
	btn->setID(1);
	btn->subscribeEvent(CEGUI::PushButton::EventClicked, CEGUI::Event::Subscriber(&TradeWindow::onTradeNextItems, this));

	
	for (j=0;j<3;j++)
	{
		for (i=0;i<7;i++)
		{
			outStream.str("");
			outStream << "TraderMediumItem" << j*7+i<< "Label";
			label = win_mgr.createWindow (CEGUIUtility::getWidgetWithSkin (m_ceguiSkinName, "BackgroundButton"), outStream.str());
			CEGUIUtility::addChildWidget (tradeContent, label);
			label->setPosition(CEGUI::UVector2 (cegui_reldim (i * dividerForMedItems), cegui_reldim (0.39f + 0.096*j)));
			CEGUIUtility::setWidgetSizeRel (label, dividerForMedItems, 0.096f);
			label->setID(Equipement::MEDIUM_ITEMS+j*7+i);
			label->subscribeEvent(CEGUI::Window::EventMouseButtonDown, CEGUI::Event::Subscriber(&TradeWindow::onTradeItemMouseButtonPressed, this));
			label->subscribeEvent(CEGUI::Window::EventMouseButtonUp, CEGUI::Event::Subscriber(&TradeWindow::onTradeItemMouseButtonReleased,  this));
			label->subscribeEvent(CEGUIUtility::EventMouseEntersWindowArea (), CEGUI::Event::Subscriber(&TradeWindow::onTradeItemHover,  this));
			label->setWantsMultiClickEvents(false);
		}
	}
	
	btn = static_cast<CEGUI::PushButton*>(CEGUIUtility::getWindowForLoadedLayoutEx (m_window, "TradeWindow/tradewindow_aux/TradeMediumPrevButton"));
	btn->setID(2);
	btn->subscribeEvent(CEGUI::PushButton::EventClicked, CEGUI::Event::Subscriber(&TradeWindow::onTradeNextItems, this));
	
	btn = static_cast<CEGUI::PushButton*>(CEGUIUtility::getWindowForLoadedLayoutEx (m_window, "TradeWindow/tradewindow_aux/TradeMediumNextButton"));
	btn->setID(3);
	btn->subscribeEvent(CEGUI::PushButton::EventClicked, CEGUI::Event::Subscriber(&TradeWindow::onTradeNextItems, this));
	
	for (j=0;j<3;j++)
	{
		for (i=0;i<10;i++)
		{
			outStream.str("");
			outStream << "TraderSmallItem" << j*10+i<< "Label";
			label = win_mgr.createWindow (CEGUIUtility::getWidgetWithSkin (m_ceguiSkinName, "BackgroundButton"), outStream.str());
			CEGUIUtility::addChildWidget (tradeContent, label);
			label->setPosition (CEGUI::UVector2 (cegui_reldim(i * dividerForSmallItems), cegui_reldim (0.68f + 0.075 * j)));
			CEGUIUtility::setWidgetSizeRel (label, dividerForSmallItems, 0.065f);
			label->setID(Equipement::SMALL_ITEMS+j*10+i);
			label->subscribeEvent(CEGUI::Window::EventMouseButtonDown, CEGUI::Event::Subscriber(&TradeWindow::onTradeItemMouseButtonPressed, this));
			label->subscribeEvent(CEGUI::Window::EventMouseButtonUp, CEGUI::Event::Subscriber(&TradeWindow::onTradeItemMouseButtonReleased,  this));
			label->subscribeEvent(CEGUIUtility::EventMouseEntersWindowArea (), CEGUI::Event::Subscriber(&TradeWindow::onTradeItemHover,  this));
			label->setWantsMultiClickEvents(false);
		}
	}
	
	btn = static_cast<CEGUI::PushButton*>(CEGUIUtility::getWindowForLoadedLayoutEx (m_window, "TradeWindow/tradewindow_aux/TradeSmallPrevButton"));
	btn->setID(4);
	btn->subscribeEvent(CEGUI::PushButton::EventClicked, CEGUI::Event::Subscriber(&TradeWindow::onTradeNextItems, this));
	
	btn = static_cast<CEGUI::PushButton*>(CEGUIUtility::getWindowForLoadedLayoutEx (m_window, "TradeWindow/tradewindow_aux/TradeSmallNextButton"));
	btn->setID(5);
	btn->subscribeEvent(CEGUI::PushButton::EventClicked, CEGUI::Event::Subscriber(&TradeWindow::onTradeNextItems, this));
	
	
	// Both a regular button and an auto-close button are available.
	btn = static_cast<CEGUI::PushButton*>(CEGUIUtility::getWindowForLoadedLayoutEx (m_window, "TradeWindow/tradewindow_aux/TradeCloseButton"));
	btn->setID(5);
	btn->subscribeEvent(CEGUI::PushButton::EventClicked, CEGUI::Event::Subscriber(&TradeWindow::onCloseTrade, this));

	CEGUI::Window* autoCloseButton;
	autoCloseButton = CEGUIUtility::getWindowForLoadedLayoutEx (m_window, "TradeWindow/__auto_closebutton__");
	if (autoCloseButton)
	{
		autoCloseButton->subscribeEvent (CEGUI::Window::EventMouseClick, CEGUI::Event::Subscriber (&TradeWindow::onCloseTrade, this));
	}


	updateTranslation();
}

void TradeWindow::update()
{
	m_silent_current_update = m_silent;
	
	Player* player = m_document->getLocalPlayer();
	if (player ==0)
		return;
	
	Creature* npc = player->getTradePartner();
	
	if (npc ==0)
	{
		return;
	}
		
	Equipement* equ = npc->getEquipement();
	if (equ ==0)
	{
		return;
	}
	
	//int gold = player->getEquipement()->getGold();
	//bool stashtrade = false;
	if (player->getId() == npc->getId())
	{
		equ = player->getStash();
		//stashtrade = true;
		//gold = -1;
	}
	
	std::ostringstream out_stream;
	
	int nbig = equ->getMaxItemNumber(Item::BIG);
	int nmedium = equ->getMaxItemNumber(Item::MEDIUM);
	int nsmall = equ->getMaxItemNumber(Item::SMALL);
	
	CEGUI::PushButton* btn;
	bool vis;
	
	btn = static_cast<CEGUI::PushButton*>(CEGUIUtility::getWindowForLoadedLayoutEx (m_window, "TradeWindow/tradewindow_aux/TradeBigPrevButton"));
	vis = (m_big_sheet >0);
	if (btn->isVisible() != vis)
		btn->setVisible(vis);
	
	btn = static_cast<CEGUI::PushButton*>(CEGUIUtility::getWindowForLoadedLayoutEx (m_window, "TradeWindow/tradewindow_aux/TradeBigNextButton"));
	vis = (m_big_sheet < nbig/15);
	if (btn->isVisible() != vis)
		btn->setVisible(vis);
	
	btn = static_cast<CEGUI::PushButton*>(CEGUIUtility::getWindowForLoadedLayoutEx (m_window, "TradeWindow/tradewindow_aux/TradeMediumPrevButton"));
	vis = (m_medium_sheet >0);
	if (btn->isVisible() != vis)
		btn->setVisible(vis);
	
	btn = static_cast<CEGUI::PushButton*>(CEGUIUtility::getWindowForLoadedLayoutEx (m_window, "TradeWindow/tradewindow_aux/TradeMediumNextButton"));
	vis = (m_medium_sheet < nmedium/21);
	if (btn->isVisible() != vis)
		btn->setVisible(vis);
	
	btn = static_cast<CEGUI::PushButton*>(CEGUIUtility::getWindowForLoadedLayoutEx (m_window, "TradeWindow/tradewindow_aux/TradeSmallPrevButton"));
	vis = (m_small_sheet >0);
	if (btn->isVisible() != vis)
		btn->setVisible(vis);
	
	btn = static_cast<CEGUI::PushButton*>(CEGUIUtility::getWindowForLoadedLayoutEx (m_window, "TradeWindow/tradewindow_aux/TradeSmallNextButton"));
	vis = (m_small_sheet < nsmall/30);
	if (btn->isVisible() != vis)
		btn->setVisible(vis);
	
	CEGUI::Window* img;
	int i;
	Item* it;
	
	// Inventar: grosse Items
	for (i=0;i<15;i++)
	{
		out_stream.str("");
		out_stream << "TradeWindow/tradewindow_aux/TraderBigItem" << i<< "Label";
		img =  CEGUIUtility::getWindowForLoadedLayoutEx (m_window, out_stream.str().c_str());
		it = equ->getItem(Equipement::BIG_ITEMS+i + m_big_sheet*15);
		updateItemWindow(img,it,player,player->getEquipement()->getGold());
	}
	
	for (i=0;i<21;i++)
	{
		out_stream.str("");
		out_stream << "TradeWindow/tradewindow_aux/TraderMediumItem" << i<< "Label";
		img =  CEGUIUtility::getWindowForLoadedLayoutEx (m_window, out_stream.str().c_str());
		it = equ->getItem(Equipement::MEDIUM_ITEMS+i + m_medium_sheet*21);
		updateItemWindow(img,it,player,player->getEquipement()->getGold());
	}
	
	for (i=0;i<30;i++)
	{
		out_stream.str("");
		out_stream << "TradeWindow/tradewindow_aux/TraderSmallItem" << i<< "Label";
		img =  CEGUIUtility::getWindowForLoadedLayoutEx (m_window, out_stream.str().c_str());
		it = equ->getItem(Equipement::SMALL_ITEMS+i + m_small_sheet*30);
		updateItemWindow(img,it,player,player->getEquipement()->getGold());
	}
}

void TradeWindow::updateTranslation()
{
	CEGUI::PushButton* btn;
	CEGUI::Window* label;
	
	label =  CEGUIUtility::getWindowForLoadedLayoutEx (m_window, "TradeWindow");
	if (label->isPropertyPresent ("Text"))
	{
		label->setProperty ("Text", (CEGUI::utf8*) gettext("Chest"));
	}

	btn = static_cast<CEGUI::PushButton*>(CEGUIUtility::getWindowForLoadedLayoutEx (m_window, "TradeWindow/tradewindow_aux/TradeCloseButton"));
	btn->setText((CEGUI::utf8*) gettext("Ok"));
}


void TradeWindow::reset()
{
	m_big_sheet =0;
	m_medium_sheet =0;
	m_small_sheet =0;
}

bool TradeWindow::onTradeItemHover(const CEGUI::EventArgs& evt)
{
	
	const CEGUI::MouseEventArgs& we =
			static_cast<const CEGUI::MouseEventArgs&>(evt);
	unsigned int id = we.window->getID();
	
	if (id >=Equipement::BIG_ITEMS && id < Equipement::MEDIUM_ITEMS)
		id += 15* m_big_sheet;
	if (id >=Equipement::MEDIUM_ITEMS && id < Equipement::SMALL_ITEMS)
		id += 21*m_medium_sheet;
	if (id >=Equipement::SMALL_ITEMS)
		id += 30* m_small_sheet;
	
	Player* player = m_document->getLocalPlayer();
	if (player ==0)
		return true;
	
	Creature* npc = player->getTradePartner();
	
	if (npc ==0)
	{
		return true;
	}
		
	Equipement* equ = npc->getEquipement();
	if (equ ==0)
	{
		return true;
	}
	
	int gold = player->getEquipement()->getGold();
	//bool stashtrade = false;
	if (player->getId() == npc->getId())
	{
		equ = player->getStash();
		//stashtrade = true;
		gold = -1;
	}
	
	updateItemWindowTooltip(we.window,equ->getItem(id) ,player,gold);
	
	return true;
}

bool TradeWindow::onTradeItemMouseButtonPressed(const CEGUI::EventArgs& evt)
{
	const CEGUI::MouseEventArgs& we =
			static_cast<const CEGUI::MouseEventArgs&>(evt);
	unsigned int id = we.window->getID();
	short pos = id;
	
	if (pos>= Equipement::BIG_ITEMS && pos <  Equipement::MEDIUM_ITEMS)
	{
		id += m_big_sheet * 15;
	}
	if (pos>= Equipement::MEDIUM_ITEMS && pos < Equipement::SMALL_ITEMS)
	{
		id += m_medium_sheet * 21;
	}
	if (pos>= Equipement::SMALL_ITEMS)
	{
		id += m_small_sheet * 30;
	}
	
	if (we.button == CEGUI::LeftButton)
	{
		m_document->onTradeItemLeftClick((short) id);
	}

	if (we.button == CEGUI::RightButton)
	{
		m_document->onTradeItemRightClick((short) id);
	}

	
	return true;
}

bool TradeWindow::onTradeAreaMouseButtonPressed(const CEGUI::EventArgs& evt)
{
	m_document->onTradeSellClicked();
	return true;
}

bool TradeWindow::onTradeItemMouseButtonReleased(const CEGUI::EventArgs& evt)
{
	
	return true;
}

bool TradeWindow::onTradeNextItems(const CEGUI::EventArgs& evt)
{
	const CEGUI::MouseEventArgs& we =
		static_cast<const CEGUI::MouseEventArgs&>(evt);
	unsigned int id = we.window->getID();
	if (id ==0) 
	{
		m_big_sheet--;
	}
	else if (id ==1) 
	{
		m_big_sheet++;
	}
	else if (id ==2) 
	{
		m_medium_sheet--;
	}
	else  if (id ==3) 
	{
		m_medium_sheet++;
	}
	else  if (id ==4) 
	{
		m_small_sheet--;
	}
	else  if (id ==5) 
	{
		m_small_sheet++;
	}
	
	setSilent(true);
	update();
	setSilent(false);
	
	return true;
}

bool TradeWindow::onCloseTrade(const CEGUI::EventArgs& evt)
{
	m_document->onButtonCloseTrade();
	return true;
	
}



