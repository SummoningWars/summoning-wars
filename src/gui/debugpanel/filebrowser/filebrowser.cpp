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

#include "filebrowser.h"
#include "listitem.h"

#include <list>
#include <fstream>

#include "Poco/File.h"
#include "Poco/Path.h"
#include "Poco/DirectoryIterator.h"
#include "Poco/Exception.h"

using namespace CEGUI;

FileBrowser::~FileBrowser()
{
}

void FileBrowser::init(CEGUI::String defaultDir, FileBrowserType type, bool visible)
{
	m_currentPath = Poco::Path::current();

	m_type = type;
	m_guiSystem = System::getSingletonPtr();
	m_winManager = WindowManager::getSingletonPtr();
	m_gameScreen = m_winManager->getWindow("GameScreen");

	m_rootWindow = m_winManager->loadWindowLayout("FileBrowser.layout");
	m_gameScreen->addChildWindow(m_rootWindow);
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
	m_pathBox->setText(m_currentPath.toString());

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

	Poco::DirectoryIterator it(m_currentPath);
	Poco::DirectoryIterator end;

	while (it != end)
	{
		try
		{
			if (it->isFile())
				m_files.push_back(it.name());
			else
				m_dirs.push_back(it.name());
			++it;
		}
		catch (Poco::Exception& exc)
		{
			std::cout << exc.displayText() << std::endl;
			++it;
		}
	}


	m_dirs.sort();
	m_files.sort();

	std::list<std::string>::iterator sit;
	for (sit=m_dirs.begin(); sit!=m_dirs.end(); ++sit)
	{
		int id = m_browserBox->addRow();
		StrListItem* ite1 = new StrListItem(*sit, "2");
		StrListItem* ite2 = new StrListItem("Directory", "4");
		m_browserBox->setItem(ite1, 0, id);
		m_browserBox->setItem(ite2, 1, id);
	}
	for (sit=m_files.begin(); sit!=m_files.end(); ++sit)
	{
		int id = m_browserBox->addRow();
		StrListItem* ite1 = new StrListItem(*sit, "f2");
		StrListItem* ite2 = new StrListItem("File", "f4");
		m_browserBox->setItem(ite1, 0, id);
		m_browserBox->setItem(ite2, 1, id);
	}
}


bool FileBrowser::handleBrowserDblClick(const CEGUI::EventArgs &e)
{
	try
	{
		m_currentPath.pushDirectory(m_browserBox->getFirstSelectedItem()->getText().c_str());
		Poco::File f(m_currentPath);
		if(f.isFile())
			m_currentPath.popDirectory();
		else
			m_pathBox->setText(m_currentPath.toString());
			fillBrowser();
	}
	catch (Poco::Exception& exc)
	{
		std::cout << exc.displayText() << std::endl;
		return true;
	}
	return true;
}

bool FileBrowser::handlePopDirectory(const CEGUI::EventArgs &e)
{
	try
	{
		m_currentPath.popDirectory();
		m_pathBox->setText(m_currentPath.toString());
		fillBrowser();
	}
	catch (Poco::Exception& exc)
	{
		std::cout << exc.displayText() << std::endl;
		return true;
	}
	return true;
}

bool FileBrowser::handleSelectionChanged(const CEGUI::EventArgs& e)
{
	try
	{
		Poco::Path tempPath(m_currentPath);
		CEGUI::String selectedItem = m_browserBox->getFirstSelectedItem()->getText();
		tempPath.pushDirectory(selectedItem.c_str());
		Poco::File file(tempPath);

		if(file.isFile())
			m_fileNameBox->setText(selectedItem);
		else
			m_fileNameBox->setText("");
		return true;
	}
	catch (Poco::Exception& exc)
	{
		std::cout << exc.displayText() << std::endl;
		return true;
	}
}


CEGUI::String FileBrowser::getCurrentSelected()
{
	try
	{
		Poco::Path tempPath(m_currentPath);
		std::string fileName = m_fileNameBox->getText().c_str();

		if(fileName != "")
			tempPath.pushDirectory(fileName.c_str());

		Poco::File f(tempPath);

		switch(m_type)
		{
			case FB_TYPE_OPEN_FILE:
				if(f.isFile() && f.exists())
					return f.path();
				else
					return "";
			case FB_TYPE_SAVE_FILE:
				if(!f.exists())
				{
					return tempPath.toString();
				}
				else
				{
					// TODO file exists already, ask the user if he wants to overwrite the file
					return tempPath.toString();
				}
			case FB_TYPE_SELECT_DIRECTORY:
				return m_currentPath.toString();
			default:
				return "";
		}
		return "";
	}
	catch (Poco::Exception& exc)
	{
		std::cout << exc.displayText() << std::endl;
		return "";
	}
}

bool FileBrowser::handleCloseWindow(const CEGUI::EventArgs& e)
{
	return true;
}
