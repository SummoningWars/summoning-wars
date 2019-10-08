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

TradeWindow::TradeWindow (Document* doc)
	: ItemWindow(doc)
{
	DEBUGX("setup trade");

	m_big_sheet =0;
	m_medium_sheet =0;
	m_small_sheet =0;
	
	
	CEGUI::WindowManager& win_mgr = CEGUI::WindowManager::getSingleton();

	// Rahmen fuer Inventar Fenster
	CEGUI::FrameWindow* trade = (CEGUI::FrameWindow*) win_mgr.loadWindowLayout("TradeWindow.layout");
	m_window = trade;
	
	trade->subscribeEvent(CEGUI::Window::EventMouseButtonDown, CEGUI::Event::Subscriber(&TradeWindow::onTradeAreaMouseButtonPressed, this));
	trade->setWantsMultiClickEvents(false);

	// Bestandteile des Charakterfensters hinzufuegen
	CEGUI::PushButton* btn;
	CEGUI::Window* label;

	int i,j;

	// Label fuer grosse Items
	std::ostringstream outStream;
	
	for (j=0;j<3;j++)
	{
		for (i=0;i<5;i++)
		{
			outStream.str("");
			outStream << "TraderBigItem" << j*5+i<< "Label";
			label = win_mgr.createWindow("TaharezLook/StaticImage", outStream.str());
			trade->addChildWindow(label);
			label->setProperty("FrameEnabled", "false");
			label->setProperty("BackgroundEnabled", "false");
			label->setPosition(CEGUI::UVector2(cegui_reldim(0.05f+i*0.166f), cegui_reldim( 0.17f +j*0.1f)));
			label->setSize(CEGUI::UVector2(cegui_reldim(0.148f), cegui_reldim( 0.09f)));
			label->setID(Equipement::BIG_ITEMS+j*5+i);
			label->subscribeEvent(CEGUI::Window::EventMouseButtonDown, CEGUI::Event::Subscriber(&TradeWindow::onTradeItemMouseButtonPressed, this));
			label->subscribeEvent(CEGUI::Window::EventMouseButtonUp, CEGUI::Event::Subscriber(&TradeWindow::onTradeItemMouseButtonReleased,  this));
			label->subscribeEvent(CEGUI::Window::EventMouseEnters, CEGUI::Event::Subscriber(&TradeWindow::onTradeItemHover,  this));
			label->setWantsMultiClickEvents(false);
		}
	}
	
	btn = static_cast<CEGUI::PushButton*>(win_mgr.getWindow("TradeBigPrevButton"));
	btn->setID(0);
	btn->subscribeEvent(CEGUI::PushButton::EventClicked, CEGUI::Event::Subscriber(&TradeWindow::onTradeNextItems, this));
	
	btn = static_cast<CEGUI::PushButton*>(win_mgr.getWindow("TradeBigNextButton"));
	btn->setID(1);
	btn->subscribeEvent(CEGUI::PushButton::EventClicked, CEGUI::Event::Subscriber(&TradeWindow::onTradeNextItems, this));

	
	for (j=0;j<3;j++)
	{
		for (i=0;i<7;i++)
		{
			outStream.str("");
			outStream << "TraderMediumItem" << j*7+i<< "Label";
			label = win_mgr.createWindow("TaharezLook/StaticImage", outStream.str());
			trade->addChildWindow(label);
			label->setProperty("FrameEnabled", "false");
			label->setProperty("BackgroundEnabled", "false");
			label->setPosition(CEGUI::UVector2(cegui_reldim(0.05f+i*0.119f), cegui_reldim( 0.487f+0.088*j)));
			label->setSize(CEGUI::UVector2(cegui_reldim(0.1f), cegui_reldim( 0.067f)));
			label->setID(Equipement::MEDIUM_ITEMS+j*7+i);
			label->subscribeEvent(CEGUI::Window::EventMouseButtonDown, CEGUI::Event::Subscriber(&TradeWindow::onTradeItemMouseButtonPressed, this));
			label->subscribeEvent(CEGUI::Window::EventMouseButtonUp, CEGUI::Event::Subscriber(&TradeWindow::onTradeItemMouseButtonReleased,  this));
			label->subscribeEvent(CEGUI::Window::EventMouseEnters, CEGUI::Event::Subscriber(&TradeWindow::onTradeItemHover,  this));
			label->setWantsMultiClickEvents(false);
		}
	}
	
	btn = static_cast<CEGUI::PushButton*>(win_mgr.getWindow("TradeMediumPrevButton"));
	btn->setID(2);
	btn->subscribeEvent(CEGUI::PushButton::EventClicked, CEGUI::Event::Subscriber(&TradeWindow::onTradeNextItems, this));
	
	btn = static_cast<CEGUI::PushButton*>(win_mgr.getWindow("TradeMediumNextButton"));
	btn->setID(3);
	btn->subscribeEvent(CEGUI::PushButton::EventClicked, CEGUI::Event::Subscriber(&TradeWindow::onTradeNextItems, this));
	
	for (j=0;j<3;j++)
	{
		for (i=0;i<10;i++)
		{
			outStream.str("");
			outStream << "TraderSmallItem" << j*10+i<< "Label";
			label = win_mgr.createWindow("TaharezLook/StaticImage", outStream.str());
			trade->addChildWindow(label);
			label->setProperty("FrameEnabled", "false");
			label->setProperty("BackgroundEnabled", "false");
			label->setPosition(CEGUI::UVector2(cegui_reldim(0.048f+i*0.0835f), cegui_reldim( 0.755f+0.065*j)));
			label->setSize(CEGUI::UVector2(cegui_reldim(0.06f), cegui_reldim( 0.04f)));
			label->setID(Equipement::SMALL_ITEMS+j*10+i);
			label->subscribeEvent(CEGUI::Window::EventMouseButtonDown, CEGUI::Event::Subscriber(&TradeWindow::onTradeItemMouseButtonPressed, this));
			label->subscribeEvent(CEGUI::Window::EventMouseButtonUp, CEGUI::Event::Subscriber(&TradeWindow::onTradeItemMouseButtonReleased,  this));
			label->subscribeEvent(CEGUI::Window::EventMouseEnters, CEGUI::Event::Subscriber(&TradeWindow::onTradeItemHover,  this));
			label->setWantsMultiClickEvents(false);
		}
	}
	
	btn = static_cast<CEGUI::PushButton*>(win_mgr.getWindow("TradeSmallPrevButton"));
	btn->setID(4);
	btn->subscribeEvent(CEGUI::PushButton::EventClicked, CEGUI::Event::Subscriber(&TradeWindow::onTradeNextItems, this));
	
	btn = static_cast<CEGUI::PushButton*>(win_mgr.getWindow("TradeSmallNextButton"));
	btn->setID(5);
	btn->subscribeEvent(CEGUI::PushButton::EventClicked, CEGUI::Event::Subscriber(&TradeWindow::onTradeNextItems, this));
	
	
	btn = static_cast<CEGUI::PushButton*>(win_mgr.getWindow("TradeCloseButton"));
	btn->setID(5);
	btn->subscribeEvent(CEGUI::PushButton::EventClicked, CEGUI::Event::Subscriber(&TradeWindow::onCloseTrade, this));
	
	updateTranslation();
}

void TradeWindow::update()
{
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
	
	int gold = player->getEquipement()->getGold();
	bool stashtrade = false;
	if (player->getId() == npc->getId())
	{
		equ = player->getStash();
		stashtrade = true;
		gold = -1;
	}
	
	std::ostringstream out_stream;
	
	int nbig = equ->getMaxItemNumber(Item::BIG);
	int nmedium = equ->getMaxItemNumber(Item::MEDIUM);
	int nsmall = equ->getMaxItemNumber(Item::SMALL);
	
	
	CEGUI::WindowManager& win_mgr = CEGUI::WindowManager::getSingleton();
	CEGUI::PushButton* btn;
	bool vis;
	
	btn = static_cast<CEGUI::PushButton*>(win_mgr.getWindow("TradeBigPrevButton"));
	vis = (m_big_sheet >0);
	if (btn->isVisible() != vis)
		btn->setVisible(vis);
	
	btn = static_cast<CEGUI::PushButton*>(win_mgr.getWindow("TradeBigNextButton"));
	vis = (m_big_sheet < nbig/15);
	if (btn->isVisible() != vis)
		btn->setVisible(vis);
	
	btn = static_cast<CEGUI::PushButton*>(win_mgr.getWindow("TradeMediumPrevButton"));
	vis = (m_medium_sheet >0);
	if (btn->isVisible() != vis)
		btn->setVisible(vis);
	
	btn = static_cast<CEGUI::PushButton*>(win_mgr.getWindow("TradeMediumNextButton"));
	vis = (m_medium_sheet < nmedium/21);
	if (btn->isVisible() != vis)
		btn->setVisible(vis);
	
	btn = static_cast<CEGUI::PushButton*>(win_mgr.getWindow("TradeSmallPrevButton"));
	vis = (m_small_sheet >0);
	if (btn->isVisible() != vis)
		btn->setVisible(vis);
	
	btn = static_cast<CEGUI::PushButton*>(win_mgr.getWindow("TradeSmallNextButton"));
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
		out_stream << "TraderBigItem" << i<< "Label";
		img =  win_mgr.getWindow(out_stream.str().c_str());
		it = equ->getItem(Equipement::BIG_ITEMS+i + m_big_sheet*15);
		updateItemWindow(img,it,player,player->getEquipement()->getGold());
	}
	
	for (i=0;i<21;i++)
	{
		out_stream.str("");
		out_stream << "TraderMediumItem" << i<< "Label";
		img =  win_mgr.getWindow(out_stream.str().c_str());
		it = equ->getItem(Equipement::MEDIUM_ITEMS+i + m_medium_sheet*21);
		updateItemWindow(img,it,player,player->getEquipement()->getGold());
	}
	
	for (i=0;i<30;i++)
	{
		out_stream.str("");
		out_stream << "TraderSmallItem" << i<< "Label";
		img =  win_mgr.getWindow(out_stream.str().c_str());
		it = equ->getItem(Equipement::SMALL_ITEMS+i + m_small_sheet*30);
		updateItemWindow(img,it,player,player->getEquipement()->getGold());
	}
}

void TradeWindow::updateTranslation()
{
	CEGUI::WindowManager& win_mgr = CEGUI::WindowManager::getSingleton();
	CEGUI::PushButton* btn;
	CEGUI::Window* label;
	
	label = win_mgr.getWindow("TradeLabel");
	label->setText((CEGUI::utf8*) gettext("Chest"));
	
	btn = static_cast<CEGUI::PushButton*>(win_mgr.getWindow("TradeCloseButton"));
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
	bool stashtrade = false;
	if (player->getId() == npc->getId())
	{
		equ = player->getStash();
		stashtrade = true;
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



