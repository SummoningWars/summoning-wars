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

// Sound operations helper.
#include "soundhelper.h"

// Utility for CEGUI cross-version compatibility
#include "ceguiutility.h"

ControlPanel::ControlPanel (Document* doc)
	: ItemWindow(doc)
{
	std::string name;

	// Load the layout for the panel that the be used to control various actions
	CEGUI::Window* ctrl_panel = CEGUIUtility::loadLayoutFromFile ("controlpanel.layout");
	if (!ctrl_panel)
	{
		SW_DEBUG ("WARNING: Failed to load [%s]", "controlpanel.layout");
	}

	CEGUI::Window* ctrl_panel_holder = CEGUIUtility::loadLayoutFromFile ("controlpanel_holder.layout");
	if (!ctrl_panel_holder)
	{
		SW_DEBUG ("WARNING: Failed to load [%s]", "characterscreen_holder.layout");
	}
	
	CEGUI::Window* wndHolder = CEGUIUtility::getWindowForLoadedLayoutEx (ctrl_panel_holder, "ControlPanel_Holder");
	CEGUI::Window* wndCharInfo = CEGUIUtility::getWindowForLoadedLayoutEx (ctrl_panel, "ControlPanel");
	if (wndHolder && wndCharInfo)
	{
		CEGUIUtility::addChildWidget (wndHolder, wndCharInfo);
	}
	else
	{
		if (!wndHolder) SW_DEBUG ("ERROR: Unable to get the window holder for control panel.");
		if (!wndCharInfo) SW_DEBUG ("ERROR: Unable to get the window for control panel.");
	}

	m_window = ctrl_panel_holder;
	
	ctrl_panel->subscribeEvent(CEGUI::Window::EventMouseButtonDown, CEGUI::Event::Subscriber(&Window::consumeEvent, (Window*) this));
	ctrl_panel->setWantsMultiClickEvents(false);


	// Bestandteile der Kontrollleiste hinzufuegen
	CEGUI::PushButton* btn;
	CEGUI::Window* label;

	// Balken fuer HP
	CEGUI::ProgressBar* bar = static_cast<CEGUI::ProgressBar*>(CEGUIUtility::getWindowForLoadedLayoutEx (m_window, "ControlPanel/CPContainer/GlobeAndSysBtnsHolder/HealthProgressBar"));
	bar->setWantsMultiClickEvents(false);
	bar->setProgress(0.0);
	
	// Balken fuer Experience
	bar = static_cast<CEGUI::ProgressBar*>(CEGUIUtility::getWindowForLoadedLayoutEx (m_window, "ControlPanel/ExperienceProgressBar"));
	bar->setWantsMultiClickEvents(false);
	bar->setProgress(0.0);


	// Button Inventar
	btn = static_cast<CEGUI::PushButton*>(CEGUIUtility::getWindowForLoadedLayoutEx (m_window, "ControlPanel/CPContainer/CP_Btns/CP_SysBtns/InventoryButton"));
	btn->subscribeEvent(CEGUI::PushButton::EventClicked, CEGUI::Event::Subscriber(&ControlPanel::onButtonInventoryClicked, this));
	btn->subscribeEvent(CEGUIUtility::EventMouseEntersPushButtonArea (), CEGUI::Event::Subscriber(&ControlPanel::onGUIItemHover, this));
	btn->setWantsMultiClickEvents(false);

	// Button Charakterinfo
	btn = static_cast<CEGUI::PushButton*>(CEGUIUtility::getWindowForLoadedLayoutEx (m_window, "ControlPanel/CPContainer/CP_Btns/CP_SysBtns/CharInfoButton"));
	btn->setWantsMultiClickEvents(false);
	btn->subscribeEvent(CEGUI::PushButton::EventClicked, CEGUI::Event::Subscriber(&ControlPanel::onButtonCharInfoClicked, this));
	btn->subscribeEvent(CEGUIUtility::EventMouseEntersPushButtonArea (), CEGUI::Event::Subscriber(&ControlPanel::onGUIItemHover, this));
	

	// Button Chat oeffnen
	btn = static_cast<CEGUI::PushButton*>(CEGUIUtility::getWindowForLoadedLayoutEx (m_window, "ControlPanel/CPContainer/CP_Btns/CP_SysBtns/ChatOpenButton"));
	btn->subscribeEvent(CEGUI::PushButton::EventClicked, CEGUI::Event::Subscriber(&ControlPanel::onButtonOpenChatClicked, this));
	btn->setWantsMultiClickEvents(false);
	btn->subscribeEvent(CEGUIUtility::EventMouseEntersPushButtonArea (), CEGUI::Event::Subscriber(&ControlPanel::onGUIItemHover, this));

	// Button SkillTree
	btn = static_cast<CEGUI::PushButton*>(CEGUIUtility::getWindowForLoadedLayoutEx (m_window, "ControlPanel/CPContainer/CP_Btns/CP_SysBtns/SkillTreeButton"));
	btn->setID(0);
	btn->setWantsMultiClickEvents(false);
	btn->subscribeEvent(CEGUI::PushButton::EventClicked, CEGUI::Event::Subscriber(&ControlPanel::onButtonSkilltreeClicked, this));
	btn->subscribeEvent(CEGUIUtility::EventMouseEntersPushButtonArea (), CEGUI::Event::Subscriber(&ControlPanel::onGUIItemHover, this));
	

	// Button Party
	btn = static_cast<CEGUI::PushButton*>(CEGUIUtility::getWindowForLoadedLayoutEx (m_window, "ControlPanel/CPContainer/CP_Btns/CP_SysBtns/PartyButton"));
	btn->setWantsMultiClickEvents(false);
	btn->subscribeEvent(CEGUI::PushButton::EventClicked, CEGUI::Event::Subscriber(&ControlPanel::onButtonPartyClicked, this));
	btn->subscribeEvent(CEGUIUtility::EventMouseEntersPushButtonArea (), CEGUI::Event::Subscriber(&ControlPanel::onGUIItemHover, this));
	
	// Quest Information Button
	btn = static_cast<CEGUI::PushButton*>(CEGUIUtility::getWindowForLoadedLayoutEx (m_window, "ControlPanel/CPContainer/CP_Btns/CP_SysBtns/QuestInfoButton"));
	btn->setWantsMultiClickEvents(false);
	btn->subscribeEvent(CEGUI::PushButton::EventClicked, CEGUI::Event::Subscriber(&ControlPanel::onButtonQuestInfoClicked, this));
	btn->subscribeEvent(CEGUIUtility::EventMouseEntersPushButtonArea (), CEGUI::Event::Subscriber(&ControlPanel::onGUIItemHover, this));
	
	// Button Speichern und Beenden
	btn = static_cast<CEGUI::PushButton*>(CEGUIUtility::getWindowForLoadedLayoutEx (m_window, "ControlPanel/CPContainer/GlobeAndSysBtnsHolder/SaveExitButton"));
	btn->setWantsMultiClickEvents(false);
	btn->subscribeEvent(CEGUI::PushButton::EventClicked, CEGUI::Event::Subscriber(&ControlPanel::onButtonSaveExitClicked, this));
	btn->subscribeEvent(CEGUIUtility::EventMouseEntersPushButtonArea (), CEGUI::Event::Subscriber(&ControlPanel::onGUIItemHover, this));

	// Button Optionen
	btn = static_cast<CEGUI::PushButton*>(CEGUIUtility::getWindowForLoadedLayoutEx (m_window, "ControlPanel/CPContainer/GlobeAndSysBtnsHolder/OptionsButton"));
	btn->setWantsMultiClickEvents(false);
	btn->subscribeEvent(CEGUI::PushButton::EventClicked, CEGUI::Event::Subscriber(&ControlPanel::onButtonOptionsClicked, this));
	btn->subscribeEvent(CEGUIUtility::EventMouseEntersPushButtonArea (), CEGUI::Event::Subscriber(&ControlPanel::onGUIItemHover, this));

	// Anzeige linke Maustaste Faehigkeit
	label = CEGUIUtility::getWindowForLoadedLayoutEx (m_window, "ControlPanel/CPContainer/CP_Btns/CP_SkillBtns/LeftClickAbilityImage");
	label->setID(1);
	label->subscribeEvent(CEGUI::Window::EventMouseClick, CEGUI::Event::Subscriber(&ControlPanel::onButtonSkilltreeClicked, this));
	label->subscribeEvent(CEGUIUtility::EventMouseEntersPushButtonArea (), CEGUI::Event::Subscriber(&ControlPanel::onGUIItemHover, this));

	// Balken fuer linke Maustaste Faehigkeit
	bar = static_cast<CEGUI::ProgressBar*>(CEGUIUtility::getWindowForLoadedLayoutEx (m_window, "ControlPanel/CPContainer/CP_Btns/CP_SkillBtns/LeftClickAbilityProgressBar"));
	bar->setWantsMultiClickEvents(false);
	bar->setProgress(0.0);
	bar->setAlpha(0.5);
	bar->setAlwaysOnTop(true);

	// Anzeige rechte Maustaste Faehigkeit
	label = CEGUIUtility::getWindowForLoadedLayoutEx (m_window, "ControlPanel/CPContainer/CP_Btns/CP_SkillBtns/RightClickAbilityImage");
	label->setID(2);
	label->subscribeEvent(CEGUI::Window::EventMouseClick, CEGUI::Event::Subscriber(&ControlPanel::onButtonSkilltreeClicked, this));
	label->subscribeEvent(CEGUIUtility::EventMouseEntersPushButtonArea (), CEGUI::Event::Subscriber(&ControlPanel::onGUIItemHover, this));

	// Balken fuer rechte Maustaste Faehigkeit
	bar = static_cast<CEGUI::ProgressBar*>(CEGUIUtility::getWindowForLoadedLayoutEx (m_window, "ControlPanel/CPContainer/CP_Btns/CP_SkillBtns/RightClickAbilityProgressBar"));
	bar->setWantsMultiClickEvents(false);
	bar->setProgress(0.0);
	bar->setAlpha(0.5);
	bar->setAlwaysOnTop(true);

	// alternate right mouse skill
	label = CEGUIUtility::getWindowForLoadedLayoutEx (m_window, "ControlPanel/CPContainer/CP_Btns/CP_SkillBtns/AlternateRightClickAbilityImage");
	label->setID(3);
	label->subscribeEvent(CEGUI::Window::EventMouseClick, CEGUI::Event::Subscriber(&ControlPanel::onButtonSkilltreeClicked, this));
	label->subscribeEvent(CEGUIUtility::EventMouseEntersPushButtonArea (), CEGUI::Event::Subscriber(&ControlPanel::onGUIItemHover, this));

	// progress bar for alternate right mouse skill
	bar = static_cast<CEGUI::ProgressBar*>(CEGUIUtility::getWindowForLoadedLayoutEx (m_window, "ControlPanel/CPContainer/CP_Btns/CP_SkillBtns/AlternateRightClickAbilityProgressBar"));
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
		outStream << "ControlPanel/CPContainer/CP_Btns/PotionHolder/";
		outStream << "InventoryItem" << i;
		label = CEGUIUtility::getWindowForLoadedLayoutEx (m_window, outStream.str());
		label->setID(Equipement::BELT_ITEMS+i);
		label->subscribeEvent(CEGUI::Window::EventMouseButtonDown, CEGUI::Event::Subscriber(&ControlPanel::onItemMouseButtonPressed, (ItemWindow*) this));
		label->subscribeEvent(CEGUI::Window::EventMouseButtonUp, CEGUI::Event::Subscriber(&ControlPanel::onItemMouseButtonReleased, (ItemWindow*) this));
		label->subscribeEvent(CEGUIUtility::EventMouseEntersWindowArea (), CEGUI::Event::Subscriber(&ControlPanel::onItemHover, (ItemWindow*) this));
		label->setWantsMultiClickEvents(false);
		
		outStream.str("");
		outStream << "ControlPanel/CPContainer/CP_Btns/PotionHolder/";
		outStream << "InventoryShortcutLabel" << i;
		label = CEGUIUtility::getWindowForLoadedLayoutEx (m_window, outStream.str());
		std::stringstream stream;
		stream << (i+1)%10;
		label->setText(stream.str());
		label->setAlwaysOnTop(true);
		label->setMousePassThroughEnabled(true);
		
		outStream.str("");
		outStream << "ControlPanel/CPContainer/CP_Btns/PotionHolder/";
		outStream << "BeltPotionCounter" << i;
		label = CEGUIUtility::getWindowForLoadedLayoutEx (m_window, outStream.str());
		label->setAlwaysOnTop(true);
		label->setMousePassThroughEnabled(true);
	}

	createAnimations ();
	
	updateTranslation();
}

void ControlPanel::update()
{
	CEGUI::Window* label, *img;
	std::ostringstream out_stream;
	int timernr;
	float perc=0;
	std::string name;

	Player* player = m_document->getLocalPlayer();
	m_silent_current_update = m_silent;

	// Balken fuer HP
	CEGUI::ProgressBar* bar = static_cast<CEGUI::ProgressBar*>(CEGUIUtility::getWindowForLoadedLayoutEx (m_window, "ControlPanel/CPContainer/GlobeAndSysBtnsHolder/HealthProgressBar"));
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
	bar = static_cast<CEGUI::ProgressBar*>(CEGUIUtility::getWindowForLoadedLayoutEx (m_window, "ControlPanel/ExperienceProgressBar"));
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
	label =  CEGUIUtility::getWindowForLoadedLayoutEx (m_window, "ControlPanel/CPContainer/CP_Btns/CP_SkillBtns/LeftClickAbilityImage");
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
		imagename = CEGUIUtility::getImageNameWithSkin ("skills", name);
	}
	if (imagename != label->getProperty("Image"))
	{
		label->setProperty("Image",imagename);
	}

	double alpha;

	// Balken fuer Schaden Attacke links
	bar = static_cast<CEGUI::ProgressBar*>(CEGUIUtility::getWindowForLoadedLayoutEx (m_window, "ControlPanel/CPContainer/CP_Btns/CP_SkillBtns/LeftClickAbilityProgressBar"));
	timernr =  Action::getActionInfo(player->getLeftAction())->m_timer_nr;
	perc = player->getTimerPercent(timernr);

	if (bar->getProgress() != perc)
	{
		bar->setProgress(perc);
	}
	
	alpha = 1;
	if (perc > 0)
	{
		alpha = 0.5;
	}
	if (label->getAlpha() != alpha)
	{
		label->setAlpha(alpha);
	}

	perc =0;

	// Image Attacke rechts
	label =  CEGUIUtility::getWindowForLoadedLayoutEx (m_window, "ControlPanel/CPContainer/CP_Btns/CP_SkillBtns/RightClickAbilityImage");
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
		imagename = CEGUIUtility::getImageNameWithSkin ("skills", name);
	}
	if (imagename != label->getProperty("Image"))
	{
		label->setProperty("Image",imagename);
	}

	// Balken fuer Schaden Attacke rechts
	bar = static_cast<CEGUI::ProgressBar*>(CEGUIUtility::getWindowForLoadedLayoutEx (m_window, "ControlPanel/CPContainer/CP_Btns/CP_SkillBtns/RightClickAbilityProgressBar"));
	timernr =  Action::getActionInfo(player->getRightAction())->m_timer_nr;
	perc = player->getTimerPercent(timernr);

	if (bar->getProgress() != perc)
	{
		bar->setProgress(perc);
	}
	
	alpha = 1;
	if (perc > 0)
	{
		alpha = 0.5;
	}
	if (label->getAlpha() != alpha)
	{
		label->setAlpha(alpha);
	}
	
	// image for alternate right skill
	label =  CEGUIUtility::getWindowForLoadedLayoutEx (m_window, "ControlPanel/CPContainer/CP_Btns/CP_SkillBtns/AlternateRightClickAbilityImage");
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
		imagename = CEGUIUtility::getImageNameWithSkin ("skills", name);
	}
	if (imagename != label->getProperty("Image"))
	{
		label->setProperty("Image",imagename);
	}

	// Balken fuer Schaden Attacke rechts
	bar = static_cast<CEGUI::ProgressBar*>(CEGUIUtility::getWindowForLoadedLayoutEx (m_window, "ControlPanel/CPContainer/CP_Btns/CP_SkillBtns/AlternateRightClickAbilityProgressBar"));
	timernr =  Action::getActionInfo(player->getRightAlternateAction())->m_timer_nr;
	perc = player->getTimerPercent(timernr);

	if (bar->getProgress() != perc)
	{
		bar->setProgress(perc);
	}

	alpha = 1;
	if (perc > 0)
	{
		alpha = 0.5;
	}
	if (label->getAlpha() != alpha)
	{
		label->setAlpha(alpha);
	}

	Item* it;
	Equipement* equ = player->getEquipement();

	// Guertel
	for (int i=0;i<equ->getMaxBeltItemNumber();i++)
	{
		out_stream.str("");
		out_stream << "ControlPanel/CPContainer/CP_Btns/PotionHolder/";
		out_stream << "InventoryItem" << i;
		img =  CEGUIUtility::getWindowForLoadedLayoutEx (m_window, out_stream.str().c_str());
		it = equ->getItem(Equipement::BELT_ITEMS+i);

		updateItemWindow(img, it, player);
		
		out_stream.str("");
		out_stream << "ControlPanel/CPContainer/CP_Btns/PotionHolder/";
		out_stream << "BeltPotionCounter" << i;
		label = CEGUIUtility::getWindowForLoadedLayoutEx (m_window, out_stream.str());
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
	CEGUI::PushButton* btn;

	btn = static_cast<CEGUI::PushButton*>(CEGUIUtility::getWindowForLoadedLayoutEx (m_window, "ControlPanel/CPContainer/GlobeAndSysBtnsHolder/OptionsButton"));
	btn->setTooltipText((CEGUI::utf8*) gettext("Options"));
	
	btn = static_cast<CEGUI::PushButton*>(CEGUIUtility::getWindowForLoadedLayoutEx (m_window, "ControlPanel/CPContainer/GlobeAndSysBtnsHolder/SaveExitButton"));
	btn->setTooltipText((CEGUI::utf8*) gettext("Save & Exit"));
	
	btn = static_cast<CEGUI::PushButton*>(CEGUIUtility::getWindowForLoadedLayoutEx (m_window, "ControlPanel/CPContainer/CP_Btns/CP_SysBtns/ChatOpenButton"));
	btn->setTooltipText((CEGUI::utf8*) gettext("Chat"));
	
	btn = static_cast<CEGUI::PushButton*>(CEGUIUtility::getWindowForLoadedLayoutEx (m_window, "ControlPanel/CPContainer/CP_Btns/CP_SysBtns/InventoryButton"));
	btn->setTooltipText((CEGUI::utf8*) gettext("Inventory"));
	
	btn = static_cast<CEGUI::PushButton*>(CEGUIUtility::getWindowForLoadedLayoutEx (m_window, "ControlPanel/CPContainer/CP_Btns/CP_SysBtns/CharInfoButton"));
	btn->setTooltipText((CEGUI::utf8*) gettext("Character"));

	btn = static_cast<CEGUI::PushButton*>(CEGUIUtility::getWindowForLoadedLayoutEx (m_window, "ControlPanel/CPContainer/CP_Btns/CP_SysBtns/SkillTreeButton"));
	btn->setTooltipText((CEGUI::utf8*) gettext("Skilltree"));
	
	btn = static_cast<CEGUI::PushButton*>(CEGUIUtility::getWindowForLoadedLayoutEx (m_window, "ControlPanel/CPContainer/CP_Btns/CP_SysBtns/PartyButton"));
	btn->setTooltipText((CEGUI::utf8*) gettext("Party"));
	
	btn = static_cast<CEGUI::PushButton*>(CEGUIUtility::getWindowForLoadedLayoutEx (m_window, "ControlPanel/CPContainer/CP_Btns/CP_SysBtns/QuestInfoButton"));
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


void ControlPanel::createAnimations ()
{
#if ((CEGUI_VERSION_MAJOR << 16) + (CEGUI_VERSION_MINOR << 8) + CEGUI_VERSION_PATCH >= (0 << 16)+(7 << 8)+5)

	CEGUI::Window* label;
    CEGUI::AnimationManager& animMgr = CEGUI::AnimationManager::getSingleton();
    animMgr.loadAnimationsFromXML("ControlPanelAnimations.xml");
	
	// Small tab animations
	label = CEGUIUtility::getWindowForLoadedLayoutEx (m_window, "ControlPanel/CPContainer/GlobeAndSysBtnsHolder/HealthProgressBar");
	CEGUI::AnimationInstance* instance = CEGUI::AnimationManager::getSingleton().instantiateAnimation("HealthbarAnimation");
	instance->setTargetWindow(label);
    instance->start();
	//label->subscribeEvent(CEGUI::Window::EventShown, CEGUI::Event::Subscriber(&CEGUI::AnimationInstance::handleStart, instance));

#endif
}



/**
 * \fn bool onGUIItemHover(const CEGUI::EventArgs& evt)
 * \brief Handle the hovering of gui items.
 */
bool ControlPanel::onGUIItemHover (const CEGUI::EventArgs& evt)
{
	SoundHelper::playAmbientSoundGroup ("main_menu_hover_item");
	return true;
}
