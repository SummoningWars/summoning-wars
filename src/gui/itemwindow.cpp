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


#include "graphicmanager.h"
#include "itemwindow.h"
#include "tooltipmanager.h"
#include "ceguiutility.h"
//#include "sound.h"

// Helper for sound operations
#include "soundhelper.h"

std::map<Item::Subtype, std::string> ItemWindow::m_item_images;


ItemWindow::ItemWindow (Document* doc)
	: Window(doc)
{
	m_silent = false;
}

bool ItemWindow::onItemHover(const CEGUI::EventArgs& evt)
{

	const CEGUI::MouseEventArgs& we =
			static_cast<const CEGUI::MouseEventArgs&>(evt);
	unsigned int id = we.window->getID();
	
	Player* player = m_document->getLocalPlayer();
	Item* itm;
	if (id == Equipement::WEAPON)
	{
		itm = player->getWeapon();
	}
	else if (id == Equipement::SHIELD)
	{
		itm = player->getShield();
	}
	else
	{
		itm = player->getEquipement()->getItem(id);
	}
	
	float factor = 1.0;
	Creature* npc = player->getTradePartner();
	if (npc != 0)
	{
		NPCTrade& tradeinfo = Dialogue::getNPCTrade(npc->getRefName());
		factor = tradeinfo.m_pay_multiplier;
	}
	updateItemWindowTooltip(we.window,itm ,player,-1,factor);
	return true;
}


bool ItemWindow::onItemMouseButtonPressed(const CEGUI::EventArgs& evt)
{
	const CEGUI::MouseEventArgs& we =
			static_cast<const CEGUI::MouseEventArgs&>(evt);
	unsigned int id = we.window->getID();

	if (we.button == CEGUI::LeftButton)
	{
		m_document->onItemLeftClick((short) id);
	}

	if (we.button == CEGUI::RightButton)
	{
		m_document->onItemRightClick((short) id);
	}

	DEBUGX("left button pressed on Item %i",id);
	return true;
}

bool ItemWindow::onItemMouseButtonReleased(const CEGUI::EventArgs& evt)
{
	const CEGUI::MouseEventArgs& we = static_cast<const CEGUI::MouseEventArgs&>(evt);
	unsigned int id = we.window->getID();
	
	if (we.button == CEGUI::LeftButton)
	{
		DEBUGX("left button released on Item %i", id);
	}
	return true;
}


std::string ItemWindow::getItemImage(Item::Subtype type)
{
	std::map<Item::Subtype, std::string>::iterator it;
	it = m_item_images.find(type);

	if (it != m_item_images.end())
	{
		return it->second;
	}

	return CEGUIUtility::getImageNameWithSkin ("noMedia.png", "full_image");
}


void ItemWindow::updateItemWindow(CEGUI::Window* img, Item* item, Player* player, int gold)
{
	std::string imgname="";
	std::string idstr = "";
	bool playsound = false;
	if (item != 0)
	{
		std::stringstream stream;
		stream << item->m_subtype << ":" << item->m_id;
		idstr = stream.str();
		
		imgname= getItemImage(item->m_subtype);
		if (img->getProperty("Image")!=imgname)
		{
			img->setProperty("Image", imgname);
		}
		
		if (!img->isUserStringDefined("idstr") || img->getUserString("idstr") != idstr)
		{
			if (item != 0)
			{
				playsound = true;
			}
			img->setUserString("idstr",idstr);
		}
	}
	else
	{
		if (img->getProperty("Image")!=imgname)
		{
			img->setProperty("Image", imgname);
			img->setUserString("idstr",idstr);
		}
	}
	
	// TODO:remove when no longer needed - end
	if (img->isPropertyPresent ("BackgroundColours") && item != 0)
	{
		std::string propold = img->getProperty("BackgroundColours").c_str();
		std::string propnew = "tl:FF000000 tr:FF000000 bl:FF000000 br:FF000000";

		// rot wenn Spieler Item nicht verwenden kann
		// oder es bezahlen muss und nicht genug Geld hat
		if (!player->checkItemRequirements(item).m_overall)
		{
			propnew = "tl:FFAA5555 tr:FFAA5555 bl:FFAA5555 br:FFAA5555";
		}
		else if (gold>=0 && gold<item->m_price)
		{
			propnew = "tl:FFAA5555 tr:FFAA5555 bl:FFAA5555 br:FFAA5555";
		}
		else if (item->m_rarity == Item::MAGICAL)
		{
			propnew = "tl:FF5555AA tr:FF5555AA bl:FF5555AA br:FF5555AA";
		}
	
		if (propold != propnew)
		{
			img->setProperty("BackgroundColours", propnew); 
		}
	}
	// TODO:remove when no longer needed - begin
	else if (img->isPropertyPresent ("BackgroundColour") && item != 0)
	{
		std::string propold = img->getProperty("BackgroundColour").c_str();
		std::string propnew = "2AFFFFFF";

		// TODO: move colours to separate locations. Preferably make non-hard-coded.
		// Red colour when the player cannot use the item or (shop scenario) not enough gold available for purchase
		if (!player->checkItemRequirements(item).m_overall)
		{
			propnew = "FFAA5555";
		}
		else if (gold>=0 && gold<item->m_price)
		{
			propnew = "FFAA5555";
		}
		else if (item->m_rarity == Item::MAGICAL)
		{
			propnew = "FF5555AA";
		}
	
		if (propold != propnew)
		{
			img->setProperty("BackgroundColour", propnew); 
		}
	}

	if (item == 0)
	{
		if (img->isPropertyPresent ("BackgroundColour"))
		{
			img->setProperty ("BackgroundColour", img->getPropertyDefault ("BackgroundColour"));
		}
	}
	
	// try to find a Progressbar with a matching name
	// remove "Label" and add "ProgressBar"
	std::size_t pos;
#ifdef CEGUI_07
	CEGUI::String  windowname = img->getName ();
#else
	CEGUI::String  windowname = img->getNamePath ();
	pos = windowname.find ("SW/");
	if (pos != std::string::npos)
	{
		windowname.erase (pos, 3);
	}
	//DEBUG ("Name [%s], namepath [%s]", windowname.c_str (), img->getNamePath ().c_str ());
#endif
	// TODO:XXX:this was initially meant only for items such as : SmallItem0Label > SmallItem0ProgressBar
	pos = windowname.find ("Label");
	if (pos != std::string::npos)
	{
		windowname.erase(pos,5);
	}
	windowname.append("ProgressBar");

  // Fenstermanager
	if (CEGUIUtility::isWindowPresent (windowname))
	{
		// update progress bar to reflect item timer
		CEGUI::ProgressBar* bar = static_cast<CEGUI::ProgressBar*>(CEGUIUtility::getWindow (windowname));
		double progress = 0;
		if (item != 0 && item->m_consumable && item->m_consume_timer_nr != 0)
		{
			// there is an item with potentially running timer
			progress = player->getTimerPercent(item->m_consume_timer_nr);
		}
		
		// update progress
		if (bar->getProgress() != progress)
		{
			bar->setProgress(progress);
		}
		
		// display image gray if the item is not available
		double alpha = 1;
		if (progress > 0)
		{
			alpha = 0.5;
			if (bar->getAlpha() != 0.5)
			{
				bar->setAlpha(0.5);
			}
		}
		if (img->getAlpha() != alpha)
		{
			img->setAlpha(alpha);
		}
	}
	
	if (playsound && !m_silent_current_update)
	{
		std::string sname = GraphicManager::getDropSound(item->m_subtype);
		if (sname != "")
		{
			//SoundSystem::playAmbientSound(sname);
			SoundHelper::playAmbientSoundGroup (sname);
			
			// play only one sound per update...
			m_silent_current_update = true;
		}
		DEBUGX("drop sound %s",sname.c_str());
	}
}

void ItemWindow::updateItemWindowTooltip(CEGUI::Window* img, Item* item, Player* player, int gold,float price_factor)
{
	TooltipManager *tMgr = TooltipManager::getSingletonPtr();

	if ( item == 0 )
		return;

	const CEGUI::Font *font = img->getTooltip()->getFont();
	std::string msg;
	ItemRequirementsMet irm = player->checkItemRequirements ( item );
	
	Item *currentEqItem = 0;
	Item *currentEqItemOffhand = 0;
	Item *attachedItem = player->getEquipement()->getItem (Equipement::CURSOR_ITEM);

	std::string primary_eq_head = gettext("Equipped:");
	std::string secondary_eq_head = gettext("Equipped:");
	
	switch ( item->m_type )
	{
		case Item::NOITEM:
			break;

		case Item::ARMOR:
			currentEqItem = player->getEquipement()->getItem ( Equipement::ARMOR );
			break;

		case Item::HELMET:
			currentEqItem = player->getEquipement()->getItem ( Equipement::HELMET );
			break;

		case Item::GLOVES:
			currentEqItem = player->getEquipement()->getItem ( Equipement::GLOVES );
			break;

		case Item::WEAPON:
			currentEqItem = player->getEquipement()->getItem ( Equipement::WEAPON );
			currentEqItemOffhand = player->getEquipement()->getItem ( Equipement::WEAPON2 );
			secondary_eq_head = gettext("Equipped (secondary):\n");
			
			// swap pointers if secondary equip is activated
			if (player->isUsingSecondaryEquip())
			{
				std::swap(currentEqItem,currentEqItemOffhand); 
			}
			break;

		case Item::SHIELD:
			currentEqItem = player->getEquipement()->getItem ( Equipement::SHIELD );
			currentEqItemOffhand = player->getEquipement()->getItem ( Equipement::SHIELD2 );
			// swap pointers if secondary equip is activated
			if (player->isUsingSecondaryEquip())
			{
				std::swap(currentEqItem,currentEqItemOffhand); 
			}
			break;

		case Item::POTION:
			break;

		case Item::RING:
			currentEqItem = player->getEquipement()->getItem ( Equipement::RING_LEFT );
			currentEqItemOffhand = player->getEquipement()->getItem ( Equipement::RING_RIGHT );
			primary_eq_head = gettext("Equipped (left):");
			secondary_eq_head = gettext("Equipped (right):");
			break;

		case Item::AMULET:
			currentEqItem = player->getEquipement()->getItem ( Equipement::AMULET );
			break;

		case Item::GOLD_TYPE:
			break;

		default:
			break;

	}
	
	bool hoverEqItem = false;
	bool hoverEqItemOffhand = false;
	// Do not display the same item twice (happens if you hover an equiped item)
	if (item == currentEqItem)
	{
		currentEqItem = 0;
		hoverEqItem = true;
	}
	else if (item == currentEqItemOffhand)
	{
		hoverEqItemOffhand = true;
		currentEqItemOffhand = 0;
	}
	
	// create main tooltip
	std::list<std::string> l = item->getDescriptionAsStringList ( price_factor, irm );
	if (hoverEqItem)
	{
		l.push_front (  CEGUIUtility::getColourizedString(CEGUIUtility::Blue, primary_eq_head, CEGUIUtility::Black ));
	}
	if (hoverEqItemOffhand)
	{
		l.push_front (  CEGUIUtility::getColourizedString(CEGUIUtility::Blue, secondary_eq_head, CEGUIUtility::Black ));
	}
	l.push_front (  CEGUIUtility::getColourizedString(CEGUIUtility::Blue, gettext("Hovered:"), CEGUIUtility::Black ));
	std::ostringstream out_stream;

	//XXX-begin; TODO: delete this
	//DEBUG ("Obtained description for item as string list.");
	//for (std::list <std::string>::iterator it = l.begin (); it != l.end (); ++ it)
	//{
	//	DEBUG ("[%s]", it->c_str ());
	//}
	//XXX-end
	tMgr->createTooltip ( img, l, 0, font, Tooltip::Main );

	// create secondary tooltips
	if (attachedItem != 0)
	{
		l = attachedItem->getDescriptionAsStringList ( price_factor );
		l.push_front (  CEGUIUtility::getColourizedString(CEGUIUtility::Blue, gettext("Attached:\n"), CEGUIUtility::Black ));
		tMgr->createTooltip ( img, l, 0, font, Tooltip::Comparision );
	}
	else
	{
		if ( currentEqItem )
		{
			l = currentEqItem->getDescriptionAsStringList ( price_factor );
			l.push_front (  CEGUIUtility::getColourizedString(CEGUIUtility::Blue, primary_eq_head, CEGUIUtility::Black ));
			tMgr->createTooltip ( img, l, 0, font, Tooltip::Comparision );
		}
		
		if ( currentEqItemOffhand )
		{
			l = currentEqItemOffhand->getDescriptionAsStringList ( price_factor );
			l.push_front (  CEGUIUtility::getColourizedString(CEGUIUtility::Blue, secondary_eq_head, CEGUIUtility::Black ));
			tMgr->createTooltip ( img, l, 0, font, Tooltip::Comparision );
		}
	}
}

void ItemWindow::registerItemImage(Item::Subtype type, std::string image)
{
	m_item_images[type] = image;
}
