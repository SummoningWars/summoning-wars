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



#include "monstereditor.h"



#include "graphicmanager.h"

#include "fixedobjecteditor.h"
#include "world.h"
#include "renderinfoeditor.h"
#include "contenteditor.h"


void MonsterEditor::init(CEGUI::Window* parent)
{
	ContentEditorTab::init(parent);
	
	CEGUI::WindowManager& win_mgr = CEGUI::WindowManager::getSingleton();
	
	
	CEGUI::Spinner* radiusSpinner =  static_cast<CEGUI::Spinner*>(win_mgr.getWindow("MonsterTab/Prop/RadiusSpinner"));
	
	
	radiusSpinner->subscribeEvent(CEGUI::Spinner::EventValueChanged, CEGUI::Event::Subscriber(&MonsterEditor::onMonsterModified, this));
	
	/*
	CEGUI::Combobox* layerSelector = static_cast<CEGUI::Combobox*>(win_mgr.getWindow("MonsterTab/Prop/LayerSelector"));
	layerSelector->addItem(new CEGUI::ListboxTextItem("Normal"));
	layerSelector->addItem(new CEGUI::ListboxTextItem("Base"));
	layerSelector->addItem(new CEGUI::ListboxTextItem("Air"));
	layerSelector->addItem(new CEGUI::ListboxTextItem("NoCollision"));
	
	layerSelector->subscribeEvent(CEGUI::Combobox::EventListSelectionAccepted, CEGUI::Event::Subscriber(&MonsterEditor::onMonsterModified, this));
	*/
	
	CEGUI::PushButton* xmlsubmitButton = static_cast<CEGUI::PushButton*>(win_mgr.getWindow("MonsterTab/XML/SubmitButton"));
	xmlsubmitButton->subscribeEvent(CEGUI::PushButton::EventClicked, CEGUI::Event::Subscriber(&MonsterEditor::onMonsterXMLModified, this));
	
	CEGUI::PushButton* createFOButton = static_cast<CEGUI::PushButton*>(win_mgr.getWindow("MonsterTab/XML/CreateButton"));
	createFOButton->subscribeEvent(CEGUI::PushButton::EventClicked, CEGUI::Event::Subscriber(&MonsterEditor::onMonsterCreate, this));
	
	CEGUI::PushButton* delallFOButton = static_cast<CEGUI::PushButton*>(win_mgr.getWindow("MonsterTab/Create/DelAllButton"));
	delallFOButton->subscribeEvent(CEGUI::PushButton::EventClicked, CEGUI::Event::Subscriber(&MonsterEditor::onDelAllObjects, this));
	
	CEGUI::PushButton* getplayerButton = static_cast<CEGUI::PushButton*>(win_mgr.getWindow("MonsterTab/Create/GetPlPosButton"));
	getplayerButton->subscribeEvent(CEGUI::PushButton::EventClicked, CEGUI::Event::Subscriber(&MonsterEditor::onGetPlayerPosition, this));
	
	CEGUI::PushButton* copyfoButton = static_cast<CEGUI::PushButton*>(win_mgr.getWindow("MonsterTab/Properties/CopyDataButton"));
	copyfoButton->subscribeEvent(CEGUI::PushButton::EventClicked, CEGUI::Event::Subscriber(&MonsterEditor::onCopyData, this));
	
	// init the internal data
	TiXmlElement * monster_root = new TiXmlElement("Monster");  
	m_monster_xml.LinkEndChild( monster_root );  
	monster_root->SetAttribute("subtype","EditorMonster");
	
	TiXmlElement * monster_ri = new TiXmlElement("RenderInfo");  
	monster_ri->SetAttribute("name","EditorRenderInfo");
	monster_root->LinkEndChild(monster_ri);
	
	m_update_base_content = true;
}

void MonsterEditor::update()
{
	if (m_update_base_content == true)
	{
		updateAllMonsterList();
		m_update_base_content = false;
	}
	
	if (m_modified_monster)
	{
		updateMonsterXML();
	}
	
	if (m_modified_monster_xml)
	{
		updateMonsterEditor();
	}
	
	m_modified_monster = false;
	m_modified_monster_xml = false;
}

void MonsterEditor::updateAllMonsterList()
{
	CEGUI::WindowManager& win_mgr = CEGUI::WindowManager::getSingleton();
	
	CEGUI::Combobox* copyfoSelector = static_cast<CEGUI::Combobox*>(win_mgr.getWindow("MonsterTab/Properties/CopyDataBox"));
	
	// Fill list of all Monsters
	const std::map<GameObject::Subtype, MonsterBasicData*>& all_fo = ObjectFactory::getAllMonsterData();
	std::map<GameObject::Subtype, MonsterBasicData*>::const_iterator it;
	for (it = all_fo.begin(); it != all_fo.end(); ++it)
	{
		copyfoSelector->addItem(new CEGUI::ListboxTextItem(it->first.c_str()));
	}
}

void MonsterEditor::updateMonsterXML()
{
	CEGUI::WindowManager& win_mgr = CEGUI::WindowManager::getSingleton();
	
	// update the XML representation
	m_edited_monster.writeToXML(m_monster_xml.FirstChildElement());
	
	// write to the editor
	TiXmlPrinter printer;
	m_monster_xml.Accept(&printer);
	
	CEGUI::MultiLineEditbox* editor = static_cast<CEGUI::MultiLineEditbox*>(win_mgr.getWindow("MonsterTab/XML/MonsterXMLEditbox"));
	editor->setText(printer.CStr());
}

void MonsterEditor::updateMonsterEditor()
{
	m_no_cegui_events = true;
	
	CEGUI::WindowManager& win_mgr = CEGUI::WindowManager::getSingleton();
	/*
	CEGUI::Combobox* layerSelector = static_cast<CEGUI::Combobox*>(win_mgr.getWindow("MonsterTab/Prop/LayerSelector"));
	*/
	CEGUI::Spinner* radiusSpinner =  static_cast<CEGUI::Spinner*>(win_mgr.getWindow("MonsterTab/Prop/RadiusSpinner"));
	// adjust the spinner values
	radiusSpinner->setCurrentValue(m_edited_monster.m_radius);
	
	/*
	// set the layer combobox
	std::string layer = "Normal";
	if (m_edited_monster.m_layer == WorldObject::LAYER_BASE)
		layer = "Base";
	else if (m_edited_monster.m_layer == WorldObject::LAYER_AIR)
		layer = "Air";
	else if (m_edited_monster.m_layer == WorldObject::LAYER_NOCOLLISION)
		layer == "NoCollision";
	
	
	CEGUI::ListboxItem* selection = layerSelector->findItemWithText(CEGUI::String(layer),0);
	if (selection != 0)
	{
		int id = selection->getID();
		layerSelector->setSelection(id,id);
		layerSelector->setText(layer);
	}
	*/
	m_no_cegui_events = false;
}


bool MonsterEditor::onMonsterModified(const CEGUI::EventArgs& evt)
{
	if (m_no_cegui_events)
		return true;
	
	CEGUI::WindowManager& win_mgr = CEGUI::WindowManager::getSingleton();
	
	CEGUI::Spinner* radiusSpinner =  static_cast<CEGUI::Spinner*>(win_mgr.getWindow("MonsterTab/Prop/RadiusSpinner"));
	
	//CEGUI::Combobox* layerSelector = static_cast<CEGUI::Combobox*>(win_mgr.getWindow("MonsterTab/Prop/LayerSelector"));
	
	m_edited_monster.m_radius = radiusSpinner->getCurrentValue();
	
	/*
	std::string layer = layerSelector->getText().c_str();
	if (layer == "Base")
		m_edited_monster.m_layer = WorldObject::LAYER_BASE;
	else if (layer == "Air")
		m_edited_monster.m_layer = WorldObject::LAYER_AIR;
	else if (layer == "NoCollision")
		m_edited_monster.m_layer = WorldObject::LAYER_NOCOLLISION;
	else
		m_edited_monster.m_layer = WorldObject::LAYER_BASE | WorldObject::LAYER_AIR;
	
	m_modified_monster = true;
	*/
	
	return false;
}


bool MonsterEditor::onMonsterXMLModified(const CEGUI::EventArgs& evt)
{
	CEGUI::WindowManager& win_mgr = CEGUI::WindowManager::getSingleton();
	CEGUI::MultiLineEditbox* editor = static_cast<CEGUI::MultiLineEditbox*>(win_mgr.getWindow("MonsterTab/XML/MonsterXMLEditbox"));
	
	// Parse the editor text to XML
	// use temporary XML document for recovering from errors
	TiXmlDocument ri_temp_xml;
	ri_temp_xml.Parse(editor->getText().c_str());
	
	if (!ri_temp_xml.Error())
	{
		// first, remove the old root
		TiXmlElement* oldroot = m_monster_xml.RootElement();
		if (oldroot != 0)
		{
			m_monster_xml.RemoveChild(oldroot);
		}
		m_monster_xml.LinkEndChild(ri_temp_xml.RootElement()->Clone());
		
		// parse the XML to the monster data
		ObjectLoader::loadMonster(m_monster_xml.FirstChildElement(),true);
		// copy to the local Data structure
		MonsterBasicData* data = ObjectFactory::getMonsterData("EditorMonster");
		if (data != 0)
		{
			m_edited_monster = *data;
		}
		
		m_modified_monster_xml = true;
	}
	else
	{
		// XML parse error
		// set the cursor to the position of the first error
		int err_row = ri_temp_xml.ErrorRow();
		int err_col = ri_temp_xml.ErrorCol();
		
		setMultiLineEditboxCursor("MonsterTab/XML/MonsterXMLEditbox",err_row,err_col);
	}
	
	return true;
}

bool MonsterEditor::onMonsterCreate(const CEGUI::EventArgs& evt)
{
	// reparse and update the Monster Data
	
	// create a unique renderinfo (to avoid that the object is modified by the editor after creation)
	RenderInfoEditor* ri_editor = dynamic_cast<RenderInfoEditor*>(ContentEditor::getSingleton().getComponent("RIEditor"));
	std::string unique_ri = ri_editor->getUniqueRenderinfo();
	// temporarily replace the renderinfo name
	TiXmlElement * fixed_ri = m_monster_xml.RootElement()->FirstChildElement("RenderInfo");
	if (fixed_ri == 0)
		return true;
	
	std::string name = fixed_ri->Attribute("name");
	fixed_ri->SetAttribute("name",unique_ri.c_str());
	
	ObjectLoader::loadMonster(m_monster_xml.FirstChildElement(),true);
	
	fixed_ri->SetAttribute("name",name.c_str());
	
	Vector pos;
	float angle;
	float height = 0.0;
	
	World* world = World::getWorld();
	if (world == 0)
		return true;
	
	WorldObject* player = world->getLocalPlayer();
	if (player == 0)
		return true;
	Region* region = player->getRegion();
	
	// if the position is set to default, use the player position
	pos.m_x = getSpinnerValue("MonsterTab/Create/PosXSpinner",0);
	pos.m_y = getSpinnerValue("MonsterTab/Create/PosYSpinner",0);
	angle = getSpinnerValue("MonsterTab/Create/AngleSpinner",0);
	if (pos.m_x == 0 && pos.m_y == 0)
	{
		pos = player->getShape()->m_center;
	}
	
	// create the object
	int id = region->createObject("EditorMonster", pos,angle, height,WorldObject::STATE_ACTIVE);
	m_created_objects.push_back(std::make_pair(region->getId(), id));

	return false;
}

bool MonsterEditor::onCopyData(const CEGUI::EventArgs& evt)
{
	std::string objname	= getComboboxSelection("MonsterTab/Properties/CopyDataBox", "");
	if (objname == "")
		return true;
	
	MonsterBasicData* modata = ObjectFactory::getMonsterData(objname);
	if (modata == 0)
		return true;
	
	m_edited_monster = *modata;
	
	m_modified_monster = true;
	m_modified_monster_xml = true;
	
	return true;
}

bool MonsterEditor::onGetPlayerPosition(const CEGUI::EventArgs& evt)
{
	World* world = World::getWorld();
	if (world == 0)
		return true;
	
	WorldObject* player = world->getLocalPlayer();
	if (player == 0)
		return true;
	Region* region = player->getRegion();
	
	Vector pos = player->getShape()->m_center;
	
	setSpinnerValue("MonsterTab/Create/PosXSpinner", pos.m_x);
	setSpinnerValue("MonsterTab/Create/PosYSpinner", pos.m_y);
	
	return true;
}

bool MonsterEditor::onDelAllObjects(const CEGUI::EventArgs& evt)
{
	World* world = World::getWorld();
	if (world == 0)
		return true;
	
	std::list< std::pair<int,int> >::iterator it;
	for (it = m_created_objects.begin(); it != m_created_objects.end(); ++it)
	{
		Region* region = world->getRegion(it->first);
		if (region == 0)
			continue;
		
		region->deleteObject(it->second);
	}
	m_created_objects.clear();
	
	return true;
}
