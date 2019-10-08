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

#include "contenteditor.h"
#include "OgreRoot.h"
#include "debug.h"

#include "CEGUI/RendererModules/Ogre/CEGUIOgreRenderer.h"
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

using namespace CEGUI;

template<> ContentEditor* Ogre::Singleton<ContentEditor>::ms_Singleton = 0;

void ContentEditor::init(bool visible)
{
	CEGUI::WindowManager& win_mgr = CEGUI::WindowManager::getSingleton();
	
	CEGUI::System* guiSystem = System::getSingletonPtr();
	CEGUI::WindowManager* winManager = WindowManager::getSingletonPtr();
	CEGUI::Window* gameScreen = winManager->getWindow("GameScreen");
	m_lastVisibilitySwitch = 0;
	
	m_rootWindow = winManager->createWindow("TaharezLook/FrameWindow", "ContentEditor");
	m_rootWindow->setPosition(UVector2(UDim(0.025f, 0.0f), UDim(0.025f, 0.0f)));
	m_rootWindow->setSize(UVector2(UDim(0.9f, 0.0f), UDim(0.85f, 0.0f)));
	m_rootWindow->setText((CEGUI::utf8*)"Content Editor");
	gameScreen->addChildWindow(m_rootWindow);
	
	CEGUI::Window* rootWindow;
	rootWindow = CEGUI::WindowManager::getSingleton().loadWindowLayout("ContentEditor.layout");
	rootWindow->setPosition(UVector2(UDim(0.0f, 0.0f), UDim(0.0f, 0.0f)));
	rootWindow->setSize(UVector2(UDim(1.0f, 0.0f), UDim(1.0f, 0.0f)));
	m_rootWindow->addChildWindow(rootWindow);
	
	// initialisation of the selected tabs for more intuitive use
	static_cast<CEGUI::TabControl*>(win_mgr.getWindow("ObjectInfoTabControl"))->setSelectedTab("RenderInfoTab");
	static_cast<CEGUI::TabControl*>(win_mgr.getWindow("RenderInfoTabControl"))->setSelectedTab("RITab/BasicMesh");
	static_cast<CEGUI::TabControl*>(win_mgr.getWindow("FixedObjectTabControl"))->setSelectedTab("FOTab/Properties");
	static_cast<CEGUI::TabControl*>(win_mgr.getWindow("MonsterTabControl"))->setSelectedTab("MonsterTab/Properties");

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
	
	
	// texture that is creates from the camera image
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
	label->subscribeEvent(CEGUI::Window::EventMouseButtonDown, CEGUI::Event::Subscriber(&ContentEditor::onPreviewWindowMouseDown, this));
	label->subscribeEvent(CEGUI::Window::EventMouseLeaves, CEGUI::Event::Subscriber(&ContentEditor::onPreviewWindowMouseUp, this));
	label->subscribeEvent(CEGUI::Window::EventMouseButtonUp, CEGUI::Event::Subscriber(&ContentEditor::onPreviewWindowMouseUp, this));
	label->subscribeEvent(CEGUI::Window::EventMouseWheel, CEGUI::Event::Subscriber(&ContentEditor::onPreviewWindowScrollWheel, this));
	
	CEGUI::PushButton* closebutton = static_cast<CEGUI::PushButton*>(win_mgr.getWindow("CloseButton"));
	closebutton->subscribeEvent(CEGUI::PushButton::EventClicked, CEGUI::Event::Subscriber(&ContentEditor::onClose, this));
	
	closebutton = static_cast<CEGUI::FrameWindow*>(m_rootWindow)->getCloseButton();
	closebutton->subscribeEvent(CEGUI::PushButton::EventClicked, CEGUI::Event::Subscriber(&ContentEditor::onClose, this));
	
	m_components["FOEditor"] = new FixedObjectEditor();
	m_components["RIEditor"] = new RenderInfoEditor();
	m_components["ItemEditor"] = new ItemEditor();
	m_components["MonsterEditor"] = new MonsterEditor();
	
	std::map<std::string, ContentEditorTab*>::iterator it;
	for (it = m_components.begin(); it != m_components.end(); ++it)
	{
		it->second->init(m_rootWindow);
	}
	
	if(!visible)
		m_rootWindow->setVisible(visible);

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
		toggleVisibility();
	
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
	CEGUI::WindowManager& win_mgr = CEGUI::WindowManager::getSingleton();
	CEGUI::Window* label;

	textdomain("tools");

	label = win_mgr.getWindow("RenderInfoTab");
	label->setText((CEGUI::utf8*) gettext("Render Info"));
	
	label = win_mgr.getWindow("RITab/BasicMesh");
	label->setText((CEGUI::utf8*) gettext("Mesh"));
	
	label = win_mgr.getWindow("RITab/BM/MainMeshLabel");
	label->setText((CEGUI::utf8*) gettext("Select main mesh:"));
	
	label = win_mgr.getWindow("RITab/SubMesh");
	label->setText((CEGUI::utf8*) gettext("Submeshes"));
	
	label = win_mgr.getWindow("RITab/SubMesh/AddSubMeshButton");
	label->setText((CEGUI::utf8*) gettext("Add Submesh"));
	
	label = win_mgr.getWindow("RITab/SM/NewSubmeshLabel");
	label->setText((CEGUI::utf8*) gettext("New submesh:"));
	
	label = win_mgr.getWindow("RITab/SM/NewSMNameLabel");
	label->setText((CEGUI::utf8*) gettext("submesh name:"));
	
	label = win_mgr.getWindow("RITab/SM/EditSMLabel");
	label->setText((CEGUI::utf8*) gettext("Edit submesh:"));
	
	label = win_mgr.getWindow("RITab/SM/AttachMeshLabel");
	label->setText((CEGUI::utf8*) gettext("Attach to:"));
	
	label = win_mgr.getWindow("RITab/SM/BoneLabel");
	label->setText((CEGUI::utf8*) gettext("at bone:"));
	
	label = win_mgr.getWindow("RITab/SM/RotateLabel");
	label->setText((CEGUI::utf8*) gettext("Rotate(x,y,z):"));
	
	label = win_mgr.getWindow("RITab/SM/ScaleLabel");
	label->setText((CEGUI::utf8*) gettext("Scale:"));
	
	label = win_mgr.getWindow("RITab/SM/OffsetLabel");
	label->setText((CEGUI::utf8*) gettext("Offset (x,y,z):"));
	
	label = win_mgr.getWindow("RITab/SubMesh/DelSubMeshButton");
	label->setText((CEGUI::utf8*) gettext("Delete Submesh"));
	
	label = win_mgr.getWindow("RITab/Animations");
	label->setText((CEGUI::utf8*) gettext("Animations"));
	
	label = win_mgr.getWindow("RITab/XML");
	label->setText((CEGUI::utf8*) gettext("XML"));
	
	label = win_mgr.getWindow("RITab/XML/SubmitButton");
	label->setText((CEGUI::utf8*) gettext("Submit XML"));
	
	label = win_mgr.getWindow("FixedObjectTab");
	label->setText((CEGUI::utf8*) gettext("FixedObject"));
	
	label = win_mgr.getWindow("FOTab/Properties");
	label->setText((CEGUI::utf8*) gettext("Properties"));
	
	label = win_mgr.getWindow("FOTab/Prop/ShapeLabel");
	label->setText((CEGUI::utf8*) gettext("Shape:"));
	
	label = win_mgr.getWindow("FOTab/Prop/CircleLabel");
	label->setText((CEGUI::utf8*) gettext("Circle"));
	
	label = win_mgr.getWindow("FOTab/Prop/RadiusLabel");
	label->setText((CEGUI::utf8*) gettext("Radius:"));
	
	label = win_mgr.getWindow("FOTab/Prop/DetectCircleButton");
	label->setText((CEGUI::utf8*) gettext("Autodetect"));
	
	label = win_mgr.getWindow("FOTab/Prop/RectangleLabel");
	label->setText((CEGUI::utf8*) gettext("Rectangle"));
	
	label = win_mgr.getWindow("FOTab/Prop/WidthLabel");
	label->setText((CEGUI::utf8*) gettext("Width:"));
	
	label = win_mgr.getWindow("FOTab/Prop/DepthLabel");
	label->setText((CEGUI::utf8*) gettext("Depth:"));
	
	label = win_mgr.getWindow("FOTab/Prop/DetectRectButton");
	label->setText((CEGUI::utf8*) gettext("Autodetect"));
	
	label = win_mgr.getWindow("FOTab/Prop/CollisionLabel");
	label->setText((CEGUI::utf8*) gettext("Collision type"));
	
	label = win_mgr.getWindow("FOTab/Properties/CopyDataLabel");
	label->setText((CEGUI::utf8*) gettext("Copy data from:"));
	
	label = win_mgr.getWindow("FOTab/Properties/CopyDataButton");
	label->setText((CEGUI::utf8*) gettext("Copy"));
	
	label = win_mgr.getWindow("FOTab/Create");
	label->setText((CEGUI::utf8*) gettext("Create Object"));
	
	label = win_mgr.getWindow("FOTab/XML/CreateButton");
	label->setText((CEGUI::utf8*) gettext("Create Object"));
	
	label = win_mgr.getWindow("FOTab/Create/PosLabel");
	label->setText((CEGUI::utf8*) gettext("Position:"));
	
	label = win_mgr.getWindow("FOTab/Create/GetPlPosButton");
	label->setText((CEGUI::utf8*) gettext("Get Player Position"));
	
	label = win_mgr.getWindow("FOTab/Create/AngleLabel");
	label->setText((CEGUI::utf8*) gettext("Angle:"));
	
	label = win_mgr.getWindow("FOTab/Create/DelAllButton");
	label->setText((CEGUI::utf8*) gettext("Delete all objects"));
	
	label = win_mgr.getWindow("FOTab/XML");
	label->setText((CEGUI::utf8*) gettext("XML"));
	
	label = win_mgr.getWindow("FOTab/XML/SubmitButton");
	label->setText((CEGUI::utf8*) gettext("Submit XML"));
	
	label = win_mgr.getWindow("ItemTab");
	label->setText((CEGUI::utf8*) gettext("Item"));
	
	label = win_mgr.getWindow("ItemTab/General");
	label->setText((CEGUI::utf8*) gettext("General"));
	
	label = win_mgr.getWindow("ItemTab/Properties/TypeLabel");
	label->setText((CEGUI::utf8*) gettext("Type:"));
	
	label = win_mgr.getWindow("ItemTab/Properties/SizeLabel");
	label->setText((CEGUI::utf8*) gettext("Size:"));
	
	label = win_mgr.getWindow("ItemTab/Properties/CopyDataLabel");
	label->setText((CEGUI::utf8*) gettext("Copy data from:"));
	
	label = win_mgr.getWindow("ItemTab/Properties/CopyDataButton");
	label->setText((CEGUI::utf8*) gettext("Copy"));
	
	label = win_mgr.getWindow("ItemTab/General/NameLabel");
	label->setText((CEGUI::utf8*) gettext("Name:"));
	
	label = win_mgr.getWindow("ItemTab/General/PriceLabel");
	label->setText((CEGUI::utf8*) gettext("Value:"));
	
	label = win_mgr.getWindow("ItemTab/General/EnchantRangeLabel");
	label->setText((CEGUI::utf8*) gettext("Enchant min:"));
	
	label = win_mgr.getWindow("ItemTab/General/EnchantMaxLabel");
	label->setText((CEGUI::utf8*) gettext("max:"));
	
	label = win_mgr.getWindow("ItemTab/Equip");
	label->setText((CEGUI::utf8*) gettext("Equip"));
	
	label = win_mgr.getWindow("ItemTab/Equip/HealthLabel");
	label->setText((CEGUI::utf8*) gettext("Health:"));
	
	label = win_mgr.getWindow("ItemTab/Equip/AttrLabel");
	label->setText((CEGUI::utf8*) gettext("Attribute:"));
	
	label = win_mgr.getWindow("ItemTab/Equip/StrengthLabel");
	label->setText((CEGUI::utf8*) gettext("Strength:"));
	
	label = win_mgr.getWindow("ItemTab/Equip/DexterityLabel");
	label->setText((CEGUI::utf8*) gettext("Dexterity:"));
	
	label = win_mgr.getWindow("ItemTab/Equip/MagicPowerLabel");
	label->setText((CEGUI::utf8*) gettext("Magic Power:"));
	
	label = win_mgr.getWindow("ItemTab/Equip/WillpowerLabel");
	label->setText((CEGUI::utf8*) gettext("Willpower:"));
	
	label = win_mgr.getWindow("ItemTab/Equip/ResistancesLabel");
	label->setText((CEGUI::utf8*) gettext("Resistances:"));
	
	label = win_mgr.getWindow("ItemTab/Equip/PhysResLabel");
	label->setText((CEGUI::utf8*) gettext("Physical:"));
	
	label = win_mgr.getWindow("ItemTab/Equip/FireResLabel");
	label->setText((CEGUI::utf8*) gettext("Fire:"));
	
	label = win_mgr.getWindow("ItemTab/Equip/IceResLabel");
	label->setText((CEGUI::utf8*) gettext("Ice:"));
	
	label = win_mgr.getWindow("ItemTab/Equip/AirResLabel");
	label->setText((CEGUI::utf8*) gettext("Air:"));
	
	label = win_mgr.getWindow("ItemTab/Equip/OtherLabel");
	label->setText((CEGUI::utf8*) gettext("Other:"));
	
	label = win_mgr.getWindow("ItemTab/Equip/ArmorLabel");
	label->setText((CEGUI::utf8*) gettext("Armor:"));
	
	label = win_mgr.getWindow("ItemTab/Equip/BlockLabel");
	label->setText((CEGUI::utf8*) gettext("Block:"));
	
	label = win_mgr.getWindow("ItemTab/Equip/AttackLabel");
	label->setText((CEGUI::utf8*) gettext("Precision:"));
	
	label = win_mgr.getWindow("ItemTab/Equip/PowerLabel");
	label->setText((CEGUI::utf8*) gettext("Power:"));
	
	label = win_mgr.getWindow("ItemTab/Weapon");
	label->setText((CEGUI::utf8*) gettext("Weapon"));
	
	label = win_mgr.getWindow("ItemTab/Weapon/TypeLabel");
	label->setText((CEGUI::utf8*) gettext("Type:"));
	
	label = win_mgr.getWindow("ItemTab/Weapon/DamageLabel");
	label->setText((CEGUI::utf8*) gettext("Damage:"));
	
	label = win_mgr.getWindow("ItemTab/Weapon/PhysLabel");
	label->setText((CEGUI::utf8*) gettext("Physical:"));
	
	label = win_mgr.getWindow("ItemTab/Weapon/PhysToLabel");
	label->setText((CEGUI::utf8*) gettext("-"));
	
	label = win_mgr.getWindow("ItemTab/Weapon/PhysMultLabel");
	label->setText((CEGUI::utf8*) gettext("x"));
	
	label = win_mgr.getWindow("ItemTab/Weapon/FireLabel");
	label->setText((CEGUI::utf8*) gettext("Fire:"));
	
	label = win_mgr.getWindow("ItemTab/Weapon/FireToLabel");
	label->setText((CEGUI::utf8*) gettext("-"));
	
	label = win_mgr.getWindow("ItemTab/Weapon/FireMultLabel");
	label->setText((CEGUI::utf8*) gettext("x"));
	
	label = win_mgr.getWindow("ItemTab/Weapon/IceLabel");
	label->setText((CEGUI::utf8*) gettext("Ice:"));
	
	label = win_mgr.getWindow("ItemTab/Weapon/IceToLabel");
	label->setText((CEGUI::utf8*) gettext("-"));
	
	label = win_mgr.getWindow("ItemTab/Weapon/IceMultLabel");
	label->setText((CEGUI::utf8*) gettext("x"));
	
	label = win_mgr.getWindow("ItemTab/Weapon/AirLabel");
	label->setText((CEGUI::utf8*) gettext("Air:"));
	
	label = win_mgr.getWindow("ItemTab/Weapon/AirToLabel");
	label->setText((CEGUI::utf8*) gettext("-"));
	
	label = win_mgr.getWindow("ItemTab/Weapon/AirMultLabel");
	label->setText((CEGUI::utf8*) gettext("x"));
	
	label = win_mgr.getWindow("ItemTab/Weapon/PrecisionLabel");
	label->setText((CEGUI::utf8*) gettext("Precision:"));
	
	label = win_mgr.getWindow("ItemTab/Weapon/PowerLabel");
	label->setText((CEGUI::utf8*) gettext("Power:"));
	
	label = win_mgr.getWindow("ItemTab/Weapon/RangeLabel");
	label->setText((CEGUI::utf8*) gettext("Range:"));
	
	label = win_mgr.getWindow("ItemTab/Weapon/SpeedLabel");
	label->setText((CEGUI::utf8*) gettext("Speed:"));
	
	label = win_mgr.getWindow("ItemTab/Weapon/TwohandedLabel");
	label->setText((CEGUI::utf8*) gettext("Twohanded weapon"));
	
	label = win_mgr.getWindow("ItemTab/Weapon/CritPercentLabel");
	label->setText((CEGUI::utf8*) gettext("Crit. Hits:"));
	
	label = win_mgr.getWindow("ItemTab/Consume");
	label->setText((CEGUI::utf8*) gettext("Consume"));
	
	label = win_mgr.getWindow("ItemTab/Consume/TypeLabel");
	label->setText((CEGUI::utf8*) gettext("Health:"));
	
	label = win_mgr.getWindow("ItemTab/Consume/CureLabel");
	label->setText((CEGUI::utf8*) gettext("Status heal and immunity time:"));
	
	label = win_mgr.getWindow("ItemTab/Consume/BlindLabel");
	label->setText((CEGUI::utf8*) gettext("Blind:"));
	
	label = win_mgr.getWindow("ItemTab/Consume/PoisonedLabel");
	label->setText((CEGUI::utf8*) gettext("Poisoned:"));
	
	label = win_mgr.getWindow("ItemTab/Consume/BerserkLabel");
	label->setText((CEGUI::utf8*) gettext("Berserk:"));
	
	label = win_mgr.getWindow("ItemTab/Consume/ConfusedLabel");
	label->setText((CEGUI::utf8*) gettext("Confused:"));
	
	label = win_mgr.getWindow("ItemTab/Consume/MuteLabel");
	label->setText((CEGUI::utf8*) gettext("Mute:"));
	
	label = win_mgr.getWindow("ItemTab/Consume/ParalyzedLabel");
	label->setText((CEGUI::utf8*) gettext("Paralyzed:"));
	
	label = win_mgr.getWindow("ItemTab/Consume/FrozenLabel");
	label->setText((CEGUI::utf8*) gettext("Frozen:"));
	
	label = win_mgr.getWindow("ItemTab/Consume/BurningLabel");
	label->setText((CEGUI::utf8*) gettext("Burning:"));
	
	label = win_mgr.getWindow("ItemTab/Create");
	label->setText((CEGUI::utf8*) gettext("Create Item"));
	
	label = win_mgr.getWindow("ItemTab/Create/EnchantLabel");
	label->setText((CEGUI::utf8*) gettext("Enchant:"));
	
	label = win_mgr.getWindow("ItemTab/Create/CreateInventoryButton");
	label->setText((CEGUI::utf8*) gettext("Create in inventory"));
	
	label = win_mgr.getWindow("ItemTab/Create/CreateDropButton");
	label->setText((CEGUI::utf8*) gettext("Create an drop"));
	
	label = win_mgr.getWindow("ItemTab/XML");
	label->setText((CEGUI::utf8*) gettext("XML"));
	
	label = win_mgr.getWindow("ItemTab/XML/SubmitButton");
	label->setText((CEGUI::utf8*) gettext("Submit XML"));
	
	label = win_mgr.getWindow("CloseButton");
	label->setText((CEGUI::utf8*) gettext("Close"));
	
	textdomain("menu");
}

bool ContentEditor::onPreviewWindowMouseDown(const CEGUI::EventArgs& evt)
{
	Ogre::TexturePtr tex = Ogre::TextureManager::getSingleton().getByName("editor_tex");
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
	Ogre::TexturePtr tex = Ogre::TextureManager::getSingleton().getByName("editor_tex");
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
	Ogre::TexturePtr tex = Ogre::TextureManager::getSingleton().getByName("editor_tex");
	tex.getPointer()->getBuffer()->getRenderTarget()->update();

	return true;
}


ContentEditor* ContentEditor::getSingletonPtr(void)
{
	return ms_Singleton;
}

ContentEditor& ContentEditor::getSingleton(void)
{
	assert( ms_Singleton );
	return ( *ms_Singleton );
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

