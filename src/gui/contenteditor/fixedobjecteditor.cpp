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

#include "fixedobjecteditor.h"
#include "world.h"
#include "renderinfoeditor.h"
#include "contenteditor.h"


void FixedObjectEditor::init(CEGUI::Window* parent)
{
	ContentEditorTab::init(parent);
	
	CEGUI::WindowManager& win_mgr = CEGUI::WindowManager::getSingleton();
	
	CEGUI::PushButton* detectCircleButton = static_cast<CEGUI::PushButton*>(win_mgr.getWindow("FOTab/Prop/DetectCircleButton"));
	CEGUI::PushButton* detectRectButton = static_cast<CEGUI::PushButton*>(win_mgr.getWindow("FOTab/Prop/DetectRectButton"));
	
	CEGUI::Spinner* radiusSpinner =  static_cast<CEGUI::Spinner*>(win_mgr.getWindow("FOTab/Prop/RadiusSpinner"));
	CEGUI::Spinner* widthSpinner =  static_cast<CEGUI::Spinner*>(win_mgr.getWindow("FOTab/Prop/WidthSpinner"));
	CEGUI::Spinner* depthSpinner =  static_cast<CEGUI::Spinner*>(win_mgr.getWindow("FOTab/Prop/DepthSpinner"));
	
	CEGUI::RadioButton* circleButton = static_cast<CEGUI::RadioButton*>(win_mgr.getWindow("FOTab/Prop/CircleCheckbox"));
	CEGUI::RadioButton* rectButton = static_cast<CEGUI::RadioButton*>(win_mgr.getWindow("FOTab/Prop/RectangleCheckbox"));
	
	radiusSpinner->subscribeEvent(CEGUI::Spinner::EventValueChanged, CEGUI::Event::Subscriber(&FixedObjectEditor::onFixedObjectModified, this));
	widthSpinner->subscribeEvent(CEGUI::Spinner::EventValueChanged, CEGUI::Event::Subscriber(&FixedObjectEditor::onFixedObjectModified, this));
	depthSpinner->subscribeEvent(CEGUI::Spinner::EventValueChanged, CEGUI::Event::Subscriber(&FixedObjectEditor::onFixedObjectModified, this));
	
	circleButton->subscribeEvent(CEGUI::RadioButton::EventSelectStateChanged, CEGUI::Event::Subscriber(&FixedObjectEditor::onFixedObjectModified, this));
	rectButton->subscribeEvent(CEGUI::RadioButton::EventSelectStateChanged, CEGUI::Event::Subscriber(&FixedObjectEditor::onFixedObjectModified, this));
	circleButton->setSelected(true);
	
	detectCircleButton->subscribeEvent(CEGUI::PushButton::EventClicked, CEGUI::Event::Subscriber(&FixedObjectEditor::onFixedObjectAutodetectSize, this));
	detectRectButton->subscribeEvent(CEGUI::PushButton::EventClicked, CEGUI::Event::Subscriber(&FixedObjectEditor::onFixedObjectAutodetectSize, this));
	
	CEGUI::Combobox* layerSelector = static_cast<CEGUI::Combobox*>(win_mgr.getWindow("FOTab/Prop/LayerSelector"));
	layerSelector->addItem(new CEGUI::ListboxTextItem("Normal"));
	layerSelector->addItem(new CEGUI::ListboxTextItem("Base"));
	layerSelector->addItem(new CEGUI::ListboxTextItem("Air"));
	layerSelector->addItem(new CEGUI::ListboxTextItem("NoCollision"));
	
	layerSelector->subscribeEvent(CEGUI::Combobox::EventListSelectionAccepted, CEGUI::Event::Subscriber(&FixedObjectEditor::onFixedObjectModified, this));
	
	CEGUI::PushButton* xmlsubmitButton = static_cast<CEGUI::PushButton*>(win_mgr.getWindow("FOTab/XML/SubmitButton"));
	xmlsubmitButton->subscribeEvent(CEGUI::PushButton::EventClicked, CEGUI::Event::Subscriber(&FixedObjectEditor::onFixedObjectXMLModified, this));
	
	CEGUI::PushButton* createFOButton = static_cast<CEGUI::PushButton*>(win_mgr.getWindow("FOTab/XML/CreateButton"));
	createFOButton->subscribeEvent(CEGUI::PushButton::EventClicked, CEGUI::Event::Subscriber(&FixedObjectEditor::onFixedObjectCreate, this));
	
	CEGUI::PushButton* delallFOButton = static_cast<CEGUI::PushButton*>(win_mgr.getWindow("FOTab/Create/DelAllButton"));
	delallFOButton->subscribeEvent(CEGUI::PushButton::EventClicked, CEGUI::Event::Subscriber(&FixedObjectEditor::onDelAllObjects, this));
	
	CEGUI::PushButton* getplayerButton = static_cast<CEGUI::PushButton*>(win_mgr.getWindow("FOTab/Create/GetPlPosButton"));
	getplayerButton->subscribeEvent(CEGUI::PushButton::EventClicked, CEGUI::Event::Subscriber(&FixedObjectEditor::onGetPlayerPosition, this));
	
	
	// init the internal data
	TiXmlElement * fixed_root = new TiXmlElement("Object");  
	m_fixed_object_xml.LinkEndChild( fixed_root );  
	fixed_root->SetAttribute("subtype","EditorFixedObject");
	
	TiXmlElement * fixed_ri = new TiXmlElement("RenderInfo");  
	fixed_ri->SetAttribute("name","EditorRenderInfo");
	fixed_root->LinkEndChild(fixed_ri);
	
	m_update_base_content = true;
}

void FixedObjectEditor::update()
{
	if (m_update_base_content == true)
	{
		updateAllFixedObjectList();
		m_update_base_content = false;
	}
	
	if (m_modified_fixed_object)
	{
		updateFixedObjectXML();
	}
	
	if (m_modified_fixed_object_xml)
	{
		updateFixedObjectEditor();
	}
	
	m_modified_fixed_object = false;
	m_modified_fixed_object_xml = false;
}

void FixedObjectEditor::updateAllFixedObjectList()
{
	CEGUI::WindowManager& win_mgr = CEGUI::WindowManager::getSingleton();
	
	CEGUI::Combobox* copyfoSelector = static_cast<CEGUI::Combobox*>(win_mgr.getWindow("FOTab/Properties/CopyDataBox"));
	
	// Fill list of all FixedObjects
	const std::map<GameObject::Subtype, FixedObjectData*>& all_fo = ObjectFactory::getAllFixedObjectData();
	std::map<GameObject::Subtype, FixedObjectData*>::const_iterator it;
	for (it = all_fo.begin(); it != all_fo.end(); ++it)
	{
		copyfoSelector->addItem(new CEGUI::ListboxTextItem(it->first.c_str()));
	}
	
	CEGUI::PushButton* copyfoButton = static_cast<CEGUI::PushButton*>(win_mgr.getWindow("FOTab/Properties/CopyDataButton"));
	copyfoButton->subscribeEvent(CEGUI::PushButton::EventClicked, CEGUI::Event::Subscriber(&FixedObjectEditor::onCopyData, this));
}

void FixedObjectEditor::updateFixedObjectXML()
{
	CEGUI::WindowManager& win_mgr = CEGUI::WindowManager::getSingleton();
	
	// update the XML representation
	m_edited_fixed_object.writeToXML(m_fixed_object_xml.FirstChildElement());
	
	// write to the editor
	TiXmlPrinter printer;
	m_fixed_object_xml.Accept(&printer);
	
	CEGUI::MultiLineEditbox* editor = static_cast<CEGUI::MultiLineEditbox*>(win_mgr.getWindow("FOTab/XML/FOXMLEditbox"));
	editor->setText(printer.CStr());
}

void FixedObjectEditor::updateFixedObjectEditor()
{
	m_no_cegui_events = true;
	
	CEGUI::WindowManager& win_mgr = CEGUI::WindowManager::getSingleton();
	
	CEGUI::Combobox* layerSelector = static_cast<CEGUI::Combobox*>(win_mgr.getWindow("FOTab/Prop/LayerSelector"));
	
	CEGUI::Spinner* radiusSpinner =  static_cast<CEGUI::Spinner*>(win_mgr.getWindow("FOTab/Prop/RadiusSpinner"));
	CEGUI::Spinner* widthSpinner =  static_cast<CEGUI::Spinner*>(win_mgr.getWindow("FOTab/Prop/WidthSpinner"));
	CEGUI::Spinner* depthSpinner =  static_cast<CEGUI::Spinner*>(win_mgr.getWindow("FOTab/Prop/DepthSpinner"));
	
	CEGUI::RadioButton* circleButton = static_cast<CEGUI::RadioButton*>(win_mgr.getWindow("FOTab/Prop/CircleCheckbox"));
	CEGUI::RadioButton* rectButton = static_cast<CEGUI::RadioButton*>(win_mgr.getWindow("FOTab/Prop/RectangleCheckbox"));
	
	// adjust the spinner values
	if (m_edited_fixed_object.m_shape.m_type == Shape::CIRCLE)
	{
		radiusSpinner->setCurrentValue(m_edited_fixed_object.m_shape.m_radius);
		circleButton->setSelected(true);
	}
	else
	{
		widthSpinner->setCurrentValue(m_edited_fixed_object.m_shape.m_extent.m_x*2);
		depthSpinner->setCurrentValue(m_edited_fixed_object.m_shape.m_extent.m_y*2);
		rectButton->setSelected(true);
	}
	
	// set the layer combobox
	std::string layer = "Normal";
	if (m_edited_fixed_object.m_layer == WorldObject::LAYER_BASE)
		layer = "Base";
	else if (m_edited_fixed_object.m_layer == WorldObject::LAYER_AIR)
		layer = "Air";
	else if (m_edited_fixed_object.m_layer == WorldObject::LAYER_NOCOLLISION)
		layer == "NoCollision";
	
	CEGUI::ListboxItem* selection = layerSelector->findItemWithText(CEGUI::String(layer),0);
	if (selection != 0)
	{
		int id = selection->getID();
		layerSelector->setSelection(id,id);
		layerSelector->setText(layer);
	}
	
	m_no_cegui_events = false;
}


bool FixedObjectEditor::onFixedObjectModified(const CEGUI::EventArgs& evt)
{
	if (m_no_cegui_events)
		return true;
	
	CEGUI::WindowManager& win_mgr = CEGUI::WindowManager::getSingleton();
	
	CEGUI::RadioButton* CircleButton = static_cast<CEGUI::RadioButton*>(win_mgr.getWindow("FOTab/Prop/CircleCheckbox"));
	CEGUI::RadioButton* RectButton = static_cast<CEGUI::RadioButton*>(win_mgr.getWindow("FOTab/Prop/RectangleCheckbox"));
	
	CEGUI::Spinner* radiusSpinner =  static_cast<CEGUI::Spinner*>(win_mgr.getWindow("FOTab/Prop/RadiusSpinner"));
	CEGUI::Spinner* widthSpinner =  static_cast<CEGUI::Spinner*>(win_mgr.getWindow("FOTab/Prop/WidthSpinner"));
	CEGUI::Spinner* depthSpinner =  static_cast<CEGUI::Spinner*>(win_mgr.getWindow("FOTab/Prop/DepthSpinner"));
	
	CEGUI::Combobox* layerSelector = static_cast<CEGUI::Combobox*>(win_mgr.getWindow("FOTab/Prop/LayerSelector"));
	
	if (CircleButton->getSelectedButtonInGroup() == CircleButton)
	{
		m_edited_fixed_object.m_shape.m_type = Shape::CIRCLE;
		m_edited_fixed_object.m_shape.m_radius = radiusSpinner->getCurrentValue();
	}
	else
	{
		m_edited_fixed_object.m_shape.m_type = Shape::RECT;
		m_edited_fixed_object.m_shape.m_extent.m_x = 0.5*widthSpinner->getCurrentValue();
		m_edited_fixed_object.m_shape.m_extent.m_y = 0.5*depthSpinner->getCurrentValue();
	}
	
	std::string layer = layerSelector->getText().c_str();
	if (layer == "Base")
		m_edited_fixed_object.m_layer = WorldObject::LAYER_BASE;
	else if (layer == "Air")
		m_edited_fixed_object.m_layer = WorldObject::LAYER_AIR;
	else if (layer == "NoCollision")
		m_edited_fixed_object.m_layer = WorldObject::LAYER_NOCOLLISION;
	else
		m_edited_fixed_object.m_layer = WorldObject::LAYER_BASE | WorldObject::LAYER_AIR;
	
	m_modified_fixed_object = true;

	return false;
}

bool FixedObjectEditor::onFixedObjectAutodetectSize(const CEGUI::EventArgs& evt)
{
	const CEGUI::MouseEventArgs& we =
	static_cast<const CEGUI::MouseEventArgs&>(evt);
	
	CEGUI::WindowManager& win_mgr = CEGUI::WindowManager::getSingleton();
	
	CEGUI::PushButton* detectCircleButton = static_cast<CEGUI::PushButton*>(win_mgr.getWindow("FOTab/Prop/DetectCircleButton"));
	CEGUI::PushButton* detectRectButton = static_cast<CEGUI::PushButton*>(win_mgr.getWindow("FOTab/Prop/DetectRectButton"));
	
	CEGUI::Spinner* radiusSpinner =  static_cast<CEGUI::Spinner*>(win_mgr.getWindow("FOTab/Prop/RadiusSpinner"));
	CEGUI::Spinner* widthSpinner =  static_cast<CEGUI::Spinner*>(win_mgr.getWindow("FOTab/Prop/WidthSpinner"));
	CEGUI::Spinner* depthSpinner =  static_cast<CEGUI::Spinner*>(win_mgr.getWindow("FOTab/Prop/DepthSpinner"));
	
	// calculate the size of the bounding box
	RenderInfoEditor* ri_editor = dynamic_cast<RenderInfoEditor*>(ContentEditor::getSingleton().getComponent("RIEditor"));
	Ogre::SceneNode* topnode = ri_editor->getEditedGraphicObject()->getTopNode();
	topnode->_updateBounds();
	
	Ogre::Vector3 bbox_min(1000,1000,1000);
	Ogre::Vector3 bbox_max(-1000,-1000,-1000);
	
	getNodeBounds(topnode,bbox_min,bbox_max);
	
	double size_x = bbox_max[0] - bbox_min[0];
	double size_z = bbox_max[2] - bbox_min[2];
	
	// set spinner values according to boundingbox
	if (we.window == detectCircleButton)
	{
		radiusSpinner->setCurrentValue(0.5*sqrt(size_x*size_x + size_z*size_z));
	}
	else
	{
		widthSpinner->setCurrentValue(size_x);
		depthSpinner->setCurrentValue(size_z);
	}
	
	m_modified_fixed_object = true;
	return true;
}

bool FixedObjectEditor::onFixedObjectXMLModified(const CEGUI::EventArgs& evt)
{
	CEGUI::WindowManager& win_mgr = CEGUI::WindowManager::getSingleton();
	CEGUI::MultiLineEditbox* editor = static_cast<CEGUI::MultiLineEditbox*>(win_mgr.getWindow("FOTab/XML/FOXMLEditbox"));
	
	// Parse the editor text to XML
	// use temporary XML document for recovering from errors
	TiXmlDocument ri_temp_xml;
	ri_temp_xml.Parse(editor->getText().c_str());
	
	if (!ri_temp_xml.Error())
	{
		// first, remove the old root
		TiXmlElement* oldroot = m_fixed_object_xml.RootElement();
		if (oldroot != 0)
		{
			m_fixed_object_xml.RemoveChild(oldroot);
		}
		m_fixed_object_xml.LinkEndChild(ri_temp_xml.RootElement()->Clone());
		
		// parse the XML to the fixed object data
		ObjectLoader::loadObject(m_fixed_object_xml.FirstChildElement(),true);
		// copy to the local Data structure
		FixedObjectData* data = ObjectFactory::getFixedObjectData("EditorFixedObject");
		if (data != 0)
		{
			m_edited_fixed_object = *data;
		}
		
		m_modified_fixed_object_xml = true;
	}
	else
	{
		// XML parse error
		// set the cursor to the position of the first error
		int err_row = ri_temp_xml.ErrorRow();
		int err_col = ri_temp_xml.ErrorCol();
		
		setMultiLineEditboxCursor("FOTab/XML/FOXMLEditbox",err_row,err_col);
	}
	
	return true;
}

bool FixedObjectEditor::onFixedObjectCreate(const CEGUI::EventArgs& evt)
{
	// reparse and update the FixedObject Data
	
	// create a unique renderinfo (to avoid that the object is modified by the editor after creation)
	RenderInfoEditor* ri_editor = dynamic_cast<RenderInfoEditor*>(ContentEditor::getSingleton().getComponent("RIEditor"));
	std::string unique_ri = ri_editor->getUniqueRenderinfo();
	// temporarily replace the renderinfo name
	TiXmlElement * fixed_ri = m_fixed_object_xml.RootElement()->FirstChildElement("RenderInfo");
	if (fixed_ri == 0)
		return true;
	
	std::string name = fixed_ri->Attribute("name");
	fixed_ri->SetAttribute("name",unique_ri.c_str());
	
	ObjectLoader::loadObject(m_fixed_object_xml.FirstChildElement(),true);
	
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
	pos.m_x = getSpinnerValue("FOTab/Create/PosXSpinner",0);
	pos.m_y = getSpinnerValue("FOTab/Create/PosYSpinner",0);
	angle = getSpinnerValue("FOTab/Create/AngleSpinner",0);
	if (pos.m_x == 0 && pos.m_y == 0)
	{
		pos = player->getShape()->m_center;
	}
	
	// create the object
	int id = region->createObject("EditorFixedObject", pos,angle, height,WorldObject::STATE_ACTIVE);
	m_created_objects.push_back(std::make_pair(region->getId(), id));

	return false;
}

bool FixedObjectEditor::onCopyData(const CEGUI::EventArgs& evt)
{
	std::string objname	= getComboboxSelection("FOTab/Properties/CopyDataBox", "");
	if (objname == "")
		return true;
	
	FixedObjectData* fodata = ObjectFactory::getFixedObjectData(objname);
	if (fodata == 0)
		return true;
	
	m_edited_fixed_object = *fodata;
	
	m_modified_fixed_object = true;
	m_modified_fixed_object_xml = true;
	
	return true;
}

bool FixedObjectEditor::onGetPlayerPosition(const CEGUI::EventArgs& evt)
{
	World* world = World::getWorld();
	if (world == 0)
		return true;
	
	WorldObject* player = world->getLocalPlayer();
	if (player == 0)
		return true;
	Region* region = player->getRegion();
	
	Vector pos = player->getShape()->m_center;
	
	setSpinnerValue("FOTab/Create/PosXSpinner", pos.m_x);
	setSpinnerValue("FOTab/Create/PosYSpinner", pos.m_y);
	
	return true;
}

bool FixedObjectEditor::onDelAllObjects(const CEGUI::EventArgs& evt)
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
