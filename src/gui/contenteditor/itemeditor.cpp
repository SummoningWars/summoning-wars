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

//
// Includes
//

//
// Own header
//
#include "itemeditor.h"

//
// C++ system headers.
//

#include <map>
#include <string>

// Utility for CEGUI cross-version compatibility
#include "ceguiutility.h"

#ifdef CEGUI_07
#include "CEGUI/RendererModules/Ogre/CEGUIOgreRenderer.h"
#else
#include "CEGUI/RendererModules/Ogre/Renderer.h"
#endif

#include <OgreHardwarePixelBuffer.h>
#include <OgreMeshManager.h>

#include "graphicmanager.h"
#include "renderinfoeditor.h"
#include "contenteditor.h"
#include "player.h"

void ItemEditor::init(CEGUI::Window* parent)
{
	ContentEditorTab::init(parent);
  m_rootWindow = parent;
	
  CEGUI::PushButton* xmlsubmitButton = static_cast<CEGUI::PushButton*>(CEGUIUtility::getWindowForLoadedLayout(
      m_rootWindow,
      "Root/ObjectInfoTabControl/__auto_TabPane__/ItemTab/ItemTabControl/__auto_TabPane__/XML/SubmitButton"));
	xmlsubmitButton->subscribeEvent(CEGUI::PushButton::EventClicked, CEGUI::Event::Subscriber(&ItemEditor::onItemXMLModified, this));
	
	CEGUI::PushButton* createDropButton = static_cast<CEGUI::PushButton*>(CEGUIUtility::getWindowForLoadedLayout(
      m_rootWindow,
      "Root/ObjectInfoTabControl/__auto_TabPane__/ItemTab/ItemTabControl/Create/CreateDropButton"));
	createDropButton->subscribeEvent(CEGUI::PushButton::EventClicked, CEGUI::Event::Subscriber(&ItemEditor::onItemCreateDrop, this));
	
	CEGUI::PushButton* createInvButton = static_cast<CEGUI::PushButton*>(CEGUIUtility::getWindowForLoadedLayout(
      m_rootWindow,
      "Root/ObjectInfoTabControl/__auto_TabPane__/ItemTab/ItemTabControl/Create/CreateInventoryButton"));
	createInvButton->subscribeEvent(CEGUI::PushButton::EventClicked, CEGUI::Event::Subscriber(&ItemEditor::onItemCreateInInventory, this));
	
	CEGUI::Spinner* enchantMin =  static_cast<CEGUI::Spinner*>(CEGUIUtility::getWindowForLoadedLayout(
      m_rootWindow,
      "Root/ObjectInfoTabControl/__auto_TabPane__/ItemTab/ItemTabControl/General/EnchantMinSpinner"));
	enchantMin->subscribeEvent(CEGUI::Spinner::EventValueChanged, CEGUI::Event::Subscriber(&ItemEditor::onItemModified, this));
	
	CEGUI::Spinner* enchantMax =  static_cast<CEGUI::Spinner*>(CEGUIUtility::getWindowForLoadedLayout(
      m_rootWindow,
      "Root/ObjectInfoTabControl/__auto_TabPane__/ItemTab/ItemTabControl/General/EnchantMaxSpinner"));
	enchantMax->subscribeEvent(CEGUI::Spinner::EventValueChanged, CEGUI::Event::Subscriber(&ItemEditor::onItemModified, this));
	
	CEGUI::Spinner* price =  static_cast<CEGUI::Spinner*>(CEGUIUtility::getWindowForLoadedLayout(
      m_rootWindow,
      "Root/ObjectInfoTabControl/__auto_TabPane__/ItemTab/ItemTabControl/General/PriceSpinner"));
	price->subscribeEvent(CEGUI::Spinner::EventValueChanged, CEGUI::Event::Subscriber(&ItemEditor::onItemModified, this));
	
	CEGUI::Editbox* nameBox = static_cast<CEGUI::Editbox*>(CEGUIUtility::getWindowForLoadedLayout(
      m_rootWindow,
      "Root/ObjectInfoTabControl/__auto_TabPane__/ItemTab/ItemTabControl/General/NameBox"));
	nameBox->subscribeEvent(CEGUI::Editbox::EventTextChanged, CEGUI::Event::Subscriber(&ItemEditor::onItemModified, this));

	CEGUI::Combobox* typeSelector = static_cast<CEGUI::Combobox*>(CEGUIUtility::getWindowForLoadedLayout(
      m_rootWindow,
      "Root/ObjectInfoTabControl/__auto_TabPane__/ItemTab/ItemTabControl/__auto_TabPane__/General/TypeBox"));

	typeSelector->subscribeEvent(
      CEGUI::Combobox::EventListSelectionAccepted, 
      CEGUI::Event::Subscriber(&ItemEditor::onItemModified, this));

	typeSelector->addItem(new CEGUI::ListboxTextItem("Armor"));
	typeSelector->addItem(new CEGUI::ListboxTextItem("Weapon"));
	typeSelector->addItem(new CEGUI::ListboxTextItem("Helmet"));
	typeSelector->addItem(new CEGUI::ListboxTextItem("Gloves"));
	typeSelector->addItem(new CEGUI::ListboxTextItem("Ring"));
	typeSelector->addItem(new CEGUI::ListboxTextItem("Amulet"));
	typeSelector->addItem(new CEGUI::ListboxTextItem("Shield"));
	typeSelector->addItem(new CEGUI::ListboxTextItem("Potion"));
	typeSelector->setText("Armor");
	typeSelector->setSelection(0, 0);
	
	
	CEGUI::Combobox* sizeSelector = static_cast<CEGUI::Combobox*>(CEGUIUtility::getWindowForLoadedLayout(m_rootWindow,
    "Root/ObjectInfoTabControl/__auto_TabPane__/ItemTab/ItemTabControl/__auto_TabPane__/General/SizeBox"));
	sizeSelector->subscribeEvent(CEGUI::Combobox::EventListSelectionAccepted, CEGUI::Event::Subscriber(&ItemEditor::onItemModified, this));
	sizeSelector->addItem(new CEGUI::ListboxTextItem("Big"));
	sizeSelector->addItem(new CEGUI::ListboxTextItem("Medium"));
	sizeSelector->addItem(new CEGUI::ListboxTextItem("Small"));
	sizeSelector->setText("Big");
	sizeSelector->setSelection(0, 0);
	
	CEGUI::PushButton* copyfoButton = static_cast<CEGUI::PushButton*>(CEGUIUtility::getWindowForLoadedLayout(m_rootWindow,
    "Root/ObjectInfoTabControl/__auto_TabPane__/ItemTab/ItemTabControl/__auto_TabPane__/General/CopyDataButton"));
	copyfoButton->subscribeEvent(CEGUI::PushButton::EventClicked, CEGUI::Event::Subscriber(&ItemEditor::onCopyData, this));
	
	// wire weaponattr tab
	CEGUI::Editbox* wtype = static_cast<CEGUI::Editbox*>(CEGUIUtility::getWindowForLoadedLayout(m_rootWindow,
    "Root/ObjectInfoTabControl/__auto_TabPane__/ItemTab/ItemTabControl/__auto_TabPane__/General/TypeBox"));
	wtype->subscribeEvent(CEGUI::Editbox::EventTextChanged, CEGUI::Event::Subscriber(&ItemEditor::onWeaponModified, this));
	
	CEGUI::Spinner* physMin =  static_cast<CEGUI::Spinner*>(CEGUIUtility::getWindowForLoadedLayout(m_rootWindow,
    "Root/ObjectInfoTabControl/__auto_TabPane__/ItemTab/ItemTabControl/__auto_TabPane__/Weapon/PhysMinSpinner"));
	physMin->subscribeEvent(CEGUI::Spinner::EventValueChanged, CEGUI::Event::Subscriber(&ItemEditor::onWeaponModified, this));

	CEGUI::Spinner* physMax =  static_cast<CEGUI::Spinner*>(CEGUIUtility::getWindowForLoadedLayout(m_rootWindow,
    "Root/ObjectInfoTabControl/__auto_TabPane__/ItemTab/ItemTabControl/__auto_TabPane__/Weapon/PhysMaxSpinner"));
	physMax->subscribeEvent(CEGUI::Spinner::EventValueChanged, CEGUI::Event::Subscriber(&ItemEditor::onWeaponModified, this));
	
	CEGUI::Spinner* physMult =  static_cast<CEGUI::Spinner*>(CEGUIUtility::getWindowForLoadedLayout(m_rootWindow,
    "Root/ObjectInfoTabControl/__auto_TabPane__/ItemTab/ItemTabControl/__auto_TabPane__/Weapon/PhysMultSpinner"));
	physMult->subscribeEvent(CEGUI::Spinner::EventValueChanged, CEGUI::Event::Subscriber(&ItemEditor::onWeaponModified, this));

	CEGUI::Spinner* fireMin =  static_cast<CEGUI::Spinner*>(CEGUIUtility::getWindowForLoadedLayout(m_rootWindow,
    "Root/ObjectInfoTabControl/__auto_TabPane__/ItemTab/ItemTabControl/__auto_TabPane__/Weapon/FireMinSpinner"));
	fireMin->subscribeEvent(CEGUI::Spinner::EventValueChanged, CEGUI::Event::Subscriber(&ItemEditor::onWeaponModified, this));

	CEGUI::Spinner* fireMax =  static_cast<CEGUI::Spinner*>(CEGUIUtility::getWindowForLoadedLayout(m_rootWindow,
    "Root/ObjectInfoTabControl/__auto_TabPane__/ItemTab/ItemTabControl/__auto_TabPane__/Weapon/FireMaxSpinner"));
	fireMax->subscribeEvent(CEGUI::Spinner::EventValueChanged, CEGUI::Event::Subscriber(&ItemEditor::onItemModified, this));
	
	CEGUI::Spinner* fireMult =  static_cast<CEGUI::Spinner*>(CEGUIUtility::getWindowForLoadedLayout(m_rootWindow,
    "Root/ObjectInfoTabControl/__auto_TabPane__/ItemTab/ItemTabControl/__auto_TabPane__/Weapon/FireMultSpinner"));
	fireMult->subscribeEvent(CEGUI::Spinner::EventValueChanged, CEGUI::Event::Subscriber(&ItemEditor::onWeaponModified, this));

	CEGUI::Spinner* iceMin =  static_cast<CEGUI::Spinner*>(CEGUIUtility::getWindowForLoadedLayout(m_rootWindow,
    "Root/ObjectInfoTabControl/__auto_TabPane__/ItemTab/ItemTabControl/__auto_TabPane__/Weapon/IceMinSpinner"));
	iceMin->subscribeEvent(CEGUI::Spinner::EventValueChanged, CEGUI::Event::Subscriber(&ItemEditor::onWeaponModified, this));

	CEGUI::Spinner* iceMax =  static_cast<CEGUI::Spinner*>(CEGUIUtility::getWindowForLoadedLayout(m_rootWindow,
    "Root/ObjectInfoTabControl/__auto_TabPane__/ItemTab/ItemTabControl/__auto_TabPane__/Weapon/IceMaxSpinner"));
	iceMax->subscribeEvent(CEGUI::Spinner::EventValueChanged, CEGUI::Event::Subscriber(&ItemEditor::onWeaponModified, this));
	
	CEGUI::Spinner* iceMult =  static_cast<CEGUI::Spinner*>(CEGUIUtility::getWindowForLoadedLayout(m_rootWindow,
    "Root/ObjectInfoTabControl/__auto_TabPane__/ItemTab/ItemTabControl/__auto_TabPane__/Weapon/IceMultSpinner"));
	iceMult->subscribeEvent(CEGUI::Spinner::EventValueChanged, CEGUI::Event::Subscriber(&ItemEditor::onWeaponModified, this));

	CEGUI::Spinner* airMin =  static_cast<CEGUI::Spinner*>(CEGUIUtility::getWindowForLoadedLayout(m_rootWindow,
    "Root/ObjectInfoTabControl/__auto_TabPane__/ItemTab/ItemTabControl/__auto_TabPane__/Weapon/AirMinSpinner"));
	airMin->subscribeEvent(CEGUI::Spinner::EventValueChanged, CEGUI::Event::Subscriber(&ItemEditor::onWeaponModified, this));

	CEGUI::Spinner* airMax =  static_cast<CEGUI::Spinner*>(CEGUIUtility::getWindowForLoadedLayout(m_rootWindow,
    "Root/ObjectInfoTabControl/__auto_TabPane__/ItemTab/ItemTabControl/__auto_TabPane__/Weapon/AirMaxSpinner"));
	airMax->subscribeEvent(CEGUI::Spinner::EventValueChanged, CEGUI::Event::Subscriber(&ItemEditor::onWeaponModified, this));
	
	CEGUI::Spinner* airMult =  static_cast<CEGUI::Spinner*>(CEGUIUtility::getWindowForLoadedLayout(m_rootWindow,
    "Root/ObjectInfoTabControl/__auto_TabPane__/ItemTab/ItemTabControl/__auto_TabPane__/Weapon/AirMultSpinner"));
	airMult->subscribeEvent(CEGUI::Spinner::EventValueChanged, CEGUI::Event::Subscriber(&ItemEditor::onWeaponModified, this));
	
	CEGUI::Spinner* precision =  static_cast<CEGUI::Spinner*>(CEGUIUtility::getWindowForLoadedLayout(m_rootWindow,
    "Root/ObjectInfoTabControl/__auto_TabPane__/ItemTab/ItemTabControl/__auto_TabPane__/Weapon/PrecisionSpinner"));
	precision->subscribeEvent(CEGUI::Spinner::EventValueChanged, CEGUI::Event::Subscriber(&ItemEditor::onWeaponModified, this));
	
	CEGUI::Spinner* power =  static_cast<CEGUI::Spinner*>(CEGUIUtility::getWindowForLoadedLayout(m_rootWindow,
    "Root/ObjectInfoTabControl/__auto_TabPane__/ItemTab/ItemTabControl/__auto_TabPane__/Weapon/PowerSpinner"));
	power->subscribeEvent(CEGUI::Spinner::EventValueChanged, CEGUI::Event::Subscriber(&ItemEditor::onWeaponModified, this));
	
	CEGUI::Spinner* range =  static_cast<CEGUI::Spinner*>(CEGUIUtility::getWindowForLoadedLayout(m_rootWindow,
    "Root/ObjectInfoTabControl/__auto_TabPane__/ItemTab/ItemTabControl/__auto_TabPane__/Weapon/RangeSpinner"));
	range->subscribeEvent(CEGUI::Spinner::EventValueChanged, CEGUI::Event::Subscriber(&ItemEditor::onWeaponModified, this));
	
	CEGUI::Spinner* speed =  static_cast<CEGUI::Spinner*>(CEGUIUtility::getWindowForLoadedLayout(m_rootWindow,
    "Root/ObjectInfoTabControl/__auto_TabPane__/ItemTab/ItemTabControl/__auto_TabPane__/Weapon/SpeedSpinner"));
	speed->subscribeEvent(CEGUI::Spinner::EventValueChanged, CEGUI::Event::Subscriber(&ItemEditor::onWeaponModified, this));
	
	CEGUI::Spinner* critperc =  static_cast<CEGUI::Spinner*>(CEGUIUtility::getWindowForLoadedLayout(m_rootWindow,
    "Root/ObjectInfoTabControl/__auto_TabPane__/ItemTab/ItemTabControl/__auto_TabPane__/Weapon/CritPercentSpinner"));
	critperc->subscribeEvent(CEGUI::Spinner::EventValueChanged, CEGUI::Event::Subscriber(&ItemEditor::onWeaponModified, this));
	
  CEGUIUtility::ToggleButton* twohanded = static_cast<CEGUIUtility::ToggleButton*>(CEGUIUtility::getWindowForLoadedLayout(m_rootWindow,
    "Root/ObjectInfoTabControl/__auto_TabPane__/ItemTab/ItemTabControl/__auto_TabPane__/Weapon/TwohandedCheckbox"));
	twohanded->subscribeEvent(CEGUIUtility::EventToggleButtonStateChanged(), CEGUI::Event::Subscriber(&ItemEditor::onWeaponModified, this));
	
	
	// Wire the consume tab
	static_cast<CEGUI::Spinner*>(CEGUIUtility::getWindowForLoadedLayout(
      m_rootWindow,
      "Root/ObjectInfoTabControl/__auto_TabPane__/ItemTab/ItemTabControl/__auto_TabPane__/Consume/HealthSpinner"))->subscribeEvent(
          CEGUI::Spinner::EventValueChanged, CEGUI::Event::Subscriber(&ItemEditor::onConsumeEffectsModified, this));
	static_cast<CEGUI::Spinner*>(CEGUIUtility::getWindowForLoadedLayout(
      m_rootWindow,
      "Root/ObjectInfoTabControl/__auto_TabPane__/ItemTab/ItemTabControl/__auto_TabPane__/Consume/BlindSpinner"))->subscribeEvent(
          CEGUI::Spinner::EventValueChanged, CEGUI::Event::Subscriber(&ItemEditor::onConsumeEffectsModified, this));

  static_cast<CEGUI::Spinner*>(CEGUIUtility::getWindowForLoadedLayout(
      m_rootWindow,
      "Root/ObjectInfoTabControl/__auto_TabPane__/ItemTab/ItemTabControl/__auto_TabPane__/Consume/PoisonedSpinner"))->subscribeEvent(
          CEGUI::Spinner::EventValueChanged, CEGUI::Event::Subscriber(&ItemEditor::onConsumeEffectsModified, this));

	static_cast<CEGUI::Spinner*>(CEGUIUtility::getWindowForLoadedLayout(m_rootWindow,
      "Root/ObjectInfoTabControl/__auto_TabPane__/ItemTab/ItemTabControl/__auto_TabPane__/Consume/BerserkSpinner"))->subscribeEvent(
          CEGUI::Spinner::EventValueChanged, CEGUI::Event::Subscriber(&ItemEditor::onConsumeEffectsModified, this));

	static_cast<CEGUI::Spinner*>(CEGUIUtility::getWindowForLoadedLayout(
      m_rootWindow,
      "Root/ObjectInfoTabControl/__auto_TabPane__/ItemTab/ItemTabControl/__auto_TabPane__/Consume/ConfusedSpinner"))->subscribeEvent(
      CEGUI::Spinner::EventValueChanged, CEGUI::Event::Subscriber(&ItemEditor::onConsumeEffectsModified, this));

	static_cast<CEGUI::Spinner*>(CEGUIUtility::getWindowForLoadedLayout(
      m_rootWindow,
      "Root/ObjectInfoTabControl/__auto_TabPane__/ItemTab/ItemTabControl/__auto_TabPane__/Consume/MuteSpinner"))->subscribeEvent(
          CEGUI::Spinner::EventValueChanged, CEGUI::Event::Subscriber(&ItemEditor::onConsumeEffectsModified, this));

	static_cast<CEGUI::Spinner*>(CEGUIUtility::getWindowForLoadedLayout(
      m_rootWindow,
      "Root/ObjectInfoTabControl/__auto_TabPane__/ItemTab/ItemTabControl/__auto_TabPane__/Consume/ParalyzedSpinner"))->subscribeEvent(
          CEGUI::Spinner::EventValueChanged, CEGUI::Event::Subscriber(&ItemEditor::onConsumeEffectsModified, this));

	static_cast<CEGUI::Spinner*>(CEGUIUtility::getWindowForLoadedLayout(
      m_rootWindow,
      "Root/ObjectInfoTabControl/__auto_TabPane__/ItemTab/ItemTabControl/__auto_TabPane__/Consume/FrozenSpinner"))->subscribeEvent(
          CEGUI::Spinner::EventValueChanged, CEGUI::Event::Subscriber(&ItemEditor::onConsumeEffectsModified, this));

	static_cast<CEGUI::Spinner*>(CEGUIUtility::getWindowForLoadedLayout(
      m_rootWindow,
      "Root/ObjectInfoTabControl/__auto_TabPane__/ItemTab/ItemTabControl/__auto_TabPane__/Consume/BurningSpinner"))->subscribeEvent(
          CEGUI::Spinner::EventValueChanged, CEGUI::Event::Subscriber(&ItemEditor::onConsumeEffectsModified, this));
	
	// wire the equip tab
	static_cast<CEGUI::Spinner*>(CEGUIUtility::getWindowForLoadedLayout(
      m_rootWindow,
      "Root/ObjectInfoTabControl/__auto_TabPane__/ItemTab/ItemTabControl/__auto_TabPane__/Equip/HealthSpinner"))->subscribeEvent(
          CEGUI::Spinner::EventValueChanged, CEGUI::Event::Subscriber(&ItemEditor::onEquipEffectsModified, this));

  static_cast<CEGUI::Spinner*>(CEGUIUtility::getWindowForLoadedLayout(
      m_rootWindow,
      "Root/ObjectInfoTabControl/__auto_TabPane__/ItemTab/ItemTabControl/__auto_TabPane__/Equip/StrengthSpinner"))->subscribeEvent(
          CEGUI::Spinner::EventValueChanged, CEGUI::Event::Subscriber(&ItemEditor::onEquipEffectsModified, this));
	
  static_cast<CEGUI::Spinner*>(CEGUIUtility::getWindowForLoadedLayout(
      m_rootWindow,
      "Root/ObjectInfoTabControl/__auto_TabPane__/ItemTab/ItemTabControl/__auto_TabPane__/Equip/DexteritySpinner"))->subscribeEvent(
          CEGUI::Spinner::EventValueChanged, CEGUI::Event::Subscriber(&ItemEditor::onEquipEffectsModified, this));
	
  static_cast<CEGUI::Spinner*>(CEGUIUtility::getWindowForLoadedLayout(
      m_rootWindow,
      "Root/ObjectInfoTabControl/__auto_TabPane__/ItemTab/ItemTabControl/__auto_TabPane__/Equip/MagicPowerSpinner"))->subscribeEvent(
          CEGUI::Spinner::EventValueChanged, CEGUI::Event::Subscriber(&ItemEditor::onEquipEffectsModified, this));
	
  static_cast<CEGUI::Spinner*>(CEGUIUtility::getWindowForLoadedLayout(
      m_rootWindow,
      "Root/ObjectInfoTabControl/__auto_TabPane__/ItemTab/ItemTabControl/__auto_TabPane__/Equip/WillpowerSpinner"))->subscribeEvent(
          CEGUI::Spinner::EventValueChanged, CEGUI::Event::Subscriber(&ItemEditor::onEquipEffectsModified, this));
	
  static_cast<CEGUI::Spinner*>(CEGUIUtility::getWindowForLoadedLayout(
      m_rootWindow,
      "Root/ObjectInfoTabControl/__auto_TabPane__/ItemTab/ItemTabControl/__auto_TabPane__/Equip/PhysResSpinner"))->subscribeEvent(
          CEGUI::Spinner::EventValueChanged, CEGUI::Event::Subscriber(&ItemEditor::onEquipEffectsModified, this));
	
  static_cast<CEGUI::Spinner*>(CEGUIUtility::getWindowForLoadedLayout(
      m_rootWindow,
      "Root/ObjectInfoTabControl/__auto_TabPane__/ItemTab/ItemTabControl/__auto_TabPane__/Equip/FireResSpinner"))->subscribeEvent(
      CEGUI::Spinner::EventValueChanged, CEGUI::Event::Subscriber(&ItemEditor::onEquipEffectsModified, this));
	
  static_cast<CEGUI::Spinner*>(CEGUIUtility::getWindowForLoadedLayout(
      m_rootWindow,
      "Root/ObjectInfoTabControl/__auto_TabPane__/ItemTab/ItemTabControl/__auto_TabPane__/Equip/IceResSpinner"))->subscribeEvent(
      CEGUI::Spinner::EventValueChanged, CEGUI::Event::Subscriber(&ItemEditor::onEquipEffectsModified, this));
	
  static_cast<CEGUI::Spinner*>(CEGUIUtility::getWindowForLoadedLayout(
      m_rootWindow,
      "Root/ObjectInfoTabControl/__auto_TabPane__/ItemTab/ItemTabControl/__auto_TabPane__/Equip/AirResSpinner"))->subscribeEvent(
      CEGUI::Spinner::EventValueChanged, CEGUI::Event::Subscriber(&ItemEditor::onEquipEffectsModified, this));
	
  static_cast<CEGUI::Spinner*>(CEGUIUtility::getWindowForLoadedLayout(
      m_rootWindow,
      "Root/ObjectInfoTabControl/__auto_TabPane__/ItemTab/ItemTabControl/__auto_TabPane__/Equip/ArmorSpinner"))->subscribeEvent(
      CEGUI::Spinner::EventValueChanged, CEGUI::Event::Subscriber(&ItemEditor::onEquipEffectsModified, this));
	
  static_cast<CEGUI::Spinner*>(CEGUIUtility::getWindowForLoadedLayout(
      m_rootWindow,
      "Root/ObjectInfoTabControl/__auto_TabPane__/ItemTab/ItemTabControl/__auto_TabPane__/Equip/BlockSpinner"))->subscribeEvent(
      CEGUI::Spinner::EventValueChanged, CEGUI::Event::Subscriber(&ItemEditor::onEquipEffectsModified, this));
	
  static_cast<CEGUI::Spinner*>(CEGUIUtility::getWindowForLoadedLayout(
      m_rootWindow,
      "Root/ObjectInfoTabControl/__auto_TabPane__/ItemTab/ItemTabControl/__auto_TabPane__/Equip/AttackSpinner"))->subscribeEvent(
      CEGUI::Spinner::EventValueChanged, CEGUI::Event::Subscriber(&ItemEditor::onEquipEffectsModified, this));
	
  static_cast<CEGUI::Spinner*>(CEGUIUtility::getWindowForLoadedLayout(
      m_rootWindow,
      "Root/ObjectInfoTabControl/__auto_TabPane__/ItemTab/ItemTabControl/__auto_TabPane__/Equip/PowerSpinner"))->subscribeEvent(
      CEGUI::Spinner::EventValueChanged, CEGUI::Event::Subscriber(&ItemEditor::onEquipEffectsModified, this));
	
	
	// init the internal data
  TiXmlNode* linkedNode = NULL;
	TiXmlElement * item_root = new TiXmlElement("Item");  
	linkedNode = m_item_xml.LinkEndChild(item_root);  
  if (linkedNode != NULL)
  {
    // Only use item_root if it was not deleted.
	  item_root->SetAttribute("subtype", "EditorItem");
	  TiXmlElement * item_image = new TiXmlElement("Image");  
	  item_image->SetAttribute("image", "noMedia.png"); // previously: "set:noMedia.png image:full_image"
	  item_root->LinkEndChild(item_image);
	
	  TiXmlElement * item_ri = new TiXmlElement("RenderInfo");  
	  item_ri->SetAttribute("name", "EditorRenderInfo");
	  item_root->LinkEndChild(item_ri);
  }
	
	// init the data
	m_edited_item.m_subtype = "EditorItem";
	
	m_update_base_content = true;
	m_modified_item = true;
	
	// create camera for item photos
	Ogre::SceneManager* editor_scene_mng = Ogre::Root::getSingleton().getSceneManager("EditorSceneManager");
	Ogre::Camera* item_camera = editor_scene_mng->createCamera("item_camera");
	item_camera->setNearClipDistance(0.1);
	item_camera->setAspectRatio(1.0);
	item_camera->setPosition(Ogre::Vector3(0, 1, 0));
	item_camera->lookAt(Ogre::Vector3(0, 0, 0));
	
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
	CEGUI::Combobox* copyItemSelector = static_cast<CEGUI::Combobox*>(
      CEGUIUtility::getWindowForLoadedLayout(
          m_rootWindow,
          "Root/ObjectInfoTabControl/__auto_TabPane__/ItemTab/ItemTabControl/__auto_TabPane__/General/CopyDataBox"));
	
	// Fill list of all FixedObjects
	const std::map<Item::Subtype, ItemBasicData*>& all_items = ItemFactory::getAllItemData();
	std::map<Item::Subtype, ItemBasicData*>::const_iterator it;
	for (it = all_items.begin(); it != all_items.end(); ++it)
	{
		copyItemSelector->addItem(new CEGUI::ListboxTextItem(it->first.c_str()));
	}
}

bool ItemEditor::onItemModified(const CEGUI::EventArgs& evt)
{
	if (m_no_cegui_events)
  {
		return true;
  }

	m_no_cegui_events = true;
	
	m_edited_item.m_price = (int) getSpinnerValue(
      "Root/ObjectInfoTabControl/__auto_TabPane__/ItemTab/ItemTabControl/__auto_TabPane__/General/PriceSpinner", 
      1);

	m_edited_item.m_min_enchant = getSpinnerValue(
      "Root/ObjectInfoTabControl/__auto_TabPane__/ItemTab/ItemTabControl/__auto_TabPane__/General/EnchantMinSpinner",
      1);

	m_edited_item.m_max_enchant = getSpinnerValue(
      "Root/ObjectInfoTabControl/__auto_TabPane__/ItemTab/ItemTabControl/__auto_TabPane__/General/EnchantMaxSpinner",
      1);

	if (m_edited_item.m_max_enchant < m_edited_item.m_min_enchant)
	{
		m_edited_item.m_max_enchant = m_edited_item.m_min_enchant;
		setSpinnerValue(
        "Root/ObjectInfoTabControl/__auto_TabPane__/ItemTab/ItemTabControl/__auto_TabPane__/General/EnchantMaxSpinner", 
        m_edited_item.m_max_enchant);
	}
	m_edited_item.m_name = getWindowText("Root/ObjectInfoTabControl/__auto_TabPane__/ItemTab/ItemTabControl/__auto_TabPane__/General/NameBox", "EditorItem");
	
	std::string type = getWindowText("Root/ObjectInfoTabControl/__auto_TabPane__/ItemTab/ItemTabControl/__auto_TabPane__/General/TypeBox", "Weapon");
	m_edited_item.m_type = Item::WEAPON;
	if (type == "Armor") 
  {
    m_edited_item.m_type = Item::ARMOR;
  }
	else if (type == "Weapon")
  {
    m_edited_item.m_type = Item::WEAPON;
  }
	else if (type == "Helmet")
  {
    m_edited_item.m_type = Item::HELMET;
  }
	else if (type == "Gloves")
  {
    m_edited_item.m_type = Item::GLOVES;
  }
	else if (type == "Ring")
  {
    m_edited_item.m_type = Item::RING;
  }
	else if (type == "Amulet")
  {
    m_edited_item.m_type = Item::AMULET;
  }
	else if (type == "Shield")
  {
    m_edited_item.m_type = Item::SHIELD;
  }
	else if (type == "Potion")
  {
    m_edited_item.m_type = Item::POTION;
  }

	std::string size = getWindowText(
      "Root/ObjectInfoTabControl/__auto_TabPane__/ItemTab/ItemTabControl/__auto_TabPane__/Properties/SizeBox", "Big");

	m_edited_item.m_size = Item::BIG;
	if (size == "Big")
  {
    m_edited_item.m_size = Item::BIG;
  }
	else if (size == "Medium")
  {
    m_edited_item.m_size = Item::MEDIUM;
  }
	else if (size == "Small")
  {
    m_edited_item.m_size = Item::SMALL;
  }

	m_no_cegui_events = false;
	m_modified_item = true;
	
	return true;
}

bool ItemEditor::onWeaponModified(const CEGUI::EventArgs& evt)
{
	if (m_no_cegui_events)
  {
		return true;
  }

	m_no_cegui_events = true;
	
	if (m_edited_item.m_weapon_attr == 0)
	{
		m_edited_item.m_weapon_attr = new WeaponAttr;
	}
	
	m_edited_item.m_weapon_attr->m_weapon_type = getWindowText("Root/ObjectInfoTabControl/__auto_TabPane__/ItemTab/ItemTabControl/__auto_TabPane__/Weapon/TypeLabel", "sword");
	
	Damage& dmg = m_edited_item.m_weapon_attr->m_damage;
	
	dmg.m_min_damage[Damage::PHYSICAL] = getSpinnerValue("Root/ObjectInfoTabControl/__auto_TabPane__/ItemTab/ItemTabControl/__auto_TabPane__/Weapon/PhysMinSpinner", 1);
	dmg.m_max_damage[Damage::PHYSICAL] = getSpinnerValue("Root/ObjectInfoTabControl/__auto_TabPane__/ItemTab/ItemTabControl/__auto_TabPane__/Weapon/PhysMaxSpinner", 1);
	if (dmg.m_max_damage[Damage::PHYSICAL] < dmg.m_min_damage[Damage::PHYSICAL])
	{
		dmg.m_max_damage[Damage::PHYSICAL] =  dmg.m_min_damage[Damage::PHYSICAL];
		setSpinnerValue("Root/ObjectInfoTabControl/__auto_TabPane__/ItemTab/ItemTabControl/__auto_TabPane__/Weapon/PhysMaxSpinner", dmg.m_max_damage[Damage::PHYSICAL]);
	}
	dmg.m_multiplier[Damage::PHYSICAL] = getSpinnerValue("Root/ObjectInfoTabControl/__auto_TabPane__/ItemTab/ItemTabControl/__auto_TabPane__/Weapon/PhysMultSpinner", 1);
	
	dmg.m_min_damage[Damage::FIRE] = getSpinnerValue("Root/ObjectInfoTabControl/__auto_TabPane__/ItemTab/ItemTabControl/__auto_TabPane__/Weapon/FireMinSpinner", 1);
	dmg.m_max_damage[Damage::FIRE] = getSpinnerValue("Root/ObjectInfoTabControl/__auto_TabPane__/ItemTab/ItemTabControl/__auto_TabPane__/Weapon/FireMaxSpinner", 1);
	if (dmg.m_max_damage[Damage::FIRE] < dmg.m_min_damage[Damage::FIRE])
	{
		dmg.m_max_damage[Damage::FIRE] =  dmg.m_min_damage[Damage::FIRE];
		setSpinnerValue("Root/ObjectInfoTabControl/__auto_TabPane__/ItemTab/ItemTabControl/__auto_TabPane__/Weapon/FireMaxSpinner", dmg.m_max_damage[Damage::FIRE]);
	}
	dmg.m_multiplier[Damage::FIRE] = getSpinnerValue("Root/ObjectInfoTabControl/__auto_TabPane__/ItemTab/ItemTabControl/__auto_TabPane__/Weapon/FireMultSpinner", 1);
	
	dmg.m_min_damage[Damage::ICE] = getSpinnerValue("Root/ObjectInfoTabControl/__auto_TabPane__/ItemTab/ItemTabControl/__auto_TabPane__/Weapon/IceMinSpinner", 1);
	dmg.m_max_damage[Damage::ICE] = getSpinnerValue("Root/ObjectInfoTabControl/__auto_TabPane__/ItemTab/ItemTabControl/__auto_TabPane__/Weapon/IceMaxSpinner", 1);
	if (dmg.m_max_damage[Damage::ICE] < dmg.m_min_damage[Damage::ICE])
	{
		dmg.m_max_damage[Damage::ICE] =  dmg.m_min_damage[Damage::ICE];
		setSpinnerValue("Root/ObjectInfoTabControl/__auto_TabPane__/ItemTab/ItemTabControl/__auto_TabPane__/Weapon/IceMaxSpinner", dmg.m_max_damage[Damage::ICE]);
	}
	dmg.m_multiplier[Damage::ICE] = getSpinnerValue("Root/ObjectInfoTabControl/__auto_TabPane__/ItemTab/ItemTabControl/__auto_TabPane__/Weapon/IceMultSpinner", 1);
	
	dmg.m_min_damage[Damage::AIR] = getSpinnerValue("Root/ObjectInfoTabControl/__auto_TabPane__/ItemTab/ItemTabControl/__auto_TabPane__/Weapon/AirMinSpinner", 1);
	dmg.m_max_damage[Damage::AIR] = getSpinnerValue("Root/ObjectInfoTabControl/__auto_TabPane__/ItemTab/ItemTabControl/__auto_TabPane__/Weapon/AirMaxSpinner", 1);
	if (dmg.m_max_damage[Damage::AIR] < dmg.m_min_damage[Damage::AIR])
	{
		dmg.m_max_damage[Damage::AIR] =  dmg.m_min_damage[Damage::AIR];
		setSpinnerValue("Root/ObjectInfoTabControl/__auto_TabPane__/ItemTab/ItemTabControl/__auto_TabPane__/Weapon/AirMaxSpinner", dmg.m_max_damage[Damage::AIR]);
	}
	dmg.m_multiplier[Damage::AIR] = getSpinnerValue("Root/ObjectInfoTabControl/__auto_TabPane__/ItemTab/ItemTabControl/__auto_TabPane__/Weapon/AirMultSpinner", 1);
	
	dmg.m_attack = getSpinnerValue("Root/ObjectInfoTabControl/__auto_TabPane__/ItemTab/ItemTabControl/__auto_TabPane__/Weapon/PrecisionSpinner", 1);
	dmg.m_power = getSpinnerValue("Root/ObjectInfoTabControl/__auto_TabPane__/ItemTab/ItemTabControl/__auto_TabPane__/Weapon/PowerSpinner", 1);
	dmg.m_crit_perc = 0.01* getSpinnerValue("Root/ObjectInfoTabControl/__auto_TabPane__/ItemTab/ItemTabControl/__auto_TabPane__/Weapon/CritPercentSpinner", 1);
	
	m_edited_item.m_weapon_attr->m_attack_range = getSpinnerValue("Root/ObjectInfoTabControl/__auto_TabPane__/ItemTab/ItemTabControl/__auto_TabPane__/Weapon/RangeSpinner");
	m_edited_item.m_weapon_attr->m_dattack_speed = (int) getSpinnerValue("Root/ObjectInfoTabControl/__auto_TabPane__/ItemTab/ItemTabControl/__auto_TabPane__/Weapon/SpeedSpinner");
	m_edited_item.m_weapon_attr->m_two_handed = getCheckboxSelected("Root/ObjectInfoTabControl/__auto_TabPane__/ItemTab/ItemTabControl/__auto_TabPane__/Weapon/TwohandedCheckbox");
	
	m_no_cegui_events = false;
	m_modified_item = true;
	
	return true;
}

bool ItemEditor::onConsumeEffectsModified(const CEGUI::EventArgs& evt)
{
	if (m_no_cegui_events)
  {
		return true;
  }

	m_no_cegui_events = true;
	
	if (m_edited_item.m_useup_effect == 0)
	{
		m_edited_item.m_useup_effect = new CreatureDynAttrMod;
	}
	
	CreatureDynAttrMod* dynmod = m_edited_item.m_useup_effect;
	
	dynmod->m_dhealth = getSpinnerValue("Root/ObjectInfoTabControl/__auto_TabPane__/ItemTab/ItemTabControl/__auto_TabPane__/Consume/HealthSpinner");
	dynmod->m_dstatus_mod_immune_time[Damage::BLIND] = getSpinnerValue("Root/ObjectInfoTabControl/__auto_TabPane__/ItemTab/ItemTabControl/__auto_TabPane__/Consume/BlindSpinner");
	dynmod->m_dstatus_mod_immune_time[Damage::POISONED] = getSpinnerValue("Root/ObjectInfoTabControl/__auto_TabPane__/ItemTab/ItemTabControl/__auto_TabPane__/Consume/PoisonedSpinner");
	dynmod->m_dstatus_mod_immune_time[Damage::BERSERK] = getSpinnerValue("Root/ObjectInfoTabControl/__auto_TabPane__/ItemTab/ItemTabControl/__auto_TabPane__/Consume/BerserkSpinner");
	dynmod->m_dstatus_mod_immune_time[Damage::CONFUSED] = getSpinnerValue("Root/ObjectInfoTabControl/__auto_TabPane__/ItemTab/ItemTabControl/__auto_TabPane__/Consume/ConfusedSpinner");
	dynmod->m_dstatus_mod_immune_time[Damage::MUTE] = getSpinnerValue("Root/ObjectInfoTabControl/__auto_TabPane__/ItemTab/ItemTabControl/__auto_TabPane__/Consume/MuteSpinner");
	dynmod->m_dstatus_mod_immune_time[Damage::PARALYZED] = getSpinnerValue("Root/ObjectInfoTabControl/__auto_TabPane__/ItemTab/ItemTabControl/__auto_TabPane__/Consume/ParalyzedSpinner");
	dynmod->m_dstatus_mod_immune_time[Damage::FROZEN] = getSpinnerValue("Root/ObjectInfoTabControl/__auto_TabPane__/ItemTab/ItemTabControl/__auto_TabPane__/Consume/FrozenSpinner");
	dynmod->m_dstatus_mod_immune_time[Damage::BURNING] = getSpinnerValue("Root/ObjectInfoTabControl/__auto_TabPane__/ItemTab/ItemTabControl/__auto_TabPane__/Consume/BurningSpinner");
	
	m_no_cegui_events = false;
	m_modified_item = true;
	
	return true;
}

bool ItemEditor::onEquipEffectsModified(const CEGUI::EventArgs& evt)
{
	if (m_no_cegui_events)
  {
		return true;
  }

	m_no_cegui_events = true;
	
	if (m_edited_item.m_equip_effect == 0)
	{
		m_edited_item.m_equip_effect = new CreatureBaseAttrMod;
	}
	
	CreatureBaseAttrMod* basemod = m_edited_item.m_equip_effect;
	
	basemod->m_dmax_health = getSpinnerValue("Root/ObjectInfoTabControl/__auto_TabPane__/ItemTab/ItemTabControl/__auto_TabPane__/Equip/HealthSpinner");
	
	basemod->m_dstrength = static_cast<int>(
      getSpinnerValue("Root/ObjectInfoTabControl/__auto_TabPane__/ItemTab/ItemTabControl/__auto_TabPane__/Equip/StrengthSpinner"));

	basemod->m_ddexterity = static_cast<int>(
      getSpinnerValue("Root/ObjectInfoTabControl/__auto_TabPane__/ItemTab/ItemTabControl/__auto_TabPane__/Equip/DexteritySpinner"));

	basemod->m_dmagic_power = static_cast<int>(
      getSpinnerValue("Root/ObjectInfoTabControl/__auto_TabPane__/ItemTab/ItemTabControl/__auto_TabPane__/Equip/MagicPowerSpinner"));
	basemod->m_dwillpower = static_cast<int>(
      getSpinnerValue("Root/ObjectInfoTabControl/__auto_TabPane__/ItemTab/ItemTabControl/__auto_TabPane__/Equip/WillpowerSpinner"));
	
	basemod->m_dresistances[Damage::PHYSICAL] = static_cast<int>(
      getSpinnerValue("Root/ObjectInfoTabControl/__auto_TabPane__/ItemTab/ItemTabControl/__auto_TabPane__/Equip/PhysResSpinner"));

	basemod->m_dresistances[Damage::FIRE] = static_cast<int>(
      getSpinnerValue("Root/ObjectInfoTabControl/__auto_TabPane__/ItemTab/ItemTabControl/__auto_TabPane__/Equip/FireResSpinner"));
	basemod->m_dresistances[Damage::ICE] = static_cast<int>(
      getSpinnerValue("Root/ObjectInfoTabControl/__auto_TabPane__/ItemTab/ItemTabControl/__auto_TabPane__/Equip/IceResSpinner"));
	basemod->m_dresistances[Damage::AIR] = static_cast<int>(
      getSpinnerValue("Root/ObjectInfoTabControl/__auto_TabPane__/ItemTab/ItemTabControl/__auto_TabPane__/Equip/AirResSpinner"));
	
	basemod->m_darmor = static_cast<int>(
      getSpinnerValue("Root/ObjectInfoTabControl/__auto_TabPane__/ItemTab/ItemTabControl/__auto_TabPane__/Equip/ArmorSpinner"));

	basemod->m_dblock = static_cast<int>(
      getSpinnerValue("Root/ObjectInfoTabControl/__auto_TabPane__/ItemTab/ItemTabControl/__auto_TabPane__/Equip/BlockSpinner"));

	basemod->m_dattack = static_cast<int>(
      getSpinnerValue("Root/ObjectInfoTabControl/__auto_TabPane__/ItemTab/ItemTabControl/__auto_TabPane__/Equip/AttackSpinner"));

	basemod->m_dpower = static_cast<int>(
      getSpinnerValue("Root/ObjectInfoTabControl/__auto_TabPane__/ItemTab/ItemTabControl/__auto_TabPane__/Equip/PowerSpinner"));
	
	
	m_no_cegui_events = false;
	m_modified_item = true;
	
	return true;
}

bool ItemEditor::onItemXMLModified(const CEGUI::EventArgs& evt)
{
	CEGUI::MultiLineEditbox* editor = static_cast<CEGUI::MultiLineEditbox*>(CEGUIUtility::getWindowForLoadedLayout(m_rootWindow,
    "Root/ObjectInfoTabControl/__auto_TabPane__/ItemTab/ItemTabControl/__auto_TabPane__/XML/ItemXMLEditbox"));
	
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
		std::string subtype = ItemLoader::loadItem(m_item_xml.FirstChildElement(), true);
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
		
		setMultiLineEditboxCursor(
        "Root/ObjectInfoTabControl/__auto_TabPane__/ItemTab/ItemTabControl/__auto_TabPane__/XML/ItemXMLEditbox",
        err_row, err_col);
	}
	
	return true;
}

Item* ItemEditor::createItem()
{
	RenderInfoEditor* ri_editor = dynamic_cast<RenderInfoEditor*>(ContentEditor::getSingleton().getComponent("RIEditor"));
	
	// create an item picture
	std::stringstream idstream;
	idstream << m_unique_id;
	
	Ogre::SceneManager* editor_scene_mng = Ogre::Root::getSingleton().getSceneManager("EditorSceneManager");
	Ogre::Camera* item_camera = editor_scene_mng->getCamera("item_camera");
	
	// adjust the camera
	Ogre::SceneNode* topnode = ri_editor->getEditedGraphicObject()->getTopNode();
	topnode->_updateBounds();
	
	Ogre::Vector3 bbox_min(1000, 1000, 1000);
	Ogre::Vector3 bbox_max(-1000, -1000, -1000);
	
	getNodeBounds(topnode, bbox_min, bbox_max);
	
	double size_x = bbox_max[0] - bbox_min[0];
	double size_z = bbox_max[2] - bbox_min[2];
	double center_x = 0.5*(bbox_max[0] + bbox_min[0]);
	double center_z = 0.5*(bbox_max[2] + bbox_min[2]);
	double viewsize = MathHelper::Max(size_x, size_z);
	
	//DEBUG("box %f %f %f - %f %f %f",bbox_min[0],bbox_min[1],bbox_min[2],bbox_max[0],bbox_max[1],bbox_max[2]);
	
	item_camera->setPosition(Ogre::Vector3(center_x, viewsize*sqrt(double(2)), center_z));
	// item_camera->setPosition(Ogre::Vector3(center_x, 1,center_z));
	//item_camera->setPosition(Ogre::Vector3(0,1,0));
	item_camera->lookAt(Ogre::Vector3(center_x, 0, center_z));
	
	// texture that is created from the camera image
	Ogre::TexturePtr item_texture = Ogre::TextureManager::getSingleton().createManual( std::string("item_tex_") + idstream.str(),
			Ogre::ResourceGroupManager::DEFAULT_RESOURCE_GROUP_NAME, Ogre::TEX_TYPE_2D,
   256, 256, 0, Ogre::PF_R8G8B8A8, Ogre::TU_RENDERTARGET );
	
   // connection texture and camera via RenderTarget
	Ogre::RenderTarget* item_rt = item_texture->getBuffer()->getRenderTarget();
	item_rt->setAutoUpdated(false);
	Ogre::Viewport *item_view = item_rt->addViewport(item_camera );
	item_view->setClearEveryFrame(true);
	item_view->setOverlaysEnabled(false);
	item_view->setBackgroundColour(Ogre::ColourValue(0, 0, 0, 1.0) );

#ifdef CEGUI_07
	// create a CEGUI Image from the Texture
  CEGUI::Texture& item_ceguiTex = static_cast<CEGUI::OgreRenderer*>(
      CEGUI::System::getSingleton().getRenderer())->createTexture(item_texture);
    
	CEGUI::Imageset& item_textureImageSet = CEGUI::ImagesetManager::getSingleton().create(std::string("item_imageset_") + idstream.str(), item_ceguiTex);

	item_textureImageSet.defineImage( "item_img_"+ idstream.str(),
			CEGUI::Point( 0.0f, 0.0f ),
			CEGUI::Size( item_ceguiTex.getSize().d_width, item_ceguiTex.getSize().d_height ),
			CEGUI::Point( 0.0f, 0.0f ) );
#else
  
  CEGUI::String imageName ("item_img_"+ idstream.str());
  CEGUI::String textureName ("ItemEditorTexture");

	if (! CEGUI::System::getSingleton ().getRenderer ()->isTextureDefined (textureName)
		&& ! CEGUI::ImageManager::getSingleton().isDefined(imageName))
	{
		CEGUI::Texture &item_ceguiTex = static_cast<CEGUI::OgreRenderer*>
			  (CEGUI::System::getSingleton ().getRenderer ())->createTexture (textureName, item_texture);

		CEGUI::OgreRenderer* rendererPtr = static_cast<CEGUI::OgreRenderer*>(CEGUI::System::getSingleton().getRenderer());
		CEGUI::TextureTarget*   d_textureTarget;
		CEGUI::BasicImage*      d_textureTargetImage;
		d_textureTarget = rendererPtr->createTextureTarget();
		d_textureTargetImage = static_cast<CEGUI::BasicImage*>(&CEGUI::ImageManager::getSingleton().create("BasicImage", imageName));
		d_textureTargetImage->setTexture(&item_ceguiTex);
		d_textureTargetImage->setArea(CEGUI::Rectf(0, 0, item_ceguiTex.getSize ().d_width, item_ceguiTex.getSize ().d_height));
	}
#endif
	item_rt->update();
	
	CEGUI::Window* label = CEGUIUtility::getWindowForLoadedLayout(m_rootWindow,
    "Root/ObjectInfoTabControl/__auto_TabPane__/ItemTab/ItemTabControl/__auto_TabPane__/BM/itemPreview");

	// reparse and update the FixedObject Data
	// create a unique renderinfo (to avoid that the object is modified by the editor after creation)
	std::string unique_ri = ri_editor->getUniqueRenderinfo();
	// temporarily replace the renderinfo name
	TiXmlElement * item_ri = m_item_xml.RootElement()->FirstChildElement("RenderInfo");
	if (item_ri == 0)
  {
		return 0;
  }

	std::string name = item_ri->Attribute("name");
	item_ri->SetAttribute("name", unique_ri.c_str());
	
	// temporarily replace the image name
	TiXmlElement * item_image = m_item_xml.RootElement()->FirstChildElement("Image");
	if (item_image == 0)
  {
    return 0;
  }

	std::string image = item_image->Attribute("image");
	
	if (image == "noMedia.png") // previously: "set:noMedia.png image:full_image"
	{
		std::stringstream itemimage;
		//itemimage <<  "set:item_imageset_" << idstream.str()<< " " << "image:item_img_" << idstream.str();
    itemimage << "item_img_" << idstream.str();
		label->setProperty("Image", itemimage.str().c_str());
		
		// itemimage.str("");
		// itemimage << "set:editor_imageset image:editor_img";
		item_image->SetAttribute("image", itemimage.str().c_str());
	}

	// make the item subtype unique by adding a number
	std::string plain_subtype = m_item_xml.RootElement()->Attribute("subtype");
	std::stringstream stream;
	stream << plain_subtype << m_unique_id;
	m_unique_id++;
	m_item_xml.RootElement()->SetAttribute("subtype", stream.str().c_str());
	
	std::string subtype = ItemLoader::loadItem(m_item_xml.FirstChildElement(), true);
	
	// reset the changed attributes
	item_ri->SetAttribute("name", name.c_str());
	m_item_xml.RootElement()->SetAttribute("subtype", plain_subtype.c_str());
	item_image->SetAttribute("image", image.c_str());
	
	float magic = getSpinnerValue("Root/ObjectInfoTabControl/__auto_TabPane__/ItemTab/ItemTabControl/__auto_TabPane__/Create/EnchantSpinner", 0);
	Item::Type type = ItemFactory::getBaseType(subtype);
	Item* item = ItemFactory::createItem(type, subtype, 0, magic);
	return item;
}

bool ItemEditor::onItemCreateDrop(const CEGUI::EventArgs& evt)
{
	World* world = World::getWorld();
	if (world == 0)
  {
		return true;
  }
	
	WorldObject* player = world->getLocalPlayer();
	if (player == 0)
  {
		return true;
  }

	Region* region = player->getRegion();
	
	// if the position is set to default, use the player position
	Vector pos = player->getShape()->m_center;
	
	// create the object
	Item* item = createItem();
	if (item == 0)
  {
		return true;
  }

	region->dropItem(item, pos);
	
	return true;
}

bool ItemEditor::onItemCreateInInventory(const CEGUI::EventArgs& evt)
{
	World* world = World::getWorld();
	if (world == 0)
  {
		return true;
  }
	
	WorldObject* player = world->getLocalPlayer();
	if (player == 0)
  {
		return true;
  }

	Item* item = createItem();
	if (item == 0)
  {
		return true;
  }

	static_cast<Player*>(player)->insertItem(item);
	return true;
}

bool ItemEditor::onCopyData(const CEGUI::EventArgs& evt)
{
	std::string objname	= getComboboxSelection("Root/ObjectInfoTabControl/__auto_TabPane__/ItemTab/ItemTabControl/__auto_TabPane__/Properties/CopyDataBox", "");
	if (objname == "")
  {
		return true;
  }
	
	ItemBasicData* data = ItemFactory::getItemBasicData(objname);

	if (data == 0)
  {
		return true;
  }
	
	data->m_subtype = "EditorItem";

	m_edited_item = *data;
	
	m_modified_item = true;
	m_modified_item_xml = true;
	
	return true;
}

void ItemEditor::updateItemXML()
{
	// update the XML representation
	m_edited_item.writeToXML(m_item_xml.FirstChildElement());
	
	// write to the editor
	TiXmlPrinter printer;
	m_item_xml.Accept(&printer);
	
	CEGUI::MultiLineEditbox* editor = static_cast<CEGUI::MultiLineEditbox*>(CEGUIUtility::getWindowForLoadedLayout(m_rootWindow,
    "Root/ObjectInfoTabControl/__auto_TabPane__/ItemTab/ItemTabControl/__auto_TabPane__/XML/ItemXMLEditbox"));
	editor->setText(printer.CStr());
}

void ItemEditor::updateItemEditor()
{
	m_no_cegui_events = true;
	
	setSpinnerValue("Root/ObjectInfoTabControl/__auto_TabPane__/ItemTab/ItemTabControl/__auto_TabPane__/General/PriceSpinner", m_edited_item.m_price);
	setSpinnerValue("Root/ObjectInfoTabControl/__auto_TabPane__/ItemTab/ItemTabControl/__auto_TabPane__/General/EnchantMinSpinner", m_edited_item.m_min_enchant);
	setSpinnerValue("Root/ObjectInfoTabControl/__auto_TabPane__/ItemTab/ItemTabControl/__auto_TabPane__/General/EnchantMaxSpinner", m_edited_item.m_max_enchant);
	setWindowText("Root/ObjectInfoTabControl/__auto_TabPane__/ItemTab/ItemTabControl/__auto_TabPane__/General/NameBox", m_edited_item.m_name);
	
	Item::Type type = m_edited_item.m_type;
	if (type == Item::ARMOR)
  {
    setComboboxSelection("Root/ObjectInfoTabControl/__auto_TabPane__/ItemTab/ItemTabControl/__auto_TabPane__/General/TypeBox", "Armor");
  }
	else if (type == Item::WEAPON)
  {
    setComboboxSelection("Root/ObjectInfoTabControl/__auto_TabPane__/ItemTab/ItemTabControl/__auto_TabPane__/General/TypeBox", "Weapon");
  }
	else if (type == Item::HELMET)
  {
    setComboboxSelection("Root/ObjectInfoTabControl/__auto_TabPane__/ItemTab/ItemTabControl/__auto_TabPane__/General/TypeBox", "Helmet");
  }
	else if (type == Item::GLOVES)
  {
    setComboboxSelection("Root/ObjectInfoTabControl/__auto_TabPane__/ItemTab/ItemTabControl/__auto_TabPane__/General/TypeBox", "Gloves");
  }
	else if (type == Item::RING)
  {
    setComboboxSelection("Root/ObjectInfoTabControl/__auto_TabPane__/ItemTab/ItemTabControl/__auto_TabPane__/General/TypeBox", "Ring");
  }
	else if (type == Item::AMULET)
  {
    setComboboxSelection("Root/ObjectInfoTabControl/__auto_TabPane__/ItemTab/ItemTabControl/__auto_TabPane__/General/TypeBox", "Amulet");
  }
	else if (type == Item::SHIELD)
  {
    setComboboxSelection("Root/ObjectInfoTabControl/__auto_TabPane__/ItemTab/ItemTabControl/__auto_TabPane__/General/TypeBox", "Shield");
  }
	else if (type == Item::POTION)
  {
    setComboboxSelection("Root/ObjectInfoTabControl/__auto_TabPane__/ItemTab/ItemTabControl/__auto_TabPane__/General/TypeBox", "Potion");
  }
	
	Item::Size size = m_edited_item.m_size;
	if (size == Item::BIG) setComboboxSelection("Root/ObjectInfoTabControl/__auto_TabPane__/ItemTab/ItemTabControl/__auto_TabPane__/Properties/SizeBox","Big");
	else if (size == Item::MEDIUM) setComboboxSelection("Root/ObjectInfoTabControl/__auto_TabPane__/ItemTab/ItemTabControl/__auto_TabPane__/Properties/SizeBox","Medium");
	else if (size == Item::SMALL) setComboboxSelection("Root/ObjectInfoTabControl/__auto_TabPane__/ItemTab/ItemTabControl/__auto_TabPane__/Properties/SizeBox","Small");
	
	// update weapon properties
	if (m_edited_item.m_weapon_attr != 0)
	{
		Damage& dmg = m_edited_item.m_weapon_attr->m_damage;
		
		setWindowText("Root/ObjectInfoTabControl/__auto_TabPane__/ItemTab/ItemTabControl/__auto_TabPane__/Weapon/TypeBox", m_edited_item.m_weapon_attr->m_weapon_type);

		setSpinnerValue("Root/ObjectInfoTabControl/__auto_TabPane__/ItemTab/ItemTabControl/__auto_TabPane__/Weapon/PhysMinSpinner", dmg.m_min_damage[Damage::PHYSICAL]);
		setSpinnerValue("Root/ObjectInfoTabControl/__auto_TabPane__/ItemTab/ItemTabControl/__auto_TabPane__/Weapon/PhysMaxSpinner", dmg.m_max_damage[Damage::PHYSICAL]);
		setSpinnerValue("Root/ObjectInfoTabControl/__auto_TabPane__/ItemTab/ItemTabControl/__auto_TabPane__/Weapon/PhysMultSpinner", dmg.m_multiplier[Damage::PHYSICAL]);
		
		setSpinnerValue("Root/ObjectInfoTabControl/__auto_TabPane__/ItemTab/ItemTabControl/__auto_TabPane__/Weapon/FireMinSpinner", dmg.m_min_damage[Damage::FIRE]);
		setSpinnerValue("Root/ObjectInfoTabControl/__auto_TabPane__/ItemTab/ItemTabControl/__auto_TabPane__/Weapon/FireMaxSpinner", dmg.m_max_damage[Damage::FIRE]);
		setSpinnerValue("Root/ObjectInfoTabControl/__auto_TabPane__/ItemTab/ItemTabControl/__auto_TabPane__/Weapon/FireMultSpinner", dmg.m_multiplier[Damage::FIRE]);
		
		setSpinnerValue("Root/ObjectInfoTabControl/__auto_TabPane__/ItemTab/ItemTabControl/__auto_TabPane__/Weapon/IceMinSpinner", dmg.m_min_damage[Damage::ICE]);
		setSpinnerValue("Root/ObjectInfoTabControl/__auto_TabPane__/ItemTab/ItemTabControl/__auto_TabPane__/Weapon/IceMaxSpinner", dmg.m_max_damage[Damage::ICE]);
		setSpinnerValue("Root/ObjectInfoTabControl/__auto_TabPane__/ItemTab/ItemTabControl/__auto_TabPane__/Weapon/IceMultSpinner", dmg.m_multiplier[Damage::ICE]);
		
		setSpinnerValue("Root/ObjectInfoTabControl/__auto_TabPane__/ItemTab/ItemTabControl/__auto_TabPane__/Weapon/AirMinSpinner", dmg.m_min_damage[Damage::AIR]);
		setSpinnerValue("Root/ObjectInfoTabControl/__auto_TabPane__/ItemTab/ItemTabControl/__auto_TabPane__/Weapon/AirMaxSpinner", dmg.m_max_damage[Damage::AIR]);
		setSpinnerValue("Root/ObjectInfoTabControl/__auto_TabPane__/ItemTab/ItemTabControl/__auto_TabPane__/Weapon/AirMultSpinner", dmg.m_multiplier[Damage::AIR]);
		
		setSpinnerValue("Root/ObjectInfoTabControl/__auto_TabPane__/ItemTab/ItemTabControl/__auto_TabPane__/Weapon/PrecisionSpinner", dmg.m_attack);
		setSpinnerValue("Root/ObjectInfoTabControl/__auto_TabPane__/ItemTab/ItemTabControl/__auto_TabPane__/Weapon/PowerSpinner", dmg.m_power);
		setSpinnerValue("Root/ObjectInfoTabControl/__auto_TabPane__/ItemTab/ItemTabControl/__auto_TabPane__/Weapon/CritPercentSpinner", dmg.m_crit_perc*100);
		
		setSpinnerValue("Root/ObjectInfoTabControl/__auto_TabPane__/ItemTab/ItemTabControl/__auto_TabPane__/Weapon/RangeSpinner", m_edited_item.m_weapon_attr->m_attack_range);
		setSpinnerValue("Root/ObjectInfoTabControl/__auto_TabPane__/ItemTab/ItemTabControl/__auto_TabPane__/Weapon/SpeedSpinner", m_edited_item.m_weapon_attr->m_dattack_speed );
		
		setCheckboxSelected("Root/ObjectInfoTabControl/__auto_TabPane__/ItemTab/ItemTabControl/__auto_TabPane__/Weapon/TwohandedCheckbox", m_edited_item.m_weapon_attr->m_two_handed);
	}


	if (m_edited_item.m_useup_effect != 0)
	{
		CreatureDynAttrMod* dynmod = m_edited_item.m_useup_effect;
		
		setSpinnerValue("Root/ObjectInfoTabControl/__auto_TabPane__/ItemTab/ItemTabControl/__auto_TabPane__/Consume/HealthSpinner", dynmod->m_dhealth);
		setSpinnerValue("Root/ObjectInfoTabControl/__auto_TabPane__/ItemTab/ItemTabControl/__auto_TabPane__/Consume/BlindSpinner", dynmod->m_dstatus_mod_immune_time[Damage::BLIND]);
		setSpinnerValue("Root/ObjectInfoTabControl/__auto_TabPane__/ItemTab/ItemTabControl/__auto_TabPane__/Consume/PoisonedSpinner", dynmod->m_dstatus_mod_immune_time[Damage::POISONED]);
		setSpinnerValue("Root/ObjectInfoTabControl/__auto_TabPane__/ItemTab/ItemTabControl/__auto_TabPane__/Consume/BerserkSpinner", dynmod->m_dstatus_mod_immune_time[Damage::BERSERK]);
		setSpinnerValue("Root/ObjectInfoTabControl/__auto_TabPane__/ItemTab/ItemTabControl/__auto_TabPane__/Consume/ConfusedSpinner", dynmod->m_dstatus_mod_immune_time[Damage::CONFUSED]);
		setSpinnerValue("Root/ObjectInfoTabControl/__auto_TabPane__/ItemTab/ItemTabControl/__auto_TabPane__/Consume/MuteSpinner", dynmod->m_dstatus_mod_immune_time[Damage::MUTE]);
		setSpinnerValue("Root/ObjectInfoTabControl/__auto_TabPane__/ItemTab/ItemTabControl/__auto_TabPane__/Consume/ParalyzedSpinner", dynmod->m_dstatus_mod_immune_time[Damage::PARALYZED]);
		setSpinnerValue("Root/ObjectInfoTabControl/__auto_TabPane__/ItemTab/ItemTabControl/__auto_TabPane__/Consume/FrozenSpinner", dynmod->m_dstatus_mod_immune_time[Damage::FROZEN]);
		setSpinnerValue("Root/ObjectInfoTabControl/__auto_TabPane__/ItemTab/ItemTabControl/__auto_TabPane__/Consume/BurningSpinner", dynmod->m_dstatus_mod_immune_time[Damage::BURNING]);
	}

	if (m_edited_item.m_equip_effect != 0)
	{
		CreatureBaseAttrMod* basemod = m_edited_item.m_equip_effect;
		
		setSpinnerValue("Root/ObjectInfoTabControl/__auto_TabPane__/ItemTab/ItemTabControl/__auto_TabPane__/Equip/HealthSpinner", basemod->m_dmax_health);
	
		setSpinnerValue("Root/ObjectInfoTabControl/__auto_TabPane__/ItemTab/ItemTabControl/__auto_TabPane__/Equip/StrengthSpinner", basemod->m_dstrength);
		setSpinnerValue("Root/ObjectInfoTabControl/__auto_TabPane__/ItemTab/ItemTabControl/__auto_TabPane__/Equip/DexteritySpinner", basemod->m_ddexterity);
		setSpinnerValue("Root/ObjectInfoTabControl/__auto_TabPane__/ItemTab/ItemTabControl/__auto_TabPane__/Equip/MagicPowerSpinner", basemod->m_dmagic_power);
		setSpinnerValue("Root/ObjectInfoTabControl/__auto_TabPane__/ItemTab/ItemTabControl/__auto_TabPane__/Equip/WillpowerSpinner", basemod->m_dwillpower);
		
		setSpinnerValue("Root/ObjectInfoTabControl/__auto_TabPane__/ItemTab/ItemTabControl/__auto_TabPane__/Equip/PhysResSpinner", basemod->m_dresistances[Damage::PHYSICAL]);
		setSpinnerValue("Root/ObjectInfoTabControl/__auto_TabPane__/ItemTab/ItemTabControl/__auto_TabPane__/Equip/FireResSpinner", basemod->m_dresistances[Damage::FIRE]);
		setSpinnerValue("Root/ObjectInfoTabControl/__auto_TabPane__/ItemTab/ItemTabControl/__auto_TabPane__/Equip/IceResSpinner", basemod->m_dresistances[Damage::ICE]);
		setSpinnerValue("Root/ObjectInfoTabControl/__auto_TabPane__/ItemTab/ItemTabControl/__auto_TabPane__/Equip/AirResSpinner", basemod->m_dresistances[Damage::AIR]);
		
		setSpinnerValue("Root/ObjectInfoTabControl/__auto_TabPane__/ItemTab/ItemTabControl/__auto_TabPane__/Equip/ArmorSpinner", basemod->m_darmor);
		setSpinnerValue("Root/ObjectInfoTabControl/__auto_TabPane__/ItemTab/ItemTabControl/__auto_TabPane__/Equip/BlockSpinner", basemod->m_dblock);
		setSpinnerValue("Root/ObjectInfoTabControl/__auto_TabPane__/ItemTab/ItemTabControl/__auto_TabPane__/Equip/AttackSpinner", basemod->m_dattack);
		setSpinnerValue("Root/ObjectInfoTabControl/__auto_TabPane__/ItemTab/ItemTabControl/__auto_TabPane__/Equip/PowerSpinner", basemod->m_dpower);
	}

	m_no_cegui_events = false;
}

