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
#include "sound.h"


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


	return true;
}

bool ItemWindow::onItemMouseButtonReleased(const CEGUI::EventArgs& evt)
{
	const CEGUI::MouseEventArgs& we =
			static_cast<const CEGUI::MouseEventArgs&>(evt);
	unsigned int id = we.window->getID();
	if (we.button == CEGUI::LeftButton)
	{
		DEBUGX("left button released on Item %i",id);
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

	return "set: noMedia.png image: full_image";
}


void ItemWindow::updateItemWindow(CEGUI::Window* img, Item* item, Player* player, int gold)
{
	std::string imgname="";
	if (item != 0)
	{
		imgname= getItemImage(item->m_subtype); 
	}
	
	bool playsound = false;
	if (img->getProperty("Image")!=imgname)
	{
		img->setProperty("Image", imgname);
		if (item != 0)
		{
			playsound = true;
		}
	}
	
	std::string propold = img->getProperty("BackgroundColours").c_str();
	std::string propnew = "tl:FF000000 tr:FF000000 bl:FF000000 br:FF000000";
	if (item !=0)
	{
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
	}
	
	if (propold != propnew)
	{
		img->setProperty("BackgroundColours", propnew); 
	}
	
	if (playsound && !m_silent)
	{
		SoundName sname = GraphicManager::getDropSound(item->m_subtype);
		if (sname != "")
		{
			SoundSystem::playAmbientSound(sname);
		}
		DEBUGX("drop sound %s",sname.c_str());
	}
}

void ItemWindow::updateItemWindowTooltip(CEGUI::Window* img, Item* item, Player* player, int gold,float price_factor)
{
	TooltipManager *tMgr = TooltipManager::getSingletonPtr();

	if ( item == 0 )
		return;

	CEGUI::Font *font = img->getTooltip()->getFont();
	std::string msg;
	ItemRequirementsMet irm = player->checkItemRequirements ( item );
	
	Item *currentEqItem = 0;
	Item *currentEqItemOffhand = 0;
	Item *attachedItem = player->getEquipement()->getItem (Equipement::CURSOR_ITEM);

	std::string primary_eq_head = gettext("Equipped:\n");
	std::string secondary_eq_head = gettext("Equipped:\n");
	
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
			primary_eq_head = gettext("Equipped (left):\n");
			secondary_eq_head = gettext("Equipped (right):\n");
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
	l.push_front (  CEGUIUtility::getColourizedString(CEGUIUtility::Blue, gettext("Hovered:\n"), CEGUIUtility::Black ));
	std::ostringstream out_stream;

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
