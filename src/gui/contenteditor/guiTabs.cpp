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
// TODO(Augustin Preda, 2014.03.29): This entire file seems to be unused.
// Verify if needed for anything and (if not used) remove it.
//

#include "guitabs.h"

#include "Ogre.h"
#include "OgrePlatformInformation.h"
#include "CEGUI/RendererModules/Ogre/CEGUIOgreRenderer.h"

#include "Poco/Environment.h"

#include "CEGUIWindowManager.h"
#include "CEGUI.h"

#include <eventsystem.h>
#include <contenteditor.h>

#include "contenttab.h"
#include "debug.h"

using namespace CEGUI;
using Poco::Environment;

CEGUI::String GuiTabs::WidgetTypeName = "GuiTabs";

GuiTabs::GuiTabs(const CEGUI::String& type, const CEGUI::String& name): CEGUI::Window(type, name), ContentTab(), Ogre::LogListener()
{
	//m_tabLayout->getChild("windowUnderMouseStaticText")->setText(win->getName());
	m_tabLayout = CEGUI::WindowManager::getSingleton().loadWindowLayout("contenteditor.layout");
	m_tabLayout->setPosition(UVector2(UDim(0.0f, 0.0f), UDim(0.0f, 0.0f)));
	m_tabLayout->setSize(UVector2(UDim(1.0f, 0.0f), UDim(1.0f, 0.0f)));
	this->addChildWindow(m_tabLayout);

	WindowManager::getSingleton().getWindow("RITab/BasicMesh")->setVisible(true);

	initMeshSelector();
	WindowManager::getSingleton().getWindow("CloseButton")->subscribeEvent(PushButton::EventClicked, CEGUI::Event::Subscriber(&GuiTabs::onCloseButton, this));
	//WindowManager::getSingleton().getWindow("RITab/SubMesh/AddSubMeshButton")->subscribeEvent(PushButton::EventClicked, CEGUI::Event::Subscriber(&GuiTabs::onAddSubMesh, this));
}

void GuiTabs::initMeshSelector()
{
	CEGUI::WindowManager& win_mgr = CEGUI::WindowManager::getSingleton();
	//get the mainmesh selector and the submesh selector combobox and fill it with the names of all meshes
	Combobox* selector = static_cast<Combobox*>(WindowManager::getSingleton().getWindow("RITab/BM/MeshSelector"));
	Combobox* subSelector = static_cast<Combobox*>(WindowManager::getSingleton().getWindow("RITab/SubMesh/Selector"));

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
	
	for (std::list<std::string>::iterator it = filenames.begin(); it != filenames.end(); ++it)
	{
		try
		{
			file = *it;
			m_listItem = new ListboxTextItem(file);
			m_subMeshListItem = new ListboxTextItem(file);
			selector->addItem(m_listItem);
			subSelector->addItem(m_subMeshListItem);
		}
		catch (Ogre::Exception& e)
		{
			DEBUG("failed with exception %s",e.what());
		}
	}
	
	selector->subscribeEvent(CEGUI::Combobox::EventListSelectionAccepted, CEGUI::Event::Subscriber(&GuiTabs::onMeshSelected, this));
	subSelector->subscribeEvent(CEGUI::Combobox::EventListSelectionAccepted, CEGUI::Event::Subscriber(&GuiTabs::onSubMeshSelected, this));
	
	// create SceneManager for renderering images for the content editor
	Ogre::SceneManager* editor_scene_mng = Ogre::Root::getSingleton().createSceneManager(Ogre::ST_GENERIC,"EditorSceneManager");
	editor_scene_mng->setAmbientLight(Ogre::ColourValue(1,1,1));
	
	// camera filming the content of the the editor scene
	Ogre::Camera* editor_camera = editor_scene_mng->createCamera("editor_camera");
	editor_camera->setPosition(Ogre::Vector3(3, 1.2,0));
	editor_camera->lookAt(Ogre::Vector3(0,1.2,0));
	editor_camera->setNearClipDistance(0.1);
	editor_camera->setAspectRatio(1.0);
	Ogre::SceneNode *editorNode = editor_scene_mng->getRootSceneNode()->createChildSceneNode("EditorCameraNode");
	editorNode->attachObject(editor_camera);
	
	// texture that is creates from the camera image
	Ogre::TexturePtr editor_texture = Ogre::TextureManager::getSingleton().createManual( "editor_tex",
			Ogre::ResourceGroupManager::DEFAULT_RESOURCE_GROUP_NAME, Ogre::TEX_TYPE_2D,
   512, 512, 0, Ogre::PF_R8G8B8A8, Ogre::TU_RENDERTARGET );

   // connection texture and camera via RenderTarget
   Ogre::RenderTarget* editor_rt = editor_texture->getBuffer()->getRenderTarget();
	editor_rt ->setAutoUpdated(false);
	Ogre::Viewport *editor_view = editor_rt->addViewport(editor_camera );
	editor_view->setClearEveryFrame( true );
	editor_view->setOverlaysEnabled (false);
	editor_view->setBackgroundColour(Ogre::ColourValue(0,0,0,1.0) );
	editor_rt->update();

	// create a CEGUI Image from the Texture
    CEGUI::Texture& editor_ceguiTex = static_cast<CEGUI::OgreRenderer*>(CEGUI::System::getSingleton().getRenderer())->createTexture(editor_texture);
    
	CEGUI::Imageset& editor_textureImageSet = CEGUI::ImagesetManager::getSingleton().create("editor_imageset", editor_ceguiTex);

	editor_textureImageSet.defineImage( "editor_img",
			CEGUI::Point( 0.0f, 0.0f ),
			CEGUI::Size( editor_ceguiTex.getSize().d_width, editor_ceguiTex.getSize().d_height ),
			CEGUI::Point( 0.0f, 0.0f ) );
	
	// place the image in a the CEGUI label
	CEGUI::Window* label = win_mgr.getWindow("RITab/BM/meshPreview");
	label->setProperty("Image", "set:editor_imageset image:editor_img"); 
}

bool GuiTabs::onMeshSelected(const CEGUI::EventArgs& evt)
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
		Ogre::SceneManager* editor_scene_mng = Ogre::Root::getSingleton().getSceneManager("EditorSceneManager");
		
		// radical solutions: clear the scene
		editor_scene_mng->clearScene();	
		
		Ogre::Entity* entity;
		try
		{
			entity = editor_scene_mng->createEntity(meshname, meshname);
			Ogre::SceneNode *entityNode = editor_scene_mng->getRootSceneNode()->createChildSceneNode("EditorEntityNode");
			entityNode->setPosition(Ogre::Vector3(0,0,0));
			entityNode->attachObject(entity);
			
			// update the texture
			Ogre::Resource* res= Ogre::TextureManager::getSingleton().createOrRetrieve ("editor_tex",Ogre::ResourceGroupManager::DEFAULT_RESOURCE_GROUP_NAME).first.getPointer();
			Ogre::Texture* texture = dynamic_cast<Ogre::Texture*>(res);
			Ogre::RenderTarget* target = texture->getBuffer()->getRenderTarget();
			target->update();

			//get the skeleton of the main mesh and fill the submesh bone selector combo box
			Ogre::Skeleton *skel = entity->getMesh()->getSkeleton().getPointer();
			if(skel!=0){
				Ogre::Skeleton::BoneIterator bit = skel->getBoneIterator();
				Combobox* boneSelector = static_cast<Combobox*>(WindowManager::getSingleton().getWindow("RITab/SubMesh/BoneSelector"));
				std::list<std::string> boneNames;
								
				while (bit.hasMoreElements())
				{
					Ogre::Bone *bone = bit.getNext();
					boneNames.push_back(bone->getName());
				}

				//Ogre::FileInfoList::iterator it;
				std::string bone;

				for (std::list<std::string>::iterator it = boneNames.begin(); it != boneNames.end(); ++it)
				{
					
						bone = *it;
						m_listItem = new ListboxTextItem(bone);
						boneSelector->addItem(m_listItem);
				}
			}
		}
		catch (Ogre::Exception& e)
		{
			WARNING("can't create mesh %s", meshname.c_str());
		}
	}
	
	return true;
}

bool GuiTabs::onSubMeshSelected(const CEGUI::EventArgs& evt)
{
	const CEGUI::MouseEventArgs& we =
	static_cast<const CEGUI::MouseEventArgs&>(evt);
	
	CEGUI::Combobox* cbo = static_cast<CEGUI::Combobox*>(we.window);
	
	CEGUI::ListboxItem* item = cbo->getSelectedItem();
	
	if (item != 0)
	{
		// do nothing of importance yet
		std::string meshname = item->getText().c_str();
		DEBUG("selected mesh %s",meshname.c_str());
	}
	
	return true;
}

bool GuiTabs::onCloseButton(const CEGUI::EventArgs& evt)
{
	WindowManager::getSingleton().getWindow("ContentEditor")->hide();
	return true;
}

void GuiTabs::onAddSubMesh(const CEGUI::EventArgs& evt)
{
	//get SubMesh info, get optional Bone Info, add Submesh to mainmesh
	//add a line for this bone with a button "kill me"
}

void GuiTabs::addTabWindow(std::string name, ContentTab* tab)
{
	//most likely obsolete
	if(!tabExists(name))
	{
		m_tabs[name] = tab;
		m_tabControl->addTab(dynamic_cast<CEGUI::Window*>(tab));
	}
}

bool GuiTabs::tabExists(std::string tabName)
{
	//most likely obsolete
	if(m_tabs.find(tabName) != m_tabs.end())
		return true;

	return false;
}

void GuiTabs::messageLogged(const Ogre::String& message, Ogre::LogMessageLevel lml, bool maskDebug, const Ogre::String& logName)
{
	//obsolete
}

void GuiTabs::update()
{
}
