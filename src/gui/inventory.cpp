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

#include "inventory.h"
//#include "sound.h"

// Helper for sound operations
#include "soundhelper.h"

// Utility for CEGUI cross-version compatibility
#include "ceguiutility.h"

Inventory::Inventory (Document* doc)
	: ItemWindow(doc)
{
	DEBUGX("setup inventory");

	CEGUI::Window* inventory = CEGUIUtility::loadLayoutFromFile ("inventory.layout");
	if (!inventory)
	{
		SW_DEBUG ("WARNING: Failed to load [%s]", "inventory.layout");
	}

	CEGUI::Window* inv_holder = CEGUIUtility::loadLayoutFromFile ("inventory_holder.layout");
	if (!inv_holder)
	{
		SW_DEBUG ("WARNING: Failed to load [%s]", "inventory_holder.layout");
	}

	SW_DEBUG ("Placing layout into holder");
	inventory->setVisible (true);
	inv_holder->setVisible (true);
	//CEGUIUtility::addChildWidget (inv_holder, inventory);


	CEGUI::Window* wndHolder = CEGUIUtility::getWindowForLoadedLayoutEx (inv_holder, "Inventory_Holder");
	CEGUI::Window* wndInventory = CEGUIUtility::getWindowForLoadedLayoutEx (inventory, "Inventory");
	if (wndHolder && wndInventory)
	{
		CEGUIUtility::addChildWidget (wndHolder, wndInventory);
	}
	else
	{
		if (!wndHolder) SW_DEBUG ("ERROR: Unable to get the window holder for inventory.");
		if (!wndInventory) SW_DEBUG ("ERROR: Unable to get the window for inventory.");
	}
	m_window = inv_holder;

	inventory->subscribeEvent(CEGUI::Window::EventMouseButtonDown, CEGUI::Event::Subscriber(&Window::consumeEvent, (Window*) this));
	inventory->setWantsMultiClickEvents(false);



	// Bestandteile des Charakterfensters hinzufuegen
	CEGUI::PushButton* btn;
	CEGUI::Window* label;
	
	label = CEGUIUtility::getWindowForLoadedLayoutEx (m_window, "Inventory/inventory_aux/GearHolder/CharImageHolder/CharacterImage");

#ifdef CEGUI_07
	label->setProperty("Image", CEGUIUtility::getImageNameWithSkin ("character", "character_img")); 
#else
	label->setProperty("Image", "character_img"); 
#endif

	label->setID(Equipement::NONE);
	label->subscribeEvent(CEGUI::Window::EventMouseButtonDown, CEGUI::Event::Subscriber(&Inventory::onItemMouseButtonPressed, (ItemWindow*) this));
	//label->setAlpha(1.0);
	
	int i,j;

	// Label fuer grosse Items
	std::ostringstream outStream;
	for (i=0;i<5;i++)
	{
		outStream.str("");
		outStream << "Inventory/inventory_aux/StashHolder/StashItemArea/BigTabMain/";
		outStream << "BigItem" << i<< "Label";
		label = CEGUIUtility::getWindowForLoadedLayoutEx (m_window, outStream.str());
		label->setID(Equipement::BIG_ITEMS+i);
		label->subscribeEvent(CEGUI::Window::EventMouseButtonDown, CEGUI::Event::Subscriber(&Inventory::onItemMouseButtonPressed, (ItemWindow*) this));
		label->subscribeEvent(CEGUI::Window::EventMouseButtonUp, CEGUI::Event::Subscriber(&Inventory::onItemMouseButtonReleased, (ItemWindow*) this));
		label->subscribeEvent(CEGUIUtility::EventMouseEntersWindowArea (), CEGUI::Event::Subscriber(&Inventory::onItemHover, (ItemWindow*) this));
		label->setWantsMultiClickEvents(false);
	}

	// Label fuer mittlere Items
	for (j=0;j<2;j++)
	{
		for (i=0;i<6;i++)
		{
			outStream.str("");
			outStream << "Inventory/inventory_aux/StashHolder/StashItemArea/MediumTabMain/";
			outStream << "MediumItem" << j*6+i<< "Label";
			label = CEGUIUtility::getWindowForLoadedLayoutEx (m_window, outStream.str());
			label->setID(Equipement::MEDIUM_ITEMS+j*6+i);
			label->subscribeEvent(CEGUI::Window::EventMouseButtonDown, CEGUI::Event::Subscriber(&Inventory::onItemMouseButtonPressed, (ItemWindow*) this));
			label->subscribeEvent(CEGUI::Window::EventMouseButtonUp, CEGUI::Event::Subscriber(&Inventory::onItemMouseButtonReleased, (ItemWindow*) this));
			label->subscribeEvent(CEGUIUtility::EventMouseEntersWindowArea (), CEGUI::Event::Subscriber(&Inventory::onItemHover, (ItemWindow*) this));
			label->setWantsMultiClickEvents(false);
		}
	}
	// Label fuer kleine Items
	for (j=0;j<3;j++)
	{
		for (i=0;i<10;i++)
		{
			outStream.str("");
			outStream << "Inventory/inventory_aux/StashHolder/StashItemArea/SmallTabMain/";
			outStream << "SmallItem" << j*10+i<< "Label";
			label = CEGUIUtility::getWindowForLoadedLayoutEx (m_window, outStream.str());
			label->setID(Equipement::SMALL_ITEMS+j*10+i);
			label->subscribeEvent(CEGUI::Window::EventMouseButtonDown, CEGUI::Event::Subscriber(&Inventory::onItemMouseButtonPressed, (ItemWindow*) this));
			label->subscribeEvent(CEGUI::Window::EventMouseButtonUp, CEGUI::Event::Subscriber(&Inventory::onItemMouseButtonReleased, (ItemWindow*) this));
			label->subscribeEvent(CEGUIUtility::EventMouseEntersWindowArea (), CEGUI::Event::Subscriber(&Inventory::onItemHover, (ItemWindow*) this));
			label->setWantsMultiClickEvents(false);
		}
	}



	// Label Ruestung
	label = CEGUIUtility::getWindowForLoadedLayoutEx (m_window, "Inventory/inventory_aux/GearHolder/ArmorItemLabel");
	label->setID(Equipement::ARMOR);
	label->subscribeEvent(CEGUI::Window::EventMouseButtonDown, CEGUI::Event::Subscriber(&Inventory::onItemMouseButtonPressed, (ItemWindow*) this));
	label->subscribeEvent(CEGUI::Window::EventMouseButtonUp, CEGUI::Event::Subscriber(&Inventory::onItemMouseButtonReleased, (ItemWindow*) this));
	label->subscribeEvent(CEGUIUtility::EventMouseEntersWindowArea (), CEGUI::Event::Subscriber(&Inventory::onItemHover, (ItemWindow*) this));
	label->setWantsMultiClickEvents(false);


	// Label Waffe
	label = CEGUIUtility::getWindowForLoadedLayoutEx (m_window, "Inventory/inventory_aux/GearHolder/WeaponItemLabel");
	label->setID(Equipement::WEAPON);
	label->subscribeEvent(CEGUI::Window::EventMouseButtonDown, CEGUI::Event::Subscriber(&Inventory::onItemMouseButtonPressed, (ItemWindow*) this));
	label->subscribeEvent(CEGUI::Window::EventMouseButtonUp, CEGUI::Event::Subscriber(&Inventory::onItemMouseButtonReleased, (ItemWindow*) this));
	label->subscribeEvent(CEGUIUtility::EventMouseEntersWindowArea (), CEGUI::Event::Subscriber(&Inventory::onItemHover, (ItemWindow*) this));
	label->setWantsMultiClickEvents(false);
	
	// Label Helm
	label = CEGUIUtility::getWindowForLoadedLayoutEx (m_window, "Inventory/inventory_aux/GearHolder/HelmetItemLabel");
	label->setID(Equipement::HELMET);
	label->subscribeEvent(CEGUI::Window::EventMouseButtonDown, CEGUI::Event::Subscriber(&Inventory::onItemMouseButtonPressed, (ItemWindow*) this));
	label->subscribeEvent(CEGUI::Window::EventMouseButtonUp, CEGUI::Event::Subscriber(&Inventory::onItemMouseButtonReleased, (ItemWindow*) this));
	label->subscribeEvent(CEGUIUtility::EventMouseEntersWindowArea (), CEGUI::Event::Subscriber(&Inventory::onItemHover, (ItemWindow*) this));
	label->setWantsMultiClickEvents(false);
	
	// Label Schild
	label = CEGUIUtility::getWindowForLoadedLayoutEx (m_window, "Inventory/inventory_aux/GearHolder/ShieldItemLabel");
	label->setID(Equipement::SHIELD);
	label->subscribeEvent(CEGUI::Window::EventMouseButtonDown, CEGUI::Event::Subscriber(&Inventory::onItemMouseButtonPressed, (ItemWindow*) this));
	label->subscribeEvent(CEGUI::Window::EventMouseButtonUp, CEGUI::Event::Subscriber(&Inventory::onItemMouseButtonReleased, (ItemWindow*) this));
	label->subscribeEvent(CEGUIUtility::EventMouseEntersWindowArea (), CEGUI::Event::Subscriber(&Inventory::onItemHover, (ItemWindow*) this));
	label->setWantsMultiClickEvents(false);
	
	// Label Handschuhe
	label = CEGUIUtility::getWindowForLoadedLayoutEx (m_window, "Inventory/inventory_aux/GearHolder/GlovesItemLabel");
	label->setID(Equipement::GLOVES);
	label->subscribeEvent(CEGUI::Window::EventMouseButtonDown, CEGUI::Event::Subscriber(&Inventory::onItemMouseButtonPressed, (ItemWindow*)this));
	label->subscribeEvent(CEGUI::Window::EventMouseButtonUp, CEGUI::Event::Subscriber(&Inventory::onItemMouseButtonReleased, (ItemWindow*)this));
	label->subscribeEvent(CEGUIUtility::EventMouseEntersWindowArea (), CEGUI::Event::Subscriber(&Inventory::onItemHover, (ItemWindow*)this));
	label->setWantsMultiClickEvents(false);
	
	// Ring links
	label = CEGUIUtility::getWindowForLoadedLayoutEx (m_window, "Inventory/inventory_aux/GearHolder/RingLeftItemLabel");
	label->setID(Equipement::RING_LEFT);
	label->subscribeEvent(CEGUI::Window::EventMouseButtonDown, CEGUI::Event::Subscriber(&Inventory::onItemMouseButtonPressed, (ItemWindow*)this));
	label->subscribeEvent(CEGUI::Window::EventMouseButtonUp, CEGUI::Event::Subscriber(&Inventory::onItemMouseButtonReleased, (ItemWindow*)this));
	label->subscribeEvent(CEGUIUtility::EventMouseEntersWindowArea (), CEGUI::Event::Subscriber(&Inventory::onItemHover, (ItemWindow*)this));
	label->setWantsMultiClickEvents(false);
	
	// Ring rechts
	label = CEGUIUtility::getWindowForLoadedLayoutEx (m_window, "Inventory/inventory_aux/GearHolder/RingRightItemLabel");
	label->setID(Equipement::RING_RIGHT);
	label->subscribeEvent(CEGUI::Window::EventMouseButtonDown, CEGUI::Event::Subscriber(&Inventory::onItemMouseButtonPressed, (ItemWindow*)this));
	label->subscribeEvent(CEGUI::Window::EventMouseButtonUp, CEGUI::Event::Subscriber(&Inventory::onItemMouseButtonReleased, (ItemWindow*)this));
	label->subscribeEvent(CEGUIUtility::EventMouseEntersWindowArea (), CEGUI::Event::Subscriber(&Inventory::onItemHover, (ItemWindow*)this));
	label->setWantsMultiClickEvents(false);
	
	// Amulett
	label = CEGUIUtility::getWindowForLoadedLayoutEx (m_window, "Inventory/inventory_aux/GearHolder/AmuletItemLabel");
	label->setID(Equipement::AMULET);
	label->subscribeEvent(CEGUI::Window::EventMouseButtonDown, CEGUI::Event::Subscriber(&Inventory::onItemMouseButtonPressed, (ItemWindow*)this));
	label->subscribeEvent(CEGUI::Window::EventMouseButtonUp, CEGUI::Event::Subscriber(&Inventory::onItemMouseButtonReleased, (ItemWindow*)this));
	label->subscribeEvent(CEGUIUtility::EventMouseEntersWindowArea (), CEGUI::Event::Subscriber(&Inventory::onItemHover, (ItemWindow*)this));
	label->setWantsMultiClickEvents(false);
	
	// Swap weapons button
	btn = static_cast<CEGUI::PushButton*>(CEGUIUtility::getWindowForLoadedLayoutEx (m_window, "Inventory/inventory_aux/GearHolder/SwapEquipButton"));
	btn->setText ("1");
	btn->subscribeEvent (CEGUI::PushButton::EventClicked, CEGUI::Event::Subscriber (&Inventory::onSwapEquipClicked, this));

	// Label drop Gold
	btn = static_cast<CEGUI::PushButton*>(CEGUIUtility::getWindowForLoadedLayoutEx (m_window, "Inventory/inventory_aux/GoldStaticImage/GoldDropButton"));
	btn->subscribeEvent(CEGUI::PushButton::EventClicked,CEGUI::Event::Subscriber(&Inventory::onDropGoldClicked, this));
	
	// Label drop Gold (Wert)
	CEGUI::Editbox* box;
	box = static_cast<CEGUI::Editbox*>(CEGUIUtility::getWindowForLoadedLayoutEx (m_window, "Inventory/inventory_aux/GoldDropValueBox"));
	box->subscribeEvent(CEGUI::Editbox::EventTextAccepted, CEGUI::Event::Subscriber(&Inventory::onDropGoldClicked,  this));
	
	
	// Tab Labels
	setState(Inventory::StateSmall);

	label = CEGUIUtility::getWindowForLoadedLayoutEx (m_window, "Inventory/inventory_aux/StashHolder/SmallTabButton");
	label->subscribeEvent(CEGUI::Window::EventMouseClick, CEGUI::Event::Subscriber(&Inventory::onSwitchTabClicked, this));
	label = CEGUIUtility::getWindowForLoadedLayoutEx (m_window, "Inventory/inventory_aux/StashHolder/MediumTabButton");
	label->subscribeEvent(CEGUI::Window::EventMouseClick, CEGUI::Event::Subscriber(&Inventory::onSwitchTabClicked, this));
	label = CEGUIUtility::getWindowForLoadedLayoutEx (m_window, "Inventory/inventory_aux/StashHolder/BigTabButton");
	label->subscribeEvent(CEGUI::Window::EventMouseClick, CEGUI::Event::Subscriber(&Inventory::onSwitchTabClicked, this));
	
	label = CEGUIUtility::getWindowForLoadedLayoutEx (m_window, "Inventory/__auto_closebutton__");
	if (label)
	{
		label->subscribeEvent(CEGUI::Window::EventMouseClick, CEGUI::Event::Subscriber(&Inventory::onCloseButtonClicked, this));
	}
	
	createAnimations();

	updateTranslation();
}

void Inventory::update()
{
	CEGUI::Window* img;
	CEGUI::Window* label;
	Item* it,*weapon;
	CEGUI::PushButton* btn;
	
	m_silent_current_update = m_silent;
	
	std::ostringstream out_stream;

	Player* player = m_document->getLocalPlayer();
	Equipement* equ = player->getEquipement();


	// Weapon - Label/Slot
	img =  CEGUIUtility::getWindowForLoadedLayoutEx (m_window, "Inventory/inventory_aux/GearHolder/WeaponItemLabel");
	it = player->getWeapon();
	updateItemWindow(img,it,player);
	weapon = it;
	bool two_hand = false;
	if (weapon !=0 && weapon->m_weapon_attr !=0 && weapon->m_weapon_attr->m_two_handed)
	{
		two_hand = true;
	}


	// Body armor - Label/Slot
	img =  CEGUIUtility::getWindowForLoadedLayoutEx (m_window, "Inventory/inventory_aux/GearHolder/ArmorItemLabel");
	it = equ->getItem(Equipement::ARMOR);
	updateItemWindow(img,it,player);
	


	// Helm - Label/Slot
	img =  CEGUIUtility::getWindowForLoadedLayoutEx (m_window, "Inventory/inventory_aux/GearHolder/HelmetItemLabel");
	it = equ->getItem(Equipement::HELMET);
	updateItemWindow(img,it,player);
	
	// Shield - Label/Slot
	// for two handed weapons this will represent the weapon.
	float alpha = 1.0;
	img =  CEGUIUtility::getWindowForLoadedLayoutEx (m_window, "Inventory/inventory_aux/GearHolder/ShieldItemLabel");
	it = player->getShield();
	if (two_hand && it ==0)
	{
		it = weapon;
		alpha = 0.7;
	}
	updateItemWindow(img,it,player);
	
	if (img->getAlpha() != alpha)
	{
		img->setAlpha(alpha);
	}
	

	// Handgloves - Label/Slot
	img =  CEGUIUtility::getWindowForLoadedLayoutEx (m_window, "Inventory/inventory_aux/GearHolder/GlovesItemLabel");
	it = equ->getItem(Equipement::GLOVES);
	updateItemWindow(img,it,player);
	
	// Left hand ring - Label/Slot
	img =  CEGUIUtility::getWindowForLoadedLayoutEx (m_window, "Inventory/inventory_aux/GearHolder/RingLeftItemLabel");
	it = equ->getItem(Equipement::RING_LEFT);
	updateItemWindow(img,it,player);
	
	// Right hand ring - Label/Slot
	img =  CEGUIUtility::getWindowForLoadedLayoutEx (m_window, "Inventory/inventory_aux/GearHolder/RingRightItemLabel");
	it = equ->getItem(Equipement::RING_RIGHT);
	updateItemWindow(img,it,player);
	

	// Amulet - Label/Slot
	img =  CEGUIUtility::getWindowForLoadedLayoutEx (m_window, "Inventory/inventory_aux/GearHolder/AmuletItemLabel");
	it = equ->getItem(Equipement::AMULET);
	updateItemWindow(img,it,player);
	
	
	// Swap button - this will also display the state of the current weapon selection.
	btn = static_cast<CEGUI::PushButton*>(CEGUIUtility::getWindowForLoadedLayoutEx (m_window, "Inventory/inventory_aux/GearHolder/SwapEquipButton"));
	std::string text;
	if (player->isUsingSecondaryEquip())
	{
		if (btn->isPropertyPresent ("AltStateImage"))
		{
			text = btn->getProperty ("AltStateImage").c_str ();
		}
	}
	else
	{
		if (btn->isPropertyPresent ("MainStateImage"))
		{
			text = btn->getProperty ("MainStateImage").c_str ();
		}
	}
	if (btn->isPropertyPresent ("NormalImage") && btn->isPropertyPresent ("HoverImage") && btn->isPropertyPresent ("PushedImage"))
	{
		if (btn->getProperty("NormalImage") != text)
		{
			btn->setProperty("NormalImage",text);
			btn->setProperty("HoverImage",text);
			btn->setProperty("PushedImage",text);		
		}
	}

	int i;
	
	// Inventar: grosse Items
	for (i=0;i<5;i++)
	{
		out_stream.str("");
		out_stream << "Inventory/inventory_aux/StashHolder/StashItemArea/BigTabMain/";
		out_stream << "BigItem" << i<< "Label";
		img =  CEGUIUtility::getWindowForLoadedLayoutEx (m_window, out_stream.str().c_str());
		it = equ->getItem(Equipement::BIG_ITEMS+i);
		updateItemWindow(img,it,player);
	}

	// Inventar: mittlere Items
	for (i=0;i<12;i++)
	{
		out_stream.str("");
		out_stream << "Inventory/inventory_aux/StashHolder/StashItemArea/MediumTabMain/";
		out_stream << "MediumItem" << i<< "Label";
		img =  CEGUIUtility::getWindowForLoadedLayoutEx (m_window, out_stream.str().c_str());
		it = equ->getItem(Equipement::MEDIUM_ITEMS+i);
		updateItemWindow(img,it,player);
	}

	// Inventar: kleine Items
	for (i=0;i<30;i++)
	{
		out_stream.str("");
		out_stream << "Inventory/inventory_aux/StashHolder/StashItemArea/SmallTabMain/";
		out_stream << "SmallItem" << i<< "Label";
		img =  CEGUIUtility::getWindowForLoadedLayoutEx (m_window, out_stream.str().c_str());
		it = equ->getItem(Equipement::SMALL_ITEMS+i);
		updateItemWindow(img,it,player);
	}
	
	label =  CEGUIUtility::getWindowForLoadedLayoutEx (m_window, "Inventory/inventory_aux/GoldValueLabel");
	out_stream.str("");
	out_stream << equ->getGold();
	if (label->getText()!=out_stream.str())
	{
		label->setText(out_stream.str());
		if (!m_silent)
		{
			//SoundSystem::playAmbientSound("sell_buy");
			SoundHelper::playAmbientSoundGroup ("sell_buy");
		}
	}
	
}

void Inventory::setState(State s)
{
	CEGUI::Window* label;

	if(s == Inventory::StateSmall)
	{
		SW_DEBUG ("Changed state to small inventory");

		CEGUI::TabButton *tabBtn;
		tabBtn = static_cast<CEGUI::TabButton*> (CEGUIUtility::getWindowForLoadedLayoutEx (m_window, "Inventory/inventory_aux/StashHolder/SmallTabButton"));
		tabBtn->setSelected (true);
		tabBtn = static_cast<CEGUI::TabButton*> (CEGUIUtility::getWindowForLoadedLayoutEx (m_window, "Inventory/inventory_aux/StashHolder/MediumTabButton"));
		tabBtn->setSelected (false);
		tabBtn = static_cast<CEGUI::TabButton*> (CEGUIUtility::getWindowForLoadedLayoutEx (m_window, "Inventory/inventory_aux/StashHolder/BigTabButton"));
		tabBtn->setSelected (false);

		label = CEGUIUtility::getWindowForLoadedLayoutEx (m_window, "Inventory/inventory_aux/StashHolder/StashItemArea/SmallTabMain");
		label->setVisible(true);
		label = CEGUIUtility::getWindowForLoadedLayoutEx (m_window, "Inventory/inventory_aux/StashHolder/StashItemArea/MediumTabMain");
		label->setVisible(false);
		label = CEGUIUtility::getWindowForLoadedLayoutEx (m_window, "Inventory/inventory_aux/StashHolder/StashItemArea/BigTabMain");
		label->setVisible(false);
	}
	else if(s == Inventory::StateMedium)
	{
		SW_DEBUG ("Changed state to medium inventory");

		CEGUI::TabButton *tabBtn;
		tabBtn = static_cast<CEGUI::TabButton*> (CEGUIUtility::getWindowForLoadedLayoutEx (m_window, "Inventory/inventory_aux/StashHolder/SmallTabButton"));
		tabBtn->setSelected (false);
		tabBtn = static_cast<CEGUI::TabButton*> (CEGUIUtility::getWindowForLoadedLayoutEx (m_window, "Inventory/inventory_aux/StashHolder/MediumTabButton"));
		tabBtn->setSelected (true);
		tabBtn = static_cast<CEGUI::TabButton*> (CEGUIUtility::getWindowForLoadedLayoutEx (m_window, "Inventory/inventory_aux/StashHolder/BigTabButton"));
		tabBtn->setSelected (false);

		label = CEGUIUtility::getWindowForLoadedLayoutEx (m_window, "Inventory/inventory_aux/StashHolder/StashItemArea/SmallTabMain");
		label->setVisible(false);
		label = CEGUIUtility::getWindowForLoadedLayoutEx (m_window, "Inventory/inventory_aux/StashHolder/StashItemArea/MediumTabMain");
		label->setVisible(true);
		label = CEGUIUtility::getWindowForLoadedLayoutEx (m_window, "Inventory/inventory_aux/StashHolder/StashItemArea/BigTabMain");
		label->setVisible(false);
	}
	else if(s == Inventory::StateBig)
	{
		SW_DEBUG ("Changed state to big inventory");

		CEGUI::TabButton *tabBtn;
		tabBtn = static_cast<CEGUI::TabButton*> (CEGUIUtility::getWindowForLoadedLayoutEx (m_window, "Inventory/inventory_aux/StashHolder/SmallTabButton"));
		tabBtn->setSelected (false);
		tabBtn = static_cast<CEGUI::TabButton*> (CEGUIUtility::getWindowForLoadedLayoutEx (m_window, "Inventory/inventory_aux/StashHolder/MediumTabButton"));
		tabBtn->setSelected (false);
		tabBtn = static_cast<CEGUI::TabButton*> (CEGUIUtility::getWindowForLoadedLayoutEx (m_window, "Inventory/inventory_aux/StashHolder/BigTabButton"));
		tabBtn->setSelected (true);

		label = CEGUIUtility::getWindowForLoadedLayoutEx (m_window, "Inventory/inventory_aux/StashHolder/StashItemArea/SmallTabMain");
		label->setVisible(false);
		label = CEGUIUtility::getWindowForLoadedLayoutEx (m_window, "Inventory/inventory_aux/StashHolder/StashItemArea/MediumTabMain");
		label->setVisible(false);
		label = CEGUIUtility::getWindowForLoadedLayoutEx (m_window, "Inventory/inventory_aux/StashHolder/StashItemArea/BigTabMain");
		label->setVisible(true);
	}
}

void Inventory::updateTranslation()
{
	CEGUI::Window* label;
	
	label = CEGUIUtility::getWindowForLoadedLayoutEx (m_window, "Inventory/inventory_aux/GoldStaticImage/GoldDropButton");
	label->setText((CEGUI::utf8*) gettext("Drop:"));
	
	label = CEGUIUtility::getWindowForLoadedLayoutEx (m_window, "Inventory");
	if (label)
	{
		label->setText((CEGUI::utf8*) gettext("Inventory"));
	}
	
	label = CEGUIUtility::getWindowForLoadedLayoutEx (m_window, "Inventory/inventory_aux/StashHolder/SmallTabButton");
	label->setText((CEGUI::utf8*) gettext("Small"));
	
	label = CEGUIUtility::getWindowForLoadedLayoutEx (m_window, "Inventory/inventory_aux/StashHolder/MediumTabButton");
	label->setText((CEGUI::utf8*) gettext("Medium"));
	
	label = CEGUIUtility::getWindowForLoadedLayoutEx (m_window, "Inventory/inventory_aux/StashHolder/BigTabButton");
	label->setText((CEGUI::utf8*) gettext("Big"));

}



bool Inventory::onSwapEquipClicked(const CEGUI::EventArgs& evt)
{
	m_document->onSwapEquip();
	return true;
}

bool Inventory::onDropGoldClicked(const CEGUI::EventArgs& evt)
{
	CEGUI::Editbox* gold = static_cast<CEGUI::Editbox*>(CEGUIUtility::getWindowForLoadedLayoutEx (m_window, "Inventory/inventory_aux/GoldDropValueBox"));
	int val =0;
	std::stringstream stream;
	stream.str(gold->getText().c_str());
	stream >> val;
	
	if (val != 0)
	{
		gold->setText("0");
		gold->deactivate();
		m_document->dropGold(val);
	}
	return true;
}

bool Inventory::onSwitchTabClicked(const CEGUI::EventArgs& evt)
{
	const CEGUI::WindowEventArgs* args = static_cast<const CEGUI::WindowEventArgs*>(&evt);
	std::string name = args->window->getName().c_str();

	SW_DEBUG ("Inventory: Switch Tab [%s]", name.c_str ());

	if(name == "SmallTabButton")
	{
		setState(Inventory::StateSmall);
	}
	else if(name == "MediumTabButton")
	{
		setState(Inventory::StateMedium);
	}
	else if(name == "BigTabButton")
	{
		setState(Inventory::StateBig);
	}

	return true;
}

bool Inventory::onCloseButtonClicked(const CEGUI::EventArgs& evt)
{
	m_document->onButtonInventoryClicked();
	return true;
}



void Inventory::createAnimations()
{
#if ((CEGUI_VERSION_MAJOR << 16) + (CEGUI_VERSION_MINOR << 8) + CEGUI_VERSION_PATCH >= (0 << 16)+(7 << 8)+5)

	CEGUI::Window* label;

	CEGUI::AnimationManager::getSingleton().loadAnimationsFromXML("InventoryAnimations.xml");
	
	// Small tab animations
	label = CEGUIUtility::getWindowForLoadedLayoutEx (m_window, "Inventory/inventory_aux/StashHolder/StashItemArea/SmallTabMain");
	CEGUI::AnimationInstance* instance = CEGUI::AnimationManager::getSingleton().instantiateAnimation("SlotFadeIn");
	instance->setTargetWindow(label);
    instance->start();
	label->subscribeEvent(CEGUI::Window::EventShown, CEGUI::Event::Subscriber(&CEGUI::AnimationInstance::handleStart, instance));

	// Medium tab animations
	label = CEGUIUtility::getWindowForLoadedLayoutEx (m_window, "Inventory/inventory_aux/StashHolder/StashItemArea/MediumTabMain");
	instance = CEGUI::AnimationManager::getSingleton().instantiateAnimation("SlotFadeIn");
	instance->setTargetWindow(label);
    instance->start();
	label->subscribeEvent(CEGUI::Window::EventShown, CEGUI::Event::Subscriber(&CEGUI::AnimationInstance::handleStart, instance));

	// Big tab animations
	label = CEGUIUtility::getWindowForLoadedLayoutEx (m_window, "Inventory/inventory_aux/StashHolder/StashItemArea/BigTabMain");
	instance = CEGUI::AnimationManager::getSingleton().instantiateAnimation("SlotFadeIn");
	instance->setTargetWindow(label);
    instance->start();
	label->subscribeEvent(CEGUI::Window::EventShown, CEGUI::Event::Subscriber(&CEGUI::AnimationInstance::handleStart, instance));
#endif
}
