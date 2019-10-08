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

#include "textfileeditwindow.h"
#include "CEGUI/CEGUI.h"
#include <iostream>
#include <fstream>
#include <string>

#include "Poco/String.h"

#define TAB "\267\267\267"

using namespace CEGUI;

CEGUI::String TextFileEditWindow::WidgetTypeName = "TextFileEditWindow";

TextFileEditWindow::TextFileEditWindow(const CEGUI::String& type, const CEGUI::String& name): Window(type, name)
{
	m_textEditBox = static_cast<MultiLineEditbox*>(WindowManager::getSingleton().createWindow("TaharezLook/MultiLineEditbox", name + "EditBox"));
	m_textEditBox->setPosition(UVector2(UDim(0.0f, 0.0f), UDim(0.0f, 0.0f)));
	m_textEditBox->setSize(UVector2(UDim(1.0f, 0.0f), UDim(1.0f, 0.0f)));
	addChildWindow(m_textEditBox);
	m_isDirty = false;
	m_file = Poco::File();
	m_fb = 0;
	
	m_textEditBox->subscribeEvent(MultiLineEditbox::EventCharacterKey, Event::Subscriber(&TextFileEditWindow::handleCharacterKey, this));
}

void TextFileEditWindow::close()
{
	save();
}

void TextFileEditWindow::getNewFileName()
{
	m_fb = new FileBrowser();
	m_fb->init("/home/stefan/Dev/s07c/sumwars", FileBrowser::FB_TYPE_SAVE_FILE, true);
	m_fb->m_acceptBtn->subscribeEvent(PushButton::EventClicked, CEGUI::Event::Subscriber(&TextFileEditWindow::handleFileBrowserSaveClicked, this));
	m_fb->m_cancelBtn->subscribeEvent(PushButton::EventClicked, CEGUI::Event::Subscriber(&TextFileEditWindow::handleFileBrowserCancelClicked, this));

	// Not in constructor to avoid the first event when initialy seting the Tab text
	m_handleTextChangedConnection = m_textEditBox->subscribeEvent(MultiLineEditbox::EventTextChanged, CEGUI::Event::Subscriber(&TextFileEditWindow::handleTextChanged, this));

}


bool TextFileEditWindow::load(const String &fileName)
{
	setFilepath(fileName.c_str());

	if(m_file.exists())
	{
		std::string s;
		std::string line;
		std::ifstream myfile;
		myfile.open(fileName.c_str());
		if (myfile.is_open())
		{
			s += '\t';
			while (! myfile.eof() )
			{
				std::getline (myfile,line);

				int pos =  line.find_last_of('\t');
				while(pos != line.npos)
				{
					line.erase(pos, 1);
					line.insert(pos, TAB);
					pos =  line.find_last_of('\t');
				}

				s += (line + "\n");
			}

#ifdef WIN32
			String::size_type pos = fileName.find_last_of("\\");
#else
			String::size_type pos = fileName.find_last_of("/");
#endif

			String name = fileName.substr(pos+1);
			setText(name);
			m_textEditBox->setText(s.c_str());
		}
		else
		{
			myfile.close();
			return false;
		}
	}
	
	// Not in constructor to avoid the first event when initialy seting the Tab text
	m_handleTextChangedConnection = m_textEditBox->subscribeEvent(MultiLineEditbox::EventTextChanged, CEGUI::Event::Subscriber(&TextFileEditWindow::handleTextChanged, this));

	return true;
}

void TextFileEditWindow::save()
{
	if(m_file.path() == "")
	{
		getNewFileName();
		return;
	}
	
	if(!m_isDirty)
		return;
	else
	{
		CEGUI::String s = m_textEditBox->getText();

		if (!m_file.exists())
			m_file.createFile();
		
		std::ofstream myfile;
		myfile.open(m_file.path().c_str());
		myfile.clear();
		int pos =  s.find(TAB);
		while(pos != s.npos)
		{
			s.erase(pos, 3);
			s.insert(pos, "\t");
			pos =  s.find(TAB);
		}
		myfile << s.c_str();
		myfile.close();
		std::string temp = getText().c_str();
		setText(Poco::replace(temp, " *", ""));
	}
}

bool TextFileEditWindow::handleTextChanged(const CEGUI::EventArgs& e)
{
	m_isDirty = true;
	String s = getText();

	if(s.find(" *") == s.npos)
		setText(s + " *");

	return true;
}

bool TextFileEditWindow::handleCharacterKey(const CEGUI::EventArgs& ee)
{
	KeyEventArgs e = static_cast<const KeyEventArgs&>(ee);

	if(e.codepoint == 9)
	{
		// Handle Tabs
		CEGUI::String s = m_textEditBox->getText();
		int pos = m_textEditBox->getSelectionStartIndex();
		s = s.insert(pos, TAB);
		m_textEditBox->setText(s);
		m_textEditBox->setCaratIndex(pos+3);
		return true;
	}
	else if(e.codepoint == 40)
	{
		// Handle (
		m_textEditBox->insertText(")", m_textEditBox->getCaratIndex());
	}
	else if(e.codepoint == 91)
	{
		// Handle [
		m_textEditBox->insertText("]", m_textEditBox->getCaratIndex());
	}
	else if(e.codepoint == 123)
	{
		// Handle {
		m_textEditBox->insertText("}", m_textEditBox->getCaratIndex());
	}
	
	return false;
}


void TextFileEditWindow::setFilepath(String path)
{
	m_file = Poco::File(path.c_str());

#ifdef WIN32
	String::size_type pos = m_file.path().find_last_of("\\");
#else
	String::size_type pos = m_file.path().find_last_of("/");
#endif

	String name = m_file.path().substr(pos+1);
	setText(name);
}

bool TextFileEditWindow::handleFileBrowserSaveClicked(const CEGUI::EventArgs& e)
{
	setFilepath(m_fb->getCurrentSelected());
	save();
	m_fb->destroy();
	delete m_fb;
	m_fb = 0;
	return true;
}

bool TextFileEditWindow::handleFileBrowserCancelClicked(const CEGUI::EventArgs& e)
{
	m_fb->destroy();
	delete m_fb;
	m_fb = 0;
	return true;
}












