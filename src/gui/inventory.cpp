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
#include "sound.h"

Inventory::Inventory (Document* doc)
	: ItemWindow(doc)
{
	DEBUGX("setup inventory");

	CEGUI::WindowManager& win_mgr = CEGUI::WindowManager::getSingleton();

	CEGUI::FrameWindow* inventory = (CEGUI::FrameWindow*)win_mgr.loadWindowLayout( "inventory.layout" );
	m_window = inventory;
	inventory->subscribeEvent(CEGUI::Window::EventMouseButtonDown, CEGUI::Event::Subscriber(&Window::consumeEvent, (Window*) this));
	inventory->setWantsMultiClickEvents(false);


	// Bestandteile des Charakterfensters hinzufuegen
	CEGUI::PushButton* btn;
	CEGUI::Window* label;
	
	label = win_mgr.getWindow("CharacterImage");
	label->setProperty("Image", "set:character image:character_img"); 
	label->setID(Equipement::NONE);
	label->subscribeEvent(CEGUI::Window::EventMouseButtonDown, CEGUI::Event::Subscriber(&Inventory::onItemMouseButtonPressed, (ItemWindow*) this));
	//label->setAlpha(1.0);
	
	int i,j;

	// Label fuer grosse Items
	std::ostringstream outStream;
	for (i=0;i<5;i++)
	{
		outStream.str("");
		outStream << "BigItem" << i<< "Label";
		label = win_mgr.getWindow(outStream.str());
		label->setID(Equipement::BIG_ITEMS+i);
		label->subscribeEvent(CEGUI::Window::EventMouseButtonDown, CEGUI::Event::Subscriber(&Inventory::onItemMouseButtonPressed, (ItemWindow*) this));
		label->subscribeEvent(CEGUI::Window::EventMouseButtonUp, CEGUI::Event::Subscriber(&Inventory::onItemMouseButtonReleased, (ItemWindow*) this));
		label->subscribeEvent(CEGUI::Window::EventMouseEnters, CEGUI::Event::Subscriber(&Inventory::onItemHover, (ItemWindow*) this));
		label->setWantsMultiClickEvents(false);
	}

	// Label fuer mittlere Items
	for (j=0;j<2;j++)
	{
		for (i=0;i<6;i++)
		{
			outStream.str("");
			outStream << "MediumItem" << j*6+i<< "Label";
			label = win_mgr.getWindow(outStream.str());
			label->setID(Equipement::MEDIUM_ITEMS+j*6+i);
			label->subscribeEvent(CEGUI::Window::EventMouseButtonDown, CEGUI::Event::Subscriber(&Inventory::onItemMouseButtonPressed, (ItemWindow*) this));
			label->subscribeEvent(CEGUI::Window::EventMouseButtonUp, CEGUI::Event::Subscriber(&Inventory::onItemMouseButtonReleased, (ItemWindow*) this));
			label->subscribeEvent(CEGUI::Window::EventMouseEnters, CEGUI::Event::Subscriber(&Inventory::onItemHover, (ItemWindow*) this));
			label->setWantsMultiClickEvents(false);
		}
	}
	// Label fuer kleine Items
	for (j=0;j<3;j++)
	{
		for (i=0;i<10;i++)
		{
			outStream.str("");
			outStream << "SmallItem" << j*10+i<< "Label";
			label = win_mgr.getWindow(outStream.str());
			label->setID(Equipement::SMALL_ITEMS+j*10+i);
			label->subscribeEvent(CEGUI::Window::EventMouseButtonDown, CEGUI::Event::Subscriber(&Inventory::onItemMouseButtonPressed, (ItemWindow*) this));
			label->subscribeEvent(CEGUI::Window::EventMouseButtonUp, CEGUI::Event::Subscriber(&Inventory::onItemMouseButtonReleased, (ItemWindow*) this));
			label->subscribeEvent(CEGUI::Window::EventMouseEnters, CEGUI::Event::Subscriber(&Inventory::onItemHover, (ItemWindow*) this));
			label->setWantsMultiClickEvents(false);
		}
	}



	// Label Ruestung
	label = win_mgr.getWindow("ArmorItemLabel");
	label->setID(Equipement::ARMOR);
	label->subscribeEvent(CEGUI::Window::EventMouseButtonDown, CEGUI::Event::Subscriber(&Inventory::onItemMouseButtonPressed, (ItemWindow*) this));
	label->subscribeEvent(CEGUI::Window::EventMouseButtonUp, CEGUI::Event::Subscriber(&Inventory::onItemMouseButtonReleased, (ItemWindow*) this));
	label->subscribeEvent(CEGUI::Window::EventMouseEnters, CEGUI::Event::Subscriber(&Inventory::onItemHover, (ItemWindow*) this));
	label->setWantsMultiClickEvents(false);


	// Label Waffe
	label = win_mgr.getWindow("WeaponItemLabel");
	label->setID(Equipement::WEAPON);
	label->subscribeEvent(CEGUI::Window::EventMouseButtonDown, CEGUI::Event::Subscriber(&Inventory::onItemMouseButtonPressed, (ItemWindow*) this));
	label->subscribeEvent(CEGUI::Window::EventMouseButtonUp, CEGUI::Event::Subscriber(&Inventory::onItemMouseButtonReleased, (ItemWindow*) this));
	label->subscribeEvent(CEGUI::Window::EventMouseEnters, CEGUI::Event::Subscriber(&Inventory::onItemHover, (ItemWindow*) this));
	label->setWantsMultiClickEvents(false);
	
	// Label Helm
	label = win_mgr.getWindow("HelmetItemLabel");
	label->setID(Equipement::HELMET);
	label->subscribeEvent(CEGUI::Window::EventMouseButtonDown, CEGUI::Event::Subscriber(&Inventory::onItemMouseButtonPressed, (ItemWindow*) this));
	label->subscribeEvent(CEGUI::Window::EventMouseButtonUp, CEGUI::Event::Subscriber(&Inventory::onItemMouseButtonReleased, (ItemWindow*) this));
	label->subscribeEvent(CEGUI::Window::EventMouseEnters, CEGUI::Event::Subscriber(&Inventory::onItemHover, (ItemWindow*) this));
	label->setWantsMultiClickEvents(false);
	
	// Label Schild
	label = win_mgr.getWindow("ShieldItemLabel");
	label->setID(Equipement::SHIELD);
	label->subscribeEvent(CEGUI::Window::EventMouseButtonDown, CEGUI::Event::Subscriber(&Inventory::onItemMouseButtonPressed, (ItemWindow*) this));
	label->subscribeEvent(CEGUI::Window::EventMouseButtonUp, CEGUI::Event::Subscriber(&Inventory::onItemMouseButtonReleased, (ItemWindow*) this));
	label->subscribeEvent(CEGUI::Window::EventMouseEnters, CEGUI::Event::Subscriber(&Inventory::onItemHover, (ItemWindow*) this));
	label->setWantsMultiClickEvents(false);
	
	// Label Handschuhe
	label = win_mgr.getWindow("GlovesItemLabel");
	label->setID(Equipement::GLOVES);
	label->subscribeEvent(CEGUI::Window::EventMouseButtonDown, CEGUI::Event::Subscriber(&Inventory::onItemMouseButtonPressed, (ItemWindow*)this));
	label->subscribeEvent(CEGUI::Window::EventMouseButtonUp, CEGUI::Event::Subscriber(&Inventory::onItemMouseButtonReleased, (ItemWindow*)this));
	label->subscribeEvent(CEGUI::Window::EventMouseEnters, CEGUI::Event::Subscriber(&Inventory::onItemHover, (ItemWindow*)this));
	label->setWantsMultiClickEvents(false);
	
	// Ring links
	label = win_mgr.getWindow("RingLeftItemLabel");
	label->setID(Equipement::RING_LEFT);
	label->subscribeEvent(CEGUI::Window::EventMouseButtonDown, CEGUI::Event::Subscriber(&Inventory::onItemMouseButtonPressed, (ItemWindow*)this));
	label->subscribeEvent(CEGUI::Window::EventMouseButtonUp, CEGUI::Event::Subscriber(&Inventory::onItemMouseButtonReleased, (ItemWindow*)this));
	label->subscribeEvent(CEGUI::Window::EventMouseEnters, CEGUI::Event::Subscriber(&Inventory::onItemHover, (ItemWindow*)this));
	label->setWantsMultiClickEvents(false);
	
	// Ring rechts
	label = win_mgr.getWindow("RingRightItemLabel");
	label->setID(Equipement::RING_RIGHT);
	label->subscribeEvent(CEGUI::Window::EventMouseButtonDown, CEGUI::Event::Subscriber(&Inventory::onItemMouseButtonPressed, (ItemWindow*)this));
	label->subscribeEvent(CEGUI::Window::EventMouseButtonUp, CEGUI::Event::Subscriber(&Inventory::onItemMouseButtonReleased, (ItemWindow*)this));
	label->subscribeEvent(CEGUI::Window::EventMouseEnters, CEGUI::Event::Subscriber(&Inventory::onItemHover, (ItemWindow*)this));
	label->setWantsMultiClickEvents(false);
	
	// Amulett
	label = win_mgr.getWindow("AmuletItemLabel");
	label->setID(Equipement::AMULET);
	label->subscribeEvent(CEGUI::Window::EventMouseButtonDown, CEGUI::Event::Subscriber(&Inventory::onItemMouseButtonPressed, (ItemWindow*)this));
	label->subscribeEvent(CEGUI::Window::EventMouseButtonUp, CEGUI::Event::Subscriber(&Inventory::onItemMouseButtonReleased, (ItemWindow*)this));
	label->subscribeEvent(CEGUI::Window::EventMouseEnters, CEGUI::Event::Subscriber(&Inventory::onItemHover, (ItemWindow*)this));
	label->setWantsMultiClickEvents(false);
	
	// Button Ausruestung umschalten
	btn = static_cast<CEGUI::PushButton*>(win_mgr.getWindow("SwapEquipButton"));
	btn->setText("1");
	btn->subscribeEvent(CEGUI::PushButton::EventClicked, CEGUI::Event::Subscriber(&Inventory::onSwapEquipClicked, this));

	// Label drop Gold
	btn = static_cast<CEGUI::PushButton*>(win_mgr.getWindow("GoldDropButton"));
	btn->subscribeEvent(CEGUI::PushButton::EventClicked,CEGUI::Event::Subscriber(&Inventory::onDropGoldClicked, this));
	
	
	// Label drop Gold (Wert)
	CEGUI::Editbox* box;
	box = static_cast<CEGUI::Editbox*>(win_mgr.getWindow("GoldDropValueBox"));
	box->subscribeEvent(CEGUI::Editbox::EventTextAccepted, CEGUI::Event::Subscriber(&Inventory::onDropGoldClicked,  this));
	
	
	// Tab Labels
	setState(Inventory::StateSmall);

	label = win_mgr.getWindow("SmallTabButton");
	label->subscribeEvent(CEGUI::Window::EventMouseClick, CEGUI::Event::Subscriber(&Inventory::onSwitchTabClicked, this));
	label = win_mgr.getWindow("MediumTabButton");
	label->subscribeEvent(CEGUI::Window::EventMouseClick, CEGUI::Event::Subscriber(&Inventory::onSwitchTabClicked, this));
	label = win_mgr.getWindow("BigTabButton");
	label->subscribeEvent(CEGUI::Window::EventMouseClick, CEGUI::Event::Subscriber(&Inventory::onSwitchTabClicked, this));
	
	label = win_mgr.getWindow("InvCloseButton");
	label->subscribeEvent(CEGUI::Window::EventMouseClick, CEGUI::Event::Subscriber(&Inventory::onCloseButtonClicked, this));
	
	createAnimations();

	updateTranslation();
}

void Inventory::update()
{
	CEGUI::WindowManager& win_mgr = CEGUI::WindowManager::getSingleton();
	CEGUI::Window* img;
	CEGUI::Window* label;
	Item* it,*weapon;
	CEGUI::PushButton* btn;
	
	
	std::ostringstream out_stream;

	Player* player = m_document->getLocalPlayer();
	Equipement* equ = player->getEquipement();


	// Label Waffe
	// Label Schild
	img =  win_mgr.getWindow("WeaponItemLabel");
	it = player->getWeapon();
	updateItemWindow(img,it,player);
	weapon = it;
	bool two_hand = false;
	if (weapon !=0 && weapon->m_weapon_attr !=0 && weapon->m_weapon_attr->m_two_handed)
	{
		two_hand = true;
	}


	// Label Ruestung
	img =  win_mgr.getWindow("ArmorItemLabel");
	it = equ->getItem(Equipement::ARMOR);
	updateItemWindow(img,it,player);

	// Label Helm
	img =  win_mgr.getWindow("HelmetItemLabel");
	it = equ->getItem(Equipement::HELMET);
	updateItemWindow(img,it,player);

	// Label Schild
	// bei Zweihandwaffen wird die Waffe eingeblendet
	float alpha = 1.0;
	img =  win_mgr.getWindow("ShieldItemLabel");
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
	

	// Label Handschuhe
	img =  win_mgr.getWindow("GlovesItemLabel");
	it = equ->getItem(Equipement::GLOVES);
	updateItemWindow(img,it,player);

	// Label Ring links
	img =  win_mgr.getWindow("RingLeftItemLabel");
	it = equ->getItem(Equipement::RING_LEFT);
	updateItemWindow(img,it,player);

	// Label Ring rechts
	img =  win_mgr.getWindow("RingRightItemLabel");
	it = equ->getItem(Equipement::RING_RIGHT);
	updateItemWindow(img,it,player);
	

	// Label Amulet
	img =  win_mgr.getWindow("AmuletItemLabel");
	it = equ->getItem(Equipement::AMULET);
	updateItemWindow(img,it,player);
	
	// Button fuer Zweitausruestung
	btn = static_cast<CEGUI::PushButton*>(win_mgr.getWindow("SwapEquipButton"));
	std::string text = "set:Inventory image:WeaponSwitchState1";
	if (player->isUsingSecondaryEquip())
	{
		text="set:Inventory image:WeaponSwitchState2";
	}
	if (btn->getProperty("NormalImage") != text)
	{
		btn->setProperty("NormalImage",text);
		btn->setProperty("HoverImage",text);
		btn->setProperty("PushedImage",text);		
	}
	
	int i;
	
	// Inventar: grosse Items
	for (i=0;i<5;i++)
	{
		out_stream.str("");
		out_stream << "BigItem" << i<< "Label";
		img =  win_mgr.getWindow(out_stream.str().c_str());
		it = equ->getItem(Equipement::BIG_ITEMS+i);
		updateItemWindow(img,it,player);
	}

	// Inventar: mittlere Items
	for (i=0;i<12;i++)
	{
		out_stream.str("");
		out_stream << "MediumItem" << i<< "Label";
		img =  win_mgr.getWindow(out_stream.str().c_str());
		it = equ->getItem(Equipement::MEDIUM_ITEMS+i);
		updateItemWindow(img,it,player);
	}

	// Inventar: kleine Items
	for (i=0;i<30;i++)
	{
		out_stream.str("");
		out_stream << "SmallItem" << i<< "Label";
		img =  win_mgr.getWindow(out_stream.str().c_str());
		it = equ->getItem(Equipement::SMALL_ITEMS+i);
		updateItemWindow(img,it,player);
	}
	
	label =  win_mgr.getWindow("GoldValueLabel");
	out_stream.str("");
	out_stream << equ->getGold();
	if (label->getText()!=out_stream.str())
	{
		label->setText(out_stream.str());
		if (!m_silent)
		{
			SoundSystem::playAmbientSound("sell_buy");
		}
	}
	
}

void Inventory::setState(State s)
{
	CEGUI::WindowManager& win_mgr = CEGUI::WindowManager::getSingleton();
	CEGUI::Window* label;

	if(s == Inventory::StateSmall)
	{
		label = win_mgr.getWindow("SmallTabMain");
		label->setVisible(true);
		label = win_mgr.getWindow("BorderHorzS");
		label->setVisible(false);
		label = win_mgr.getWindow("MediumTabMain");
		label->setVisible(false);
		label = win_mgr.getWindow("BorderHorzM");
		label->setVisible(true);
		label = win_mgr.getWindow("BigTabMain");
		label->setVisible(false);
		label = win_mgr.getWindow("BorderHorzB");
		label->setVisible(true);
	}
	else if(s == Inventory::StateMedium)
	{
		label = win_mgr.getWindow("SmallTabMain");
		label->setVisible(false);
		label = win_mgr.getWindow("BorderHorzS");
		label->setVisible(true);
		label = win_mgr.getWindow("MediumTabMain");
		label->setVisible(true);
		label = win_mgr.getWindow("BorderHorzM");
		label->setVisible(false);
		label = win_mgr.getWindow("BigTabMain");
		label->setVisible(false);
		label = win_mgr.getWindow("BorderHorzB");
		label->setVisible(true);
	}
	else if(s == Inventory::StateBig)
	{
		label = win_mgr.getWindow("SmallTabMain");
		label->setVisible(false);
		label = win_mgr.getWindow("BorderHorzS");
		label->setVisible(true);
		label = win_mgr.getWindow("MediumTabMain");
		label->setVisible(false);
		label = win_mgr.getWindow("BorderHorzM");
		label->setVisible(true);
		label = win_mgr.getWindow("BigTabMain");
		label->setVisible(true);
		label = win_mgr.getWindow("BorderHorzB");
		label->setVisible(false);
	}
}

void Inventory::updateTranslation()
{
	CEGUI::WindowManager& win_mgr = CEGUI::WindowManager::getSingleton();
	CEGUI::Window* label;
	
	label = win_mgr.getWindow("GoldDropButton");
	label->setText((CEGUI::utf8*) gettext("Drop:"));
	
	label = win_mgr.getWindow("InvLabel");
	label->setText((CEGUI::utf8*) gettext("Inventory"));
	
	label = win_mgr.getWindow("SmallTabButton");
	label->setText((CEGUI::utf8*) gettext("Small"));
	
	label = win_mgr.getWindow("MediumTabButton");
	label->setText((CEGUI::utf8*) gettext("Medium"));
	
	label = win_mgr.getWindow("BigTabButton");
	label->setText((CEGUI::utf8*) gettext("Big"));

}



bool Inventory::onSwapEquipClicked(const CEGUI::EventArgs& evt)
{
	m_document->onSwapEquip();
	return true;
}

bool Inventory::onDropGoldClicked(const CEGUI::EventArgs& evt)
{
	CEGUI::WindowManager& win_mgr = CEGUI::WindowManager::getSingleton();

	
	CEGUI::Editbox* gold = static_cast<CEGUI::Editbox*>(win_mgr.getWindow("GoldDropValueBox"));
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

	CEGUI::WindowManager& win_mgr = CEGUI::WindowManager::getSingleton();
	CEGUI::Window* label;

	CEGUI::AnimationManager::getSingleton().loadAnimationsFromXML("InventoryAnimations.xml");
	
	// Small tab animations
	label = win_mgr.getWindow("SmallTabMain");
	CEGUI::AnimationInstance* instance = CEGUI::AnimationManager::getSingleton().instantiateAnimation("SlotFadeIn");
	instance->setTargetWindow(label);
    instance->start();
	label->subscribeEvent(CEGUI::Window::EventShown, CEGUI::Event::Subscriber(&CEGUI::AnimationInstance::handleStart, instance));

	// Medium tab animations
	label = win_mgr.getWindow("MediumTabMain");
	instance = CEGUI::AnimationManager::getSingleton().instantiateAnimation("SlotFadeIn");
	instance->setTargetWindow(label);
    instance->start();
	label->subscribeEvent(CEGUI::Window::EventShown, CEGUI::Event::Subscriber(&CEGUI::AnimationInstance::handleStart, instance));

	// Big tab animations
	label = win_mgr.getWindow("BigTabMain");
	instance = CEGUI::AnimationManager::getSingleton().instantiateAnimation("SlotFadeIn");
	instance->setTargetWindow(label);
    instance->start();
	label->subscribeEvent(CEGUI::Window::EventShown, CEGUI::Event::Subscriber(&CEGUI::AnimationInstance::handleStart, instance));
#endif
}