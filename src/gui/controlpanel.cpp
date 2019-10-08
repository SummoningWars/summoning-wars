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


#include "controlpanel.h"
#include "player.h"

ControlPanel::ControlPanel (Document* doc)
	: ItemWindow(doc)
{
	CEGUI::WindowManager& win_mgr = CEGUI::WindowManager::getSingleton();

	std::string name;

	// Rahmen fuer die untere Kontrollleiste
	CEGUI::FrameWindow* ctrl_panel = (CEGUI::FrameWindow*) win_mgr.loadWindowLayout("ControlPanel.layout");
	m_window = ctrl_panel;
	
	ctrl_panel->subscribeEvent(CEGUI::Window::EventMouseButtonDown, CEGUI::Event::Subscriber(&Window::consumeEvent, (Window*) this));
	ctrl_panel->setWantsMultiClickEvents(false);


	// Bestandteile der Kontrollleiste hinzufuegen
	CEGUI::PushButton* btn;
	CEGUI::Window* label;

	// Balken fuer HP
	CEGUI::ProgressBar* bar = static_cast<CEGUI::ProgressBar*>(win_mgr.getWindow("HealthProgressBar"));
	bar->setWantsMultiClickEvents(false);
	bar->setProgress(0.0);
	
	// Balken fuer Experience
	bar = static_cast<CEGUI::ProgressBar*>(win_mgr.getWindow("ExperienceProgressBar"));
	bar->setWantsMultiClickEvents(false);
	bar->setProgress(0.0);


	// Button Inventar
	btn = static_cast<CEGUI::PushButton*>(win_mgr.getWindow("InventoryButton"));
	btn->subscribeEvent(CEGUI::PushButton::EventClicked, CEGUI::Event::Subscriber(&ControlPanel::onButtonInventoryClicked, this));
	btn->setWantsMultiClickEvents(false);

	// Button Charakterinfo
	btn = static_cast<CEGUI::PushButton*>(win_mgr.getWindow("CharInfoButton"));
	btn->setWantsMultiClickEvents(false);
	btn->subscribeEvent(CEGUI::PushButton::EventClicked, CEGUI::Event::Subscriber(&ControlPanel::onButtonCharInfoClicked, this));
	

	// Button Chat oeffnen
	btn = static_cast<CEGUI::PushButton*>(win_mgr.getWindow("ChatOpenButton"));
	btn->subscribeEvent(CEGUI::PushButton::EventClicked, CEGUI::Event::Subscriber(&ControlPanel::onButtonOpenChatClicked, this));
	btn->setWantsMultiClickEvents(false);

	// Button SkillTree
	btn = static_cast<CEGUI::PushButton*>(win_mgr.getWindow("SkillTreeButton"));
	btn->setID(0);
	btn->setWantsMultiClickEvents(false);
	btn->subscribeEvent(CEGUI::PushButton::EventClicked, CEGUI::Event::Subscriber(&ControlPanel::onButtonSkilltreeClicked, this));
	

	// Button Party
	btn = static_cast<CEGUI::PushButton*>(win_mgr.getWindow("PartyButton"));
	btn->setWantsMultiClickEvents(false);
	btn->subscribeEvent(CEGUI::PushButton::EventClicked, CEGUI::Event::Subscriber(&ControlPanel::onButtonPartyClicked, this));
	
	// Button Party
	btn = static_cast<CEGUI::PushButton*>(win_mgr.getWindow("QuestInfoButton"));
	//btn->setText("Q");
	btn->setWantsMultiClickEvents(false);
	btn->subscribeEvent(CEGUI::PushButton::EventClicked, CEGUI::Event::Subscriber(&ControlPanel::onButtonQuestInfoClicked, this));
	
	// Button Speichern und Beenden
	btn = static_cast<CEGUI::PushButton*>(win_mgr.getWindow("SaveExitButton"));
	btn->setWantsMultiClickEvents(false);
	btn->subscribeEvent(CEGUI::PushButton::EventClicked, CEGUI::Event::Subscriber(&ControlPanel::onButtonSaveExitClicked, this));

	// Button Optionen
	btn = static_cast<CEGUI::PushButton*>(win_mgr.getWindow("OptionsButton"));
	btn->setWantsMultiClickEvents(false);
	btn->subscribeEvent(CEGUI::PushButton::EventClicked, CEGUI::Event::Subscriber(&ControlPanel::onButtonOptionsClicked, this));
	
	// Anzeige linke Maustaste Faehigkeit
	label = win_mgr.getWindow("LeftClickAbilityImage");
	label->setID(1);
	label->subscribeEvent(CEGUI::Window::EventMouseClick, CEGUI::Event::Subscriber(&ControlPanel::onButtonSkilltreeClicked, this));

	// Balken fuer linke Maustaste Faehigkeit
	bar = static_cast<CEGUI::ProgressBar*>(win_mgr.getWindow("LeftClickAbilityProgressBar"));
	bar->setWantsMultiClickEvents(false);
	bar->setProgress(0.0);
	bar->setAlpha(0.5);
	bar->setAlwaysOnTop(true);

	// Anzeige rechte Maustaste Faehigkeit
	label = win_mgr.getWindow("RightClickAbilityImage");
	label->setID(2);
	label->subscribeEvent(CEGUI::Window::EventMouseClick, CEGUI::Event::Subscriber(&ControlPanel::onButtonSkilltreeClicked, this));

	// Balken fuer rechte Maustaste Faehigkeit
	bar = static_cast<CEGUI::ProgressBar*>(win_mgr.getWindow("RightClickAbilityProgressBar"));
	bar->setWantsMultiClickEvents(false);
	bar->setProgress(0.0);
	bar->setAlpha(0.5);
	bar->setAlwaysOnTop(true);

	// alternate right mouse skill
	label = win_mgr.getWindow("AlternateRightClickAbilityImage");
	label->setID(3);
	label->subscribeEvent(CEGUI::Window::EventMouseClick, CEGUI::Event::Subscriber(&ControlPanel::onButtonSkilltreeClicked, this));

	// progress bar for alternate right mouse skill
	bar = static_cast<CEGUI::ProgressBar*>(win_mgr.getWindow("AlternateRightClickAbilityProgressBar"));
	bar->setWantsMultiClickEvents(false);
	bar->setProgress(0.0);
	bar->setAlpha(0.5);
	bar->setAlwaysOnTop(true);
	
	// Inventar Guertel
	int i;
	std::ostringstream outStream;
	for (i=0;i<Equipement::getMaxBeltItemNumber();i++)
	{
		outStream.str("");
		outStream << "InventoryItem" << i;
		label = win_mgr.getWindow(outStream.str());
		label->setID(Equipement::BELT_ITEMS+i);
		label->subscribeEvent(CEGUI::Window::EventMouseButtonDown, CEGUI::Event::Subscriber(&ControlPanel::onItemMouseButtonPressed, (ItemWindow*) this));
		label->subscribeEvent(CEGUI::Window::EventMouseButtonUp, CEGUI::Event::Subscriber(&ControlPanel::onItemMouseButtonReleased, (ItemWindow*) this));
		label->subscribeEvent(CEGUI::Window::EventMouseEnters, CEGUI::Event::Subscriber(&ControlPanel::onItemHover, (ItemWindow*) this));
		label->setWantsMultiClickEvents(false);
		
		outStream.str("");
		outStream << "InventoryShortcutLabel" << i;
		label = win_mgr.getWindow(outStream.str());
		std::stringstream stream;
		stream << (i+1)%10;
		label->setText(stream.str());
		label->setAlwaysOnTop(true);
		label->setMousePassThroughEnabled(true);
		
		outStream.str("");
		outStream << "BeltPotionCounter" << i;
		label = win_mgr.getWindow(outStream.str());
		label->setAlwaysOnTop(true);
		label->setMousePassThroughEnabled(true);
	}

	
	updateTranslation();
}

void ControlPanel::update()
{
		// Fenstermanager
	CEGUI::WindowManager& win_mgr = CEGUI::WindowManager::getSingleton();
	CEGUI::Window* label, *img;
	std::ostringstream out_stream;
	int timernr;
	float perc=0;
	std::string name;

	Player* player = m_document->getLocalPlayer();

	// Balken fuer HP
	CEGUI::ProgressBar* bar = static_cast<CEGUI::ProgressBar*>(win_mgr.getWindow( "HealthProgressBar"));
	float hperc = player->getDynAttr()->m_health / player->getBaseAttrMod()->m_max_health;
	if (bar->getProgress() != hperc)
	{
		bar->setProgress(hperc);
	}
	out_stream.str("");
	out_stream << gettext("Hitpoints");
	out_stream << ": ";
	out_stream << (int) player->getDynAttr()->m_health<<"/"<< (int) player->getBaseAttrMod()->m_max_health;
	if (bar->getTooltipText() != (CEGUI::utf8*) out_stream.str().c_str())
	{
		bar->setTooltipText((CEGUI::utf8*) out_stream.str().c_str());
	}
	
	// Balken fuer Experience
	bar = static_cast<CEGUI::ProgressBar*>(win_mgr.getWindow( "ExperienceProgressBar"));
	float eperc = player->getDynAttr()->m_experience / player->getBaseAttr()->m_max_experience;
	if (bar->getProgress() != eperc)
	{
		bar->setProgress(eperc);
	}
	out_stream.str("");
	out_stream << gettext("Experience");
	out_stream << ": ";
	out_stream << (int) player->getDynAttr()->m_experience<<"/"<<(int) player->getBaseAttr()->m_max_experience;
	if (bar->getTooltipText() != (CEGUI::utf8*) out_stream.str().c_str())
	{
		bar->setTooltipText((CEGUI::utf8*) out_stream.str().c_str());
	}
	
	std::map<int,LearnableAbility>::iterator iter;
	std::map<int,LearnableAbility>& ablt = player->getLearnableAbilities();
	
	// Image Schaden Attacke links
	label =  win_mgr.getWindow( "LeftClickAbilityImage");
	name = player->getLeftAction();
	
	std::string imagename = "";
	for (iter = ablt.begin(); iter != ablt.end(); ++iter)
	{
		if (iter->second.m_type == name)
		{
			imagename = iter->second.m_image;
		}
	}
	
	std::string tooltip;
	tooltip= m_document->getAbilityDescription(name);
	if (label->getTooltipText() != (CEGUI::utf8*) tooltip.c_str())
	{
		label->setTooltipText((CEGUI::utf8*) tooltip.c_str());
	}
	
	if (imagename == "")
	{
		imagename = "set:skills image:";
		imagename += name;
	}
	if (imagename != label->getProperty("Image"))
	{
		label->setProperty("Image",imagename);
	}

	// Balken fuer Schaden Attacke links
	bar = static_cast<CEGUI::ProgressBar*>(win_mgr.getWindow( "LeftClickAbilityProgressBar"));
	timernr =  Action::getActionInfo(player->getLeftAction())->m_timer_nr;
	perc = player->getTimerPercent(timernr);

	if (bar->getProgress() != perc)
	{
		bar->setProgress(perc);
	}

	perc =0;

	// Image Attacke rechts
	label =  win_mgr.getWindow( "RightClickAbilityImage");
	name = player->getRightAction();
	for (iter = ablt.begin(); iter != ablt.end(); ++iter)
	{
		if (iter->second.m_type == name)
		{
			imagename = iter->second.m_image;
		}
	}
	
	tooltip= m_document->getAbilityDescription(name);
	if (label->getTooltipText() != (CEGUI::utf8*) tooltip.c_str())
	{
		label->setTooltipText((CEGUI::utf8*) tooltip.c_str());
	}
	
	if (imagename == "")
	{
		imagename = "set:skills image:";
		imagename += name;
	}
	if (imagename != label->getProperty("Image"))
	{
		label->setProperty("Image",imagename);
	}

	// Balken fuer Schaden Attacke rechts
	bar = static_cast<CEGUI::ProgressBar*>(win_mgr.getWindow( "RightClickAbilityProgressBar"));
	timernr =  Action::getActionInfo(player->getRightAction())->m_timer_nr;
	perc = player->getTimerPercent(timernr);

	if (bar->getProgress() != perc)
	{
		bar->setProgress(perc);
	}
	
	
	// image for alternate right skill
	label =  win_mgr.getWindow( "AlternateRightClickAbilityImage");
	name = player->getRightAlternateAction();
	for (iter = ablt.begin(); iter != ablt.end(); ++iter)
	{
		if (iter->second.m_type == name)
		{
			imagename = iter->second.m_image;
		}
	}
	
	tooltip= m_document->getAbilityDescription(name);
	if (label->getTooltipText() != (CEGUI::utf8*) tooltip.c_str())
	{
		label->setTooltipText((CEGUI::utf8*) tooltip.c_str());
	}
	
	if (imagename == "")
	{
		imagename = "set:skills image:";
		imagename += name;
	}
	if (imagename != label->getProperty("Image"))
	{
		label->setProperty("Image",imagename);
	}

	// Balken fuer Schaden Attacke rechts
	bar = static_cast<CEGUI::ProgressBar*>(win_mgr.getWindow( "AlternateRightClickAbilityProgressBar"));
	timernr =  Action::getActionInfo(player->getRightAlternateAction())->m_timer_nr;
	perc = player->getTimerPercent(timernr);

	if (bar->getProgress() != perc)
	{
		bar->setProgress(perc);
	}

	Item* it;
	Equipement* equ = player->getEquipement();

	// Guertel
	for (int i=0;i<equ->getMaxBeltItemNumber();i++)
	{
		out_stream.str("");
		out_stream << "InventoryItem" << i;
		img =  win_mgr.getWindow(out_stream.str().c_str());
		it = equ->getItem(Equipement::BELT_ITEMS+i);

		updateItemWindow(img, it, player);
		
		out_stream.str("");
		out_stream << "BeltPotionCounter" << i;
		label = win_mgr.getWindow(out_stream.str());
		if (it == 0)
		{
			if (label->isVisible())
				label->setVisible(false);
			continue;
		}
		
		int count = equ->getNumSmallItemsOfType(it->m_subtype);
		if (count == 1)
		{
			if (label->isVisible())
				label->setVisible(false);
			continue;
		}
		
		out_stream.str("");
		out_stream << "("<<count << ")";
		if (! label->isVisible())
			label->setVisible(true);
		if (label->getText() != out_stream.str().c_str())
		{
			label->setText(out_stream.str().c_str());
		}
		
		label->setAlwaysOnTop(true);
		label->setMousePassThroughEnabled(true);
	}
}

void ControlPanel::updateTranslation()
{
	CEGUI::WindowManager& win_mgr = CEGUI::WindowManager::getSingleton();
	CEGUI::PushButton* btn;

	btn = static_cast<CEGUI::PushButton*>(win_mgr.getWindow("OptionsButton"));
	btn->setTooltipText((CEGUI::utf8*) gettext("Options"));
	
	btn = static_cast<CEGUI::PushButton*>(win_mgr.getWindow("SaveExitButton"));
	btn->setTooltipText((CEGUI::utf8*) gettext("Save & Exit"));
	
	btn = static_cast<CEGUI::PushButton*>(win_mgr.getWindow("ChatOpenButton"));
	btn->setTooltipText((CEGUI::utf8*) gettext("Chat"));
	
	btn = static_cast<CEGUI::PushButton*>(win_mgr.getWindow("InventoryButton"));
	btn->setTooltipText((CEGUI::utf8*) gettext("Inventory"));
	
	btn = static_cast<CEGUI::PushButton*>(win_mgr.getWindow("CharInfoButton"));
	btn->setTooltipText((CEGUI::utf8*) gettext("Character"));

	btn = static_cast<CEGUI::PushButton*>(win_mgr.getWindow("SkillTreeButton"));
	btn->setTooltipText((CEGUI::utf8*) gettext("Skilltree"));
	
	btn = static_cast<CEGUI::PushButton*>(win_mgr.getWindow( "PartyButton"));
	btn->setTooltipText((CEGUI::utf8*) gettext("Party"));
	
	btn = static_cast<CEGUI::PushButton*>(win_mgr.getWindow( "QuestInfoButton"));
	btn->setTooltipText((CEGUI::utf8*) gettext("Quests"));

	
}

bool ControlPanel::onButtonSaveExitClicked(const CEGUI::EventArgs& evt)
{
	m_document->onButtonSaveExitClicked();
	return true;
}

bool ControlPanel::onButtonInventoryClicked(const CEGUI::EventArgs& evt)
{
	m_document->onButtonInventoryClicked();
	return true;
}

bool ControlPanel::onButtonCharInfoClicked(const CEGUI::EventArgs& evt)
{
	m_document->onButtonCharInfoClicked();
	return true;
}

bool ControlPanel::onButtonSkilltreeClicked(const CEGUI::EventArgs& evt)
{
	const CEGUI::MouseEventArgs& we =
			static_cast<const CEGUI::MouseEventArgs&>(evt);
	unsigned int id = we.window->getID();
	
	//ID 2 is right mouse button skill, ID 3 is right mouse button alternate skill
	bool right = (id ==2 || id == 3);
	bool alternate = (id == 3);
	
	m_document->onButtonSkilltreeClicked(right, alternate);
	return true;
}

bool ControlPanel::onButtonPartyClicked(const CEGUI::EventArgs& evt)
{
	m_document->onButtonPartyInfoClicked();
	return true;
}

bool ControlPanel::onButtonOpenChatClicked(const CEGUI::EventArgs& evt)
{
	m_document->onButtonOpenChatClicked();
	return true;
}

bool ControlPanel::onButtonQuestInfoClicked(const CEGUI::EventArgs& evt)
{
	m_document->onButtonQuestInfoClicked();
	return true;
}

bool ControlPanel::onButtonOptionsClicked(const CEGUI::EventArgs& evt)
{
	m_document->onButtonOptionsClicked();
	return true;
}

