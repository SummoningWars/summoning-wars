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

#include "filebrowser.h"
#include "listitem.h"
#include "logger.h"

#include <list>
#include <fstream>
#include <iterator>
#include <vector>

#include <boost/filesystem.hpp>
using namespace boost;

using namespace CEGUI;

FileBrowser::~FileBrowser()
{
}

void FileBrowser::init(CEGUI::String defaultDir, FileBrowserType type, bool visible, const std::string& ceguiSkinName)
{
	m_ceguiSkinName = ceguiSkinName;
    m_currentPath = filesystem::current_path();

	m_type = type;
	m_guiSystem = System::getSingletonPtr();
	m_winManager = WindowManager::getSingletonPtr();
  m_gameScreen = CEGUIUtility::getWindow("GameScreen");

	m_rootWindow = CEGUIUtility::loadLayoutFromFile ("filebrowser.layout");
  CEGUIUtility::addChildWidget (m_gameScreen, m_rootWindow);
	m_rootWindow->setVisible(visible);

	m_acceptBtn = static_cast<PushButton*>(m_rootWindow->getChild("Ok"));
	m_cancelBtn = static_cast<PushButton*>(m_rootWindow->getChild("Cancel"));
	CEGUI::PushButton *btn = static_cast<PushButton*>(m_rootWindow->getChild("PopDir"));
    btn->subscribeEvent(CEGUI::PushButton::EventClicked, Event::Subscriber(&FileBrowser::handlePopDirectory, this));

	m_rootWindow->subscribeEvent(CEGUI::FrameWindow::EventCloseClicked, Event::Subscriber(&FileBrowser::handleCloseWindow, this));

	switch(m_type)
	{
		case FB_TYPE_OPEN_FILE:
			m_rootWindow->setText("Open - Select file");
			m_acceptBtn->setText("Open");
			break;
		case FB_TYPE_SAVE_FILE:
			m_rootWindow->setText("Save - Select filename");
			m_acceptBtn->setText("Save");
			break;
		case FB_TYPE_SELECT_DIRECTORY:
			m_rootWindow->setText("Select directory");
			m_acceptBtn->setText("Select");
			break;
		default:
			break;
	}
	
	m_pathBox = static_cast<Editbox*>(m_rootWindow->getChild("CurrentPath"));
    m_pathBox->setText(m_currentPath.string());

	m_fileNameBox = static_cast<Editbox*>(m_rootWindow->getChild("FileName"));
	
	m_browserBox = static_cast<MultiColumnList*>(m_rootWindow->getChild("Browser"));
	m_browserBox->subscribeEvent(CEGUI::MultiColumnList::EventSelectionChanged, Event::Subscriber(&FileBrowser::handleSelectionChanged, this));
	m_browserBox->subscribeEvent(CEGUI::MultiColumnList::EventMouseDoubleClick, CEGUI::Event::Subscriber(&FileBrowser::handleBrowserDblClick, this));
	m_browserBox->addColumn("", 0, CEGUI::UDim(0.8, 0));
	m_browserBox->addColumn("Type", 1, CEGUI::UDim(0.2, 0));
	m_browserBox->setSelectionMode(CEGUI::MultiColumnList::RowSingle);

	std::cout << m_browserBox->isDestroyedByParent() << m_fileNameBox->isDestroyedByParent() << std::endl;
	std::cout << m_pathBox->isDestroyedByParent() << m_acceptBtn->isDestroyedByParent() << std::endl;

	fillBrowser();
}

void FileBrowser::destroy()
{
	m_winManager->destroyWindow(m_rootWindow);
}


void FileBrowser::fillBrowser()
{
	m_dirs.clear();
	m_files.clear();
	m_browserBox->resetList();
    std::cout << "   " << m_currentPath.string() << '\n';

    try
    {
        if(exists(m_currentPath))
        {
                std::vector<filesystem::path> vec;

                copy(filesystem::directory_iterator(m_currentPath), filesystem::directory_iterator(), back_inserter(vec));

                sort(vec.begin(), vec.end());

                for (std::vector<filesystem::path>::const_iterator it (vec.begin()); it != vec.end(); ++it)
                {
                    filesystem::path p = (filesystem::path)(*it);
                    if(filesystem::is_directory(*it))
                        m_dirs.push_back((*it).string());
                    else
                        m_files.push_back((*it).string());
                }
        }
        else
            Logger::getSingleton().logEvent(m_currentPath.string() + " does not exist\n");
    }
    catch (const filesystem::filesystem_error& ex)
    {
        Logger::getSingleton().logEvent(ex.what());
    }


	std::list<std::string>::iterator sit;
	for (sit=m_dirs.begin(); sit!=m_dirs.end(); ++sit)
	{
		int id = m_browserBox->addRow();
		StrListItem* ite1 = new StrListItem ((CEGUI::utf8*) m_ceguiSkinName.c_str (), *sit, "2");
		StrListItem* ite2 = new StrListItem ((CEGUI::utf8*) m_ceguiSkinName.c_str (), "Directory", "4");
		m_browserBox->setItem(ite1, 0, id);
		m_browserBox->setItem(ite2, 1, id);
	}
	for (sit=m_files.begin(); sit!=m_files.end(); ++sit)
	{
		int id = m_browserBox->addRow();
		StrListItem* ite1 = new StrListItem ((CEGUI::utf8*) m_ceguiSkinName.c_str (), *sit, "f2");
		StrListItem* ite2 = new StrListItem ((CEGUI::utf8*) m_ceguiSkinName.c_str (), "File", "f4");
		m_browserBox->setItem(ite1, 0, id);
		m_browserBox->setItem(ite2, 1, id);
	}
}


bool FileBrowser::handleBrowserDblClick(const CEGUI::EventArgs &e)
{
	try
	{
        m_currentPath = filesystem::path(m_browserBox->getFirstSelectedItem()->getText().c_str());

        if(filesystem::is_regular_file(m_currentPath))
            m_currentPath = m_currentPath.root_path();
		else
        {
			m_pathBox->setText(m_currentPath.string());
			fillBrowser();
        }
	}
    catch (const filesystem::filesystem_error& ex)
    {
        Logger::getSingleton().logEvent(ex.what());
    }

	return true;
}

bool FileBrowser::handlePopDirectory(const CEGUI::EventArgs &e)
{
	try
	{
        m_currentPath = m_currentPath.parent_path();
        m_pathBox->setText(m_currentPath.string());
		fillBrowser();
	}
    catch (const filesystem::filesystem_error& ex)
    {
        Logger::getSingleton().logEvent(ex.what());
    }

	return true;
}

bool FileBrowser::handleSelectionChanged(const CEGUI::EventArgs& e)
{
	try
	{
		CEGUI::String selectedItem = m_browserBox->getFirstSelectedItem()->getText();
        filesystem::path tempPath(m_currentPath.string() + selectedItem.c_str());

        if(filesystem::is_regular_file(tempPath))
			m_fileNameBox->setText(selectedItem);
		else
			m_fileNameBox->setText("");
		return true;
	}
    catch (const filesystem::filesystem_error& ex)
    {
        Logger::getSingleton().logEvent(ex.what());
        return true;
    }
}


CEGUI::String FileBrowser::getCurrentSelected()
{
    try
    {
        std::string fileName = m_fileNameBox->getText().c_str();
        filesystem::path tempPath(fileName);;

        switch(m_type)
		{
			case FB_TYPE_OPEN_FILE:
                if(filesystem::is_regular_file(tempPath) && filesystem::exists(tempPath))
                    return tempPath.string();
				else
					return "";
			case FB_TYPE_SAVE_FILE:
                if(!filesystem::exists(tempPath))
                    return tempPath.string();
				else
                    // TODO file exists already, ask the user if he wants to overwrite the file
                    return tempPath.string();
			case FB_TYPE_SELECT_DIRECTORY:
                return m_currentPath.string();
			default:
				return "";
		}
		return "";
	}
    catch (const filesystem::filesystem_error& ex)
    {
        Logger::getSingleton().logEvent(ex.what());
        return "";
    }
}

bool FileBrowser::handleCloseWindow(const CEGUI::EventArgs& e)
{
	return true;
}
