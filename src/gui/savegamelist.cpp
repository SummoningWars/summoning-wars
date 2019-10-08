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

#include "OgreResourceGroupManager.h"
#include "OgreTextureManager.h"
#include "CEGUI/RendererModules/Ogre/CEGUIOgreRenderer.h"
#include "savegamelist.h"
#include "stdstreamconv.h"

SavegameList::SavegameList (Document* doc)
	:Window(doc)
{
	DEBUGX("setup main menu");
	// GUI Elemente erzeugen

	CEGUI::WindowManager& win_mgr = CEGUI::WindowManager::getSingleton();
	
	m_currentSelected = 0;

	// Rahmen fuer das Menue Savegame auswaehlen
	CEGUI::FrameWindow* save_menu = (CEGUI::FrameWindow*) win_mgr.createWindow("TaharezLook/FrameWindow", "SavegameMenu");
	save_menu->setPosition(CEGUI::UVector2(cegui_reldim(0.0f), cegui_reldim( 0.0f))); //0.0/0.8
	save_menu->setSize(CEGUI::UVector2(cegui_reldim(1.0f), cegui_reldim( 1.0f))); //1.0/0.2
	save_menu->setProperty("FrameEnabled","false");
	save_menu->setProperty("TitlebarEnabled","false");
	save_menu->setProperty("CloseButtonEnabled","false");
	save_menu->setAlpha(0.0f);
	save_menu->setInheritsAlpha(false);
	
	m_window = save_menu;

	// Bestandteile der Kontrollleiste hinzufuegen
	CEGUI::PushButton* btn;
	//CEGUI::Window* label;// detected as unused, 2011.10.23.

	
	// Button neu
	btn = static_cast<CEGUI::PushButton*>(win_mgr.createWindow("TaharezLook/Button", "NewCharButton"));
	save_menu->addChildWindow(btn);
	btn->setPosition(CEGUI::UVector2(cegui_reldim(0.1f), cegui_reldim( 0.85f)));
	btn->setSize(CEGUI::UVector2(cegui_reldim(0.4f), cegui_reldim( 0.05f)));
	btn->subscribeEvent(CEGUI::PushButton::EventClicked, CEGUI::Event::Subscriber(&SavegameList::onNewCharClicked, this));
	btn->setWantsMultiClickEvents(false);
	btn->setInheritsAlpha(false);
	btn->setProperty("NormalImage", "set:MainMenu image:SPBtnNormal");
	btn->setProperty("DisabledImage", "set:MainMenu image:SPBtnNormal");
	btn->setProperty("HoverImage", "set:MainMenu image:SPBtnHover");
	btn->setProperty("PushedImage", "set:MainMenu image:SPBtnPushed");
	
	updateTranslation();
}

void SavegameList::update()
{	
	CEGUI::WindowManager& win_mgr = CEGUI::WindowManager::getSingleton();

	// Liste aller Files im Save Ordner der Form *.sav
	Ogre::FileInfoListPtr files;
	Ogre::FileInfoList::iterator it;
	files = Ogre::ResourceGroupManager::getSingleton().findResourceFileInfo("Savegame","*.sav");
	
	std::fstream file;
	char bin;
	int n = 0;
	char lev;
	
	std::string name;
	std::string classname;
	std::ostringstream stream;
	std::string filename;
	PlayerLook look;
	int version;
	// iterieren ueber die Files
	unsigned char* data;

	float height = m_window->getPixelSize().d_width / 4.0f;

	for (it = files->begin(); it!= files->end();++it)
	{
		filename = it->archive->getName();
		filename += "/";
		filename += it->filename;
		DEBUGX("file found %s",filename.c_str());
		//File oeffnen

		file.open(filename.c_str(),std::ios::in| std::ios::binary);
		if (file.is_open())
		{
			std::ostringstream s;
			s << n;

			CEGUI::Window* saveItem = 0;
			try
			{
				saveItem = win_mgr.getWindow(s.str().append("SaveItemRoot"));
				m_currentSelected = saveItem;
			}
			catch(CEGUI::UnknownObjectException&)
			{
				saveItem = (CEGUI::Window*) win_mgr.loadWindowLayout("SaveItem.layout", s.str());
				m_currentSelected = saveItem;
				saveItem->subscribeEvent(CEGUI::PushButton::EventClicked, CEGUI::Event::Subscriber(&SavegameList::onSavegameChosen, this));
				m_window->addChildWindow(saveItem);
				
				// make buttons resolution independant
				saveItem->setPosition(CEGUI::UVector2(cegui_reldim(0.0f), cegui_absdim((height + 2.0f)*n)));
				saveItem->setSize(CEGUI::UVector2(cegui_reldim(1.0f), cegui_absdim(height)));
				saveItem->getChild(s.str().append("SaveItemRoot/Name"))->setMousePassThroughEnabled(true);
				saveItem->getChild(s.str().append("SaveItemRoot/DecriptionLabel"))->setMousePassThroughEnabled(true);
				saveItem->getChild(s.str().append("SaveItemRoot/Avatar"))->setMousePassThroughEnabled(true);
				saveItem->getChild(s.str().append("SaveItemRoot/DelChar"))->subscribeEvent(CEGUI::PushButton::EventClicked, CEGUI::Event::Subscriber(&SavegameList::onDeleteCharClicked, this));	
			}
			
			// set the proper Image
			std::string texName = filename.erase(filename.length() - 4, filename.length());
			Ogre::TextureManager *tmgr = Ogre::TextureManager::getSingletonPtr();

			int pos = texName.find_last_of("\\");
			if (texName.find_last_of("/") > (size_t)pos)
			{
				pos = texName.find_last_of("/");
			}

			std::string nameNoPath = texName.erase(0, pos+1).append(".png");
			std::string imagesetName = texName + "SaveItemRootAvatarImageset";
			std::stringstream tempStream;
			tempStream << "set:" << imagesetName << " " << "image:MainMenuAvatarImg";
					
			if (CEGUI::ImagesetManager::getSingleton().isDefined(imagesetName))
			{
				saveItem->getChild(s.str().append("SaveItemRoot/Avatar"))->setProperty("Image", tempStream.str());
			}
			else
			{
				// create CEGUI texture for the character thumbnail
				if(Ogre::ResourceGroupManager::getSingleton().resourceExists("Savegame", nameNoPath))
				{
					Ogre::TexturePtr tex = tmgr->load(texName, "Savegame");

					CEGUI::Texture &ceguiTex = static_cast<CEGUI::OgreRenderer*>(CEGUI::System::getSingleton().getRenderer())->createTexture(tex);

					CEGUI::Imageset& textureImageSet = CEGUI::ImagesetManager::getSingleton().create(imagesetName, ceguiTex);
					textureImageSet.defineImage( "MainMenuAvatarImg",
								CEGUI::Point( 0.0f, 0.0f ),
								CEGUI::Size( ceguiTex.getSize().d_width, ceguiTex.getSize().d_height ),
								CEGUI::Point( 0.0f, 0.0f ) );
					
					saveItem->getChild(s.str().append("SaveItemRoot/Avatar"))->setProperty("Image", tempStream.str());
				}
				else
				{
					saveItem->getChild(s.str().append("SaveItemRoot/Avatar"))->setProperty("Image","set:MainMenu image:Logo");
				}
			}

			file.get(bin);
			
			CharConv* save;
			data =0;
			if (bin == '0')
			{
				save = new StdStreamConv(&file);
			}
			else
			{
				// binary Savegames are not supported anymore
				file.close();
				continue;
			}
			
			save->fromBuffer(version);
			save->setVersion(version);
			
			save->fromBuffer(classname);
			save->fromBuffer(name);
			look.fromString(save);
			save->fromBuffer(lev);
			
			stream.str("");
			stream << (int) lev;


			CEGUI::String temp;
			temp.append((CEGUI::utf8*) gettext("Level")).append(" ").append(stream.str()).append(" ").append((CEGUI::utf8*) gettext(classname.c_str()));
			saveItem->getChild(s.str().append("SaveItemRoot/Name"))->setText(name);
			saveItem->getChild(s.str().append("SaveItemRoot/DecriptionLabel"))->setText(temp);

			n++;

			file.close();
			
			delete save;
			if (data != 0)
				 delete data;
		}
	}
	
	CEGUI::PushButton *btn = static_cast<CEGUI::PushButton*>(win_mgr.getWindow("NewCharButton"));
	btn->setPosition(CEGUI::UVector2(cegui_reldim(0.0f), cegui_absdim((height + 2.0f)*n+1)));
	btn->setSize(CEGUI::UVector2(cegui_reldim(1.0f), cegui_absdim(height)));
}

void SavegameList::updateTranslation()
{
	CEGUI::WindowManager& win_mgr = CEGUI::WindowManager::getSingleton();
	CEGUI::PushButton* btn;
	/*
	CEGUI::Window* label;
	label = win_mgr.getWindow("SavegameChooseLabel");
	label->setText((CEGUI::utf8*) gettext("Characters"));*/
	
	/*btn = static_cast<CEGUI::PushButton*>(win_mgr.getWindow("SelectSavegameButton"));
	btn->setText((CEGUI::utf8*) gettext("Ok"));*/
	
	btn = static_cast<CEGUI::PushButton*>(win_mgr.getWindow("NewCharButton"));
	btn->setText((CEGUI::utf8*) gettext("Create"));
}

bool SavegameList::onSavegameChosen(const CEGUI::EventArgs& evt)
{
	const CEGUI::WindowEventArgs& we = static_cast<const CEGUI::WindowEventArgs&>(evt);
	CEGUI::WindowManager& win_mgr = CEGUI::WindowManager::getSingleton();
	std::string prefix = we.window->getName().c_str();
	prefix.erase(prefix.length()-12, prefix.length());

	std::string name = we.window->getChild(prefix.append("SaveItemRoot/Name"))->getText().c_str();
	name.append(".sav");

	m_document->setSaveFile(name.c_str());
	DEBUGX("selected Savegame %s", sitm->m_data.c_str());

	return true;
}

bool SavegameList::onSavegameSelected(const CEGUI::EventArgs& evt)
{
	if (m_document->getSaveFile() != "")
	{
		DEBUG("savegame accepted %s", m_document->getSaveFile().c_str());
		m_document->getGUIState()->m_shown_windows = Document::START_MENU;
		m_document->setModified(Document::WINDOWS_MODIFIED);
	}
	return true;
}

bool SavegameList::onNewCharClicked(const CEGUI::EventArgs& evt)
{
	m_document->onCreateNewCharButton();
	return true;
}


bool SavegameList::onDeleteCharClicked(const CEGUI::EventArgs& evt)
{
	CEGUI::WindowManager& win_mgr = CEGUI::WindowManager::getSingleton();
	const CEGUI::WindowEventArgs& we = static_cast<const CEGUI::WindowEventArgs&>(evt);
	onSavegameChosen(CEGUI::WindowEventArgs(we.window->getParent()));

	CEGUI::FrameWindow* message = (CEGUI::FrameWindow*) win_mgr.getWindow("DeleteChar");
	message->setInheritsAlpha(false);
		
	message->setVisible(true);
	message->setModalState(true);

	return true;
}

bool SavegameList::onDeleteCharConfirmClicked(const CEGUI::EventArgs& evt)
{
	// Get the save file to remove.
	std::string saveFile =  m_document->getSaveFile ();
	if (saveFile.length () <= 0)
	{
		return false;
	}

	CEGUI::WindowManager& win_mgr = CEGUI::WindowManager::getSingleton();
	CEGUI::FrameWindow* message = (CEGUI::FrameWindow*) win_mgr.getWindow("DeleteChar");
	
	message->setVisible(false);
	message->setModalState(false);
	
	// Get rid of the save file
	remove (saveFile.c_str ());

	// Also get rif od the preview image.
	// The preview image should have the same name as the save file, but with a different extension.
	// Using the standard 3 char extension (4 if the "." is included).
	if (saveFile.length () > 4)
	{
		// Parse the name.
		std::string previewPicture (saveFile);
		previewPicture.erase (saveFile.length () - 4, saveFile.length());
		previewPicture.append (".png");

		// Get rid of the file
		remove (previewPicture.c_str ());
	}

	// Clear the selection in the menu.
	if(m_currentSelected != 0)
	{
		CEGUI::WindowManager::getSingleton().destroyWindow(m_currentSelected);
		m_currentSelected = 0;
	}
	m_document->setSaveFile("");
	
	update();
	return true;
}

bool SavegameList::onDeleteCharAbortClicked(const CEGUI::EventArgs& evt)
{
	CEGUI::WindowManager& win_mgr = CEGUI::WindowManager::getSingleton();
	CEGUI::FrameWindow* message = (CEGUI::FrameWindow*) win_mgr.getWindow("DeleteChar");
	
	message->setVisible(false);
	message->setModalState(false);
	return true;
}

bool SavegameList::onSavegameDoubleClick(const CEGUI::EventArgs& evt)
{
	onSavegameSelected(evt);
	return true;
}
