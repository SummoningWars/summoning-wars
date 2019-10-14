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

// Utility for CEGUI cross-version compatibility
#include "ceguiutility.h"


#define DEFAULT_SKILL_ITEM_WIDTH 0.18f
#define DEFAULT_SKILL_ITEM_HEIGHT 0.16f
#define DEFAULT_SKILL_BTN_WIDTH 0.07f
#define DEFAULT_SKILL_BTN_HEIGHT 0.05f

SkillTree::SkillTree(Document* doc, OIS::Keyboard *keyboard, const std::string& ceguiSkinName)
	: Window(doc),
    m_ceguiSkinName(ceguiSkinName)
{
	DEBUGX("setup skilltree");
	m_keyboard = keyboard;
	m_skill_widgets_pics.clear ();
	m_skill_widgets_btns.clear ();
	
	// Rahmen fuer Skilltree Fenster

	// Bestandteile des Charakterfensters hinzufuegen

	std::ostringstream outStream;
	std::string name;
	std::string lname,bname;
	CEGUI::UVector2 pos;
	CEGUI::Window* label;
	
	m_player_id =0;
	
	CEGUI::TabControl* skilltree = (CEGUI::TabControl*) CEGUIUtility::loadLayoutFromFile ("skilltree.layout");
	if (!skilltree)
	{
		SW_DEBUG ("WARNING: Failed to load [%s]", "skilltree.layout");
	}
	CEGUI::Window* skilltree_holder = CEGUIUtility::loadLayoutFromFile ("skilltree_holder.layout");
	if (!skilltree_holder)
	{
		SW_DEBUG("WARNING: Failed to load [%s]", "skilltree_holder.layout");
	}

	CEGUI::Window* wndHolder = CEGUIUtility::getWindowForLoadedLayoutEx (skilltree_holder, "Skilltree_Holder");
	CEGUI::Window* wndSkilltree = CEGUIUtility::getWindowForLoadedLayoutEx (skilltree, "Skilltree");
	if (wndHolder && wndSkilltree)
	{
		CEGUIUtility::addChildWidget (wndHolder, wndSkilltree);
	}
	else
	{
		if (!wndHolder)
    {
      SW_DEBUG("ERROR: Unable to get the window holder for skills.");
    }

		if (!wndSkilltree) 
    {
      SW_DEBUG("ERROR: Unable to get the window for skills.");
    }
	}

	m_window = skilltree_holder;
	skilltree->subscribeEvent(CEGUI::Window::EventMouseButtonDown, CEGUI::Event::Subscriber(&Window::consumeEvent,  (Window*) this));
	skilltree->setWantsMultiClickEvents(false);
	
	// the panel should also have an auto-close button; connect it to the Cancel/Abort event.
	label = CEGUIUtility::getWindowForLoadedLayoutEx (m_window, "Skilltree/__auto_closebutton__");
	if (label)
	{
		label->subscribeEvent(CEGUI::Window::EventMouseClick, CEGUI::Event::Subscriber(&SkillTree::onCloseButtonClicked, this));
	}


	m_nr_tabs =0;
	m_nr_skills =0;
	m_nr_dependencies = 0;
	m_shortkey_labels = 0;
}

void SkillTree::update()
{
	CEGUI::WindowManager& win_mgr = CEGUI::WindowManager::getSingleton();
	Player* player = m_document->getLocalPlayer();
	CEGUI::TabControl* skilltree = (CEGUI::TabControl*) CEGUIUtility::getWindowForLoadedLayoutEx (m_window, "Skilltree/skilltree_aux/SkilltreeTabs");
	
	CEGUI::Window* wnd;
	std::vector<CEGUI::DefaultWindow*> tabs(3);
	CEGUI::Window* label;
	CEGUI::PushButton* button;
	std::stringstream stream;
	
	if (player->getId() != m_player_id)
	{
		m_player_id = player->getId();
		DEBUGX("new Player id %i", m_player_id);
		
		// alle bisherigen Tabs und Skills entfernen
		
		for (int i = 0; i < m_nr_skills; i++)
		{
			//stream.str("");
			//stream << "SkillImage"<<i;
			std::string skillImageWidget = m_skill_widgets_pics[i];
			std::string skillButtonWidget = m_skill_widgets_btns[i];
			wnd = CEGUIUtility::getWindowForLoadedLayoutEx (m_window, skillImageWidget);
			
			CEGUIUtility::removeChildWidget (wnd->getParent (), wnd);
			win_mgr.destroyWindow(wnd);
			
			//stream.str("");
			//stream << "SkillButton"<<i;
			wnd = CEGUIUtility::getWindowForLoadedLayoutEx (m_window, skillButtonWidget);
			
			CEGUIUtility::removeChildWidget (wnd->getParent (), wnd);
			win_mgr.destroyWindow(wnd);
		}
		
		for (int i = 0; i < m_nr_dependencies; i++)
		{
			//stream.str("");
			//stream << "SkillDependencyConnection"<<i;
			std::string skillConnWidget = m_skill_widgets_connections[i];
			wnd = CEGUIUtility::getWindowForLoadedLayoutEx (m_window, skillConnWidget);
			
			CEGUIUtility::removeChildWidget (wnd->getParent (), wnd);
			win_mgr.destroyWindow(wnd);
		}
		m_nr_dependencies = 0;
		
		for (int i = 0; i < m_shortkey_labels; i++)
		{
			//stream.str("");
			//stream << "SkillShortkeyLabel"<<i;
			std::string skillShortcutsWidget = m_skill_widgets_shortcuts[i];
			wnd = CEGUIUtility::getWindowForLoadedLayoutEx (m_window, skillShortcutsWidget);
			
			CEGUIUtility::removeChildWidget (wnd->getParent (), wnd);
			win_mgr.destroyWindow(wnd);
		}
		m_shortkey_labels = 0;
		
		for (int i = 0; i < m_nr_tabs; i++)
		{
			stream.str("");
			stream << "Skilltree/skilltree_aux/SkilltreeTabs/__auto_TabPane__/";
			stream << "SkilltreeTab"<<i;
			wnd = CEGUIUtility::getWindowForLoadedLayoutEx (m_window, stream.str());
			
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
			
			
			tabs[i] = (CEGUI::DefaultWindow*) win_mgr.createWindow(CEGUIUtility::getWidgetWithSkin (m_ceguiSkinName, "TabContentPane"), stream.str());
			skilltree->addTab(tabs[i]);
			
			stream.str("");
			stream << "Skilltree/skilltree_aux/SkilltreeTabs/__auto_TabPane__/SkilltreeTab" << i;

			label =  CEGUIUtility::getWindowForLoadedLayoutEx (m_window, stream.str());
			label->setInheritsAlpha(false);
		}
		
		//Set tab name colors 
		stream.str("");
		stream << "FFD43D1A";

		label = CEGUIUtility::getWindowForLoadedLayoutEx (m_window, "Skilltree/skilltree_aux/SkilltreeTabs/__auto_TabPane__/SkilltreeTab0");
		if (label->isPropertyPresent ("NormalTextColour"))
		{
			label->setProperty("NormalTextColour", stream.str());
		}
		if (label->isPropertyPresent ("HoverTextColour"))
		{
			label->setProperty("HoverTextColour", stream.str());
		}
		if (label->isPropertyPresent ("SelectedTextColour"))
		{
			label->setProperty("SelectedTextColour", stream.str());
		}
		if (label->isPropertyPresent ("PushedTextColour"))
		{
			label->setProperty("PushedTextColour", stream.str());
		}
		if (label->isPropertyPresent ("DisabledTextColour"))
		{
			label->setProperty("DisabledTextColour", stream.str());
		}

		stream.str("");
		stream << "FF5CACEE";

		label = CEGUIUtility::getWindowForLoadedLayoutEx (m_window, "Skilltree/skilltree_aux/SkilltreeTabs/__auto_TabPane__/SkilltreeTab1");
		if (label->isPropertyPresent ("NormalTextColour"))
		{
			label->setProperty("NormalTextColour", stream.str());
		}
		if (label->isPropertyPresent ("HoverTextColour"))
		{
			label->setProperty("HoverTextColour", stream.str());
		}
		if (label->isPropertyPresent ("SelectedTextColour"))
		{
			label->setProperty("SelectedTextColour", stream.str());
		}
		if (label->isPropertyPresent ("PushedTextColour"))
		{
			label->setProperty("PushedTextColour", stream.str());
		}
		if (label->isPropertyPresent ("DisabledTextColour"))
		{
			label->setProperty("DisabledTextColour", stream.str());
		}

		stream.str("");
		stream << "FFD15FEE";

		label = CEGUIUtility::getWindowForLoadedLayoutEx (m_window, "Skilltree/skilltree_aux/SkilltreeTabs/__auto_TabPane__/SkilltreeTab2");
		if (label->isPropertyPresent ("NormalTextColour"))
		{
			label->setProperty("NormalTextColour", stream.str());
		}
		if (label->isPropertyPresent ("HoverTextColour"))
		{
			label->setProperty("HoverTextColour", stream.str());
		}
		if (label->isPropertyPresent ("SelectedTextColour"))
		{
			label->setProperty("SelectedTextColour", stream.str());
		}
		if (label->isPropertyPresent ("PushedTextColour"))
		{
			label->setProperty("PushedTextColour", stream.str());
		}
		if (label->isPropertyPresent ("DisabledTextColour"))
		{
			label->setProperty("DisabledTextColour", stream.str());
		}

		// labels for skillpoints and next skillpoint
		
		label = CEGUIUtility::getWindowForLoadedLayoutEx (m_window, "Skilltree/skilltree_aux/SkillTreeAdditionalInfoHolder/SkillpointsValueLabel");
		label->setText("0");
		
		
		label = CEGUIUtility::getWindowForLoadedLayoutEx (m_window, "Skilltree/skilltree_aux/SkillTreeAdditionalInfoHolder/NextSkillpointsValueLabel");
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
			// Picture of skill
			DEBUGX("ability %s nr %i",it->second.m_type.c_str(),cnt);
			stream.str("");
			stream << "SkillImage"<<cnt;
			label = win_mgr.createWindow (CEGUIUtility::getWidgetWithSkin (m_ceguiSkinName, "StaticImage"), stream.str());

			CEGUIUtility::addChildWidget (tabs[it->second.m_skilltree_tab-1], label);
			spos = it->second.m_skilltree_position;
			pos = CEGUI::UVector2(cegui_reldim(spos.m_x), cegui_reldim( spos.m_y));

			if (label->isPropertyDefault ("FrameEnabled"))
			{
				label->setProperty ("FrameEnabled", "True");
			}
			label->setPosition(pos);
			CEGUIUtility::setWidgetSizeRel (label, DEFAULT_SKILL_ITEM_WIDTH, DEFAULT_SKILL_ITEM_HEIGHT);
			std::string imageWidgetName = label->getNamePath ().c_str ();
			size_t string_pos = imageWidgetName.find ("SW/GameScreen/Skilltree_Holder/");
			if (string_pos != std::string::npos)
			{
				imageWidgetName.erase (string_pos, 31);
			}
			m_skill_widgets_pics[cnt] = imageWidgetName;
			SW_DEBUG ("Added widget name (img) [%s]", imageWidgetName.c_str ());

			//
			// Setup the button to be used to learn the corresponding skill.
			// Also connect the callbacks to the button click events.
			//
			pos += CEGUI::UVector2 (cegui_reldim (DEFAULT_SKILL_ITEM_WIDTH - 0.01f), cegui_reldim (DEFAULT_SKILL_ITEM_HEIGHT - 0.05f));
			stream.str ("");
			stream << "SkillButton" << cnt;
			button = static_cast<CEGUI::PushButton*>(win_mgr.createWindow (CEGUIUtility::getWidgetWithSkin (m_ceguiSkinName, "ImageButton"), stream.str ()));
			button->setInheritsAlpha (false);
			CEGUIUtility::addChildWidget (tabs [it->second.m_skilltree_tab - 1], button);
			if (button->isPropertyPresent ("HoverImage"))
			{
				button->setProperty("HoverImage", CEGUIUtility::getImageNameWithSkin ("CharScreen", "PlusBtnReleased"));
			}
			if (button->isPropertyPresent ("NormalImage"))
			{
				button->setProperty("NormalImage", CEGUIUtility::getImageNameWithSkin ("CharScreen", "PlusBtnReleased"));
			}
			if (button->isPropertyPresent ("PushedImage"))
			{
				button->setProperty("PushedImage", CEGUIUtility::getImageNameWithSkin ("CharScreen", "PlusBtnPressed"));
			}
			button->setPosition (pos);
			CEGUIUtility::setWidgetSizeRel (button, DEFAULT_SKILL_BTN_WIDTH, DEFAULT_SKILL_BTN_HEIGHT);
			button->setID(it->first);
			button->subscribeEvent(CEGUI::Window::EventMouseClick, CEGUI::Event::Subscriber(&SkillTree::onSkillLearnMouseClicked, this));
			
			std::string btnWidgetName = button->getNamePath ().c_str ();
			string_pos = btnWidgetName.find ("SW/GameScreen/Skilltree_Holder/");
			if (string_pos != std::string::npos)
			{
				btnWidgetName.erase (string_pos, 31);
			}
			m_skill_widgets_btns[cnt] = btnWidgetName;
			SW_DEBUG ("Added widget name (btn) [%s]", btnWidgetName.c_str ());

			//
			// Set the skill picture settings. This loads the picture and sets specific properties.
			// 
			label->setInheritsAlpha(false);
			label->setProperty("BackgroundEnabled", "false");
			label->setID(it->first);
			std::string imagename = it->second.m_image;
			if (imagename == "")
			{
				imagename = CEGUIUtility::getImageNameWithSkin ("skills", it->second.m_type);
			}
			label->setProperty("Image", imagename);
			label->subscribeEvent(CEGUI::Window::EventMouseClick, CEGUI::Event::Subscriber(&SkillTree::onSkillMouseClicked, this));
			label->subscribeEvent(CEGUIUtility::EventMouseEntersWindowArea (), CEGUI::Event::Subscriber(&SkillTree::onAbilityHover, this));
			
			//
			// Create Connections for Dependencies
			// loop over dependencies
			for (std::list< Action::ActionType >::iterator at = it->second.m_req_abilities.begin(); at != it->second.m_req_abilities.end(); ++at)
			{
				// another loop over all abilities
				std::map<int, LearnableAbility>::iterator jt;
				for (jt = player->getLearnableAbilities().begin(); jt != player->getLearnableAbilities().end(); ++jt)
				{
					if (*at == jt->second.m_type)
					{
						// found dependency
						Vector deppos = jt->second.m_skilltree_position;
						
						stream.str("");
						stream << "SkillDependencyConnection"<<m_nr_dependencies;
						label = win_mgr.createWindow (CEGUIUtility::getWidgetWithSkin (m_ceguiSkinName, "StaticImage"), stream.str());
						CEGUIUtility::addChildWidget (tabs[it->second.m_skilltree_tab-1], label);
						label->setMousePassThroughEnabled(true);
						label->setInheritsAlpha(false);
						label->setProperty("FrameEnabled", "false");
						label->setProperty("BackgroundEnabled", "false");
						label->moveToBack ();
			
						CEGUI::UVector2 start = CEGUI::UVector2 (cegui_reldim (deppos.m_x + (DEFAULT_SKILL_ITEM_WIDTH / 2) - 0.02f), cegui_reldim (deppos.m_y + DEFAULT_SKILL_ITEM_HEIGHT));
						CEGUI::UVector2 end   = CEGUI::UVector2 (cegui_reldim (spos.m_x + (DEFAULT_SKILL_ITEM_WIDTH / 2) + 0.02f), cegui_reldim (spos.m_y));
						label->setPosition(start);
						CEGUIUtility::setWidgetSize (label, end - start);
						label->setID(jt->first);
						
						// distinguish vertical and diagonal connectors
						if (deppos.m_x == spos.m_x)
						{
							label->setProperty("Image", CEGUIUtility::getImageNameWithSkin ("SumWarsExtras", "SkilltreeVertConnection")); 
						}
						else if (deppos.m_x <= spos.m_x)
						{
							label->setProperty("Image", CEGUIUtility::getImageNameWithSkin ("SumWarsExtras", "SkilltreeDiagConnection")); 
						}
						else
						{
							label->setProperty("Image", CEGUIUtility::getImageNameWithSkin ("SumWarsExtras", "SkilltreeDiag2Connection"));
							start= CEGUI::UVector2(cegui_reldim(spos.m_x+0.07f), cegui_reldim( deppos.m_y+0.1f));
							end= CEGUI::UVector2(cegui_reldim(deppos.m_x+0.06f), cegui_reldim( spos.m_y));
							label->setPosition(start);
							CEGUIUtility::setWidgetSize (label, end - start);
						}

						std::string depsWidgetName = label->getNamePath ().c_str ();
						size_t string_pos = depsWidgetName.find ("SW/GameScreen/Skilltree_Holder/");
						if (string_pos != std::string::npos)
						{
							depsWidgetName.erase (string_pos, 31);
						}
						m_skill_widgets_connections[m_nr_dependencies] = depsWidgetName;
						SW_DEBUG ("Added widget name (conn) [%s]", depsWidgetName.c_str ());

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
	std::map<int, LearnableAbility> &ablts = player->getLearnableAbilities();
	bool vis;
	Action::ActionType act;
	
	// Schleife ueber alle Skill labels
	for (int j=0;j<m_nr_skills;j++)
	{
		out_stream.str("");
		out_stream << "SkillImage" << j;
		std::string skillImageWidget = m_skill_widgets_pics[j];

		// Label welches das Skillicon zeigt
		label = CEGUIUtility::getWindowForLoadedLayoutEx (m_window, skillImageWidget);

		// A gray semi-transparent layer is added on top of the items that are unavailable
		std::string availAbility ("00FFFFFF");
		std::string unavailAbility ("CC000000");
		std::string usedColour (unavailAbility);
		
		act = ablts[j].m_type;

		if (player->checkAbility(act))
		{
			// Ability is available.
			usedColour = availAbility;
		}
		else
		{
			// Ability is unavailable.
			usedColour = unavailAbility;
		}

		if (label->isPropertyPresent ("OverlayColour"))
		{
			label->setProperty ("OverlayColour", usedColour.c_str ());
		}
		else
		{
            SW_DEBUG ("Property not found: OverlayColour (in %s)", label->getNamePath().c_str());
		}
		
		std::string skillButtonWidget = m_skill_widgets_btns[j];

		button = static_cast<CEGUI::PushButton*>(CEGUIUtility::getWindowForLoadedLayoutEx (m_window, skillButtonWidget));
		vis = player->checkAbilityLearnable(act) && (player->getSkillPoints()>0);
		if (button->isVisible() != vis)
		{
			button->setVisible(vis);
		}
	}

	// Loop over all Dependencies
	for (int j = 0; j < m_nr_dependencies; j++)
	{
		std::string skillConnWidget = m_skill_widgets_connections[j];

		// Label for the dependency connecntion
		label = CEGUIUtility::getWindowForLoadedLayoutEx (m_window, skillConnWidget);
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
	int skilllvls[20] = {
    2,   3,  5,  7, 10,
    15, 20, 25, 30, 35,
    40, 45, 50, 60, 70,
    80, 90, 100, 1000};

	int lvlid;
	for (lvlid = 0; lvlid < 20; ++lvlid)
	{
		if (player->getBaseAttr()->m_level < skilllvls[lvlid])
    {
			break;
    }
	}
	
	std::string lvlstr;
	if (lvlid < 20 && skilllvls[lvlid] != 1000)
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
	label = CEGUIUtility::getWindowForLoadedLayoutEx(
      m_window, "Skilltree/skilltree_aux/SkillTreeAdditionalInfoHolder/SkillpointsValueLabel");

	if (label->getText() != spstream.str())
	{
		label->setText(spstream.str());
	}
		
	label =  CEGUIUtility::getWindowForLoadedLayoutEx(
      m_window, "Skilltree/skilltree_aux/SkillTreeAdditionalInfoHolder/NextSkillpointsValueLabel");

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
		if (it->second < USE_SKILL_LEFT || it->second > USE_SKILL_RIGHT + 200)
    {
			continue;
    }
		
		DEBUGX("shortkey %i to %i", it->first, it->second);
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
		
		if (ablts.count(id) == 0)
    {
			continue;
    }

		act= ablts[id].m_type;
		
		stream.str("");
		stream << "SkillShortkeyLabel";
		stream << nr;
		
		if (nr >= m_shortkey_labels)
		{
			m_shortkey_labels ++;
			
			std::map<int, std::string>::iterator it = m_skill_widgets_shortcuts.find (nr);
			if (it != m_skill_widgets_shortcuts.end ())
			{
				label = CEGUIUtility::getWindowForLoadedLayoutEx (m_window, it->second);
			}
			else
			{
				label = win_mgr.createWindow (CEGUIUtility::getWidgetWithSkin (m_ceguiSkinName, "StaticText"), stream.str());
			}

			label->setInheritsAlpha(false);
			label->setProperty("FrameEnabled", "false");
			label->setProperty("BackgroundEnabled", "false");
			label->setProperty("HorzFormatting", "Centred");
			label->setID(-1);
			label->setText("");
			CEGUIUtility::setWidgetSizeRel (label, 0.07f, 0.05f);
			label->setAlpha(1.0);
			label->setMousePassThroughEnabled(true);
			label->setAlwaysOnTop(true);

			std::string shortkeywidget = label->getNamePath ().c_str ();
			size_t string_pos = shortkeywidget.find ("SW/GameScreen/Skilltree_Holder/");
			if (string_pos != std::string::npos)
			{
				shortkeywidget.erase (string_pos, 31);
			}
			m_skill_widgets_shortcuts[nr] = shortkeywidget;
		}
		else
		{
			std::string shortkeywidget = m_skill_widgets_shortcuts[nr];
			label = CEGUIUtility::getWindowForLoadedLayoutEx (m_window, shortkeywidget);
		}
		
		std::string skillImageWidget = m_skill_widgets_pics[id];
		label2 = CEGUIUtility::getWindowForLoadedLayoutEx (m_window, skillImageWidget);
		
		
		if (label2->getParent() != label->getParent())
		{
			SW_DEBUG ("Reassigning siblings: [%s], to image widgets [%s]", label->getNamePath ().c_str (), label2->getNamePath ().c_str ());

			if (label->getParent() != 0)
			{
				CEGUIUtility::removeChildWidget (label->getParent(), label);
			}
			CEGUIUtility::addChildWidget (label2->getParent(), label);
			
			// Re-calculate the name of the widget. 
			std::string shortkeywidget = label->getNamePath ().c_str ();
			size_t string_pos = shortkeywidget.find ("SW/GameScreen/Skilltree_Holder/");
			if (string_pos != std::string::npos)
			{
				shortkeywidget.erase (string_pos, 31);
			}
			m_skill_widgets_shortcuts[nr] = shortkeywidget;
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
				pos += CEGUI::UVector2(cegui_reldim(0.12f), cegui_reldim( 0.01f));
			}
			else
			{
				pos += CEGUI::UVector2(cegui_reldim(0.01f), cegui_reldim( 0.01f));
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
		//stream.str("");
		//stream << "SkillShortkeyLabel";
		//stream << nr;
		std::string shortkeywidget = m_skill_widgets_shortcuts[nr];
		label = CEGUIUtility::getWindowForLoadedLayoutEx (m_window, shortkeywidget);

		label->setVisible(false);
	}
	
}


void SkillTree::updateTranslation()
{
	std::vector<CEGUI::DefaultWindow*> tabs(m_nr_tabs);
	
	std::stringstream stream;
	
	for (int i = 0; i < m_nr_tabs; i++)
	{
		stream.str("");
		stream << "Skilltree/skilltree_aux/SkilltreeTabs/__auto_TabPane__/";
		stream << "SkilltreeTab" << i;
			
		tabs[i] = (CEGUI::DefaultWindow*) CEGUIUtility::getWindowForLoadedLayoutEx (m_window, stream.str());
	}
	
	// Ueberschriften der Tabs setzen
	Player* player = m_document->getLocalPlayer();
	if (m_nr_tabs < 3)
  {
		return;
  }
	
	PlayerBasicData* pdata = ObjectFactory::getPlayerData(player->getSubtype());
	CEGUI::Window* label;
	for (int i = 0; i < 3; i++)
	{
		tabs[i]->setText((CEGUI::utf8*) gettext(pdata->m_tabnames[i].c_str()));
	}

	label =  CEGUIUtility::getWindowForLoadedLayoutEx (m_window, "Skilltree/skilltree_aux/SkillTreeAdditionalInfoHolder/SkillpointsLabel");
	label->setText((CEGUI::utf8*) gettext("Skillpoints"));
		
	label =  CEGUIUtility::getWindowForLoadedLayoutEx (m_window, "Skilltree/skilltree_aux/SkillTreeAdditionalInfoHolder/NextSkillpointsLabel");
	label->setText((CEGUI::utf8*) gettext("Next Skillpoint at Level"));

	label =  CEGUIUtility::getWindowForLoadedLayoutEx (m_window, "Skilltree");
	if (label->isPropertyPresent ("Text"))
	{
		label->setProperty ("Text", (CEGUI::utf8*) gettext("Skilltree"));
	}

}

void SkillTree::updateAbilityTooltip(unsigned int pos)
{
	Player* player = m_document->getLocalPlayer();
	std::map<int, LearnableAbility> &ablts = player->getLearnableAbilities();
	
	
	DEBUGX("update tooltip for %i %s", pos, ablts[pos].m_type.c_str());

	CEGUI::Window* label;
	//std::ostringstream out_stream;
	//out_stream.str("");
	//out_stream << "SkillImage" << pos;
	std::string skillImageWidget = m_skill_widgets_pics[pos];

	
	label = CEGUIUtility::getWindowForLoadedLayoutEx (m_window, skillImageWidget);
	std::string tooltip = m_document->getAbilityDescription(ablts[pos].m_type);
	
	FormatedText txt =  CEGUIUtility::fitTextToWindow((CEGUI::utf8*)tooltip.c_str(), 400, CEGUIUtility::Centred, label->getFont());

	TooltipManager::getSingleton().createTooltip(label, CEGUIUtility::getColourizedString(CEGUIUtility::White, txt.text.c_str(), CEGUIUtility::White));
	
}

bool SkillTree::onSkillMouseClicked(const CEGUI::EventArgs& evt)
{
	Player* player = m_document->getLocalPlayer();
	std::map<int, LearnableAbility> &ablts = player->getLearnableAbilities();
	
	const CEGUI::MouseEventArgs& we =
			static_cast<const CEGUI::MouseEventArgs&>(evt);
	unsigned int id = we.window->getID();

	if (we.button == CEGUI::LeftButton)
	{
		m_document->setLeftAction(ablts[id].m_type);
	}

	if (we.button == CEGUI::RightButton)
	{
		DEBUGX("right button pressed on skill %i", id);
		m_document->setRightAction(ablts[id].m_type);
	}
	
	return true;
	
}

bool SkillTree::onSkillLearnMouseClicked(const CEGUI::EventArgs& evt)
{
	Player* player = m_document->getLocalPlayer();
	std::map<int, LearnableAbility> &ablts = player->getLearnableAbilities();
	
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
	std::map<int, LearnableAbility> &ablts = player->getLearnableAbilities();
	
	const CEGUI::MouseEventArgs& we =
			static_cast<const CEGUI::MouseEventArgs&>(evt);
	unsigned int id = we.window->getID();
	DEBUGX("mouse entered Ability %i", id);
	updateAbilityTooltip(id);
	m_document->getGUIState()->m_hover_ability = ablts[id].m_type;
	
	return true;
}

bool SkillTree::onAbilityHoverLeave(const CEGUI::EventArgs& evt)
{
	m_document->getGUIState()->m_hover_ability = "noaction";
	return true;
}

