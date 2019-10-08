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

#include "renderinfoeditor.h"

#include "graphicmanager.h"
#include "worldobject.h"
#include "objectloader.h"
#include "world.h"

#include <OgreHardwarePixelBuffer.h>

void RenderInfoEditor::init(CEGUI::Window* parent)
{
	ContentEditorTab::init(parent);
	
	m_edited_graphicobject=0;
	m_unique_id = 1;
	
	CEGUI::WindowManager& win_mgr = CEGUI::WindowManager::getSingleton(); 
	
	// add parts to the mesh selectors
	CEGUI::Combobox* selector = static_cast<CEGUI::Combobox*>(CEGUI::WindowManager::getSingleton().getWindow("RITab/BM/MeshSelector"));
	CEGUI::Combobox* subSelector = static_cast<CEGUI::Combobox*>(CEGUI::WindowManager::getSingleton().getWindow("RITab/SubMesh/Selector"));
	
	Ogre::FileInfoListPtr files;
	Ogre::FileInfoList::iterator it;
	std::string file;
	
	files = Ogre::ResourceGroupManager::getSingleton().findResourceFileInfo("General","*.mesh");
	
	std::list<std::string> filenames;
	for (it = files->begin(); it != files->end(); ++it)
	{
		filenames.push_back(it->filename);
	}
	filenames.sort();
	
	CEGUI::ListboxTextItem* listitem;
	for (std::list<std::string>::iterator it = filenames.begin(); it != filenames.end(); ++it)
	{
		try
		{
			file = *it;
			listitem = new CEGUI::ListboxTextItem(file);
			selector->addItem(listitem);
			listitem = new CEGUI::ListboxTextItem(file);
			subSelector->addItem(listitem);
		}
		catch (Ogre::Exception& e)
		{
			DEBUG("failed with exception %s",e.what());
		}
	}
	
	// wire the GUI
	selector->subscribeEvent(CEGUI::Combobox::EventListSelectionAccepted, CEGUI::Event::Subscriber(&RenderInfoEditor::onMeshSelected, this));
	subSelector->subscribeEvent(CEGUI::Combobox::EventListSelectionAccepted, CEGUI::Event::Subscriber(&RenderInfoEditor::onSubMeshSelected, this));
	
	CEGUI::PushButton* addSubmeshbutton = static_cast<CEGUI::PushButton*>(win_mgr.getWindow("RITab/SubMesh/AddSubMeshButton"));
	addSubmeshbutton->subscribeEvent(CEGUI::PushButton::EventClicked, CEGUI::Event::Subscriber(&RenderInfoEditor::onSubMeshAdded, this));
	
	CEGUI::PushButton* submitRIXMLbutton = static_cast<CEGUI::PushButton*>(win_mgr.getWindow("RITab/XML/SubmitButton"));
	submitRIXMLbutton->subscribeEvent(CEGUI::PushButton::EventClicked, CEGUI::Event::Subscriber(&RenderInfoEditor::onRenderinfoXMLModified, this));
	
	CEGUI::PushButton* delSubmeshbutton = static_cast<CEGUI::PushButton*>(win_mgr.getWindow("RITab/SubMesh/DelSubMeshButton"));
	delSubmeshbutton->subscribeEvent(CEGUI::PushButton::EventClicked, CEGUI::Event::Subscriber(&RenderInfoEditor::onSubMeshDeleted, this));
	
	
	CEGUI::Spinner* rotXspinner =  static_cast<CEGUI::Spinner*>(win_mgr.getWindow("RITab/SM/SMRotateX"));
	CEGUI::Spinner* rotYspinner =  static_cast<CEGUI::Spinner*>(win_mgr.getWindow("RITab/SM/SMRotateY"));
	CEGUI::Spinner* rotZspinner =  static_cast<CEGUI::Spinner*>(win_mgr.getWindow("RITab/SM/SMRotateZ"));
	
	CEGUI::Spinner* posXspinner =  static_cast<CEGUI::Spinner*>(win_mgr.getWindow("RITab/SM/SMOffsetX"));
	CEGUI::Spinner* posYspinner =  static_cast<CEGUI::Spinner*>(win_mgr.getWindow("RITab/SM/SMOffsetY"));
	CEGUI::Spinner* posZspinner =  static_cast<CEGUI::Spinner*>(win_mgr.getWindow("RITab/SM/SMOffsetZ"));
	
	CEGUI::Spinner* scalespinner =  static_cast<CEGUI::Spinner*>(win_mgr.getWindow("RITab/SM/SMScale"));
	
	CEGUI::Combobox* objSelector = static_cast<CEGUI::Combobox*>(win_mgr.getWindow("RITab/SubMesh/EditSMSelector"));
	CEGUI::Combobox* boneobjSelector = static_cast<CEGUI::Combobox*>(win_mgr.getWindow("RITab/SubMesh/AttachMeshSelector"));
	CEGUI::Combobox* boneSelector = static_cast<CEGUI::Combobox*>(win_mgr.getWindow("RITab/SubMesh/BoneSelector"));
	CEGUI::Checkbox* attachCheckbox = static_cast<CEGUI::Checkbox*>(win_mgr.getWindow("RITab/SM/AttachSMCheckbox"));
	
	
	objSelector->subscribeEvent(CEGUI::Combobox::EventListSelectionAccepted, CEGUI::Event::Subscriber(&RenderInfoEditor::onSubObjectSelected, this));
	
	rotXspinner->subscribeEvent(CEGUI::Spinner::EventValueChanged, CEGUI::Event::Subscriber(&RenderInfoEditor::onSubMeshModified, this));
	rotYspinner->subscribeEvent(CEGUI::Spinner::EventValueChanged, CEGUI::Event::Subscriber(&RenderInfoEditor::onSubMeshModified, this));
	rotZspinner->subscribeEvent(CEGUI::Spinner::EventValueChanged, CEGUI::Event::Subscriber(&RenderInfoEditor::onSubMeshModified, this));
	
	posXspinner->subscribeEvent(CEGUI::Spinner::EventValueChanged, CEGUI::Event::Subscriber(&RenderInfoEditor::onSubMeshModified, this));
	posYspinner->subscribeEvent(CEGUI::Spinner::EventValueChanged, CEGUI::Event::Subscriber(&RenderInfoEditor::onSubMeshModified, this));
	posZspinner->subscribeEvent(CEGUI::Spinner::EventValueChanged, CEGUI::Event::Subscriber(&RenderInfoEditor::onSubMeshModified, this));
	
	scalespinner->subscribeEvent(CEGUI::Spinner::EventValueChanged, CEGUI::Event::Subscriber(&RenderInfoEditor::onSubMeshModified, this));
	
	boneobjSelector->subscribeEvent(CEGUI::Combobox::EventListSelectionAccepted, CEGUI::Event::Subscriber(&RenderInfoEditor::onSubMeshModified, this));
	boneSelector->subscribeEvent(CEGUI::Combobox::EventListSelectionAccepted, CEGUI::Event::Subscriber(&RenderInfoEditor::onSubMeshModified, this));
	attachCheckbox->subscribeEvent(CEGUI::Checkbox::EventCheckStateChanged, CEGUI::Event::Subscriber(&RenderInfoEditor::onSubMeshModified, this));
	
	// init the internal data
	TiXmlElement * renderinfo_root = new TiXmlElement("RenderInfo");  
	m_renderinfo_xml.LinkEndChild( renderinfo_root );  
	renderinfo_root->SetAttribute("name","EditorRenderInfo");
	
}

void RenderInfoEditor::update()
{
	if (m_modified_renderinfo)
	{
		updateRenderInfoXML();
		updatePreviewImage();
	}
	
	if (m_modified_renderinfo_xml)
	{
		updateSubmeshEditor();
		updatePreviewImage();
	}
	
	m_modified_renderinfo_xml = false;
	m_modified_renderinfo = false;
}

void checkBounds(const Ogre::SceneNode* node, int level=0)
{
	const Ogre::AxisAlignedBox& boundingbox = node->_getWorldAABB();
	
	// current node boundss
	Ogre::Vector3 bbox_min = boundingbox.getMinimum();
	Ogre::Vector3 bbox_max = boundingbox.getMaximum();
	
	std::string indent = "";
	for (int i=0; i<level; i++)
		indent += "  ";
	std::cout << indent << "Node: " << node->getName() << "\n";
	std::cout << indent << bbox_min[0] << " " << bbox_min[1] << " " <<  bbox_min[2] << "\n";
	std::cout << indent << bbox_max[0] << " " << bbox_max[1] << " " <<  bbox_max[2] << "\n";
	
	// SubNodes
	Ogre::SceneNode::ConstChildNodeIterator child_it = node->getChildIterator();
	while (child_it.hasMoreElements())
	{
		const Ogre::SceneNode* subnode = dynamic_cast<Ogre::SceneNode*>(child_it.getNext());
		if (subnode != 0)
		{
			checkBounds(subnode,level+1);
		}
	}
	
	// Entities
	indent += "  ";
	Ogre::SceneNode::ConstObjectIterator attch_obj_it = node->getAttachedObjectIterator();
	while (attch_obj_it.hasMoreElements())
	{
		const Ogre::Entity* ent = dynamic_cast<Ogre::Entity*>(attch_obj_it.getNext());
		if (ent != 0)
		{
			const Ogre::AxisAlignedBox  & box = ent->getMesh()->getBounds();
			Ogre::Vector3 box_min = box.getMinimum();
			Ogre::Vector3 box_max = box.getMaximum();
			
			std::cout << indent << "Entity: " << ent->getName() << "\n";
			std::cout << indent << box_min[0] << " " << box_min[1] << " " <<  box_min[2] << "\n";
			std::cout << indent << box_max[0] << " " << box_max[1] << " " <<  box_max[2] << "\n";
		}
	}
	
}




void RenderInfoEditor::updatePreviewImage()
{
	Ogre::SceneManager* old_scene_mng = GraphicManager::getSceneManager();
	Ogre::SceneManager* editor_scene_mng = Ogre::Root::getSingleton().getSceneManager("EditorSceneManager");
	GraphicManager::setSceneManager(editor_scene_mng);
	
	// reparse and update the renderinfo
	GraphicRenderInfo* edited_renderinfo = new GraphicRenderInfo;
	GraphicManager::loadRenderInfo(m_renderinfo_xml.FirstChildElement(), edited_renderinfo);
	GraphicManager::registerRenderInfo("EditorRenderInfo", edited_renderinfo, true);
	
	// if not present: create the graphicObject
	if (m_edited_graphicobject == 0)
	{
		// the ID is just arbitrary...
		m_edited_graphicobject = GraphicManager::createGraphicObject("EditorRenderInfo", "EditedGraphicObject", 123456789);
	}
	
	Ogre::MeshManager::getSingleton().setBoundsPaddingFactor(0.0f);
	m_edited_graphicobject->getTopNode()->setPosition(0.0,0.0,0.0);
	m_edited_graphicobject->update(0);
	
	// update the camera to show the full object
	// first, the subtree needs to be updated
	Ogre::SceneNode* topnode = m_edited_graphicobject->getTopNode();
	topnode->_update(true,true);
	topnode->_updateBounds();
	topnode->showBoundingBox(true);
	const Ogre::AxisAlignedBox& boundingbox = topnode->_getWorldAABB();
	
	Ogre::Vector3 bbox_min(1000,1000,1000);
	Ogre::Vector3 bbox_max(-1000,-1000,-1000);
	
	getNodeBounds(topnode,bbox_min,bbox_max);
	
	// camera is placed looking along negative X axis, with Y and Z offset
	double center_y = 0.5*(bbox_max[1] + bbox_min[1]);
	double center_z = 0.5*(bbox_max[2] + bbox_min[2]);
	double size_y = bbox_max[1] - bbox_min[1];
	double size_z = bbox_max[2] - bbox_min[2];
	
	
	double viewsize = MathHelper::Max(size_y, size_z);
	
	Ogre::Camera* editor_camera = editor_scene_mng->getCamera("editor_camera");
	editor_camera->setPosition(Ogre::Vector3(bbox_max[0] + viewsize*sqrt(double(2)) , center_y,center_z));
	editor_camera->lookAt(Ogre::Vector3(bbox_max[0], center_y,center_z));
	
	// update the texture
	Ogre::Resource* res= Ogre::TextureManager::getSingleton().createOrRetrieve ("editor_tex",Ogre::ResourceGroupManager::DEFAULT_RESOURCE_GROUP_NAME).first.getPointer();
	Ogre::Texture* texture = dynamic_cast<Ogre::Texture*>(res);
	Ogre::RenderTarget* target = texture->getBuffer()->getRenderTarget();
	target->update();
	
	GraphicManager::setSceneManager(old_scene_mng);
}

void RenderInfoEditor::updateSubmeshEditor(std::string objectname, bool updateList)
{
	CEGUI::WindowManager& win_mgr = CEGUI::WindowManager::getSingleton();
	
	CEGUI::Spinner* rotXspinner =  static_cast<CEGUI::Spinner*>(win_mgr.getWindow("RITab/SM/SMRotateX"));
	CEGUI::Spinner* rotYspinner =  static_cast<CEGUI::Spinner*>(win_mgr.getWindow("RITab/SM/SMRotateY"));
	CEGUI::Spinner* rotZspinner =  static_cast<CEGUI::Spinner*>(win_mgr.getWindow("RITab/SM/SMRotateZ"));
	
	CEGUI::Spinner* posXspinner =  static_cast<CEGUI::Spinner*>(win_mgr.getWindow("RITab/SM/SMOffsetX"));
	CEGUI::Spinner* posYspinner =  static_cast<CEGUI::Spinner*>(win_mgr.getWindow("RITab/SM/SMOffsetY"));
	CEGUI::Spinner* posZspinner =  static_cast<CEGUI::Spinner*>(win_mgr.getWindow("RITab/SM/SMOffsetZ"));
	
	CEGUI::Spinner* scalespinner =  static_cast<CEGUI::Spinner*>(win_mgr.getWindow("RITab/SM/SMScale"));
	
	CEGUI::Combobox* objSelector = static_cast<CEGUI::Combobox*>(win_mgr.getWindow("RITab/SubMesh/EditSMSelector"));
	CEGUI::Combobox* boneobjSelector = static_cast<CEGUI::Combobox*>(win_mgr.getWindow("RITab/SubMesh/AttachMeshSelector"));
	CEGUI::Combobox* boneSelector = static_cast<CEGUI::Combobox*>(win_mgr.getWindow("RITab/SubMesh/BoneSelector"));
	CEGUI::Checkbox* attachCheckbox = static_cast<CEGUI::Checkbox*>(win_mgr.getWindow("RITab/SM/AttachSMCheckbox"));
	
	if (objectname == "")
		objectname = objSelector->getText().c_str();
	
	// update the content of the objectSelector
	if (updateList)
	{
		bool objectfound = false;
		bool boneobjfound = false;
		
		// preserve selected boneobj if possible (store the name)
		CEGUI::ListboxItem* boneobjitem = boneobjSelector->getSelectedItem();
		std::string boneobj = "mainmesh";
		if (boneobjitem != 0)
			boneobj = boneobjitem->getText().c_str();
		
		objSelector->resetList();
		boneobjSelector->resetList();
		objSelector->setText("");
		boneobjSelector->setText("");
		
		
		std::list<MovableObjectInfo>& objects = m_edited_renderinfo.getObjects();
		CEGUI::ListboxTextItem* listitem;
		//CEGUI::ListboxTextItem* selecteditem; // 2011.10.28: found as unused
		for (std::list<MovableObjectInfo>::iterator it = objects.begin(); it != objects.end(); ++it)
		{
			if (it->m_type != MovableObjectInfo::ENTITY)
				continue;
			
			listitem = new CEGUI::ListboxTextItem(it->m_objectname);
			objSelector->addItem(listitem);
			
			// automatically select the right item
			if (it->m_objectname == objectname)
			{
				objSelector->setItemSelectState(listitem,true);
				objSelector->setText(objectname);
				objectfound = true;
			}
			
			// also update the list of objects to attach to
			// do not list the object itself
			if (objectname != it->m_objectname)
			{
				listitem = new CEGUI::ListboxTextItem(it->m_objectname);
				boneobjSelector->addItem(listitem);
				
				// automatically select the right object
				if (boneobj == it->m_objectname)
				{
					boneobjSelector->setItemSelectState(listitem,true);
					objSelector->setText(boneobj);
					boneobjfound = true;
				}
			}
		}
		
		// if the whole object is missing, clear all attach selections
		if (!objectfound)
		{
			boneobjSelector->clearAllSelections();
			boneobjSelector->setText("");
			boneSelector->clearAllSelections();
			boneSelector->setText("");
		}
		
		// if just the object to attach to is missing, clear the bone selection
		if (!boneobjfound)
		{
			boneSelector->clearAllSelections();
			boneSelector->setText("");
		}
	}
	
	m_no_cegui_events = true;
	
	// get the selected item and the underlying RenderInfo data structure
	CEGUI::ListboxItem* item = objSelector->getSelectedItem();
	MovableObjectInfo* minfo = 0;
	if (item != 0)
		minfo = m_edited_renderinfo.getObject(objectname);
	
	if (minfo == 0)
	{
		objSelector->clearAllSelections();
		objSelector->setText("");
		boneobjSelector->clearAllSelections();
		boneobjSelector->setText("");
		boneSelector->clearAllSelections();
		boneSelector->setText("");
		return;
	}
	
	// update the list of bones if required
	updateBoneList();
	
	// now update all the GUI elements
	// bone and *attached to*
	attachCheckbox->setSelected(minfo->m_bone != "");
	std::string bone;
	std::string boneobj ="";
	int pos = minfo->m_bone.find(':');
	if (pos == -1)
	{
		bone = minfo->m_bone;
		if (bone != "")
		{
			boneobj = "mainmesh";
		}
	}
	else
	{
		boneobj = minfo->m_bone.substr(0,pos);
		bone = minfo->m_bone.substr(pos+1);
	}
	
	// set the selectors of the mesh attached to and the bone to the right state
	if (boneobj != "" && bone != "")
	{
		boneobjSelector->clearAllSelections();
		boneSelector->clearAllSelections();
		boneobjSelector->setText("");
		boneSelector->setText("");
		
		for (size_t i=0; i<boneobjSelector->getItemCount(); i++)
		{
			if (boneobjSelector->getListboxItemFromIndex(i)->getText() == boneobj)
			{
				boneobjSelector->setSelection(i,i);
				boneobjSelector->setText(boneobj);
			}
		}
		for (size_t i=0; i<boneSelector->getItemCount(); i++)
		{
			if (boneSelector->getListboxItemFromIndex(i)->getText() == bone)
			{
				boneSelector->setSelection(i,i);
				boneSelector->setText(bone);
			}
		}
	}
	else
	{
		boneobjSelector->clearAllSelections();
		boneSelector->clearAllSelections();
	}
	
	// set the rotation spinners
	rotXspinner->setCurrentValue(minfo->m_rotation[0]);
	rotYspinner->setCurrentValue(minfo->m_rotation[1]);
	rotZspinner->setCurrentValue(minfo->m_rotation[2]);
	
	// set the position spinners
	posXspinner->setCurrentValue(minfo->m_position[0]);
	posYspinner->setCurrentValue(minfo->m_position[1]);
	posZspinner->setCurrentValue(minfo->m_position[2]);
	
	// set scale spinner
	scalespinner->setCurrentValue(minfo->m_scale);
	
	m_no_cegui_events = false;
}

void RenderInfoEditor::updateBoneList()
{
	CEGUI::WindowManager& win_mgr = CEGUI::WindowManager::getSingleton();
	CEGUI::Combobox* boneSelector = static_cast<CEGUI::Combobox*>(win_mgr.getWindow("RITab/SubMesh/BoneSelector"));
	CEGUI::Combobox* boneobjSelector = static_cast<CEGUI::Combobox*>(win_mgr.getWindow("RITab/SubMesh/AttachMeshSelector"));
	
	// get the selected object and the underlying RenderInfo data structure
	std::string boneobj = boneobjSelector->getText().c_str();
	
	MovableObjectInfo* minfo = m_edited_renderinfo.getObject(boneobj);
	
	if (minfo == 0)
	{
		boneSelector->resetList();
		return;
	}
	
	
	std::string bonesel = boneSelector->getText().c_str();
	
	boneSelector->resetList();
	boneSelector->setText("");
	
	if (minfo->m_type == MovableObjectInfo::ENTITY)
	{
		//get the skeleton of the selected mesh and fill the bone selector combo box
		Ogre::Mesh* mesh = dynamic_cast<Ogre::Mesh*>(Ogre::MeshManager::getSingleton().createOrRetrieve(minfo->m_source,"General").first.getPointer());
		
		Ogre::Skeleton *skel = 0;
		
		if (mesh != 0)
		{
			skel = mesh->getSkeleton().getPointer();
		}
		
		if(skel!=0)
		{
			// iterate the skeleton to get the bones
			Ogre::Skeleton::BoneIterator bit = skel->getBoneIterator();
			std::set<std::string> boneNames;
			
			while (bit.hasMoreElements())
			{
				Ogre::Bone *bone = bit.getNext();
				boneNames.insert(bone->getName());
			}
			
			// add the bones to the combo box
			std::string bone;
			CEGUI::ListboxTextItem* item;
			for (std::set<std::string>::iterator it = boneNames.begin(); it != boneNames.end(); ++it)
			{
				
				bone = *it;
				item = new CEGUI::ListboxTextItem(bone);
				boneSelector->addItem(item);
				// automatically select the right object
				if (bonesel == bone)
				{
					boneSelector->setItemSelectState(item,true);
					boneSelector->setText(bone);
				}
			}
		}
	}
}

void RenderInfoEditor::updateRenderInfoXML()
{
	CEGUI::WindowManager& win_mgr = CEGUI::WindowManager::getSingleton();
	
	// update the XML representation
	m_edited_renderinfo.writeToXML(m_renderinfo_xml.FirstChildElement());
	
	// write to the editor
	TiXmlPrinter printer;
	m_renderinfo_xml.Accept(&printer);
	
	CEGUI::MultiLineEditbox* editor = static_cast<CEGUI::MultiLineEditbox*>(win_mgr.getWindow("RITab/XML/RIXMLEditbox"));
	editor->setText(printer.CStr());
}


bool RenderInfoEditor::onMeshSelected(const CEGUI::EventArgs& evt)
{
	
	
	const CEGUI::MouseEventArgs& we =
	static_cast<const CEGUI::MouseEventArgs&>(evt);
	
	CEGUI::Combobox* cbo = static_cast<CEGUI::Combobox*>(we.window);
	
	CEGUI::ListboxItem* item = cbo->getSelectedItem();
	
	if (item != 0)
	{
		// place the required mesh in the editor scene
		std::string meshname = item->getText().c_str();
		DEBUGX("selected mesh %s",meshname.c_str());
		
		// if the Renderinfo already has a main mesh, edit it
		// otherwise, create it
		MovableObjectInfo* minfo = m_edited_renderinfo.getObject("mainmesh");
		MovableObjectInfo newminfo;
		bool addnew = false;
		
		if (minfo == 0)
		{
			// no mainmesh yet
			minfo = &newminfo;
			addnew = true;
		}
		
		// edit the Object
		minfo->m_type = MovableObjectInfo::ENTITY;
		minfo->m_objectname = "mainmesh";
		minfo->m_source = meshname;
		
		if (addnew)
		{
			m_edited_renderinfo.addObject(*minfo);
		}
		
		m_modified_renderinfo = true;
	}
	
	return true;
}


bool RenderInfoEditor::onSubObjectSelected(const CEGUI::EventArgs& evt)
{
	const CEGUI::MouseEventArgs& we =
	static_cast<const CEGUI::MouseEventArgs&>(evt);
	
	CEGUI::Combobox* cbo = static_cast<CEGUI::Combobox*>(we.window);
	
	CEGUI::ListboxItem* item = cbo->getSelectedItem();
	
	if (item != 0)
	{
		// place the required mesh in the editor scene
		std::string meshname = item->getText().c_str();
		DEBUG("selected mesh %s",meshname.c_str());
		
		updateSubmeshEditor(meshname, false);
	}
	
	return true;
}

bool RenderInfoEditor::onSubMeshSelected(const CEGUI::EventArgs& evt)
{
	CEGUI::WindowManager& win_mgr = CEGUI::WindowManager::getSingleton();
	CEGUI::Combobox* subSelector = static_cast<CEGUI::Combobox*>(CEGUI::WindowManager::getSingleton().getWindow("RITab/SubMesh/Selector"));
	CEGUI::Editbox* subMeshNameBox = static_cast<CEGUI::Editbox*>(CEGUI::WindowManager::getSingleton().getWindow("RITab/SM/SMNameEditbox"));
	
	CEGUI::ListboxItem* item = subSelector->getSelectedItem();
	if (item != 0)
	{
		std::string meshname = item->getText().c_str();
		
		// crop the .mesh...
		size_t pos = meshname.find_first_of('.');
		if (pos != std::string::npos)
		{
			meshname.erase(pos);
		}
		subMeshNameBox->setText(meshname);
	}
	return true;
}

bool RenderInfoEditor::onSubMeshAdded(const CEGUI::EventArgs& evt)
{
	CEGUI::WindowManager& win_mgr = CEGUI::WindowManager::getSingleton();
	CEGUI::Combobox* subSelector = static_cast<CEGUI::Combobox*>(CEGUI::WindowManager::getSingleton().getWindow("RITab/SubMesh/Selector"));
	CEGUI::Editbox* subMeshNameBox = static_cast<CEGUI::Editbox*>(CEGUI::WindowManager::getSingleton().getWindow("RITab/SM/SMNameEditbox"));
	
	CEGUI::ListboxItem* item = subSelector->getSelectedItem();
	if (item != 0)
	{
		std::string meshname = item->getText().c_str();
		std::string objectname = subMeshNameBox->getText().c_str();
		
		if (objectname == "")
		{
			// crop the .mesh...
			objectname = meshname;
			size_t pos = objectname.find_first_of('.');
			if (pos != std::string::npos)
			{
				objectname.erase(pos);
			}
		}
		
		// if the Renderinfo already has a an object with this name, edit it
		// otherwise, create it
		MovableObjectInfo* minfo = m_edited_renderinfo.getObject(objectname);
		MovableObjectInfo newminfo;
		bool addnew = false;
		
		if (minfo == 0)
		{
			// no mainmesh yet
			minfo = &newminfo;
			addnew = true;
		}
		
		// edit the Object
		minfo->m_type = MovableObjectInfo::ENTITY;
		minfo->m_objectname = objectname;
		minfo->m_source = meshname;
		
		if (addnew)
		{
			m_edited_renderinfo.addObject(*minfo);
		}
		m_modified_renderinfo = true;
		
		updateSubmeshEditor(objectname, true);
	}
	return true;
}

bool RenderInfoEditor::onSubMeshModified(const CEGUI::EventArgs& evt)
{
	if (m_no_cegui_events)
		return true;
	
	CEGUI::WindowManager& win_mgr = CEGUI::WindowManager::getSingleton();
	
	// the the GUI parts
	CEGUI::Spinner* rotXspinner =  static_cast<CEGUI::Spinner*>(win_mgr.getWindow("RITab/SM/SMRotateX"));
	CEGUI::Spinner* rotYspinner =  static_cast<CEGUI::Spinner*>(win_mgr.getWindow("RITab/SM/SMRotateY"));
	CEGUI::Spinner* rotZspinner =  static_cast<CEGUI::Spinner*>(win_mgr.getWindow("RITab/SM/SMRotateZ"));
	
	CEGUI::Spinner* posXspinner =  static_cast<CEGUI::Spinner*>(win_mgr.getWindow("RITab/SM/SMOffsetX"));
	CEGUI::Spinner* posYspinner =  static_cast<CEGUI::Spinner*>(win_mgr.getWindow("RITab/SM/SMOffsetY"));
	CEGUI::Spinner* posZspinner =  static_cast<CEGUI::Spinner*>(win_mgr.getWindow("RITab/SM/SMOffsetZ"));
	
	CEGUI::Spinner* scalespinner =  static_cast<CEGUI::Spinner*>(win_mgr.getWindow("RITab/SM/SMScale"));
	
	CEGUI::Combobox* objSelector = static_cast<CEGUI::Combobox*>(win_mgr.getWindow("RITab/SubMesh/EditSMSelector"));
	CEGUI::Combobox* boneobjSelector = static_cast<CEGUI::Combobox*>(win_mgr.getWindow("RITab/SubMesh/AttachMeshSelector"));
	CEGUI::Combobox* boneSelector = static_cast<CEGUI::Combobox*>(win_mgr.getWindow("RITab/SubMesh/BoneSelector"));
	CEGUI::Checkbox* attachCheckbox = static_cast<CEGUI::Checkbox*>(win_mgr.getWindow("RITab/SM/AttachSMCheckbox"));
	
	// get the MovableObjectInfo
	CEGUI::ListboxItem* item = objSelector->getSelectedItem();
	if (item == 0)
		return true;
	
	MovableObjectInfo* minfo = m_edited_renderinfo.getObject(item->getText().c_str());
	if (minfo == 0)
		return true;
	
	// update MovableObjectInfo data
	updateBoneList();
	
	// the bone
	minfo->m_bone = "";
	if (attachCheckbox->isSelected())
	{
		std::string objname="";
		std::string bone="";
		CEGUI::ListboxItem* boneobjitem = boneobjSelector->getSelectedItem();
		if (boneobjitem != 0)
		{
			objname = boneobjitem->getText().c_str();
		}
		CEGUI::ListboxItem* boneitem = boneSelector->getSelectedItem();
		if (boneitem != 0)
		{
			bone = boneitem->getText().c_str();
		}
		
		if (objname != "" && objname != "mainmesh" && bone != "")
		{
			minfo->m_bone += objname;
			minfo->m_bone += ":";
		}
		minfo->m_bone += bone;
	}
	
	// rotation
	minfo->m_rotation[0] = rotXspinner->getCurrentValue();
	minfo->m_rotation[1] = rotYspinner->getCurrentValue();
	minfo->m_rotation[2] = rotZspinner->getCurrentValue();
	
	// position
	minfo->m_position[0] = posXspinner->getCurrentValue();
	minfo->m_position[1] = posYspinner->getCurrentValue();
	minfo->m_position[2] = posZspinner->getCurrentValue();
	
	// scale
	minfo->m_scale = scalespinner->getCurrentValue();
	
	m_modified_renderinfo = true;
	
	return true;
}

bool RenderInfoEditor::onSubMeshDeleted(const CEGUI::EventArgs& evt)
{
	if (m_no_cegui_events)
		return true;
	
	CEGUI::WindowManager& win_mgr = CEGUI::WindowManager::getSingleton();
	CEGUI::Combobox* objSelector = static_cast<CEGUI::Combobox*>(win_mgr.getWindow("RITab/SubMesh/EditSMSelector"));
	
	// get the MovableObjectInfo
	CEGUI::ListboxItem* item = objSelector->getSelectedItem();
	if (item == 0)
		return true;
	
	MovableObjectInfo* minfo = m_edited_renderinfo.getObject(item->getText().c_str());
	if (minfo == 0)
		return true;
	
	// Remove it from the renderinfo
	m_edited_renderinfo.removeObject(minfo->m_objectname);
	
	m_modified_renderinfo = true;
	return true;
}

bool RenderInfoEditor::onRenderinfoXMLModified(const CEGUI::EventArgs& evt)
{
	CEGUI::WindowManager& win_mgr = CEGUI::WindowManager::getSingleton();
	CEGUI::MultiLineEditbox* editor = static_cast<CEGUI::MultiLineEditbox*>(win_mgr.getWindow("RITab/XML/RIXMLEditbox"));
	
	// Parse the editor text to XML
	// use temporary XML document for recovering from errors
	TiXmlDocument ri_temp_xml;
	ri_temp_xml.Parse(editor->getText().c_str());
	
	if (!ri_temp_xml.Error())
	{
		// copy the first Element to the real Renderinfo
		// first, remove the old root
		TiXmlElement* oldroot = m_renderinfo_xml.RootElement();
		if (oldroot != 0)
		{
			m_renderinfo_xml.RemoveChild(oldroot);
		}
		m_renderinfo_xml.LinkEndChild(ri_temp_xml.RootElement()->Clone());
		
		// parse the XML to the renderinfo
		// firs,t clear the RenderInfo
		m_edited_renderinfo.clear();
		GraphicManager::loadRenderInfo(m_renderinfo_xml.FirstChildElement(), &m_edited_renderinfo);
		
		m_modified_renderinfo_xml = true;
	}
	else
	{
		// XML parse error
		// set the cursor to the position of the first error
		int err_row = ri_temp_xml.ErrorRow();
		int err_col = ri_temp_xml.ErrorCol();
		
		int row =1, col = 1;	
		size_t pos = 0;	// cursor position found
		const CEGUI::String& text = editor->getText();
		while (pos < text.size())
		{
			// second condition ensures, that cursor is placed on the end,
			// if the row err_row is shorter than err_col for some reason
			if ((row == err_row && col == err_col)
				|| (row > err_row))
			{
				break;
			}
			
			if (text.compare(pos,1,"\n") == 0)
			{
				row++;
				col = 1;
			}
			else
			{
				col ++;
			}
			pos++;
		}
		
		editor->setCaratIndex(pos);
		editor->ensureCaratIsVisible();
		editor->activate();
	}
	return true;
}

std::string RenderInfoEditor::getUniqueRenderinfo()
{
	GraphicRenderInfo* edited_renderinfo = new GraphicRenderInfo;
	GraphicManager::loadRenderInfo(m_renderinfo_xml.FirstChildElement(), edited_renderinfo);
	
	std::stringstream stream;
	stream << "EditorRenderInfo_" << m_unique_id;
	m_unique_id++;
	GraphicManager::registerRenderInfo(stream.str(), edited_renderinfo, true);
	
	return stream.str();
}