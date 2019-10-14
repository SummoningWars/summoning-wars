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

#include "questinfo.h"

// Utility for CEGUI cross-version compatibility
#include "ceguiutility.h"

QuestInfo::QuestInfo (Document* doc, const std::string& ceguiSkinName)
	: Window (doc)
	, m_ceguiSkinName (ceguiSkinName)
{
	m_questname = "";
	
	DEBUGX("setup main menu");
	// GUI Elemente erzeugen

	// Rahmen fuer das Menue Savegame auswaehlen
	CEGUI::FrameWindow* quest_info = (CEGUI::FrameWindow*) CEGUIUtility::loadLayoutFromFile ("questinfo.layout");
	if (!quest_info)
	{
		SW_DEBUG ("WARNING: Failed to load [%s]", "questinfo.layout");
	}
	CEGUI::Window* quest_info_holder = CEGUIUtility::loadLayoutFromFile ("questinfo_holder.layout");
	if (!quest_info_holder)
	{
		SW_DEBUG ("WARNING: Failed to load [%s]", "questinfo_holder.layout");
	}

	CEGUI::Window* wndHolder = CEGUIUtility::getWindowForLoadedLayoutEx (quest_info_holder, "QuestInfo_Holder");
	CEGUI::Window* wndQuest = CEGUIUtility::getWindowForLoadedLayoutEx (quest_info, "QuestInfo");
	if (wndHolder && wndQuest)
	{
		CEGUIUtility::addChildWidget (wndHolder, wndQuest);
	}
	else
	{
		if (!wndHolder) SW_DEBUG ("ERROR: Unable to get the window holder for quests.");
		if (!wndQuest) SW_DEBUG ("ERROR: Unable to get the window for quests.");
	}

	m_window = quest_info_holder;
	
	quest_info->subscribeEvent(CEGUI::Window::EventMouseButtonDown, CEGUI::Event::Subscriber(&Window::consumeEvent, (Window*) this));
	
	// Bestandteile der Kontrollleiste hinzufuegen
	
	CEGUI::Listbox* questlist = (CEGUI::Listbox*) CEGUIUtility::getWindowForLoadedLayoutEx (m_window, "QuestInfo/questinfo_aux/QuestList");
	questlist->subscribeEvent(CEGUI::Listbox::EventSelectionChanged, CEGUI::Event::Subscriber(&QuestInfo::onQuestSelected, this));

	CEGUIUtility::ToggleButton * box;
	
	box = (CEGUIUtility::ToggleButton *) CEGUIUtility::getWindowForLoadedLayoutEx (m_window, "QuestInfo/questinfo_aux/QuestOpenBox");
	box->subscribeEvent(CEGUIUtility::EventToggleButtonStateChanged (), CEGUI::Event::Subscriber(&QuestInfo::onFilterSelected, this));
	box->setSelected(true);
	
	box = (CEGUIUtility::ToggleButton *) CEGUIUtility::getWindowForLoadedLayoutEx (m_window, "QuestInfo/questinfo_aux/QuestDoneBox");
	box->subscribeEvent(CEGUIUtility::EventToggleButtonStateChanged (), CEGUI::Event::Subscriber(&QuestInfo::onFilterSelected, this));
	box->setSelected(false);
	
	box = (CEGUIUtility::ToggleButton *) CEGUIUtility::getWindowForLoadedLayoutEx (m_window, "QuestInfo/questinfo_aux/QuestFailedBox");
	box->subscribeEvent(CEGUIUtility::EventToggleButtonStateChanged (), CEGUI::Event::Subscriber(&QuestInfo::onFilterSelected, this));
	box->setSelected(false);
	
	CEGUI::MultiLineEditbox* quest_descr;
	quest_descr = static_cast<CEGUI::MultiLineEditbox*>(CEGUIUtility::getWindowForLoadedLayoutEx (m_window, "QuestInfo/questinfo_aux/QuestDescription"));
	quest_descr->setWantsMultiClickEvents(false);
	quest_descr->setReadOnly(true);
	quest_descr->setText("");
	
	CEGUI::Window* autoCloseButton;
	autoCloseButton = CEGUIUtility::getWindowForLoadedLayoutEx (m_window, "QuestInfo/__auto_closebutton__");
	if (autoCloseButton)
	{
		autoCloseButton->subscribeEvent (CEGUI::Window::EventMouseClick, CEGUI::Event::Subscriber (&QuestInfo::onCloseButtonClicked, this));
	}

	updateTranslation();
}


void QuestInfo::update()
{
	if (World::getWorld() ==0)
		return;
	
	// Auswahl ermitteln
	bool open,done,failed;
	CEGUIUtility::ToggleButton * box;
	
	box = (CEGUIUtility::ToggleButton *) CEGUIUtility::getWindowForLoadedLayoutEx (m_window, "QuestInfo/questinfo_aux/QuestOpenBox");
	open = box->isSelected();
	
	box = (CEGUIUtility::ToggleButton *) CEGUIUtility::getWindowForLoadedLayoutEx (m_window, "QuestInfo/questinfo_aux/QuestDoneBox");
	done = box->isSelected();
	
	box = (CEGUIUtility::ToggleButton *) CEGUIUtility::getWindowForLoadedLayoutEx (m_window, "QuestInfo/questinfo_aux/QuestFailedBox");
	failed = box->isSelected();
	
	CEGUI::Listbox* questlist = (CEGUI::Listbox*) CEGUIUtility::getWindowForLoadedLayoutEx (m_window, "QuestInfo/questinfo_aux/QuestList");
	questlist->resetList();
	
	// Liste mit den Quests aktualisieren
	std::map<std::string, Quest*>& quests = World::getWorld()->getQuests();
	std::map<std::string, Quest*>::iterator it;
	
	Quest::State state;
	CEGUI::ListboxItem* newitem;
	for (it = quests.begin(); it != quests.end(); ++it)
	{
		Quest* quest = it->second;
		state = it->second->getState();
		if ((state == Quest::STARTED && open) || 
		    (state == Quest::FINISHED && done) || 
		    (state == Quest::FAILED && failed))
		{
			std::string name = quest->getName().getTranslation().c_str();
			newitem = new StrListItem((CEGUI::utf8*) m_ceguiSkinName.c_str (), (CEGUI::utf8*) name.c_str(),it->first);
			questlist->addItem(newitem);
			DEBUGX("add quest %s %s",quest->getName().getTranslation().c_str(),it->first.c_str());
			
			if (it->first == m_questname)
			{
				questlist->setItemSelectState(newitem,true);
			}
		}
	}
	
	// wenn kein Quest selektiert ist, das erste auswaehlen
	if (questlist->getSelectedCount()==0)
	{
		if (questlist->getItemCount() >0)
		{
			int id =0;
			questlist->setItemSelectState (id,true);
		}
		updateDescription();
	}
	
}

void QuestInfo::updateTranslation()
{
	CEGUI::Window* label;

	label =  CEGUIUtility::getWindowForLoadedLayoutEx (m_window, "QuestInfo");
	if (label->isPropertyPresent ("Text"))
	{
		label->setProperty ("Text", (CEGUI::utf8*) gettext("Quests"));
	}
	
	label = CEGUIUtility::getWindowForLoadedLayoutEx (m_window, "QuestInfo/questinfo_aux/QuestOpenLabel");
	label->setText((CEGUI::utf8*) gettext("open"));
	
	label = CEGUIUtility::getWindowForLoadedLayoutEx (m_window, "QuestInfo/questinfo_aux/QuestDoneLabel");
	label->setText((CEGUI::utf8*) gettext("finished"));
	
	label = CEGUIUtility::getWindowForLoadedLayoutEx (m_window, "QuestInfo/questinfo_aux/QuestFailedLabel");
	label->setText((CEGUI::utf8*) gettext("failed"));
	
	
}

bool QuestInfo::onQuestSelected(const CEGUI::EventArgs& evt)
{
	updateDescription();
	
	return true;
}

bool QuestInfo::onFilterSelected(const CEGUI::EventArgs& evt)
{
	update();
	return true;
}

bool QuestInfo::onCloseButtonClicked(const CEGUI::EventArgs& evt)
{
	m_document->onButtonQuestInfoClicked();
	return true;
}

void QuestInfo::updateDescription()
{
	CEGUI::Listbox* classlist = (CEGUI::Listbox*) CEGUIUtility::getWindowForLoadedLayoutEx (m_window, "QuestInfo/questinfo_aux/QuestList");
	CEGUI::ListboxItem * itm = classlist->getFirstSelectedItem();
	
	CEGUI::MultiLineEditbox* quest_descr;
	quest_descr = static_cast<CEGUI::MultiLineEditbox*>(CEGUIUtility::getWindowForLoadedLayoutEx (m_window, "QuestInfo/questinfo_aux/QuestDescription"));
	
	if (itm !=0 && World::getWorld()!=0)
	{
		std::map<std::string, Quest*>& quests = World::getWorld()->getQuests();
		std::map<std::string, Quest*>::iterator it;
		
		StrListItem * sitm = (StrListItem *) itm;
		DEBUGX("selected quest %s", sitm->m_data.c_str());
		
		m_questname = sitm->m_data;
		
		it = quests.find(m_questname);
		if (it != quests.end())
		{
			
			quest_descr->setText((CEGUI::utf8*) it->second->getDescription().c_str());
		}
		else
		{
			quest_descr->setText("");
		}
		
	}
	else
	{
		quest_descr->setText("");
	}
}

