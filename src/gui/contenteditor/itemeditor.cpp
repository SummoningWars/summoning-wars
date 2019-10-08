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

#include "CEGUI/RendererModules/Ogre/CEGUIOgreRenderer.h"
#include <OgreHardwarePixelBuffer.h>
#include <OgreMeshManager.h>

#include "graphicmanager.h"
#include "itemeditor.h"
#include "renderinfoeditor.h"
#include "contenteditor.h"
#include "player.h"

void ItemEditor::init(CEGUI::Window* parent)
{
	ContentEditorTab::init(parent);
	
	CEGUI::WindowManager& win_mgr = CEGUI::WindowManager::getSingleton();
	
	CEGUI::PushButton* xmlsubmitButton = static_cast<CEGUI::PushButton*>(win_mgr.getWindow("ItemTab/XML/SubmitButton"));
	xmlsubmitButton->subscribeEvent(CEGUI::PushButton::EventClicked, CEGUI::Event::Subscriber(&ItemEditor::onItemXMLModified, this));
	
	CEGUI::PushButton* createDropButton = static_cast<CEGUI::PushButton*>(win_mgr.getWindow("ItemTab/Create/CreateDropButton"));
	createDropButton->subscribeEvent(CEGUI::PushButton::EventClicked, CEGUI::Event::Subscriber(&ItemEditor::onItemCreateDrop, this));
	
	CEGUI::PushButton* createInvButton = static_cast<CEGUI::PushButton*>(win_mgr.getWindow("ItemTab/Create/CreateInventoryButton"));
	createInvButton->subscribeEvent(CEGUI::PushButton::EventClicked, CEGUI::Event::Subscriber(&ItemEditor::onItemCreateInInventory, this));
	
	CEGUI::Spinner* enchantMin =  static_cast<CEGUI::Spinner*>(win_mgr.getWindow("ItemTab/General/EnchantMinSpinner"));
	enchantMin->subscribeEvent(CEGUI::Spinner::EventValueChanged, CEGUI::Event::Subscriber(&ItemEditor::onItemModified, this));
	
	CEGUI::Spinner* enchantMax =  static_cast<CEGUI::Spinner*>(win_mgr.getWindow("ItemTab/General/EnchantMaxSpinner"));
	enchantMax->subscribeEvent(CEGUI::Spinner::EventValueChanged, CEGUI::Event::Subscriber(&ItemEditor::onItemModified, this));
	
	CEGUI::Spinner* price =  static_cast<CEGUI::Spinner*>(win_mgr.getWindow("ItemTab/General/PriceSpinner"));
	price->subscribeEvent(CEGUI::Spinner::EventValueChanged, CEGUI::Event::Subscriber(&ItemEditor::onItemModified, this));
	
	CEGUI::Editbox* nameBox = static_cast<CEGUI::Editbox*>(win_mgr.getWindow("ItemTab/General/NameBox"));
	nameBox->subscribeEvent(CEGUI::Editbox::EventTextChanged, CEGUI::Event::Subscriber(&ItemEditor::onItemModified, this));
	
	CEGUI::Combobox* typeSelector = static_cast<CEGUI::Combobox*>(win_mgr.getWindow("ItemTab/Properties/TypeBox"));
	typeSelector->subscribeEvent(CEGUI::Combobox::EventListSelectionAccepted, CEGUI::Event::Subscriber(&ItemEditor::onItemModified, this));
	typeSelector->addItem(new CEGUI::ListboxTextItem("Armor"));
	typeSelector->addItem(new CEGUI::ListboxTextItem("Weapon"));
	typeSelector->addItem(new CEGUI::ListboxTextItem("Helmet"));
	typeSelector->addItem(new CEGUI::ListboxTextItem("Gloves"));
	typeSelector->addItem(new CEGUI::ListboxTextItem("Ring"));
	typeSelector->addItem(new CEGUI::ListboxTextItem("Amulet"));
	typeSelector->addItem(new CEGUI::ListboxTextItem("Shield"));
	typeSelector->addItem(new CEGUI::ListboxTextItem("Potion"));
	typeSelector->setText("Armor");
	typeSelector->setSelection(0,0);
	
	
	CEGUI::Combobox* sizeSelector = static_cast<CEGUI::Combobox*>(win_mgr.getWindow("ItemTab/Properties/SizeBox"));
	sizeSelector->subscribeEvent(CEGUI::Combobox::EventListSelectionAccepted, CEGUI::Event::Subscriber(&ItemEditor::onItemModified, this));
	sizeSelector->addItem(new CEGUI::ListboxTextItem("Big"));
	sizeSelector->addItem(new CEGUI::ListboxTextItem("Medium"));
	sizeSelector->addItem(new CEGUI::ListboxTextItem("Small"));
	sizeSelector->setText("Big");
	sizeSelector->setSelection(0,0);
	
	CEGUI::PushButton* copyfoButton = static_cast<CEGUI::PushButton*>(win_mgr.getWindow("ItemTab/Properties/CopyDataButton"));
	copyfoButton->subscribeEvent(CEGUI::PushButton::EventClicked, CEGUI::Event::Subscriber(&ItemEditor::onCopyData, this));
	
	// wire weaponattr tab
	CEGUI::Editbox* wtype = static_cast<CEGUI::Editbox*>(win_mgr.getWindow("ItemTab/Weapon/TypeBox"));
	wtype->subscribeEvent(CEGUI::Editbox::EventTextChanged, CEGUI::Event::Subscriber(&ItemEditor::onWeaponModified, this));
	
	CEGUI::Spinner* physMin =  static_cast<CEGUI::Spinner*>(win_mgr.getWindow("ItemTab/Weapon/PhysMinSpinner"));
	physMin->subscribeEvent(CEGUI::Spinner::EventValueChanged, CEGUI::Event::Subscriber(&ItemEditor::onWeaponModified, this));

	CEGUI::Spinner* physMax =  static_cast<CEGUI::Spinner*>(win_mgr.getWindow("ItemTab/Weapon/PhysMaxSpinner"));
	physMax->subscribeEvent(CEGUI::Spinner::EventValueChanged, CEGUI::Event::Subscriber(&ItemEditor::onWeaponModified, this));
	
	CEGUI::Spinner* physMult =  static_cast<CEGUI::Spinner*>(win_mgr.getWindow("ItemTab/Weapon/PhysMultSpinner"));
	physMult->subscribeEvent(CEGUI::Spinner::EventValueChanged, CEGUI::Event::Subscriber(&ItemEditor::onWeaponModified, this));

	CEGUI::Spinner* fireMin =  static_cast<CEGUI::Spinner*>(win_mgr.getWindow("ItemTab/Weapon/FireMinSpinner"));
	fireMin->subscribeEvent(CEGUI::Spinner::EventValueChanged, CEGUI::Event::Subscriber(&ItemEditor::onWeaponModified, this));

	CEGUI::Spinner* fireMax =  static_cast<CEGUI::Spinner*>(win_mgr.getWindow("ItemTab/Weapon/FireMaxSpinner"));
	fireMax->subscribeEvent(CEGUI::Spinner::EventValueChanged, CEGUI::Event::Subscriber(&ItemEditor::onItemModified, this));
	
	CEGUI::Spinner* fireMult =  static_cast<CEGUI::Spinner*>(win_mgr.getWindow("ItemTab/Weapon/FireMultSpinner"));
	fireMult->subscribeEvent(CEGUI::Spinner::EventValueChanged, CEGUI::Event::Subscriber(&ItemEditor::onWeaponModified, this));

	CEGUI::Spinner* iceMin =  static_cast<CEGUI::Spinner*>(win_mgr.getWindow("ItemTab/Weapon/IceMinSpinner"));
	iceMin->subscribeEvent(CEGUI::Spinner::EventValueChanged, CEGUI::Event::Subscriber(&ItemEditor::onWeaponModified, this));

	CEGUI::Spinner* iceMax =  static_cast<CEGUI::Spinner*>(win_mgr.getWindow("ItemTab/Weapon/IceMaxSpinner"));
	iceMax->subscribeEvent(CEGUI::Spinner::EventValueChanged, CEGUI::Event::Subscriber(&ItemEditor::onWeaponModified, this));
	
	CEGUI::Spinner* iceMult =  static_cast<CEGUI::Spinner*>(win_mgr.getWindow("ItemTab/Weapon/IceMultSpinner"));
	iceMult->subscribeEvent(CEGUI::Spinner::EventValueChanged, CEGUI::Event::Subscriber(&ItemEditor::onWeaponModified, this));

	CEGUI::Spinner* airMin =  static_cast<CEGUI::Spinner*>(win_mgr.getWindow("ItemTab/Weapon/AirMinSpinner"));
	airMin->subscribeEvent(CEGUI::Spinner::EventValueChanged, CEGUI::Event::Subscriber(&ItemEditor::onWeaponModified, this));

	CEGUI::Spinner* airMax =  static_cast<CEGUI::Spinner*>(win_mgr.getWindow("ItemTab/Weapon/AirMaxSpinner"));
	airMax->subscribeEvent(CEGUI::Spinner::EventValueChanged, CEGUI::Event::Subscriber(&ItemEditor::onWeaponModified, this));
	
	CEGUI::Spinner* airMult =  static_cast<CEGUI::Spinner*>(win_mgr.getWindow("ItemTab/Weapon/AirMultSpinner"));
	airMult->subscribeEvent(CEGUI::Spinner::EventValueChanged, CEGUI::Event::Subscriber(&ItemEditor::onWeaponModified, this));
	
	CEGUI::Spinner* precision =  static_cast<CEGUI::Spinner*>(win_mgr.getWindow("ItemTab/Weapon/PrecisionSpinner"));
	precision->subscribeEvent(CEGUI::Spinner::EventValueChanged, CEGUI::Event::Subscriber(&ItemEditor::onWeaponModified, this));
	
	CEGUI::Spinner* power =  static_cast<CEGUI::Spinner*>(win_mgr.getWindow("ItemTab/Weapon/PowerSpinner"));
	power->subscribeEvent(CEGUI::Spinner::EventValueChanged, CEGUI::Event::Subscriber(&ItemEditor::onWeaponModified, this));
	
	CEGUI::Spinner* range =  static_cast<CEGUI::Spinner*>(win_mgr.getWindow("ItemTab/Weapon/RangeSpinner"));
	range->subscribeEvent(CEGUI::Spinner::EventValueChanged, CEGUI::Event::Subscriber(&ItemEditor::onWeaponModified, this));
	
	CEGUI::Spinner* speed =  static_cast<CEGUI::Spinner*>(win_mgr.getWindow("ItemTab/Weapon/SpeedSpinner"));
	speed->subscribeEvent(CEGUI::Spinner::EventValueChanged, CEGUI::Event::Subscriber(&ItemEditor::onWeaponModified, this));
	
	CEGUI::Spinner* critperc =  static_cast<CEGUI::Spinner*>(win_mgr.getWindow("ItemTab/Weapon/CritPercentSpinner"));
	critperc->subscribeEvent(CEGUI::Spinner::EventValueChanged, CEGUI::Event::Subscriber(&ItemEditor::onWeaponModified, this));
	
	CEGUI::Checkbox* twohanded = static_cast<CEGUI::Checkbox*>(win_mgr.getWindow("ItemTab/Weapon/TwohandedCheckbox"));
	twohanded->subscribeEvent(CEGUI::Checkbox::EventCheckStateChanged, CEGUI::Event::Subscriber(&ItemEditor::onWeaponModified, this));
	
	
	// Wire the consume tab
	static_cast<CEGUI::Spinner*>(win_mgr.getWindow("ItemTab/Consume/HealthSpinner"))-> subscribeEvent(CEGUI::Spinner::EventValueChanged, CEGUI::Event::Subscriber(&ItemEditor::onConsumeEffectsModified, this));
	static_cast<CEGUI::Spinner*>(win_mgr.getWindow("ItemTab/Consume/BlindSpinner"))-> subscribeEvent(CEGUI::Spinner::EventValueChanged, CEGUI::Event::Subscriber(&ItemEditor::onConsumeEffectsModified, this));
	static_cast<CEGUI::Spinner*>(win_mgr.getWindow("ItemTab/Consume/PoisonedSpinner"))-> subscribeEvent(CEGUI::Spinner::EventValueChanged, CEGUI::Event::Subscriber(&ItemEditor::onConsumeEffectsModified, this));
	static_cast<CEGUI::Spinner*>(win_mgr.getWindow("ItemTab/Consume/BerserkSpinner"))-> subscribeEvent(CEGUI::Spinner::EventValueChanged, CEGUI::Event::Subscriber(&ItemEditor::onConsumeEffectsModified, this));
	static_cast<CEGUI::Spinner*>(win_mgr.getWindow("ItemTab/Consume/ConfusedSpinner"))-> subscribeEvent(CEGUI::Spinner::EventValueChanged, CEGUI::Event::Subscriber(&ItemEditor::onConsumeEffectsModified, this));
	static_cast<CEGUI::Spinner*>(win_mgr.getWindow("ItemTab/Consume/MuteSpinner"))-> subscribeEvent(CEGUI::Spinner::EventValueChanged, CEGUI::Event::Subscriber(&ItemEditor::onConsumeEffectsModified, this));
	static_cast<CEGUI::Spinner*>(win_mgr.getWindow("ItemTab/Consume/ParalyzedSpinner"))-> subscribeEvent(CEGUI::Spinner::EventValueChanged, CEGUI::Event::Subscriber(&ItemEditor::onConsumeEffectsModified, this));
	static_cast<CEGUI::Spinner*>(win_mgr.getWindow("ItemTab/Consume/FrozenSpinner"))-> subscribeEvent(CEGUI::Spinner::EventValueChanged, CEGUI::Event::Subscriber(&ItemEditor::onConsumeEffectsModified, this));
	static_cast<CEGUI::Spinner*>(win_mgr.getWindow("ItemTab/Consume/BurningSpinner"))-> subscribeEvent(CEGUI::Spinner::EventValueChanged, CEGUI::Event::Subscriber(&ItemEditor::onConsumeEffectsModified, this));
	
	// wire the equip tab
	static_cast<CEGUI::Spinner*>(win_mgr.getWindow("ItemTab/Equip/HealthSpinner"))-> subscribeEvent(CEGUI::Spinner::EventValueChanged, CEGUI::Event::Subscriber(&ItemEditor::onEquipEffectsModified, this));
	static_cast<CEGUI::Spinner*>(win_mgr.getWindow("ItemTab/Equip/StrengthSpinner"))-> subscribeEvent(CEGUI::Spinner::EventValueChanged, CEGUI::Event::Subscriber(&ItemEditor::onEquipEffectsModified, this));
	static_cast<CEGUI::Spinner*>(win_mgr.getWindow("ItemTab/Equip/DexteritySpinner"))-> subscribeEvent(CEGUI::Spinner::EventValueChanged, CEGUI::Event::Subscriber(&ItemEditor::onEquipEffectsModified, this));
	static_cast<CEGUI::Spinner*>(win_mgr.getWindow("ItemTab/Equip/MagicPowerSpinner"))-> subscribeEvent(CEGUI::Spinner::EventValueChanged, CEGUI::Event::Subscriber(&ItemEditor::onEquipEffectsModified, this));
	static_cast<CEGUI::Spinner*>(win_mgr.getWindow("ItemTab/Equip/WillpowerSpinner"))-> subscribeEvent(CEGUI::Spinner::EventValueChanged, CEGUI::Event::Subscriber(&ItemEditor::onEquipEffectsModified, this));
	static_cast<CEGUI::Spinner*>(win_mgr.getWindow("ItemTab/Equip/PhysResSpinner"))-> subscribeEvent(CEGUI::Spinner::EventValueChanged, CEGUI::Event::Subscriber(&ItemEditor::onEquipEffectsModified, this));
	static_cast<CEGUI::Spinner*>(win_mgr.getWindow("ItemTab/Equip/FireResSpinner"))-> subscribeEvent(CEGUI::Spinner::EventValueChanged, CEGUI::Event::Subscriber(&ItemEditor::onEquipEffectsModified, this));
	static_cast<CEGUI::Spinner*>(win_mgr.getWindow("ItemTab/Equip/IceResSpinner"))-> subscribeEvent(CEGUI::Spinner::EventValueChanged, CEGUI::Event::Subscriber(&ItemEditor::onEquipEffectsModified, this));
	static_cast<CEGUI::Spinner*>(win_mgr.getWindow("ItemTab/Equip/AirResSpinner"))-> subscribeEvent(CEGUI::Spinner::EventValueChanged, CEGUI::Event::Subscriber(&ItemEditor::onEquipEffectsModified, this));
	static_cast<CEGUI::Spinner*>(win_mgr.getWindow("ItemTab/Equip/ArmorSpinner"))-> subscribeEvent(CEGUI::Spinner::EventValueChanged, CEGUI::Event::Subscriber(&ItemEditor::onEquipEffectsModified, this));
	static_cast<CEGUI::Spinner*>(win_mgr.getWindow("ItemTab/Equip/BlockSpinner"))-> subscribeEvent(CEGUI::Spinner::EventValueChanged, CEGUI::Event::Subscriber(&ItemEditor::onEquipEffectsModified, this));
	static_cast<CEGUI::Spinner*>(win_mgr.getWindow("ItemTab/Equip/AttackSpinner"))-> subscribeEvent(CEGUI::Spinner::EventValueChanged, CEGUI::Event::Subscriber(&ItemEditor::onEquipEffectsModified, this));
	static_cast<CEGUI::Spinner*>(win_mgr.getWindow("ItemTab/Equip/PowerSpinner"))-> subscribeEvent(CEGUI::Spinner::EventValueChanged, CEGUI::Event::Subscriber(&ItemEditor::onEquipEffectsModified, this));
	
	
	// init the internal data
	TiXmlElement * item_root = new TiXmlElement("Item");  
	m_item_xml.LinkEndChild( item_root );  
	item_root->SetAttribute("subtype","EditorItem");
	
	TiXmlElement * item_image = new TiXmlElement("Image");  
	item_image->SetAttribute("image","set:noMedia.png image:full_image");
	item_root->LinkEndChild(item_image);
	
	TiXmlElement * item_ri = new TiXmlElement("RenderInfo");  
	item_ri->SetAttribute("name","EditorRenderInfo");
	item_root->LinkEndChild(item_ri);
	
	// init the data
	m_edited_item.m_subtype = "EditorItem";
	
	m_update_base_content = true;
	m_modified_item = true;
	
	// create camera for item photos
	Ogre::SceneManager* editor_scene_mng = Ogre::Root::getSingleton().getSceneManager("EditorSceneManager");
	Ogre::Camera* item_camera = editor_scene_mng->createCamera("item_camera");
	item_camera->setNearClipDistance(0.1);
	item_camera->setAspectRatio(1.0);
	item_camera->setPosition(Ogre::Vector3(0,1,0));
	item_camera->lookAt(Ogre::Vector3(0,0,0));
	
	m_unique_id = 1;

}

void ItemEditor::update()
{
	if (m_update_base_content == true)
	{
		updateAllItemList();
		m_update_base_content = false;
	}
	
	if (m_modified_item)
	{
		updateItemXML();
	}
	
	if (m_modified_item_xml)
	{
		updateItemEditor();
	}
	
	m_modified_item = false;
	m_modified_item_xml = false;
}

void ItemEditor::updateAllItemList()
{
	CEGUI::WindowManager& win_mgr = CEGUI::WindowManager::getSingleton();
	
	CEGUI::Combobox* copyItemSelector = static_cast<CEGUI::Combobox*>(win_mgr.getWindow("ItemTab/Properties/CopyDataBox"));
	
	// Fill list of all FixedObjects
	const std::map<Item::Subtype,ItemBasicData*>& all_items = ItemFactory::getAllItemData();
	std::map<Item::Subtype,ItemBasicData*>::const_iterator it;
	for (it = all_items.begin(); it != all_items.end(); ++it)
	{
		copyItemSelector->addItem(new CEGUI::ListboxTextItem(it->first.c_str()));
	}
}

bool ItemEditor::onItemModified(const CEGUI::EventArgs& evt)
{
	if (m_no_cegui_events)
		return true;
	m_no_cegui_events = true;
	
	m_edited_item.m_price = (int) getSpinnerValue("ItemTab/General/PriceSpinner",1);
	m_edited_item.m_min_enchant = getSpinnerValue("ItemTab/General/EnchantMinSpinner",1);
	m_edited_item.m_max_enchant = getSpinnerValue("ItemTab/General/EnchantMaxSpinner",1);
	if (m_edited_item.m_max_enchant < m_edited_item.m_min_enchant)
	{
		m_edited_item.m_max_enchant = m_edited_item.m_min_enchant;
		setSpinnerValue("ItemTab/General/EnchantMaxSpinner",m_edited_item.m_max_enchant);
	}
	m_edited_item.m_name = getWindowText("ItemTab/General/NameBox","EditorItem");
	
	std::string type = getWindowText("ItemTab/Properties/TypeBox","Weapon");
	m_edited_item.m_type = Item::WEAPON;
	if (type == "Armor") m_edited_item.m_type = Item::ARMOR;
	else if (type == "Weapon") m_edited_item.m_type = Item::WEAPON;
	else if (type == "Helmet") m_edited_item.m_type = Item::HELMET;
	else if (type == "Gloves") m_edited_item.m_type = Item::GLOVES;
	else if (type == "Ring") m_edited_item.m_type = Item::RING;
	else if (type == "Amulet") m_edited_item.m_type = Item::AMULET;
	else if (type == "Shield") m_edited_item.m_type = Item::SHIELD;
	else if (type == "Potion") m_edited_item.m_type = Item::POTION;

	std::string size = getWindowText("ItemTab/Properties/SizeBox","Big");
	m_edited_item.m_size = Item::BIG;
	if (size == "Big") m_edited_item.m_size = Item::BIG;
	else if (size == "Medium") m_edited_item.m_size = Item::MEDIUM;
	else if (size == "Small") m_edited_item.m_size = Item::SMALL;

	m_no_cegui_events = false;
	m_modified_item = true;
	
	return true;
}

bool ItemEditor::onWeaponModified(const CEGUI::EventArgs& evt)
{
	if (m_no_cegui_events)
		return true;
	m_no_cegui_events = true;
	
	if (m_edited_item.m_weapon_attr == 0)
	{
		m_edited_item.m_weapon_attr = new WeaponAttr;
	}
	
	m_edited_item.m_weapon_attr->m_weapon_type = getWindowText("ItemTab/Weapon/TypeLabel","sword");
	
	Damage& dmg = m_edited_item.m_weapon_attr->m_damage;
	
	dmg.m_min_damage[Damage::PHYSICAL] = getSpinnerValue("ItemTab/Weapon/PhysMinSpinner",1);
	dmg.m_max_damage[Damage::PHYSICAL] = getSpinnerValue("ItemTab/Weapon/PhysMaxSpinner",1);
	if (dmg.m_max_damage[Damage::PHYSICAL] < dmg.m_min_damage[Damage::PHYSICAL])
	{
		dmg.m_max_damage[Damage::PHYSICAL] =  dmg.m_min_damage[Damage::PHYSICAL];
		setSpinnerValue("ItemTab/Weapon/PhysMaxSpinner",dmg.m_max_damage[Damage::PHYSICAL]);
	}
	dmg.m_multiplier[Damage::PHYSICAL] = getSpinnerValue("ItemTab/Weapon/PhysMultSpinner",1);
	
	dmg.m_min_damage[Damage::FIRE] = getSpinnerValue("ItemTab/Weapon/FireMinSpinner",1);
	dmg.m_max_damage[Damage::FIRE] = getSpinnerValue("ItemTab/Weapon/FireMaxSpinner",1);
	if (dmg.m_max_damage[Damage::FIRE] < dmg.m_min_damage[Damage::FIRE])
	{
		dmg.m_max_damage[Damage::FIRE] =  dmg.m_min_damage[Damage::FIRE];
		setSpinnerValue("ItemTab/Weapon/FireMaxSpinner",dmg.m_max_damage[Damage::FIRE]);
	}
	dmg.m_multiplier[Damage::FIRE] = getSpinnerValue("ItemTab/Weapon/FireMultSpinner",1);
	
	dmg.m_min_damage[Damage::ICE] = getSpinnerValue("ItemTab/Weapon/IceMinSpinner",1);
	dmg.m_max_damage[Damage::ICE] = getSpinnerValue("ItemTab/Weapon/IceMaxSpinner",1);
	if (dmg.m_max_damage[Damage::ICE] < dmg.m_min_damage[Damage::ICE])
	{
		dmg.m_max_damage[Damage::ICE] =  dmg.m_min_damage[Damage::ICE];
		setSpinnerValue("ItemTab/Weapon/IceMaxSpinner",dmg.m_max_damage[Damage::ICE]);
	}
	dmg.m_multiplier[Damage::ICE] = getSpinnerValue("ItemTab/Weapon/IceMultSpinner",1);
	
	dmg.m_min_damage[Damage::AIR] = getSpinnerValue("ItemTab/Weapon/AirMinSpinner",1);
	dmg.m_max_damage[Damage::AIR] = getSpinnerValue("ItemTab/Weapon/AirMaxSpinner",1);
	if (dmg.m_max_damage[Damage::AIR] < dmg.m_min_damage[Damage::AIR])
	{
		dmg.m_max_damage[Damage::AIR] =  dmg.m_min_damage[Damage::AIR];
		setSpinnerValue("ItemTab/Weapon/AirMaxSpinner",dmg.m_max_damage[Damage::AIR]);
	}
	dmg.m_multiplier[Damage::AIR] = getSpinnerValue("ItemTab/Weapon/AirMultSpinner",1);
	
	dmg.m_attack = getSpinnerValue("ItemTab/Weapon/PrecisionSpinner",1);
	dmg.m_power = getSpinnerValue("ItemTab/Weapon/PowerSpinner",1);
	dmg.m_crit_perc = 0.01* getSpinnerValue("ItemTab/Weapon/CritPercentSpinner",1);
	
	m_edited_item.m_weapon_attr->m_attack_range = getSpinnerValue("ItemTab/Weapon/RangeSpinner");
	m_edited_item.m_weapon_attr->m_dattack_speed = (int) getSpinnerValue("ItemTab/Weapon/SpeedSpinner");
	m_edited_item.m_weapon_attr->m_two_handed = getCheckboxSelected("ItemTab/Weapon/TwohandedCheckbox");
	
	m_no_cegui_events = false;
	m_modified_item = true;
	
	return true;
}

bool ItemEditor::onConsumeEffectsModified(const CEGUI::EventArgs& evt)
{
	if (m_no_cegui_events)
		return true;
	m_no_cegui_events = true;
	
	if (m_edited_item.m_useup_effect == 0)
	{
		m_edited_item.m_useup_effect = new CreatureDynAttrMod;
	}
	
	CreatureDynAttrMod* dynmod = m_edited_item.m_useup_effect;
	
	dynmod->m_dhealth = getSpinnerValue("ItemTab/Consume/HealthSpinner");
	dynmod->m_dstatus_mod_immune_time[Damage::BLIND] = getSpinnerValue("ItemTab/Consume/BlindSpinner");
	dynmod->m_dstatus_mod_immune_time[Damage::POISONED] = getSpinnerValue("ItemTab/Consume/PoisonedSpinner");
	dynmod->m_dstatus_mod_immune_time[Damage::BERSERK] = getSpinnerValue("ItemTab/Consume/BerserkSpinner");
	dynmod->m_dstatus_mod_immune_time[Damage::CONFUSED] = getSpinnerValue("ItemTab/Consume/ConfusedSpinner");
	dynmod->m_dstatus_mod_immune_time[Damage::MUTE] = getSpinnerValue("ItemTab/Consume/MuteSpinner");
	dynmod->m_dstatus_mod_immune_time[Damage::PARALYZED] = getSpinnerValue("ItemTab/Consume/ParalyzedSpinner");
	dynmod->m_dstatus_mod_immune_time[Damage::FROZEN] = getSpinnerValue("ItemTab/Consume/FrozenSpinner");
	dynmod->m_dstatus_mod_immune_time[Damage::BURNING] = getSpinnerValue("ItemTab/Consume/BurningSpinner");
	
	m_no_cegui_events = false;
	m_modified_item = true;
	
	return true;
}

bool ItemEditor::onEquipEffectsModified(const CEGUI::EventArgs& evt)
{
	if (m_no_cegui_events)
		return true;
	m_no_cegui_events = true;
	
	if (m_edited_item.m_equip_effect == 0)
	{
		m_edited_item.m_equip_effect = new CreatureBaseAttrMod;
	}
	
	CreatureBaseAttrMod* basemod = m_edited_item.m_equip_effect;
	
	basemod->m_dmax_health = getSpinnerValue("ItemTab/Equip/HealthSpinner");
	
	basemod->m_dstrength = (int) getSpinnerValue("ItemTab/Equip/StrengthSpinner");
	basemod->m_ddexterity = (int) getSpinnerValue("ItemTab/Equip/DexteritySpinner");
	basemod->m_dmagic_power = (int) getSpinnerValue("ItemTab/Equip/MagicPowerSpinner");
	basemod->m_dwillpower = (int) getSpinnerValue("ItemTab/Equip/WillpowerSpinner");
	
	basemod->m_dresistances[Damage::PHYSICAL] = (int) getSpinnerValue("ItemTab/Equip/PhysResSpinner");
	basemod->m_dresistances[Damage::FIRE] = (int) getSpinnerValue("ItemTab/Equip/FireResSpinner");
	basemod->m_dresistances[Damage::ICE] = (int) getSpinnerValue("ItemTab/Equip/IceResSpinner");
	basemod->m_dresistances[Damage::AIR] = (int) getSpinnerValue("ItemTab/Equip/AirResSpinner");
	
	basemod->m_darmor = (int) getSpinnerValue("ItemTab/Equip/ArmorSpinner");
	basemod->m_dblock = (int) getSpinnerValue("ItemTab/Equip/BlockSpinner");
	basemod->m_dattack = (int) getSpinnerValue("ItemTab/Equip/AttackSpinner");
	basemod->m_dpower = (int) getSpinnerValue("ItemTab/Equip/PowerSpinner");
	
	
	m_no_cegui_events = false;
	m_modified_item = true;
	
	return true;
}

bool ItemEditor::onItemXMLModified(const CEGUI::EventArgs& evt)
{
	CEGUI::WindowManager& win_mgr = CEGUI::WindowManager::getSingleton();
	CEGUI::MultiLineEditbox* editor = static_cast<CEGUI::MultiLineEditbox*>(win_mgr.getWindow("ItemTab/XML/ItemXMLEditbox"));
	
	// Parse the editor text to XML
	// use temporary XML document for recovering from errors
	TiXmlDocument item_temp_xml;
	item_temp_xml.Parse(editor->getText().c_str());
	
	if (!item_temp_xml.Error())
	{
		// first, remove the old root
		TiXmlElement* oldroot = m_item_xml.RootElement();
		if (oldroot != 0)
		{
			m_item_xml.RemoveChild(oldroot);
		}
		// clone the first Element
		m_item_xml.LinkEndChild(item_temp_xml.RootElement()->Clone());
		
		// parse the XML to the fixed object data
		std::string subtype = ItemLoader::loadItem(m_item_xml.FirstChildElement(),true);
		// copy to the local Data structure
		ItemBasicData* data = ItemFactory::getItemBasicData(subtype);
		if (data != 0)
		{
			m_edited_item = *data;
		}
		
		m_modified_item_xml = true;
	}
	else
	{
		// XML parse error
		// set the cursor to the position of the first error
		int err_row = item_temp_xml.ErrorRow();
		int err_col = item_temp_xml.ErrorCol();
		
		setMultiLineEditboxCursor("ItemTab/XML/ItemXMLEditbox",err_row,err_col);
	}
	
	return true;
}

Item* ItemEditor::createItem()
{
	CEGUI::WindowManager& win_mgr = CEGUI::WindowManager::getSingleton();
	RenderInfoEditor* ri_editor = dynamic_cast<RenderInfoEditor*>(ContentEditor::getSingleton().getComponent("RIEditor"));
	
	// create an item picture
	std::stringstream idstream;
	idstream << m_unique_id;
	
	Ogre::SceneManager* editor_scene_mng = Ogre::Root::getSingleton().getSceneManager("EditorSceneManager");
	Ogre::Camera* item_camera = editor_scene_mng->getCamera("item_camera");
	
	// adjust the camera
	Ogre::SceneNode* topnode = ri_editor->getEditedGraphicObject()->getTopNode();
	topnode->_updateBounds();
	
	Ogre::Vector3 bbox_min(1000,1000,1000);
	Ogre::Vector3 bbox_max(-1000,-1000,-1000);
	
	getNodeBounds(topnode,bbox_min,bbox_max);
	
	double size_x = bbox_max[0] - bbox_min[0];
	double size_z = bbox_max[2] - bbox_min[2];
	double center_x = 0.5*(bbox_max[0] + bbox_min[0]);
	double center_z = 0.5*(bbox_max[2] + bbox_min[2]);
	double viewsize = MathHelper::Max(size_x, size_z);
	
	//DEBUG("box %f %f %f - %f %f %f",bbox_min[0],bbox_min[1],bbox_min[2],bbox_max[0],bbox_max[1],bbox_max[2]);
	
	item_camera->setPosition(Ogre::Vector3(center_x, viewsize*sqrt(double(2)),center_z));
	// item_camera->setPosition(Ogre::Vector3(center_x, 1,center_z));
	//item_camera->setPosition(Ogre::Vector3(0,1,0));
	item_camera->lookAt(Ogre::Vector3(center_x, 0,center_z));
	
	// texture that is created from the camera image
	Ogre::TexturePtr item_texture = Ogre::TextureManager::getSingleton().createManual( std::string("item_tex_") + idstream.str(),
			Ogre::ResourceGroupManager::DEFAULT_RESOURCE_GROUP_NAME, Ogre::TEX_TYPE_2D,
   256, 256, 0, Ogre::PF_R8G8B8A8, Ogre::TU_RENDERTARGET );
	
   // connection texture and camera via RenderTarget
	Ogre::RenderTarget* item_rt = item_texture->getBuffer()->getRenderTarget();
	item_rt->setAutoUpdated(false);
	Ogre::Viewport *item_view = item_rt->addViewport(item_camera );
	item_view->setClearEveryFrame( true );
	item_view->setOverlaysEnabled (false);
	item_view->setBackgroundColour(Ogre::ColourValue(0,0,0,1.0) );

	// create a CEGUI Image from the Texture
    CEGUI::Texture& item_ceguiTex = static_cast<CEGUI::OgreRenderer*>(CEGUI::System::getSingleton().getRenderer())->createTexture(item_texture);
    
	CEGUI::Imageset& item_textureImageSet = CEGUI::ImagesetManager::getSingleton().create(std::string("item_imageset_") + idstream.str(), item_ceguiTex);

	item_textureImageSet.defineImage( "item_img_"+ idstream.str(),
			CEGUI::Point( 0.0f, 0.0f ),
			CEGUI::Size( item_ceguiTex.getSize().d_width, item_ceguiTex.getSize().d_height ),
			CEGUI::Point( 0.0f, 0.0f ) );
	
	item_rt->update();
	
	CEGUI::Window* label = win_mgr.getWindow("ItemTab/BM/itemPreview");

	// reparse and update the FixedObject Data
	// create a unique renderinfo (to avoid that the object is modified by the editor after creation)
	std::string unique_ri = ri_editor->getUniqueRenderinfo();
	// temporarily replace the renderinfo name
	TiXmlElement * item_ri = m_item_xml.RootElement()->FirstChildElement("RenderInfo");
	if (item_ri == 0)
		return 0;
	std::string name = item_ri->Attribute("name");
	item_ri->SetAttribute("name",unique_ri.c_str());
	
	// temporarily replace the image name
	TiXmlElement * item_image = m_item_xml.RootElement()->FirstChildElement("Image");
	if (item_image == 0)
		return 0;
	std::string image = item_image->Attribute("image");
	
	if (image == "set:noMedia.png image:full_image")
	{
		std::stringstream itemimage;
		itemimage <<  "set:item_imageset_" << idstream.str()<< " " << "image:item_img_" << idstream.str();
		label->setProperty("Image", itemimage.str().c_str());
		
		// itemimage.str("");
		// itemimage << "set:editor_imageset image:editor_img";
		item_image->SetAttribute("image",itemimage.str().c_str());
	}

	// make the item subtype unique by adding a number
	std::string plain_subtype = m_item_xml.RootElement()->Attribute("subtype");
	std::stringstream stream;
	stream << plain_subtype << m_unique_id;
	m_unique_id++;
	m_item_xml.RootElement()->SetAttribute("subtype",stream.str().c_str());
	
	std::string subtype = ItemLoader::loadItem(m_item_xml.FirstChildElement(),true);
	
	// reset the changed attributes
	item_ri->SetAttribute("name",name.c_str());
	m_item_xml.RootElement()->SetAttribute("subtype",plain_subtype.c_str());
	item_image->SetAttribute("image",image.c_str());
	
	float magic = getSpinnerValue("ItemTab/Create/EnchantSpinner",0);
	Item::Type type = ItemFactory::getBaseType(subtype);
	Item* item = ItemFactory::createItem(type, subtype, 0, magic);
	return item;
}

bool ItemEditor::onItemCreateDrop(const CEGUI::EventArgs& evt)
{
	World* world = World::getWorld();
	if (world == 0)
		return true;
	
	WorldObject* player = world->getLocalPlayer();
	if (player == 0)
		return true;
	Region* region = player->getRegion();
	
	// if the position is set to default, use the player position
	Vector pos = player->getShape()->m_center;
	
	// create the object
	Item* item = createItem();
	if (item == 0)
		return true;
	region->dropItem(item,pos);
	
	return true;
}

bool ItemEditor::onItemCreateInInventory(const CEGUI::EventArgs& evt)
{
	World* world = World::getWorld();
	if (world == 0)
		return true;
	
	WorldObject* player = world->getLocalPlayer();
	if (player == 0)
		return true;

	Item* item = createItem();
	if (item == 0)
		return true;
	static_cast<Player*>(player)->insertItem(item);
	return true;
}

bool ItemEditor::onCopyData(const CEGUI::EventArgs& evt)
{
	std::string objname	= getComboboxSelection("ItemTab/Properties/CopyDataBox", "");
	if (objname == "")
		return true;
	
	ItemBasicData* data = ItemFactory::getItemBasicData(objname);
	data->m_subtype = "EditorItem";
	
	if (data == 0)
		return true;
	
	m_edited_item = *data;
	
	m_modified_item = true;
	m_modified_item_xml = true;
	
	return true;
}

void ItemEditor::updateItemXML()
{
	CEGUI::WindowManager& win_mgr = CEGUI::WindowManager::getSingleton();
	
	// update the XML representation
	m_edited_item.writeToXML(m_item_xml.FirstChildElement());
	
	// write to the editor
	TiXmlPrinter printer;
	m_item_xml.Accept(&printer);
	
	CEGUI::MultiLineEditbox* editor = static_cast<CEGUI::MultiLineEditbox*>(win_mgr.getWindow("ItemTab/XML/ItemXMLEditbox"));
	editor->setText(printer.CStr());
}

void ItemEditor::updateItemEditor()
{
	m_no_cegui_events = true;
	
	setSpinnerValue("ItemTab/General/PriceSpinner",m_edited_item.m_price);
	setSpinnerValue("ItemTab/General/EnchantMinSpinner",m_edited_item.m_min_enchant);
	setSpinnerValue("ItemTab/General/EnchantMaxSpinner",m_edited_item.m_max_enchant);
	setWindowText("ItemTab/General/NameBox",m_edited_item.m_name);
	
	Item::Type type = m_edited_item.m_type;
	if (type == Item::ARMOR) setComboboxSelection("ItemTab/Properties/TypeBox","Armor");
	else if (type == Item::WEAPON) setComboboxSelection("ItemTab/Properties/TypeBox","Weapon");
	else if (type == Item::HELMET) setComboboxSelection("ItemTab/Properties/TypeBox","Helmet");
	else if (type == Item::GLOVES) setComboboxSelection("ItemTab/Properties/TypeBox","Gloves");
	else if (type == Item::RING) setComboboxSelection("ItemTab/Properties/TypeBox","Ring");
	else if (type == Item::AMULET) setComboboxSelection("ItemTab/Properties/TypeBox","Amulet");
	else if (type == Item::SHIELD) setComboboxSelection("ItemTab/Properties/TypeBox","Shield");
	else if (type == Item::POTION) setComboboxSelection("ItemTab/Properties/TypeBox","Potion");
	
	Item::Size size = m_edited_item.m_size;
	if (size == Item::BIG) setComboboxSelection("ItemTab/Properties/SizeBox","Big");
	else if (size == Item::MEDIUM) setComboboxSelection("ItemTab/Properties/SizeBox","Medium");
	else if (size == Item::SMALL) setComboboxSelection("ItemTab/Properties/SizeBox","Small");
	
	// update weapon properties
	if (m_edited_item.m_weapon_attr != 0)
	{
		Damage& dmg = m_edited_item.m_weapon_attr->m_damage;
		
		setWindowText("ItemTab/Weapon/TypeBox",m_edited_item.m_weapon_attr->m_weapon_type);

		setSpinnerValue("ItemTab/Weapon/PhysMinSpinner",dmg.m_min_damage[Damage::PHYSICAL]);
		setSpinnerValue("ItemTab/Weapon/PhysMaxSpinner",dmg.m_max_damage[Damage::PHYSICAL]);
		setSpinnerValue("ItemTab/Weapon/PhysMultSpinner",dmg.m_multiplier[Damage::PHYSICAL]);
		
		setSpinnerValue("ItemTab/Weapon/FireMinSpinner",dmg.m_min_damage[Damage::FIRE]);
		setSpinnerValue("ItemTab/Weapon/FireMaxSpinner",dmg.m_max_damage[Damage::FIRE]);
		setSpinnerValue("ItemTab/Weapon/FireMultSpinner",dmg.m_multiplier[Damage::FIRE]);
		
		setSpinnerValue("ItemTab/Weapon/IceMinSpinner",dmg.m_min_damage[Damage::ICE]);
		setSpinnerValue("ItemTab/Weapon/IceMaxSpinner",dmg.m_max_damage[Damage::ICE]);
		setSpinnerValue("ItemTab/Weapon/IceMultSpinner",dmg.m_multiplier[Damage::ICE]);
		
		setSpinnerValue("ItemTab/Weapon/AirMinSpinner",dmg.m_min_damage[Damage::AIR]);
		setSpinnerValue("ItemTab/Weapon/AirMaxSpinner",dmg.m_max_damage[Damage::AIR]);
		setSpinnerValue("ItemTab/Weapon/AirMultSpinner",dmg.m_multiplier[Damage::AIR]);
		
		setSpinnerValue("ItemTab/Weapon/PrecisionSpinner",dmg.m_attack);
		setSpinnerValue("ItemTab/Weapon/PowerSpinner",dmg.m_power);
		setSpinnerValue("ItemTab/Weapon/CritPercentSpinner",dmg.m_crit_perc*100);
		
		setSpinnerValue("ItemTab/Weapon/RangeSpinner",m_edited_item.m_weapon_attr->m_attack_range);
		setSpinnerValue("ItemTab/Weapon/SpeedSpinner",m_edited_item.m_weapon_attr->m_dattack_speed );
		
		setCheckboxSelected("ItemTab/Weapon/TwohandedCheckbox", m_edited_item.m_weapon_attr->m_two_handed);
	}


	if (m_edited_item.m_useup_effect != 0)
	{
		CreatureDynAttrMod* dynmod = m_edited_item.m_useup_effect;
		
		setSpinnerValue("ItemTab/Consume/HealthSpinner", dynmod->m_dhealth);
		setSpinnerValue("ItemTab/Consume/BlindSpinner",dynmod->m_dstatus_mod_immune_time[Damage::BLIND] );
		setSpinnerValue("ItemTab/Consume/PoisonedSpinner",dynmod->m_dstatus_mod_immune_time[Damage::POISONED] );
		setSpinnerValue("ItemTab/Consume/BerserkSpinner",dynmod->m_dstatus_mod_immune_time[Damage::BERSERK] );
		setSpinnerValue("ItemTab/Consume/ConfusedSpinner",dynmod->m_dstatus_mod_immune_time[Damage::CONFUSED] );
		setSpinnerValue("ItemTab/Consume/MuteSpinner",dynmod->m_dstatus_mod_immune_time[Damage::MUTE] );
		setSpinnerValue("ItemTab/Consume/ParalyzedSpinner",dynmod->m_dstatus_mod_immune_time[Damage::PARALYZED] );
		setSpinnerValue("ItemTab/Consume/FrozenSpinner",dynmod->m_dstatus_mod_immune_time[Damage::FROZEN] );
		setSpinnerValue("ItemTab/Consume/BurningSpinner",dynmod->m_dstatus_mod_immune_time[Damage::BURNING] );
	}

	if (m_edited_item.m_equip_effect != 0)
	{
		CreatureBaseAttrMod* basemod = m_edited_item.m_equip_effect;
		
		setSpinnerValue("ItemTab/Equip/HealthSpinner",basemod->m_dmax_health );
	
		setSpinnerValue("ItemTab/Equip/StrengthSpinner",basemod->m_dstrength );
		setSpinnerValue("ItemTab/Equip/DexteritySpinner",basemod->m_ddexterity );
		setSpinnerValue("ItemTab/Equip/MagicPowerSpinner",basemod->m_dmagic_power );
		setSpinnerValue("ItemTab/Equip/WillpowerSpinner",basemod->m_dwillpower );
		
		setSpinnerValue("ItemTab/Equip/PhysResSpinner",basemod->m_dresistances[Damage::PHYSICAL] );
		setSpinnerValue("ItemTab/Equip/FireResSpinner",basemod->m_dresistances[Damage::FIRE] );
		setSpinnerValue("ItemTab/Equip/IceResSpinner",basemod->m_dresistances[Damage::ICE] );
		setSpinnerValue("ItemTab/Equip/AirResSpinner",basemod->m_dresistances[Damage::AIR] );
		
		setSpinnerValue("ItemTab/Equip/ArmorSpinner",basemod->m_darmor );
		setSpinnerValue("ItemTab/Equip/BlockSpinner",basemod->m_dblock );
		setSpinnerValue("ItemTab/Equip/AttackSpinner",basemod->m_dattack );
		setSpinnerValue("ItemTab/Equip/PowerSpinner",basemod->m_dpower );
		
	}

	m_no_cegui_events = false;
}



