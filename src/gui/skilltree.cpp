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

#include "skilltree.h"

#include "tooltipmanager.h"
#include "ceguiutility.h"

SkillTree::SkillTree(Document* doc, OIS::Keyboard *keyboard)
	:Window(doc)
{
	DEBUGX("setup skilltree");
	m_keyboard = keyboard;
	
	CEGUI::WindowManager& win_mgr = CEGUI::WindowManager::getSingleton();


	// Rahmen fuer Skilltree Fenster

	// Bestandteile des Charakterfensters hinzufuegen

	std::ostringstream outStream;
	std::string name;
	std::string lname,bname;
	CEGUI::UVector2 pos;
	CEGUI::Window* label;
	
	m_player_id =0;
	
	CEGUI::TabControl* skilltree = (CEGUI::TabControl*) win_mgr.loadWindowLayout( "SkillTree.layout" );
	m_window = skilltree;
	skilltree->subscribeEvent(CEGUI::Window::EventMouseButtonDown, CEGUI::Event::Subscriber(&Window::consumeEvent,  (Window*) this));
	skilltree->setWantsMultiClickEvents(false);
	
	label = win_mgr.getWindow("SkillTreeCloseButton");
	label->subscribeEvent(CEGUI::Window::EventMouseClick, CEGUI::Event::Subscriber(&SkillTree::onCloseButtonClicked, this));
	
	m_nr_tabs =0;
	m_nr_skills =0;
	m_nr_dependencies = 0;
	m_shortkey_labels = 0;
}

void SkillTree::update()
{
	CEGUI::WindowManager& win_mgr = CEGUI::WindowManager::getSingleton();
	Player* player = m_document->getLocalPlayer();
	CEGUI::TabControl* skilltree = (CEGUI::TabControl*) win_mgr.getWindow("SkilltreeTabs");
	
	CEGUI::Window* wnd;
	std::vector<CEGUI::DefaultWindow*> tabs(3);
	CEGUI::Window* label;
	CEGUI::Window* bg;
	CEGUI::PushButton* button;
	std::stringstream stream;
	
	if (player->getId() != m_player_id)
	{
		m_player_id = player->getId();
		DEBUGX("new Player id %i",m_player_id);
		
		// alle bisherigen Tabs und Skills entfernen
		
		for (int i=0; i<m_nr_skills; i++)
		{
			stream.str("");
			stream << "SkillImage"<<i;
			wnd = win_mgr.getWindow(stream.str());
			
			wnd->getParent()->removeChildWindow(wnd);
			win_mgr.destroyWindow(wnd);
			
			stream.str("");
			stream << "SkillButton"<<i;
			wnd = win_mgr.getWindow(stream.str());
			
			wnd->getParent()->removeChildWindow(wnd);
			win_mgr.destroyWindow(wnd);
		}
		
		for (int i=0; i<m_nr_dependencies; i++)
		{
			stream.str("");
			stream << "SkillDependencyConnection"<<i;
			wnd = win_mgr.getWindow(stream.str());
			
			wnd->getParent()->removeChildWindow(wnd);
			win_mgr.destroyWindow(wnd);
		}
		m_nr_dependencies=0;
		
		for (int i=0; i<m_shortkey_labels; i++)
		{
			stream.str("");
			stream << "SkillShortkeyLabel"<<i;
			wnd = win_mgr.getWindow(stream.str());
			
			wnd->getParent()->removeChildWindow(wnd);
			win_mgr.destroyWindow(wnd);
		}
		m_shortkey_labels=0;
		
		for (int i=0; i<m_nr_tabs; i++)
		{
			stream.str("");
			stream << "SkilltreeTab"<<i;
			wnd = win_mgr.getWindow(stream.str());
			
			skilltree->removeTab(wnd->getID());
			win_mgr.destroyWindow(wnd);
		}
		
		
		
		
		// neue Tabs
		// TODO: generisch ausprogrammieren
		
		m_nr_tabs =3;
		for (int i=0; i<m_nr_tabs; i++)
		{
			stream.str("");
			stream << "SkilltreeTab" << i;
			
			tabs[i] = (CEGUI::DefaultWindow*) win_mgr.createWindow("TaharezLook/TabContentPane", stream.str());
			skilltree->addTab(tabs[i]);
			
			stream.str("");
			stream << "SkilltreeTabs__auto_TabPane__Buttons__auto_btnSkilltreeTab" << i;
			label =  win_mgr.getWindow(stream.str());
			label->setInheritsAlpha(false);
		}
		
		//Set tab name colors 
		stream.str("");
		stream << "FFD43D1A";

		label = win_mgr.getWindow("SkilltreeTabs__auto_TabPane__Buttons__auto_btnSkilltreeTab0");
		label->setProperty("NormalTextColour", stream.str());
		label->setProperty("HoverTextColour", stream.str());
		label->setProperty("SelectedTextColour", stream.str());
		label->setProperty("PushedTextColour", stream.str());
		label->setProperty("DisabledTextColour", stream.str());

		stream.str("");
		stream << "FF5CACEE";

		label = win_mgr.getWindow("SkilltreeTabs__auto_TabPane__Buttons__auto_btnSkilltreeTab1");
		label->setProperty("NormalTextColour", stream.str());
		label->setProperty("HoverTextColour", stream.str());
		label->setProperty("SelectedTextColour", stream.str());
		label->setProperty("PushedTextColour", stream.str());
		label->setProperty("DisabledTextColour", stream.str());

		stream.str("");
		stream << "FFD15FEE";

		label = win_mgr.getWindow("SkilltreeTabs__auto_TabPane__Buttons__auto_btnSkilltreeTab2");
		label->setProperty("NormalTextColour", stream.str());
		label->setProperty("HoverTextColour", stream.str());
		label->setProperty("SelectedTextColour", stream.str());
		label->setProperty("PushedTextColour", stream.str());
		label->setProperty("DisabledTextColour", stream.str());

		// labels for skillpoints and next skillpoint
		
		label = win_mgr.getWindow("SkillpointsValueLabel");
		label->setText("0");
		
		
		label = win_mgr.getWindow("NextSkillpointsValueLabel");
		label->setText("0");
		
		
		CEGUI::UVector2 pos;
		CEGUI::UVector2 bgpos;
		std::string name,lname,bname;
		Vector spos;
		
		DEBUGX("nr of abilities %i", player->getLearnableAbilities().size());
		
		std::map<int,LearnableAbility>::iterator it;
		int cnt =0;
		for (it = player->getLearnableAbilities().begin(); it != player->getLearnableAbilities().end(); ++it)
		{
			// Label mit dem Bild
			DEBUGX("ability %s nr %i",it->second.m_type.c_str(),cnt);
			stream.str("");
			stream << "ImageBg"<<cnt;
			bg = win_mgr.createWindow("TaharezLook/StaticImage", stream.str());
			bg->setProperty("Image", "set:SkillTree image:SkillBg");
			bg->setInheritsAlpha(false);
			bg->setProperty("RiseOnClick", "false");
			stream.str("");
			stream << "SkillImage"<<cnt;
			label = win_mgr.createWindow("TaharezLook/StaticImage", stream.str());
			
			tabs[it->second.m_skilltree_tab-1]->addChildWindow(bg);
			tabs[it->second.m_skilltree_tab-1]->addChildWindow(label);
			
			spos = it->second.m_skilltree_position;
			pos = CEGUI::UVector2(cegui_reldim(spos.m_x), cegui_reldim( spos.m_y));
			bgpos = CEGUI::UVector2(cegui_reldim(spos.m_x -0.02), cegui_reldim( spos.m_y -0.02));
			bg->setPosition(bgpos);
			bg->setSize(CEGUI::UVector2(cegui_reldim(0.165f), cegui_reldim( 0.145f)));
			bg->setProperty("FrameEnabled", "false");
			bg->setProperty("BackgroundEnabled", "false");

			label->setPosition(pos);
			label->setSize(CEGUI::UVector2(cegui_reldim(0.13f), cegui_reldim( 0.1f)));
		
			// Button zum Faehigkeit lernen
			pos += CEGUI::UVector2(cegui_reldim(0.14f), cegui_reldim( 0.05f));
			stream.str("");
			stream << "SkillButton"<<cnt;
			button = static_cast<CEGUI::PushButton*>(win_mgr.createWindow("TaharezLook/Button", stream.str()));
			button->setInheritsAlpha(false);
			tabs[it->second.m_skilltree_tab-1]->addChildWindow(button);
			button->setProperty("HoverImage", "set:CharScreen image:PlusBtnReleased");
			button->setProperty("NormalImage", "set:CharScreen image:PlusBtnReleased");
			button->setProperty("PushedImage", "set:CharScreen image:PlusBtnPressed");
			button->setPosition(pos);
			button->setSize(CEGUI::UVector2(cegui_reldim(0.07f), cegui_reldim( 0.05f)));
			
			// beschriften und verknuepfen
			label->setProperty("FrameEnabled", "false");
			label->setInheritsAlpha(false);
			label->setProperty("BackgroundEnabled", "false");
			label->setID(it->first);
			std::string imagename = it->second.m_image;
			if (imagename == "")
			{
				imagename = "set:skills image:";
				imagename += it->second.m_type;
			}
			label->setProperty("Image", imagename);
			label->subscribeEvent(CEGUI::Window::EventMouseClick, CEGUI::Event::Subscriber(&SkillTree::onSkillMouseClicked, this));
			label->subscribeEvent(CEGUI::Window::EventMouseEnters, CEGUI::Event::Subscriber(&SkillTree::onAbilityHover, this));

			button->setID(it->first);
			button->subscribeEvent(CEGUI::Window::EventMouseClick, CEGUI::Event::Subscriber(&SkillTree::onSkillLearnMouseClicked, this));
			
			// create Connections for Dependencies
			// loop over dependencies
			for (std::list< Action::ActionType >::iterator at = it->second.m_req_abilities.begin(); at != it->second.m_req_abilities.end(); ++at)
			{
				// another loop over all abilities
				std::map<int,LearnableAbility>::iterator jt;
				for (jt = player->getLearnableAbilities().begin(); jt != player->getLearnableAbilities().end(); ++jt)
				{
					if (*at == jt->second.m_type)
					{
						// found dependency
						Vector deppos = jt->second.m_skilltree_position;
						
						stream.str("");
						stream << "SkillDependencyConnection"<<m_nr_dependencies;
						label = win_mgr.createWindow("TaharezLook/StaticImage", stream.str());
						tabs[it->second.m_skilltree_tab-1]->addChildWindow(label);
						label->setMousePassThroughEnabled(true);
						label->setInheritsAlpha(false);
						label->setProperty("FrameEnabled", "false");
						label->setProperty("BackgroundEnabled", "false");
			
						CEGUI::UVector2 start= CEGUI::UVector2(cegui_reldim(deppos.m_x+0.06f), cegui_reldim( deppos.m_y+0.1f));
						CEGUI::UVector2 end= CEGUI::UVector2(cegui_reldim(spos.m_x+0.07f), cegui_reldim( spos.m_y));
						label->setPosition(start);
						label->setSize(end - start);
						label->setID(jt->first);
						
						// distinguish vertical and diagonal connectors
						if (deppos.m_x == spos.m_x)
						{
							label->setProperty("Image", "set:TaharezLook image:SkilltreeVertConnection"); 
						}
						else if (deppos.m_x <= spos.m_x)
						{
							label->setProperty("Image", "set:TaharezLook image:SkilltreeDiagConnection"); 
						}
						else
						{
							label->setProperty("Image", "set:TaharezLook image:SkilltreeDiag2Connection");
							start= CEGUI::UVector2(cegui_reldim(spos.m_x+0.07f), cegui_reldim( deppos.m_y+0.1f));
							end= CEGUI::UVector2(cegui_reldim(deppos.m_x+0.06f), cegui_reldim( spos.m_y));
							label->setPosition(start);
							label->setSize(end - start);
						}
						m_nr_dependencies++;
						
						DEBUGX("dependency %s from %s", it->second.m_type.c_str(), at->c_str());
					}
				}
			}
			
			cnt ++;
		}
		
		m_nr_skills = cnt;
		
		updateTranslation();
	}
	
	std::ostringstream out_stream;
	std::map<int,LearnableAbility> &ablts = player->getLearnableAbilities();
	bool vis;
	Action::ActionType act;
	
	// Schleife ueber alle Skill labels
	for (int j=0;j<m_nr_skills;j++)
	{
		out_stream.str("");
		out_stream << "SkillImage" << j;

		// Label welches das Skillicon zeigt
		label = win_mgr.getWindow(out_stream.str());

		// Alpha Wert des Labels
		float alpha = 0.2;
		
		act = ablts[j].m_type;

		if (player->checkAbility(act))
		{
			// Faehigkeit steht zur Verfuegung
			alpha = 1.0;
		}
		else
		{
			alpha = 0.3;
		}

		if (label->getAlpha() != alpha)
		{
			label->setAlpha(alpha);
		}
		
		out_stream.str("");
		out_stream << "SkillButton" << j;
		
		button = static_cast<CEGUI::PushButton*>(win_mgr.getWindow(out_stream.str()));
		vis = player->checkAbilityLearnable(act) && (player->getSkillPoints()>0);
		if (button->isVisible() != vis)
		{
			button->setVisible(vis);
		}
	}
	
	// Loop over all Dependencies
	for (int j=0;j<m_nr_dependencies;j++)
	{
		out_stream.str("");
		out_stream << "SkillDependencyConnection" << j;

		// Label for the dependency connecntion
		label = win_mgr.getWindow(out_stream.str());
		act = ablts[label->getID()].m_type;
		
		float alpha = 0.2;
		if (player->checkAbility(act))
		{
			// Faehigkeit steht zur Verfuegung
			alpha = 1.0;
		}
		else
		{
			alpha = 0.3;
		}

		if (label->getAlpha() != alpha)
		{
			label->setAlpha(alpha);
		}
	}
	
	// update labels for skillpoints and nextskillpoint at
	// value for next skillpoint level calculation is hacky!
	int skilllvls[20] = {2,3,5,7,10,15,20,25,30,35,40,45,50,60,70,80,90,100,1000};
	//int nextlvl; // 2011.10.26: found as unused
	int lvlid;
	for (lvlid=0; lvlid<20; lvlid++)
	{
		if (player->getBaseAttr()->m_level < skilllvls[lvlid])
			break;
	}
	
	std::string lvlstr;
	if (skilllvls[lvlid] != 1000)
	{
		std::stringstream lvlstream;
		lvlstream << skilllvls[lvlid];
		lvlstr = lvlstream.str();
	}
	else
	{
		lvlstr = "-";
	}
	
	std::stringstream spstream;
	spstream << player->getSkillPoints();
	
	// there is one label on _each_ tab
	label =  win_mgr.getWindow("SkillpointsValueLabel");
		if (label->getText() != spstream.str())
		{
			label->setText(spstream.str());
		}
		
	label =  win_mgr.getWindow("NextSkillpointsValueLabel");
		if (label->getText() != lvlstr)
		{
			label->setText(lvlstr);
		}
	
	
	// Markierer fuer Shortkeys einbauen
	// Zaehler fuer die Fenster
	
	ShortkeyMap& shortkeys = m_document->getAbilityShortkeys();
	ShortkeyMap::iterator it;
	int nr =0;
	
	CEGUI::Window *label2;
	bool right;
	std::string keyname;
	int key;
	CEGUI::UVector2 pos;
	
	for (it = shortkeys.begin(); it != shortkeys.end(); ++it)
	{
		if (it->second < USE_SKILL_LEFT || it->second >USE_SKILL_RIGHT+200)
			continue;
		
		DEBUGX("shortkey %i to %i",it->first,it->second);
		key = it->first;
		
		right = false;
		int id =0;
		if (it->second >= USE_SKILL_RIGHT)
		{
			id = it->second - USE_SKILL_RIGHT;
			right = true;
		}
		else
		{
			id = it->second - USE_SKILL_LEFT;
		}
		
		if (ablts.count(id) ==0)
			continue;
		act= ablts[id].m_type;
		
		stream.str("");
		stream << "SkillShortkeyLabel";
		stream << nr;
		
		if (nr >= m_shortkey_labels)
		{
			m_shortkey_labels ++;
			label = win_mgr.createWindow("TaharezLook/StaticText", stream.str());
			label->setInheritsAlpha(false);
			label->setProperty("FrameEnabled", "false");
			label->setProperty("BackgroundEnabled", "false");
			label->setProperty("HorzFormatting", "Centred");
			label->setID(-1);
			label->setText("");
			label->setSize(CEGUI::UVector2(cegui_reldim(0.07f), cegui_reldim( 0.05f)));
			label->setAlpha(1.0);
			label->setMousePassThroughEnabled(true);
			label->setAlwaysOnTop(true);
		}
		else
		{
			label = win_mgr.getWindow(stream.str());
		}
		
		stream.str("");
		stream << "SkillImage" << id;
		label2 = win_mgr.getWindow(stream.str());
		
		
		if (label2->getParent() != label->getParent())
		{
			if (label->getParent() != 0)
			{
				label->getParent()->removeChildWindow (label);
			}
			label2->getParent()->addChildWindow (label);
		}
		
		keyname = m_keyboard->getAsString ( (OIS::KeyCode) key);
		if (label->getText() != keyname)
		{
			label->setText(keyname);
		}
		
		if ((int) label->getID() != it->second)
		{
			label->setID(it->second);
			pos = label2->getPosition();
			if (right)
			{
				pos += CEGUI::UVector2(cegui_reldim(0.07f), cegui_reldim( 0.05f));
			}
			else
			{
				pos += CEGUI::UVector2(cegui_reldim(0.01f), cegui_reldim( 0.05f));
			}
			label->setPosition(pos);
		}
		
		if (!label->isVisible())
		{
			label->setVisible(true);
		}
		nr ++;
	}
	
	
	// restliche Labels ausblenden
	for (; nr<m_shortkey_labels; nr++)
	{
		stream.str("");
		stream << "SkillShortkeyLabel";
		stream << nr;
			
		label = win_mgr.getWindow(stream.str());
		label->setVisible(false);
	}
	
}


void SkillTree::updateTranslation()
{
	
	CEGUI::WindowManager& win_mgr = CEGUI::WindowManager::getSingleton();
	std::vector<CEGUI::DefaultWindow*> tabs(m_nr_tabs);
	
	std::stringstream stream;
	
	for (int i=0; i<m_nr_tabs; i++)
	{
		stream.str("");
		stream << "SkilltreeTab" << i;
			
		tabs[i] = (CEGUI::DefaultWindow*) win_mgr.getWindow(stream.str());
	}
	
	// Ueberschriften der Tabs setzen
	Player* player = m_document->getLocalPlayer();
	if (m_nr_tabs <3)
		return;
	
	PlayerBasicData* pdata = ObjectFactory::getPlayerData(player->getSubtype());
	CEGUI::Window* label;
	for (int i=0; i<3; i++)
	{
		tabs[i]->setText((CEGUI::utf8*) gettext(pdata->m_tabnames[i].c_str()));
	}

		label =  win_mgr.getWindow("SkillpointsLabel");
		label->setText((CEGUI::utf8*) gettext("Skillpoints"));
		
		label =  win_mgr.getWindow("NextSkillpointsLabel");
		label->setText((CEGUI::utf8*) gettext("Next Skillpoint at Level"));
		
		label =  win_mgr.getWindow("SkilltreeLabel");
		label->setText((CEGUI::utf8*) gettext("Skilltree"));
}

void SkillTree::updateAbilityTooltip(unsigned int pos)
{
	Player* player = m_document->getLocalPlayer();
	std::map<int,LearnableAbility> &ablts = player->getLearnableAbilities();
	
	
	DEBUGX("update tooltip for %i %s", pos, ablts[pos].m_type.c_str());

	CEGUI::WindowManager& win_mgr = CEGUI::WindowManager::getSingleton();
	CEGUI::Window* label;
	std::ostringstream out_stream;
	out_stream.str("");
	out_stream << "SkillImage" << pos;
	
	label = win_mgr.getWindow(out_stream.str());
	std::string tooltip = m_document->getAbilityDescription(ablts[pos].m_type);
	
	FormatedText txt =  CEGUIUtility::fitTextToWindow((CEGUI::utf8*)tooltip.c_str(), 400, CEGUIUtility::Centred, label->getFont());

	TooltipManager::getSingleton().createTooltip(label, CEGUIUtility::getColourizedString(CEGUIUtility::Black, txt.text.c_str(), CEGUIUtility::Black));
	
}

bool SkillTree::onSkillMouseClicked(const CEGUI::EventArgs& evt)
{
	Player* player = m_document->getLocalPlayer();
	std::map<int,LearnableAbility> &ablts = player->getLearnableAbilities();
	
	const CEGUI::MouseEventArgs& we =
			static_cast<const CEGUI::MouseEventArgs&>(evt);
	unsigned int id = we.window->getID();

	if (we.button == CEGUI::LeftButton)
	{
		m_document->setLeftAction(ablts[id].m_type);
	}

	if (we.button == CEGUI::RightButton)
	{
		DEBUGX("right button pressed on skill %i",id);
		m_document->setRightAction(ablts[id].m_type);
	}
	
	return true;
	
}

bool SkillTree::onSkillLearnMouseClicked(const CEGUI::EventArgs& evt)
{
	Player* player = m_document->getLocalPlayer();
	std::map<int,LearnableAbility> &ablts = player->getLearnableAbilities();
	
	const CEGUI::MouseEventArgs& we =
			static_cast<const CEGUI::MouseEventArgs&>(evt);
	unsigned int id = we.window->getID();
	
	// versuche Faehigkeit zu lernen
	m_document->learnAbility(ablts[id].m_type);
	
	return true;
}

bool SkillTree::onCloseButtonClicked(const CEGUI::EventArgs& evt)
{
	m_document->onButtonSkilltreeClicked();
	return true;
}

bool SkillTree::onAbilityHover(const CEGUI::EventArgs& evt)
{

	Player* player = m_document->getLocalPlayer();
	std::map<int,LearnableAbility> &ablts = player->getLearnableAbilities();
	
	const CEGUI::MouseEventArgs& we =
			static_cast<const CEGUI::MouseEventArgs&>(evt);
	unsigned int id = we.window->getID();
	DEBUGX("mouse entered Ability %i",id);
	updateAbilityTooltip(id);
	m_document->getGUIState()->m_hover_ability = ablts[id].m_type;
	
	return true;
}

bool SkillTree::onAbilityHoverLeave(const CEGUI::EventArgs& evt)
{
	m_document->getGUIState()->m_hover_ability = "noaction";
	return true;
}

