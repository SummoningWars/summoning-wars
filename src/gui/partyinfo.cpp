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

#include "partyinfo.h"

PartyInfo::PartyInfo (Document* doc)
	: Window(doc)
{
	CEGUI::WindowManager& win_mgr = CEGUI::WindowManager::getSingleton();
	
	// Rahmen fuer CharInfo Fenster
	CEGUI::FrameWindow* party_info = (CEGUI::FrameWindow*) win_mgr.createWindow("TaharezLook/FrameWindow", "PartyInfo");
	m_window = party_info;
	
	party_info->setPosition(CEGUI::UVector2(cegui_reldim(0.0f), cegui_reldim( 0.0f)));
	party_info->setSize(CEGUI::UVector2(cegui_reldim(0.48f), cegui_reldim( 0.77f)));
	party_info->setProperty("FrameEnabled","false");
	party_info->setProperty("TitlebarEnabled","false");
	party_info->setProperty("CloseButtonEnabled","false");
	party_info->setProperty("MousePassThroughEnabled","true");
	party_info->subscribeEvent(CEGUI::Window::EventMouseButtonDown, CEGUI::Event::Subscriber(&Window::consumeEvent, (Window*) this));
	party_info->setWantsMultiClickEvents(false);
	party_info->setAlpha(0.0);

	
	CEGUI::PushButton* btn;
	CEGUI::Window* label, *img;
	std::ostringstream stream;
	for (int i=0; i<7; i++)
	{
		stream.str("");
		stream << "PlayerImage";
		stream << i;
		
		img = win_mgr.createWindow("TaharezLook/StaticImage",stream.str());
		party_info->addChildWindow(img);
		img->setProperty("InheritsAlpha", "false");
		img->setProperty("FrameEnabled", "false");
		img->setProperty("BackgroundEnabled", "false");
		img->setPosition(CEGUI::UVector2(cegui_reldim(0.01f), cegui_reldim( 0.01f + 0.12f *i)));
		img->setSize(CEGUI::UVector2(cegui_reldim(0.13f), cegui_reldim( 0.1f)));
		img->setID(i);
		
		stream.str("");
		stream << "PlayerParty";
		stream << i;
		
		label = win_mgr.createWindow("TaharezLook/StaticText", stream.str());
		party_info->addChildWindow(label);
		label->setProperty("InheritsAlpha", "false");
		label->setProperty("FrameEnabled", "false");
		label->setProperty("BackgroundEnabled", "false");
		label->setPosition(CEGUI::UVector2(cegui_reldim(0.15f), cegui_reldim( 0.01f+ 0.12f *i)));
		label->setSize(CEGUI::UVector2(cegui_reldim(0.06f), cegui_reldim( 0.1f)));
		label->setText("0");
		
		stream.str("");
		stream << "PlayerName";
		stream << i;
		
		label = win_mgr.createWindow("TaharezLook/StaticText", stream.str());
		party_info->addChildWindow(label);
		label->setProperty("InheritsAlpha", "false");
		label->setProperty("FrameEnabled", "false");
		label->setProperty("BackgroundEnabled", "false");
		label->setPosition(CEGUI::UVector2(cegui_reldim(0.21f), cegui_reldim( 0.01f+ 0.12f *i)));
		label->setSize(CEGUI::UVector2(cegui_reldim(0.30f), cegui_reldim( 0.1f)));
		label->setText("Name");
		
		stream.str("");
		stream << "PlayerClass";
		stream << i;
		
		label = win_mgr.createWindow("TaharezLook/StaticText", stream.str());
		party_info->addChildWindow(label);
		label->setProperty("InheritsAlpha", "false");
		label->setProperty("FrameEnabled", "false");
		label->setProperty("BackgroundEnabled", "false");
		label->setPosition(CEGUI::UVector2(cegui_reldim(0.53f), cegui_reldim( 0.01f + 0.12f *i)));
		label->setSize(CEGUI::UVector2(cegui_reldim(0.15f), cegui_reldim( 0.1f)));
		label->setText("Klasse");
		
		stream.str("");
		stream << "AcceptMemberButton";
		stream << i;
		
		btn = static_cast<CEGUI::PushButton*>(win_mgr.createWindow("TaharezLook/Button",stream.str() ));
		party_info->addChildWindow(btn);
		btn->setProperty("HoverImage", "set:MainMenu image:SPBtnHover");
		btn->setProperty("NormalImage", "set:MainMenu image:SPBtnNormal");
		btn->setProperty("PushedImage", "set:MainMenu image:SPBtnPushed");
		btn->setProperty("DisabledImage", "set:MainMenu image:SPBtnNormal");
		btn->setProperty("InheritsAlpha", "false");
		btn->setPosition(CEGUI::UVector2(cegui_reldim(0.7), cegui_reldim( 0.02f + 0.12f *i)));
		btn->setSize(CEGUI::UVector2(cegui_reldim(0.12f), cegui_reldim( 0.08f)));
		btn->setText("A");
		btn->subscribeEvent(CEGUI::PushButton::EventClicked, CEGUI::Event::Subscriber(&PartyInfo::onAcceptMemberButtonClicked, this));
		btn->setWantsMultiClickEvents(false);
		btn->setID(i);
		
		stream.str("");
		stream << "RejectMemberButton";
		stream << i;
		
		btn = static_cast<CEGUI::PushButton*>(win_mgr.createWindow("TaharezLook/Button",stream.str() ));
		party_info->addChildWindow(btn);
		btn->setProperty("HoverImage", "set:MainMenu image:SPBtnHover");
		btn->setProperty("NormalImage", "set:MainMenu image:SPBtnNormal");
		btn->setProperty("PushedImage", "set:MainMenu image:SPBtnPushed");
		btn->setProperty("DisabledImage", "set:MainMenu image:SPBtnNormal");
		btn->setProperty("InheritsAlpha", "false");
		btn->setPosition(CEGUI::UVector2(cegui_reldim(0.85), cegui_reldim( 0.02f + 0.12f *i)));
		btn->setSize(CEGUI::UVector2(cegui_reldim(0.12f), cegui_reldim( 0.08f)));
		btn->setText("R");
		btn->subscribeEvent(CEGUI::PushButton::EventClicked, CEGUI::Event::Subscriber(&PartyInfo::onRejectMemberButtonClicked, this));
		btn->setWantsMultiClickEvents(false);
		btn->setID(i);
		
		
		
		stream.str("");
		stream << "ApplyButton";
		stream << i;
		
		btn = static_cast<CEGUI::PushButton*>(win_mgr.createWindow("TaharezLook/Button",stream.str() ));
		party_info->addChildWindow(btn);
		btn->setProperty("HoverImage", "set:MainMenu image:SPBtnHover");
		btn->setProperty("NormalImage", "set:MainMenu image:SPBtnNormal");
		btn->setProperty("PushedImage", "set:MainMenu image:SPBtnPushed");
		btn->setProperty("DisabledImage", "set:MainMenu image:SPBtnNormal");
		btn->setProperty("InheritsAlpha", "false");
		btn->setPosition(CEGUI::UVector2(cegui_reldim(0.7), cegui_reldim( 0.02f + 0.12f *i)));
		btn->setSize(CEGUI::UVector2(cegui_reldim(0.12f), cegui_reldim( 0.08f)));
		btn->setText("C");
		btn->subscribeEvent(CEGUI::PushButton::EventClicked, CEGUI::Event::Subscriber(&PartyInfo::onApplyButtonClicked, this));
		btn->setWantsMultiClickEvents(false);
		btn->setID(i);
		
		stream.str("");
		stream << "KickMemberButton";
		stream << i;
		
		btn = static_cast<CEGUI::PushButton*>(win_mgr.createWindow("TaharezLook/Button",stream.str() ));
		party_info->addChildWindow(btn);
		btn->setProperty("HoverImage", "set:MainMenu image:SPBtnHover");
		btn->setProperty("NormalImage", "set:MainMenu image:SPBtnNormal");
		btn->setProperty("PushedImage", "set:MainMenu image:SPBtnPushed");
		btn->setProperty("DisabledImage", "set:MainMenu image:SPBtnNormal");
		btn->setProperty("InheritsAlpha", "false");
		btn->setPosition(CEGUI::UVector2(cegui_reldim(0.85), cegui_reldim( 0.02f + 0.12f *i)));
		btn->setSize(CEGUI::UVector2(cegui_reldim(0.12f), cegui_reldim( 0.08f)));
		btn->setText("K");
		btn->subscribeEvent(CEGUI::PushButton::EventClicked, CEGUI::Event::Subscriber(&PartyInfo::onKickMemberButtonClicked, this));
		btn->setWantsMultiClickEvents(false);
		btn->setID(i);
		
		stream.str("");
		stream << "LeavePartyButton";
		stream << i;
		
		btn = static_cast<CEGUI::PushButton*>(win_mgr.createWindow("TaharezLook/Button",stream.str() ));
		party_info->addChildWindow(btn);
		btn->setProperty("HoverImage", "set:MainMenu image:SPBtnHover");
		btn->setProperty("NormalImage", "set:MainMenu image:SPBtnNormal");
		btn->setProperty("PushedImage", "set:MainMenu image:SPBtnPushed");
		btn->setProperty("DisabledImage", "set:MainMenu image:SPBtnNormal");
		btn->setProperty("InheritsAlpha", "false");
		btn->setPosition(CEGUI::UVector2(cegui_reldim(0.85), cegui_reldim( 0.02f + 0.12f *i)));
		btn->setSize(CEGUI::UVector2(cegui_reldim(0.12f), cegui_reldim( 0.08f)));
		btn->setText("L");
		btn->subscribeEvent(CEGUI::PushButton::EventClicked, CEGUI::Event::Subscriber(&PartyInfo::onLeavePartyButtonClicked, this));
		btn->setWantsMultiClickEvents(false);
		btn->setID(i);
		
		
		
		stream.str("");
		stream << "PeaceButton";
		stream << i;
		
		btn = static_cast<CEGUI::PushButton*>(win_mgr.createWindow("TaharezLook/Button",stream.str() ));
		party_info->addChildWindow(btn);
		btn->setProperty("HoverImage", "set:MainMenu image:SPBtnHover");
		btn->setProperty("NormalImage", "set:MainMenu image:SPBtnNormal");
		btn->setProperty("PushedImage", "set:MainMenu image:SPBtnPushed");
		btn->setProperty("DisabledImage", "set:MainMenu image:SPBtnNormal");
		btn->setProperty("InheritsAlpha", "false");
		btn->setPosition(CEGUI::UVector2(cegui_reldim(0.7), cegui_reldim( 0.02f + 0.12f *i)));
		btn->setSize(CEGUI::UVector2(cegui_reldim(0.12f), cegui_reldim( 0.08f)));
		btn->setText("P");
		btn->subscribeEvent(CEGUI::PushButton::EventClicked, CEGUI::Event::Subscriber(&PartyInfo::onPeaceButtonClicked, this));
		btn->setWantsMultiClickEvents(false);
		btn->setID(i);
		
		stream.str("");
		stream << "DeclareWarButton";
		stream << i;
		
		btn = static_cast<CEGUI::PushButton*>(win_mgr.createWindow("TaharezLook/Button",stream.str() ));
		party_info->addChildWindow(btn);
		btn->setProperty("HoverImage", "set:MainMenu image:SPBtnHover");
		btn->setProperty("NormalImage", "set:MainMenu image:SPBtnNormal");
		btn->setProperty("PushedImage", "set:MainMenu image:SPBtnPushed");
		btn->setProperty("DisabledImage", "set:MainMenu image:SPBtnNormal");
		btn->setProperty("InheritsAlpha", "false");
		btn->setPosition(CEGUI::UVector2(cegui_reldim(0.85), cegui_reldim( 0.02f + 0.12f *i)));
		btn->setSize(CEGUI::UVector2(cegui_reldim(0.12f), cegui_reldim( 0.08f)));
		btn->setText("W");
		btn->subscribeEvent(CEGUI::PushButton::EventClicked, CEGUI::Event::Subscriber(&PartyInfo::onDeclareWarButtonClicked, this));
		btn->setWantsMultiClickEvents(false);
		btn->setID(i);
		
		

	}
	
	updateTranslation();
}

void PartyInfo::update()
{
	Player* player = m_document->getLocalPlayer();
	Player* pl;
	Party* party =  World::getWorld()->getParty(player->getFraction());
	
	// Gibt an, ob der Spieler aktuell Alleinspieler ist
	bool solo = false;
	if (party->getNrMembers ()==1)
		solo = true;
	
	WorldObjectMap* players = World::getWorld()->getPlayers();
	WorldObjectMap::iterator it;
	
	int nr=0;
	
	// Gibt an, ob der Spieler Leiter seiner Party ist
	bool leader = false;
	if (player->getId() == party->getLeader())
	{
		leader = true;
	}
	
	bool leader2;
	DEBUGX("party %i solo %i leader %i",party->getId(),solo,leader);
	
	Fraction::Relation rel,rel2;
	
	CEGUI::WindowManager& win_mgr = CEGUI::WindowManager::getSingleton();
	std::ostringstream stream;
	CEGUI::Window* label, *img, *btn;
	bool vis;
	
	for (it = players->begin(); it != players->end(); ++it)
	{
		leader2 = false;
		
		pl = static_cast<Player*>(it->second);
		m_player_ids[nr] = pl->getId();
		
		if (pl == player)
		{
			//continue;
		}
		
		leader2 = false;
		if (pl->getParty()->getLeader() == pl->getId())
		{
			leader2 = true;
		}
		
		rel = party->getRelations()[pl->getParty()->getId()];
		rel2 = pl->getParty()->getRelations()[party->getId()];
				
		stream.str("");
		stream << "PlayerImage";
		stream << nr;
		
		img = win_mgr.getWindow(stream.str());
		if (img->isVisible()!=true)
		{
			img->setVisible(true);
		}
		
		std::string imagestr = pl->getEmotionImage("normal");
		if (img->getProperty("Image") != imagestr)
		{
			img->setProperty("Image",imagestr);
		}
		
		stream.str("");
		stream << "PlayerParty";
		stream << nr;
		label = win_mgr.getWindow(stream.str());
		
		stream.str("");
		stream<< pl->getParty()->getId();
		if (label->isVisible()!=true)
		{
			label->setVisible(true);
		}
		if (label->getText() != (CEGUI::utf8*)  stream.str().c_str())
		{
			label->setText((CEGUI::utf8*)  stream.str().c_str());
		}
		
		stream.str("");
		stream << "PlayerName";
		stream << nr;
		
		label = win_mgr.getWindow(stream.str());
		stream.str("");
		stream << pl->getName().getRawText();
		if (label->isVisible()!=true)
		{
			label->setVisible(true);
		}
		if (label->getText() != (CEGUI::utf8*)  stream.str().c_str())
		{
			label->setText((CEGUI::utf8*)  stream.str().c_str());
		}
		
		
		stream.str("");
		stream << "PlayerClass";
		stream << nr;
		
		label = win_mgr.getWindow(stream.str());
		PlayerBasicData* pdata = ObjectFactory::getPlayerData(pl->getSubtype());
		if (pdata != 0)
		{
			stream.str(gettext(pdata->m_name.c_str()));
		}
		else
		{
			stream.str("");
		}
		
		if (label->getText() != (CEGUI::utf8*)  stream.str().c_str())
		{
			label->setText((CEGUI::utf8*)  stream.str().c_str());
		}
		
		// Accept button nur Anzeigen wenn der Spieler Partyleiter ist und der andere Spieler ein Bewerber
		vis = false;
		if (leader && party->getCandidates().count(pl->getId())>0)
		{
			vis = true;
		}
		stream.str("");
		stream << "AcceptMemberButton";
		stream << nr;
		
		
		btn = win_mgr.getWindow(stream.str());
		if (btn->isVisible()!=vis)
		{
			btn->setVisible(vis);
		}
		
		// gleiche Regeln fuer den Ablehnen Button
		stream.str("");
		stream << "RejectMemberButton";
		stream << nr;
		
		btn = win_mgr.getWindow(stream.str());
		if (btn->isVisible()!=vis)
		{
			btn->setVisible(vis);
		}
		
		// Bewerbung erlauben, wenn der andere Spieler ein Partyleiter und man selbst Solo
		vis = false;
		if (solo && player->getCandidateParty() ==-1 && leader2 && party->getId() != pl->getParty()->getId() && party->getCandidates().count(pl->getId())==0 && rel == Fraction::NEUTRAL && rel2 == Fraction::NEUTRAL)
		{
			vis = true;
		}
		stream.str("");
		stream << "ApplyButton";
		stream << nr;
		
		btn = win_mgr.getWindow(stream.str());
		if (btn->isVisible()!=vis)
		{
			btn->setVisible(vis);
		}
		
		// Kick Button anzeigen, wenn man Partyleiter ist und der andere Spieler ein Verbuendeter
		vis = false;
		if (leader && party->getMembers().count(pl->getId())>0 && pl != player)
		{
			if (World::getWorld() !=0 && !World::getWorld()->isCooperative())
				vis = true;
		}
		stream.str("");
		stream << "KickMemberButton";
		stream << nr;
		
		btn = win_mgr.getWindow(stream.str());
		if (btn->isVisible()!=vis)
		{
			btn->setVisible(vis);
		}
		
		vis = false;
		if ((!solo && pl->getId() == party->getLeader()) || (solo && pl->getParty()->getId() == player->getCandidateParty()))
		{
			if (World::getWorld() !=0 && !World::getWorld()->isCooperative())
				vis = true;
		}
		stream.str("");
		stream << "LeavePartyButton";
		stream << nr;
		
		btn = win_mgr.getWindow(stream.str());
		if (btn->isVisible()!=vis)
		{
			btn->setVisible(vis);
		}
		
		// Frieden Button anbieten, wenn aktuelles Verhaeltnis feindlich ist
		vis = false;
		if (leader && leader2 && rel == Fraction::HOSTILE)
		{
			vis = true;
		}
		stream.str("");
		stream << "PeaceButton";
		stream << nr;
		
		btn = win_mgr.getWindow(stream.str());
		if (btn->isVisible()!=vis)
		{
			btn->setVisible(vis);
		}
		
		// Krieg Button anbieten, wenn aktuelles Verhaeltnis neutral ist
		vis = false;
		if (leader && leader2 && rel == Fraction::NEUTRAL && player->getCandidateParty() != pl->getParty()->getId() && pl->getCandidateParty() != player->getParty()->getId())
		{
			vis = true;
		}
		stream.str("");
		stream << "DeclareWarButton";
		stream << nr;
		
		btn = win_mgr.getWindow(stream.str());
		if (btn->isVisible()!=vis)
		{
			btn->setVisible(vis);
		}
		
		
		nr ++;
		
	}
	
	// alle GUI Elemente zu denen kein Spieler gehoehrt ausblenden
	std::string elements[11] = {"PlayerImage","PlayerParty", "PlayerName", "PlayerClass", "AcceptMemberButton", "RejectMemberButton", "ApplyButton", "KickMemberButton", "PeaceButton", "DeclareWarButton","LeavePartyButton"};
	
	
	for (int i=nr; i<7; i++)
	{
		for (int j=0; j<11; j++)
		{
			stream.str("");
			stream << elements[j];
			stream << i;
			
			label = win_mgr.getWindow(stream.str());
			if (label->isVisible())
			{
				label->setVisible(false);
			}
			
		}
	}
	
}

void PartyInfo::updateTranslation()
{
	CEGUI::WindowManager& win_mgr = CEGUI::WindowManager::getSingleton();
	CEGUI::Window* label;
	
	std::ostringstream stream;
	for (int i=0; i<7; i++)
	{
		stream.str("");
		stream << "PlayerName";
		stream << i;
		
		label = win_mgr.getWindow(stream.str());
		label->setText((CEGUI::utf8*) gettext("Name"));
	}
}

bool PartyInfo::onAcceptMemberButtonClicked(const CEGUI::EventArgs& evt)
{
	const CEGUI::MouseEventArgs& we =
			static_cast<const CEGUI::MouseEventArgs&>(evt);
	unsigned int id = we.window->getID();

	m_document->onButtonPartyAccept(m_player_ids[id]);
	
	return true;
}

bool PartyInfo::onApplyButtonClicked(const CEGUI::EventArgs& evt)
{
	const CEGUI::MouseEventArgs& we =
			static_cast<const CEGUI::MouseEventArgs&>(evt);
	unsigned int id = we.window->getID();

	m_document->onButtonPartyApply(m_player_ids[id]);
	return true;
}

bool PartyInfo::onRejectMemberButtonClicked(const CEGUI::EventArgs& evt)
{
	const CEGUI::MouseEventArgs& we =
			static_cast<const CEGUI::MouseEventArgs&>(evt);
	unsigned int id = we.window->getID();

	m_document->onButtonPartyReject(m_player_ids[id]);
	return true;
}

bool PartyInfo::onKickMemberButtonClicked(const CEGUI::EventArgs& evt)
{
	const CEGUI::MouseEventArgs& we =
			static_cast<const CEGUI::MouseEventArgs&>(evt);
	unsigned int id = we.window->getID();

	m_document->onButtonKick(m_player_ids[id]);
	
	return true;
}

bool PartyInfo::onLeavePartyButtonClicked(const CEGUI::EventArgs& evt)
{
	m_document->onButtonPartyLeave();
	return true;
}

bool PartyInfo::onPeaceButtonClicked(const CEGUI::EventArgs& evt)
{
	const CEGUI::MouseEventArgs& we =
			static_cast<const CEGUI::MouseEventArgs&>(evt);
	unsigned int id = we.window->getID();

	m_document->onButtonPartyPeace(m_player_ids[id]);
	return true;
}

bool PartyInfo::onDeclareWarButtonClicked(const CEGUI::EventArgs& evt)
{
	
	const CEGUI::MouseEventArgs& we =
			static_cast<const CEGUI::MouseEventArgs&>(evt);
	unsigned int id = we.window->getID();

	m_document->onButtonPartyWar(m_player_ids[id]);
	return true;
}

