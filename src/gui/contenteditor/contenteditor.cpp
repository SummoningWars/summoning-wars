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

// Utility for CEGUI cross-version compatibility
#include "ceguiutility.h"

#include "contenteditor.h"
#include "OgreRoot.h"
#include "debug.h"
#include "config.h"

#ifdef CEGUI_07
#include "CEGUI/RendererModules/Ogre/CEGUIOgreRenderer.h"
#else
#include "CEGUI/RendererModules/Ogre/Renderer.h"
#endif

#include <OgreHardwarePixelBuffer.h>
#include <OgreMeshManager.h>
#include <OISMouse.h>

#include "graphicmanager.h"
#include "worldobject.h"
#include "objectloader.h"
#include "world.h"

#include "renderinfoeditor.h"
#include "fixedobjecteditor.h"
#include "itemeditor.h"
#include "monstereditor.h"
#include "gameinfotab.h"

using namespace CEGUI;

template<> ContentEditor* Ogre::Singleton<ContentEditor>::SUMWARS_OGRE_SINGLETON = 0;

void ContentEditor::init(bool visible)
{
	//CEGUI::System* guiSystem = System::getSingletonPtr();
	CEGUI::WindowManager* winManager = WindowManager::getSingletonPtr();
  CEGUI::Window* gameScreen = CEGUIUtility::getWindow("GameScreen");
	m_lastVisibilitySwitch = 0;
	
	m_rootWindow = winManager->createWindow("TaharezLook/FrameWindow", "ContentEditor");
	m_rootWindow->setPosition(UVector2(UDim(0.025f, 0.0f), UDim(0.025f, 0.0f)));
  CEGUIUtility::setWidgetSizeRel(m_rootWindow, 0.9f, 0.85f);
	m_rootWindow->setText((CEGUI::utf8*)"Content Editor");
  CEGUIUtility::addChildWidget (gameScreen, m_rootWindow);
	
	CEGUI::Window* rootWindow;
	rootWindow = CEGUIUtility::loadLayoutFromFile ("contenteditor.layout");
	rootWindow->setPosition(UVector2(UDim(0.0f, 0.0f), UDim(0.0f, 0.0f)));
  CEGUIUtility::setWidgetSizeRel(rootWindow, 1.0f, 1.0f);
  CEGUIUtility::addChildWidget (m_rootWindow, rootWindow);
	
  CEGUIUtility::dumpFullWindowToLog(rootWindow);

	// initialisation of the selected tabs for more intuitive use
  CEGUI::TabControl* widget = static_cast<CEGUI::TabControl*>(CEGUIUtility::getWindowForLoadedLayout(m_rootWindow, "Root/ObjectInfoTabControl"));
  if (widget)
  {
    widget->setSelectedTab("RenderInfoTab");
  }

  widget = static_cast<CEGUI::TabControl*>(CEGUIUtility::getWindowForLoadedLayout(m_rootWindow, "Root/ObjectInfoTabControl/__auto_TabPane__/RenderInfoTab/RenderInfoTabControl"));
  if (widget)
  {
    widget->setSelectedTab("BasicMesh");
  }

  widget = static_cast<CEGUI::TabControl*>(CEGUIUtility::getWindowForLoadedLayout(m_rootWindow, "Root/ObjectInfoTabControl/__auto_TabPane__/FixedObjectTab/FixedObjectTabControl"));
  if (widget)
  {
    widget->setSelectedTab("Properties");
  }

  widget = static_cast<CEGUI::TabControl*>(CEGUIUtility::getWindowForLoadedLayout(m_rootWindow, "Root/ObjectInfoTabControl/__auto_TabPane__/MonsterTab/MonsterTabControl"));
  if (widget)
  {
    widget->setSelectedTab("Properties");
  }

	// create SceneManager for renderering images for the content editor
	Ogre::SceneManager* editor_scene_mng = Ogre::Root::getSingleton().createSceneManager(Ogre::ST_GENERIC,"EditorSceneManager");
	editor_scene_mng->setAmbientLight(Ogre::ColourValue(1,1,1));
	
	// camera filming the content of the the editor scene
	Ogre::Camera* editor_camera = editor_scene_mng->createCamera("editor_camera");
	editor_camera->setNearClipDistance(0.1);
	editor_camera->setAspectRatio(1.0);
	Ogre::SceneNode *mainEditorNode = editor_scene_mng->getRootSceneNode()->createChildSceneNode("MainEditorCameraNode");
	Ogre::SceneNode *editorNode = mainEditorNode->createChildSceneNode("EditorCameraNode");
	Ogre::SceneNode *editorCamPitchNode = editorNode->createChildSceneNode("EditorCamerapitchNode");
	editorCamPitchNode->attachObject(editor_camera);
	mainEditorNode->setFixedYawAxis(true);
	editorNode->setFixedYawAxis(true);
	editorCamPitchNode->setFixedYawAxis(true);
	
	
    // texture that is created from the camera image
	Ogre::TexturePtr editor_texture = Ogre::TextureManager::getSingleton().createManual( "editor_tex",
			Ogre::ResourceGroupManager::DEFAULT_RESOURCE_GROUP_NAME, Ogre::TEX_TYPE_2D,
   512, 512, 0, Ogre::PF_R8G8B8A8, Ogre::TU_RENDERTARGET );
	
   // connection texture and camera via RenderTarget
	Ogre::RenderTarget* editor_rt = editor_texture->getBuffer()->getRenderTarget();
	editor_rt->setAutoUpdated(false);
	Ogre::Viewport *editor_view = editor_rt->addViewport(editor_camera );
	editor_view->setClearEveryFrame( true );
	editor_view->setOverlaysEnabled (false);
	editor_view->setBackgroundColour(Ogre::ColourValue(0,0,0,1.0) );
	editor_rt->update();

	// get the OgreRenderer from CEGUI and create a CEGUI texture from the Ogre texture
	CEGUI::OgreRenderer* renderer_ptr = static_cast<CEGUI::OgreRenderer*>(CEGUI::System::getSingleton().getRenderer());
  CEGUI::String imageName("editor_img");

#ifdef CEGUI_07
	// create a CEGUI Image from the Texture
  CEGUI::Texture& editor_ceguiTex = renderer_ptr->createTexture(editor_texture);
    
	CEGUI::Imageset& editor_textureImageSet = CEGUI::ImagesetManager::getSingleton().create("editor_imageset", editor_ceguiTex);

	editor_textureImageSet.defineImage(
      imageName,
			CEGUI::Point( 0.0f, 0.0f ),
			CEGUI::Size( editor_ceguiTex.getSize().d_width, editor_ceguiTex.getSize().d_height ),
			CEGUI::Point( 0.0f, 0.0f ) );
#else
  CEGUI::Texture &editor_cegui_texture = renderer_ptr->createTexture (editor_texture->getName (), editor_texture);
	{
		CEGUI::TextureTarget*   d_textureTarget;
		CEGUI::BasicImage*      d_textureTargetImage;
		d_textureTarget = renderer_ptr->createTextureTarget();
		d_textureTargetImage = static_cast<CEGUI::BasicImage*>(&CEGUI::ImageManager::getSingleton().create("BasicImage", imageName));
		d_textureTargetImage->setTexture(&editor_cegui_texture);
		d_textureTargetImage->setArea(
        CEGUI::Rectf(
            0, 
            0, 
            editor_cegui_texture.getSize ().d_width,
            editor_cegui_texture.getSize ().d_height));
	}
#endif

	// place the image in a the CEGUI label
	CEGUI::Window* label = CEGUIUtility::getWindowForLoadedLayout(m_rootWindow, "Root/ObjectInfoTabControl/__auto_TabPane__/RenderInfoTab/meshPreview");

  CEGUI::String content_editor_image_name("editor_img");
	label->setProperty("Image", content_editor_image_name); // prev: "set:editor_imageset image:editor_img"
	label->subscribeEvent(CEGUI::Window::EventMouseButtonDown, CEGUI::Event::Subscriber(&ContentEditor::onPreviewWindowMouseDown, this));
  label->subscribeEvent(CEGUIUtility::EventMouseLeavesWindowArea(), CEGUI::Event::Subscriber(&ContentEditor::onPreviewWindowMouseUp, this));
	label->subscribeEvent(CEGUI::Window::EventMouseButtonUp, CEGUI::Event::Subscriber(&ContentEditor::onPreviewWindowMouseUp, this));
	label->subscribeEvent(CEGUI::Window::EventMouseWheel, CEGUI::Event::Subscriber(&ContentEditor::onPreviewWindowScrollWheel, this));
	
  CEGUI::PushButton* closebutton = static_cast<CEGUI::PushButton*>(CEGUIUtility::getWindowForLoadedLayout(m_rootWindow, "Root/CloseButton"));
	closebutton->subscribeEvent(CEGUI::PushButton::EventClicked, CEGUI::Event::Subscriber(&ContentEditor::onClose, this));
	
	closebutton = static_cast<CEGUI::FrameWindow*>(m_rootWindow)->getCloseButton();
	closebutton->subscribeEvent(CEGUI::PushButton::EventClicked, CEGUI::Event::Subscriber(&ContentEditor::onClose, this));
	
  SW_DEBUG("Content Editor: creating fixed object editor");
	m_components["FOEditor"] = new FixedObjectEditor();

  SW_DEBUG("Content Editor: creating render info editor");
	m_components["RIEditor"] = new RenderInfoEditor();

  SW_DEBUG("Content Editor: creating item editor");
	m_components["ItemEditor"] = new ItemEditor();

  SW_DEBUG("Content Editor: creating monster editor");
	m_components["MonsterEditor"] = new MonsterEditor();

  SW_DEBUG("Content Editor: creating game info tab");
  m_components["GameInfoTab"] = new GameInfoTab();
	
	std::map<std::string, ContentEditorTab*>::iterator it;
	for (it = m_components.begin(); it != m_components.end(); ++it)
	{
		it->second->init(m_rootWindow);
	}
	
	if(!visible)
  {
		m_rootWindow->setVisible(visible);
  }

	updateTranslation();
}

void ContentEditor::toggleVisibility()
{
	if((Ogre::Root::getSingleton().getTimer()->getMilliseconds() - m_lastVisibilitySwitch) > 500)
	{
		m_rootWindow->setVisible(!m_rootWindow->isVisible());
		m_lastVisibilitySwitch = Ogre::Root::getSingleton().getTimer()->getMilliseconds();
	}
}


void ContentEditor::update(OIS::Keyboard *keyboard, OIS::Mouse *mouse)
{
	if(m_leftMouseDown)
	{
		Ogre::SceneManager *editor_scene_mng = Ogre::Root::getSingleton().getSceneManager("EditorSceneManager");
		Ogre::Radian rotX(mouse->getMouseState().X.rel * 0.05f);
		Ogre::Radian rotY(mouse->getMouseState().Y.rel * 0.05f);
		
		Ogre::SceneNode *editorNode = editor_scene_mng->getSceneNode("EditorCameraNode");
		Ogre::SceneNode *editorCamPitchNode = editor_scene_mng->getSceneNode("EditorCamerapitchNode");
		editorNode->yaw(-rotX);
		editorCamPitchNode->roll(rotY);
	}
	else if(m_rightMouseDown)
	{
		Ogre::SceneManager *editor_scene_mng = Ogre::Root::getSingleton().getSceneManager("EditorSceneManager");
		Ogre::Camera *cam = editor_scene_mng->getCamera("editor_camera");
		
		Ogre::Vector3 vec = Ogre::Vector3(0.0f, 0.0f, mouse->getMouseState().Y.rel * 0.1f);
		cam->moveRelative(vec);
	}
	
	if(keyboard->isKeyDown(OIS::KC_LCONTROL) && keyboard->isKeyDown(OIS::KC_K))
  {
		toggleVisibility();
  }
	
	std::map<std::string, ContentEditorTab*>::iterator it;
	for (it = m_components.begin(); it != m_components.end(); ++it)
	{
		it->second->update();
	}
}

void ContentEditor::fullUpdateComponent(std::string name)
{
	if (m_components.count(name) == 0)
		return;
	
	m_components[name]->setUpdateBaseContent();
}

bool ContentEditor::handleCloseWindow(const CEGUI::EventArgs& e)
{
	m_rootWindow->hide();
	return true;
}

void ContentEditor::updateTranslation()
{
	CEGUI::Window* label;

	textdomain("tools");

	label = CEGUIUtility::getWindowForLoadedLayout(m_rootWindow, 
      "Root/ObjectInfoTabControl/__auto_TabPane__/RenderInfoTab");
	label->setText((CEGUI::utf8*) gettext("Render Info"));
	
	label = CEGUIUtility::getWindowForLoadedLayout(m_rootWindow, 
      "Root/ObjectInfoTabControl/__auto_TabPane__/RenderInfoTab/RenderInfoTabControl");
	label->setText((CEGUI::utf8*) gettext("Mesh"));
	
	label = CEGUIUtility::getWindowForLoadedLayout(m_rootWindow, 
      "Root/ObjectInfoTabControl/__auto_TabPane__/RenderInfoTab/RenderInfoTabControl/__auto_TabPane__/BasicMesh/MainMeshLabel");
	label->setText((CEGUI::utf8*) gettext("Select main mesh:"));
	
	label = CEGUIUtility::getWindowForLoadedLayout(m_rootWindow, 
      "Root/ObjectInfoTabControl/__auto_TabPane__/RenderInfoTab/RenderInfoTabControl/__auto_TabPane__/SubMesh");
	label->setText((CEGUI::utf8*) gettext("Submeshes"));
	
	label = CEGUIUtility::getWindowForLoadedLayout(m_rootWindow, 
      "Root/ObjectInfoTabControl/__auto_TabPane__/RenderInfoTab/RenderInfoTabControl/__auto_TabPane__/SubMesh/AddSubMeshButton"); 
	label->setText((CEGUI::utf8*) gettext("Add Submesh"));
	
	label = CEGUIUtility::getWindowForLoadedLayout(m_rootWindow, 
      "Root/ObjectInfoTabControl/__auto_TabPane__/RenderInfoTab/RenderInfoTabControl/__auto_TabPane__/SubMesh/NewSubmeshLabel"); 
	label->setText((CEGUI::utf8*) gettext("New submesh:"));
	
	label = CEGUIUtility::getWindowForLoadedLayout(m_rootWindow, 
      "Root/ObjectInfoTabControl/__auto_TabPane__/RenderInfoTab/RenderInfoTabControl/__auto_TabPane__/SubMesh/NewSMNameLabel"); 
	label->setText((CEGUI::utf8*) gettext("submesh name:"));
	
	label = CEGUIUtility::getWindowForLoadedLayout(m_rootWindow, 
      "Root/ObjectInfoTabControl/__auto_TabPane__/RenderInfoTab/RenderInfoTabControl/__auto_TabPane__/SubMesh/EditSMLabel"); 
	label->setText((CEGUI::utf8*) gettext("Edit submesh:"));
	
	label = CEGUIUtility::getWindowForLoadedLayout(m_rootWindow, 
      "Root/ObjectInfoTabControl/__auto_TabPane__/RenderInfoTab/RenderInfoTabControl/__auto_TabPane__/SubMesh/AttachMeshLabel");
	label->setText((CEGUI::utf8*) gettext("Attach to:"));
	
	label = CEGUIUtility::getWindowForLoadedLayout(m_rootWindow, 
      "Root/ObjectInfoTabControl/__auto_TabPane__/RenderInfoTab/RenderInfoTabControl/__auto_TabPane__/SubMesh/BoneLabel"); 
	label->setText((CEGUI::utf8*) gettext("at bone:"));
	
	label = CEGUIUtility::getWindowForLoadedLayout(m_rootWindow, 
      "Root/ObjectInfoTabControl/__auto_TabPane__/RenderInfoTab/RenderInfoTabControl/__auto_TabPane__/SubMesh/RotateLabel");
	label->setText((CEGUI::utf8*) gettext("Rotate(x,y,z):"));
	
	label = CEGUIUtility::getWindowForLoadedLayout(m_rootWindow, 
      "Root/ObjectInfoTabControl/__auto_TabPane__/RenderInfoTab/RenderInfoTabControl/__auto_TabPane__/SubMesh/ScaleLabel");
	label->setText((CEGUI::utf8*) gettext("Scale:"));
	
	label = CEGUIUtility::getWindowForLoadedLayout(m_rootWindow, 
      "Root/ObjectInfoTabControl/__auto_TabPane__/RenderInfoTab/RenderInfoTabControl/__auto_TabPane__/SubMesh/OffsetLabel");
	label->setText((CEGUI::utf8*) gettext("Offset (x,y,z):"));
	
	label = CEGUIUtility::getWindowForLoadedLayout(m_rootWindow, 
      "Root/ObjectInfoTabControl/__auto_TabPane__/RenderInfoTab/RenderInfoTabControl/__auto_TabPane__/SubMesh/DelSubMeshButton"); 
	label->setText((CEGUI::utf8*) gettext("Delete Submesh"));
	
	label = CEGUIUtility::getWindowForLoadedLayout(m_rootWindow, 
      "Root/ObjectInfoTabControl/__auto_TabPane__/RenderInfoTab/RenderInfoTabControl/__auto_TabPane__/Animations"); 
	label->setText((CEGUI::utf8*) gettext("Animations"));
	
	label = CEGUIUtility::getWindowForLoadedLayout(m_rootWindow, 
      "Root/ObjectInfoTabControl/__auto_TabPane__/RenderInfoTab/RenderInfoTabControl/__auto_TabPane__/XML"); 
	label->setText((CEGUI::utf8*) gettext("XML"));
	
	label = CEGUIUtility::getWindowForLoadedLayout(m_rootWindow, 
      "Root/ObjectInfoTabControl/__auto_TabPane__/RenderInfoTab/RenderInfoTabControl/__auto_TabPane__/XML/SubmitButton"); 
	label->setText((CEGUI::utf8*) gettext("Submit XML"));
	
	label = CEGUIUtility::getWindowForLoadedLayout(m_rootWindow, 
      "Root/ObjectInfoTabControl/__auto_TabPane__/FixedObjectTab"); 
	label->setText((CEGUI::utf8*) gettext("FixedObject"));
	
	label = CEGUIUtility::getWindowForLoadedLayout(m_rootWindow, 
      "Root/ObjectInfoTabControl/__auto_TabPane__/FixedObjectTab/FixedObjectTabControl/__auto_TabPane__/Properties"); 
	label->setText((CEGUI::utf8*) gettext("Properties"));
	
	label = CEGUIUtility::getWindowForLoadedLayout(m_rootWindow, 
      "Root/ObjectInfoTabControl/__auto_TabPane__/FixedObjectTab/FixedObjectTabControl/__auto_TabPane__/Properties/ShapeLabel"); 
	label->setText((CEGUI::utf8*) gettext("Shape:"));
	
	label = CEGUIUtility::getWindowForLoadedLayout(m_rootWindow, 
      "Root/ObjectInfoTabControl/__auto_TabPane__/FixedObjectTab/FixedObjectTabControl/__auto_TabPane__/Properties/CircleLabel"); 
	label->setText((CEGUI::utf8*) gettext("Circle"));
	
	label = CEGUIUtility::getWindowForLoadedLayout(m_rootWindow, 
      "Root/ObjectInfoTabControl/__auto_TabPane__/FixedObjectTab/FixedObjectTabControl/__auto_TabPane__/Properties/RadiusLabel"); 
	label->setText((CEGUI::utf8*) gettext("Radius:"));
	
	label = CEGUIUtility::getWindowForLoadedLayout(m_rootWindow, 
      "Root/ObjectInfoTabControl/__auto_TabPane__/FixedObjectTab/FixedObjectTabControl/__auto_TabPane__/Properties/DetectCircleButton"); 
	label->setText((CEGUI::utf8*) gettext("Autodetect"));
	
	label = CEGUIUtility::getWindowForLoadedLayout(m_rootWindow, 
      "Root/ObjectInfoTabControl/__auto_TabPane__/FixedObjectTab/FixedObjectTabControl/__auto_TabPane__/Properties/RectangleLabel");
	label->setText((CEGUI::utf8*) gettext("Rectangle"));
	
	label = CEGUIUtility::getWindowForLoadedLayout(m_rootWindow, 
      "Root/ObjectInfoTabControl/__auto_TabPane__/FixedObjectTab/FixedObjectTabControl/__auto_TabPane__/Properties/WidthLabel");
	label->setText((CEGUI::utf8*) gettext("Width:"));
	
	label = CEGUIUtility::getWindowForLoadedLayout(m_rootWindow, 
      "Root/ObjectInfoTabControl/__auto_TabPane__/FixedObjectTab/FixedObjectTabControl/__auto_TabPane__/Properties/DepthLabel");
	label->setText((CEGUI::utf8*) gettext("Depth:"));
	
	label = CEGUIUtility::getWindowForLoadedLayout(m_rootWindow, 
      "Root/ObjectInfoTabControl/__auto_TabPane__/FixedObjectTab/FixedObjectTabControl/__auto_TabPane__/Properties/DetectRectButton");
	label->setText((CEGUI::utf8*) gettext("Autodetect"));
	
	label = CEGUIUtility::getWindowForLoadedLayout(m_rootWindow, 
      "Root/ObjectInfoTabControl/__auto_TabPane__/FixedObjectTab/FixedObjectTabControl/__auto_TabPane__/Properties/CollisionLabel");
	label->setText((CEGUI::utf8*) gettext("Collision type"));
	
	label = CEGUIUtility::getWindowForLoadedLayout(m_rootWindow, 
      "Root/ObjectInfoTabControl/__auto_TabPane__/FixedObjectTab/FixedObjectTabControl/__auto_TabPane__/Properties/CopyDataLabel");
	label->setText((CEGUI::utf8*) gettext("Copy data from:"));
	
	label = CEGUIUtility::getWindowForLoadedLayout(m_rootWindow, 
      "Root/ObjectInfoTabControl/__auto_TabPane__/FixedObjectTab/FixedObjectTabControl/__auto_TabPane__/Properties/CopyDataButton");
	label->setText((CEGUI::utf8*) gettext("Copy"));
	
	label = CEGUIUtility::getWindowForLoadedLayout(m_rootWindow, 
      "Root/ObjectInfoTabControl/__auto_TabPane__/FixedObjectTab/FixedObjectTabControl/__auto_TabPane__/Create");
	label->setText((CEGUI::utf8*) gettext("Create Object"));
	
	label = CEGUIUtility::getWindowForLoadedLayout(m_rootWindow, 
      "Root/ObjectInfoTabControl/__auto_TabPane__/FixedObjectTab/FixedObjectTabControl/__auto_TabPane__/Create/CreateButton");
	label->setText((CEGUI::utf8*) gettext("Create Object"));
	
	label = CEGUIUtility::getWindowForLoadedLayout(m_rootWindow, 
      "Root/ObjectInfoTabControl/__auto_TabPane__/FixedObjectTab/FixedObjectTabControl/__auto_TabPane__/Create/PosLabel");
	label->setText((CEGUI::utf8*) gettext("Position:"));
	
	label = CEGUIUtility::getWindowForLoadedLayout(m_rootWindow, 
      "Root/ObjectInfoTabControl/__auto_TabPane__/FixedObjectTab/FixedObjectTabControl/__auto_TabPane__/Create/GetPlPosButton");
	label->setText((CEGUI::utf8*) gettext("Get Player Position"));
	
	label = CEGUIUtility::getWindowForLoadedLayout(m_rootWindow, 
      "Root/ObjectInfoTabControl/__auto_TabPane__/FixedObjectTab/FixedObjectTabControl/__auto_TabPane__/Create/AngleLabel");
	label->setText((CEGUI::utf8*) gettext("Angle:"));
	
	label = CEGUIUtility::getWindowForLoadedLayout(m_rootWindow, 
      "Root/ObjectInfoTabControl/__auto_TabPane__/FixedObjectTab/FixedObjectTabControl/__auto_TabPane__/Create/DelAllButton");
	label->setText((CEGUI::utf8*) gettext("Delete all objects"));
	
	label = CEGUIUtility::getWindowForLoadedLayout(m_rootWindow, 
      "Root/ObjectInfoTabControl/__auto_TabPane__/FixedObjectTab/FixedObjectTabControl/__auto_TabPane__/XML");
	label->setText((CEGUI::utf8*) gettext("XML"));
	
	label = CEGUIUtility::getWindowForLoadedLayout(m_rootWindow,
      "Root/ObjectInfoTabControl/__auto_TabPane__/FixedObjectTab/FixedObjectTabControl/__auto_TabPane__/XML/SubmitButton");
	label->setText((CEGUI::utf8*) gettext("Submit XML"));
	
	label = CEGUIUtility::getWindowForLoadedLayout(m_rootWindow, 
      "Root/ObjectInfoTabControl/__auto_TabPane__/ItemTab");
	label->setText((CEGUI::utf8*) gettext("Item"));
	
	label = CEGUIUtility::getWindowForLoadedLayout(m_rootWindow, 
      "Root/ObjectInfoTabControl/__auto_TabPane__/ItemTab/ItemTabControl/__auto_TabPane__/General");
	label->setText((CEGUI::utf8*) gettext("General"));
	
	label = CEGUIUtility::getWindowForLoadedLayout(m_rootWindow, 
      "Root/ObjectInfoTabControl/__auto_TabPane__/ItemTab/ItemTabControl/__auto_TabPane__/General/TypeLabel");
	label->setText((CEGUI::utf8*) gettext("Type:"));
	
	label = CEGUIUtility::getWindowForLoadedLayout(m_rootWindow, 
      "Root/ObjectInfoTabControl/__auto_TabPane__/ItemTab/ItemTabControl/__auto_TabPane__/General/SizeLabel");
	label->setText((CEGUI::utf8*) gettext("Size:"));
	
	label = CEGUIUtility::getWindowForLoadedLayout(m_rootWindow, 
      "Root/ObjectInfoTabControl/__auto_TabPane__/ItemTab/ItemTabControl/__auto_TabPane__/General/CopyDataLabel");
	label->setText((CEGUI::utf8*) gettext("Copy data from:"));
	
	label = CEGUIUtility::getWindowForLoadedLayout(m_rootWindow, 
      "Root/ObjectInfoTabControl/__auto_TabPane__/ItemTab/ItemTabControl/__auto_TabPane__/General/CopyDataButton");
	label->setText((CEGUI::utf8*) gettext("Copy"));
	
	label = CEGUIUtility::getWindowForLoadedLayout(m_rootWindow, 
      "Root/ObjectInfoTabControl/__auto_TabPane__/ItemTab/ItemTabControl/__auto_TabPane__/General/NameLabel");
	label->setText((CEGUI::utf8*) gettext("Name:"));
	
	label = CEGUIUtility::getWindowForLoadedLayout(m_rootWindow, 
      "Root/ObjectInfoTabControl/__auto_TabPane__/ItemTab/ItemTabControl/__auto_TabPane__/General/PriceLabel");
	label->setText((CEGUI::utf8*) gettext("Value:"));
	
	label = CEGUIUtility::getWindowForLoadedLayout(m_rootWindow, 
      "Root/ObjectInfoTabControl/__auto_TabPane__/ItemTab/ItemTabControl/__auto_TabPane__/General/EnchantRangeLabel");
	label->setText((CEGUI::utf8*) gettext("Enchant min:"));
	
	label = CEGUIUtility::getWindowForLoadedLayout(m_rootWindow, 
      "Root/ObjectInfoTabControl/__auto_TabPane__/ItemTab/ItemTabControl/__auto_TabPane__/General/EnchantMaxLabel");
	label->setText((CEGUI::utf8*) gettext("max:"));
	
	label = CEGUIUtility::getWindowForLoadedLayout(m_rootWindow, 
      "Root/ObjectInfoTabControl/__auto_TabPane__/ItemTab/ItemTabControl/__auto_TabPane__/Equip");
	label->setText((CEGUI::utf8*) gettext("Equip"));
	
	label = CEGUIUtility::getWindowForLoadedLayout(m_rootWindow, 
      "Root/ObjectInfoTabControl/__auto_TabPane__/ItemTab/ItemTabControl/__auto_TabPane__/Equip/HealthLabel");
	label->setText((CEGUI::utf8*) gettext("Health:"));
	
	label = CEGUIUtility::getWindowForLoadedLayout(m_rootWindow, 
      "Root/ObjectInfoTabControl/__auto_TabPane__/ItemTab/ItemTabControl/__auto_TabPane__/Equip/AttrLabel");
	label->setText((CEGUI::utf8*) gettext("Attribute:"));
	
	label = CEGUIUtility::getWindowForLoadedLayout(m_rootWindow, 
      "Root/ObjectInfoTabControl/__auto_TabPane__/ItemTab/ItemTabControl/__auto_TabPane__/Equip/StrengthLabel");
	label->setText((CEGUI::utf8*) gettext("Strength:"));
	
	label = CEGUIUtility::getWindowForLoadedLayout(m_rootWindow, 
      "Root/ObjectInfoTabControl/__auto_TabPane__/ItemTab/ItemTabControl/__auto_TabPane__/Equip/DexterityLabel");
	label->setText((CEGUI::utf8*) gettext("Dexterity:"));
	
	label = CEGUIUtility::getWindowForLoadedLayout(m_rootWindow, 
      "Root/ObjectInfoTabControl/__auto_TabPane__/ItemTab/ItemTabControl/__auto_TabPane__/Equip/MagicPowerLabel");
	label->setText((CEGUI::utf8*) gettext("Magic Power:"));
	
	label = CEGUIUtility::getWindowForLoadedLayout(m_rootWindow, 
      "Root/ObjectInfoTabControl/__auto_TabPane__/ItemTab/ItemTabControl/__auto_TabPane__/Equip/WillpowerLabel");
	label->setText((CEGUI::utf8*) gettext("Willpower:"));
	
	label = CEGUIUtility::getWindowForLoadedLayout(m_rootWindow, 
      "Root/ObjectInfoTabControl/__auto_TabPane__/ItemTab/ItemTabControl/__auto_TabPane__/Equip/ResistancesLabel");
	label->setText((CEGUI::utf8*) gettext("Resistances:"));
	
	label = CEGUIUtility::getWindowForLoadedLayout(m_rootWindow, 
      "Root/ObjectInfoTabControl/__auto_TabPane__/ItemTab/ItemTabControl/__auto_TabPane__/Equip/PhysResLabel");
	label->setText((CEGUI::utf8*) gettext("Physical:"));
	
	label = CEGUIUtility::getWindowForLoadedLayout(m_rootWindow, 
      "Root/ObjectInfoTabControl/__auto_TabPane__/ItemTab/ItemTabControl/__auto_TabPane__/Equip/FireResLabel");
	label->setText((CEGUI::utf8*) gettext("Fire:"));
	
	label = CEGUIUtility::getWindowForLoadedLayout(m_rootWindow, 
      "Root/ObjectInfoTabControl/__auto_TabPane__/ItemTab/ItemTabControl/__auto_TabPane__/Equip/IceResLabel");
	label->setText((CEGUI::utf8*) gettext("Ice:"));
	
	label = CEGUIUtility::getWindowForLoadedLayout(m_rootWindow, 
      "Root/ObjectInfoTabControl/__auto_TabPane__/ItemTab/ItemTabControl/__auto_TabPane__/Equip/AirResLabel");
	label->setText((CEGUI::utf8*) gettext("Air:"));
	
	label = CEGUIUtility::getWindowForLoadedLayout(m_rootWindow, 
      "Root/ObjectInfoTabControl/__auto_TabPane__/ItemTab/ItemTabControl/__auto_TabPane__/Equip/OtherLabel");
	label->setText((CEGUI::utf8*) gettext("Other:"));
	
	label = CEGUIUtility::getWindowForLoadedLayout(m_rootWindow, 
      "Root/ObjectInfoTabControl/__auto_TabPane__/ItemTab/ItemTabControl/__auto_TabPane__/Equip/ArmorLabel");
	label->setText((CEGUI::utf8*) gettext("Armor:"));
	
	label = CEGUIUtility::getWindowForLoadedLayout(m_rootWindow, 
      "Root/ObjectInfoTabControl/__auto_TabPane__/ItemTab/ItemTabControl/__auto_TabPane__/Equip/BlockLabel");
	label->setText((CEGUI::utf8*) gettext("Block:"));
	
	label = CEGUIUtility::getWindowForLoadedLayout(m_rootWindow, 
      "Root/ObjectInfoTabControl/__auto_TabPane__/ItemTab/ItemTabControl/__auto_TabPane__/Equip/AttackLabel");
	label->setText((CEGUI::utf8*) gettext("Precision:"));
	
	label = CEGUIUtility::getWindowForLoadedLayout(m_rootWindow, 
      "Root/ObjectInfoTabControl/__auto_TabPane__/ItemTab/ItemTabControl/__auto_TabPane__/Equip/PowerLabel");
	label->setText((CEGUI::utf8*) gettext("Power:"));
	
	label = CEGUIUtility::getWindowForLoadedLayout(m_rootWindow, 
      "Root/ObjectInfoTabControl/__auto_TabPane__/ItemTab/ItemTabControl/__auto_TabPane__/Weapon");
	label->setText((CEGUI::utf8*) gettext("Weapon"));
	
	label = CEGUIUtility::getWindowForLoadedLayout(m_rootWindow, 
      "Root/ObjectInfoTabControl/__auto_TabPane__/ItemTab/ItemTabControl/__auto_TabPane__/Weapon/TypeLabel");
	label->setText((CEGUI::utf8*) gettext("Type:"));
	
	label = CEGUIUtility::getWindowForLoadedLayout(m_rootWindow, 
      "Root/ObjectInfoTabControl/__auto_TabPane__/ItemTab/ItemTabControl/__auto_TabPane__/Weapon/DamageLabel");
	label->setText((CEGUI::utf8*) gettext("Damage:"));
	
	label = CEGUIUtility::getWindowForLoadedLayout(m_rootWindow, 
      "Root/ObjectInfoTabControl/__auto_TabPane__/ItemTab/ItemTabControl/__auto_TabPane__/Weapon/PhysLabel");
	label->setText((CEGUI::utf8*) gettext("Physical:"));
	
	label = CEGUIUtility::getWindowForLoadedLayout(m_rootWindow, 
      "Root/ObjectInfoTabControl/__auto_TabPane__/ItemTab/ItemTabControl/__auto_TabPane__/Weapon/PhysToLabel");
	label->setText((CEGUI::utf8*) gettext("-"));
	
	label = CEGUIUtility::getWindowForLoadedLayout(m_rootWindow, 
      "Root/ObjectInfoTabControl/__auto_TabPane__/ItemTab/ItemTabControl/__auto_TabPane__/Weapon/PhysMultLabel");
	label->setText((CEGUI::utf8*) gettext("x"));
	
	label = CEGUIUtility::getWindowForLoadedLayout(m_rootWindow, 
      "Root/ObjectInfoTabControl/__auto_TabPane__/ItemTab/ItemTabControl/__auto_TabPane__/Weapon/FireLabel");
	label->setText((CEGUI::utf8*) gettext("Fire:"));
	
	label = CEGUIUtility::getWindowForLoadedLayout(m_rootWindow, 
      "Root/ObjectInfoTabControl/__auto_TabPane__/ItemTab/ItemTabControl/__auto_TabPane__/Weapon/FireToLabel");
	label->setText((CEGUI::utf8*) gettext("-"));
	
	label = CEGUIUtility::getWindowForLoadedLayout(m_rootWindow, 
      "Root/ObjectInfoTabControl/__auto_TabPane__/ItemTab/ItemTabControl/__auto_TabPane__/Weapon/FireMultLabel");
	label->setText((CEGUI::utf8*) gettext("x"));
	
	label = CEGUIUtility::getWindowForLoadedLayout(m_rootWindow, 
      "Root/ObjectInfoTabControl/__auto_TabPane__/ItemTab/ItemTabControl/__auto_TabPane__/Weapon/IceLabel");
	label->setText((CEGUI::utf8*) gettext("Ice:"));
	
	label = CEGUIUtility::getWindowForLoadedLayout(m_rootWindow, 
      "Root/ObjectInfoTabControl/__auto_TabPane__/ItemTab/ItemTabControl/__auto_TabPane__/Weapon/IceToLabel");
	label->setText((CEGUI::utf8*) gettext("-"));
	
	label = CEGUIUtility::getWindowForLoadedLayout(m_rootWindow, 
      "Root/ObjectInfoTabControl/__auto_TabPane__/ItemTab/ItemTabControl/__auto_TabPane__/Weapon/IceMultLabel");
	label->setText((CEGUI::utf8*) gettext("x"));
	
	label = CEGUIUtility::getWindowForLoadedLayout(m_rootWindow, 
      "Root/ObjectInfoTabControl/__auto_TabPane__/ItemTab/ItemTabControl/__auto_TabPane__/Weapon/AirLabel");
	label->setText((CEGUI::utf8*) gettext("Air:"));
	
	label = CEGUIUtility::getWindowForLoadedLayout(m_rootWindow, 
      "Root/ObjectInfoTabControl/__auto_TabPane__/ItemTab/ItemTabControl/__auto_TabPane__/Weapon/AirToLabel");
	label->setText((CEGUI::utf8*) gettext("-"));
	
	label = CEGUIUtility::getWindowForLoadedLayout(m_rootWindow, 
      "Root/ObjectInfoTabControl/__auto_TabPane__/ItemTab/ItemTabControl/__auto_TabPane__/Weapon/AirMultLabel");
	label->setText((CEGUI::utf8*) gettext("x"));
	
	label = CEGUIUtility::getWindowForLoadedLayout(m_rootWindow, 
      "Root/ObjectInfoTabControl/__auto_TabPane__/ItemTab/ItemTabControl/__auto_TabPane__/Weapon/PrecisionLabel");
	label->setText((CEGUI::utf8*) gettext("Precision:"));
	
	label = CEGUIUtility::getWindowForLoadedLayout(m_rootWindow, 
      "Root/ObjectInfoTabControl/__auto_TabPane__/ItemTab/ItemTabControl/__auto_TabPane__/Weapon/PowerLabel");
	label->setText((CEGUI::utf8*) gettext("Power:"));
	
	label = CEGUIUtility::getWindowForLoadedLayout(m_rootWindow, 
      "Root/ObjectInfoTabControl/__auto_TabPane__/ItemTab/ItemTabControl/__auto_TabPane__/Weapon/RangeLabel");
	label->setText((CEGUI::utf8*) gettext("Range:"));
	
	label = CEGUIUtility::getWindowForLoadedLayout(m_rootWindow, 
      "Root/ObjectInfoTabControl/__auto_TabPane__/ItemTab/ItemTabControl/__auto_TabPane__/Weapon/SpeedLabel");
	label->setText((CEGUI::utf8*) gettext("Speed:"));
	
	label = CEGUIUtility::getWindowForLoadedLayout(m_rootWindow, 
      "Root/ObjectInfoTabControl/__auto_TabPane__/ItemTab/ItemTabControl/__auto_TabPane__/Weapon/TwohandedLabel");
	label->setText((CEGUI::utf8*) gettext("Twohanded weapon"));
	
	label = CEGUIUtility::getWindowForLoadedLayout(m_rootWindow, 
      "Root/ObjectInfoTabControl/__auto_TabPane__/ItemTab/ItemTabControl/__auto_TabPane__/Weapon/CritPercentLabel");
	label->setText((CEGUI::utf8*) gettext("Crit. Hits:"));
	
	label = CEGUIUtility::getWindowForLoadedLayout(m_rootWindow, 
      "Root/ObjectInfoTabControl/__auto_TabPane__/ItemTab/ItemTabControl/__auto_TabPane__/Consume");
	label->setText((CEGUI::utf8*) gettext("Consume"));
	
	label = CEGUIUtility::getWindowForLoadedLayout(m_rootWindow, 
      "Root/ObjectInfoTabControl/__auto_TabPane__/ItemTab/ItemTabControl/__auto_TabPane__/Consume/TypeLabel");
	label->setText((CEGUI::utf8*) gettext("Health:"));
	
	label = CEGUIUtility::getWindowForLoadedLayout(m_rootWindow, 
      "Root/ObjectInfoTabControl/__auto_TabPane__/ItemTab/ItemTabControl/__auto_TabPane__/Consume/CureLabel");
	label->setText((CEGUI::utf8*) gettext("Status heal and immunity time:"));
	
	label = CEGUIUtility::getWindowForLoadedLayout(m_rootWindow, 
      "Root/ObjectInfoTabControl/__auto_TabPane__/ItemTab/ItemTabControl/__auto_TabPane__/Consume/BlindLabel");
	label->setText((CEGUI::utf8*) gettext("Blind:"));
	
	label = CEGUIUtility::getWindowForLoadedLayout(m_rootWindow, 
      "Root/ObjectInfoTabControl/__auto_TabPane__/ItemTab/ItemTabControl/__auto_TabPane__/Consume/PoisonedLabel");
	label->setText((CEGUI::utf8*) gettext("Poisoned:"));
	
	label = CEGUIUtility::getWindowForLoadedLayout(m_rootWindow, 
      "Root/ObjectInfoTabControl/__auto_TabPane__/ItemTab/ItemTabControl/__auto_TabPane__/Consume/BerserkLabel");
	label->setText((CEGUI::utf8*) gettext("Berserk:"));
	
	label = CEGUIUtility::getWindowForLoadedLayout(m_rootWindow, 
      "Root/ObjectInfoTabControl/__auto_TabPane__/ItemTab/ItemTabControl/__auto_TabPane__/Consume/ConfusedLabel");
	label->setText((CEGUI::utf8*) gettext("Confused:"));
	
	label = CEGUIUtility::getWindowForLoadedLayout(m_rootWindow, 
      "Root/ObjectInfoTabControl/__auto_TabPane__/ItemTab/ItemTabControl/__auto_TabPane__/Consume/MuteLabel");
	label->setText((CEGUI::utf8*) gettext("Mute:"));
	
	label = CEGUIUtility::getWindowForLoadedLayout(m_rootWindow, 
      "Root/ObjectInfoTabControl/__auto_TabPane__/ItemTab/ItemTabControl/__auto_TabPane__/Consume/ParalyzedLabel");
	label->setText((CEGUI::utf8*) gettext("Paralyzed:"));
	
	label = CEGUIUtility::getWindowForLoadedLayout(m_rootWindow, 
      "Root/ObjectInfoTabControl/__auto_TabPane__/ItemTab/ItemTabControl/__auto_TabPane__/Consume/FrozenLabel");
	label->setText((CEGUI::utf8*) gettext("Frozen:"));
	
	label = CEGUIUtility::getWindowForLoadedLayout(m_rootWindow, 
      "Root/ObjectInfoTabControl/__auto_TabPane__/ItemTab/ItemTabControl/__auto_TabPane__/Consume/BurningLabel");
	label->setText((CEGUI::utf8*) gettext("Burning:"));
	
	label = CEGUIUtility::getWindowForLoadedLayout(m_rootWindow, 
      "Root/ObjectInfoTabControl/__auto_TabPane__/ItemTab/ItemTabControl/__auto_TabPane__/Create");
	label->setText((CEGUI::utf8*) gettext("Create Item"));
	
	label = CEGUIUtility::getWindowForLoadedLayout(m_rootWindow, 
      "Root/ObjectInfoTabControl/__auto_TabPane__/ItemTab/ItemTabControl/__auto_TabPane__/Create/EnchantLabel");
	label->setText((CEGUI::utf8*) gettext("Enchant:"));
	
	label = CEGUIUtility::getWindowForLoadedLayout(m_rootWindow, 
      "Root/ObjectInfoTabControl/__auto_TabPane__/ItemTab/ItemTabControl/__auto_TabPane__/Create/CreateInventoryButton");
	label->setText((CEGUI::utf8*) gettext("Create in inventory"));
	
	label = CEGUIUtility::getWindowForLoadedLayout(m_rootWindow, 
      "Root/ObjectInfoTabControl/__auto_TabPane__/ItemTab/ItemTabControl/__auto_TabPane__/Create/CreateDropButton");
	label->setText((CEGUI::utf8*) gettext("Create an drop"));
	
	label = CEGUIUtility::getWindowForLoadedLayout(m_rootWindow, 
      "Root/ObjectInfoTabControl/__auto_TabPane__/ItemTab/ItemTabControl/__auto_TabPane__/XML");
	label->setText((CEGUI::utf8*) gettext("XML"));
	
	label = CEGUIUtility::getWindowForLoadedLayout(m_rootWindow, 
      "Root/ObjectInfoTabControl/__auto_TabPane__/ItemTab/ItemTabControl/__auto_TabPane__/XML/SubmitButton");
	label->setText((CEGUI::utf8*) gettext("Submit XML"));
	
	label = CEGUIUtility::getWindowForLoadedLayout(m_rootWindow, "Root/CloseButton");
	label->setText((CEGUI::utf8*) gettext("Close"));
	
	textdomain("menu");
}

bool ContentEditor::onPreviewWindowMouseDown(const CEGUI::EventArgs& evt)
{
#if (OGRE_VERSION < ((1 << 16) | (9 << 8) | 0))
    Ogre::TexturePtr tex = Ogre::TextureManager::getSingletonPtr()->getByName("editor_tex");
#else
    Ogre::TexturePtr tex = Ogre::TextureManager::getSingletonPtr()->getByName("editor_tex").staticCast<Ogre::Texture>();
#endif
    tex.getPointer()->getBuffer()->getRenderTarget()->setAutoUpdated(true);
	
	const CEGUI::MouseEventArgs* args = static_cast<const CEGUI::MouseEventArgs*>(&evt);
	if(args->button == CEGUI::LeftButton)
		m_leftMouseDown = true;
	if(args->button == CEGUI::RightButton)
		m_rightMouseDown = true;

	return true;
}

bool ContentEditor::onPreviewWindowMouseUp(const CEGUI::EventArgs& evt)
{
#if (OGRE_VERSION < ((1 << 16) | (9 << 8) | 0))
    Ogre::TexturePtr tex = Ogre::TextureManager::getSingletonPtr()->getByName("editor_tex", "General");
#else
    Ogre::TexturePtr tex = Ogre::TextureManager::getSingletonPtr()->getByName("editor_tex", "General").staticCast<Ogre::Texture>();
#endif
	tex.getPointer()->getBuffer()->getRenderTarget()->setAutoUpdated(false);
	
	const CEGUI::MouseEventArgs* args = static_cast<const CEGUI::MouseEventArgs*>(&evt);
	if(args->button == CEGUI::LeftButton)
		m_leftMouseDown = false;
	if(args->button == CEGUI::RightButton)
		m_rightMouseDown = false;

	return true;
}

bool ContentEditor::onPreviewWindowScrollWheel(const CEGUI::EventArgs& evt)
{
	const CEGUI::MouseEventArgs* args = static_cast<const CEGUI::MouseEventArgs*>(&evt);
	
	Ogre::SceneManager *editor_scene_mng = Ogre::Root::getSingleton().getSceneManager("EditorSceneManager");
	Ogre::Camera *cam = editor_scene_mng->getCamera("editor_camera");

	Ogre::Vector3 vec = Ogre::Vector3(0.0f, 0.0f, args->wheelChange * 0.02f);
	cam->moveRelative(vec);


#if (OGRE_VERSION < ((1 << 16) | (9 << 8) | 0))
    Ogre::TexturePtr tex = Ogre::TextureManager::getSingletonPtr()->getByName("editor_tex", "General");
#else
    Ogre::TexturePtr tex = Ogre::TextureManager::getSingletonPtr()->getByName("editor_tex", "General").staticCast<Ogre::Texture>();
#endif
	tex.getPointer()->getBuffer()->getRenderTarget()->update();

	return true;
}


ContentEditor* ContentEditor::getSingletonPtr(void)
{
	return SUMWARS_OGRE_SINGLETON;
}

ContentEditor& ContentEditor::getSingleton(void)
{
	assert( SUMWARS_OGRE_SINGLETON );
	return ( *SUMWARS_OGRE_SINGLETON );
}

ContentEditorTab* ContentEditor::getComponent(std::string name)
{
	if (m_components.count(name) == 0)
		return 0;
	
	return m_components[name];
}

bool ContentEditor::onClose(const CEGUI::EventArgs& evt)
{
	m_rootWindow->setVisible(!m_rootWindow->isVisible());
	m_lastVisibilitySwitch = Ogre::Root::getSingleton().getTimer()->getMilliseconds();
	return true;
}

