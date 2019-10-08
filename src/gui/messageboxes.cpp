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

#include "messageboxes.h"

SaveExitWindow::SaveExitWindow (Document* doc)
	:Window(doc)
{
	CEGUI::WindowManager& win_mgr = CEGUI::WindowManager::getSingleton();
	CEGUI::PushButton* btn;
	//CEGUI::Window* label; // 2011.10.23: found as unused.
	
	CEGUI::FrameWindow* save_exit = (CEGUI::FrameWindow*) win_mgr.loadWindowLayout("SaveExitWindow.layout");
	m_window = save_exit;
	
	btn = static_cast<CEGUI::PushButton*>(win_mgr.getWindow("GameExitConfirmButton"));
	btn->subscribeEvent(CEGUI::PushButton::EventClicked, CEGUI::Event::Subscriber(&SaveExitWindow ::onExitGameConfirmed, this));
	
	btn = static_cast<CEGUI::PushButton*>(win_mgr.getWindow("GameExitAbortButton"));
	btn->subscribeEvent(CEGUI::PushButton::EventClicked, CEGUI::Event::Subscriber(&SaveExitWindow ::onExitGameAborted, this));
	
	updateTranslation();
}

void SaveExitWindow::update()
{
}

void SaveExitWindow::updateTranslation()
{
	CEGUI::WindowManager& win_mgr = CEGUI::WindowManager::getSingleton();
	CEGUI::Window* label;
	
	CEGUI::PushButton* btn = static_cast<CEGUI::PushButton*>(win_mgr.getWindow( "GameExitConfirmButton"));
	btn->setText((CEGUI::utf8*) gettext("Ok"));
	
	btn = static_cast<CEGUI::PushButton*>(win_mgr.getWindow( "GameExitAbortButton"));
	btn->setText((CEGUI::utf8*) gettext("Abort"));
	
	label = win_mgr.getWindow("SaveExitLabel");
	label->setText((CEGUI::utf8*) gettext("Save and Exit?"));
}


bool SaveExitWindow::onExitGameConfirmed(const CEGUI::EventArgs& evt)
{
	m_document->onButtonSaveExitConfirm();
	return true;
}

bool SaveExitWindow::onExitGameAborted(const CEGUI::EventArgs& evt)
{
	m_document->onButtonSaveExitAbort();
	return true;
}

MessageQuestionWindow::MessageQuestionWindow (Document* doc, std::string name,std::string question, std::string button1,CEGUI::Event::Subscriber subscriber1,  std::string button2, CEGUI::Event::Subscriber subscriber2)
	:Window(doc)
{
	m_button1 = button1;
	m_button2 = button2;
	m_question = question;
	
	std::string wname = name;
	
	CEGUI::WindowManager& win_mgr = CEGUI::WindowManager::getSingleton();
	CEGUI::PushButton* btn;
	CEGUI::Window* label;
	
	CEGUI::FrameWindow* message = (CEGUI::FrameWindow*) win_mgr.createWindow("TaharezLook/StaticImage", wname);
	m_window = message;
	
	
	message->setPosition(CEGUI::UVector2(cegui_reldim(0.3f), cegui_reldim( 0.25f))); //0.0/0.8
	message->setSize(CEGUI::UVector2(cegui_reldim(0.4f), cegui_reldim( 0.3f))); //1.0/0.2
	message->setProperty("Image","set:Misc image:QuestionDialog");
	message->setProperty("FrameEnabled","false");
	message->setProperty("BackgroundEnabled","false");
	message->setVisible(false);
	
	wname = name;
	wname += "Button1";
	btn = static_cast<CEGUI::PushButton*>(win_mgr.createWindow("TaharezLook/Button", wname));
	message->addChildWindow(btn);
	btn->setProperty("NormalImage", "set:MainMenu image:SPBtnNormal"); 	 
	btn->setProperty("DisabledImage", "set:MainMenu image:SPBtnNormal"); 	 
	btn->setProperty("HoverImage", "set:MainMenu image:SPBtnHover"); 	 
	btn->setProperty("PushedImage", "set:MainMenu image:SPBtnPushed");
	btn->setPosition(CEGUI::UVector2(cegui_reldim(0.20f), cegui_reldim( 0.6f)));
	btn->setSize(CEGUI::UVector2(cegui_reldim(0.2f), cegui_reldim( 0.15f)));
	btn->subscribeEvent(CEGUI::PushButton::EventClicked, subscriber1);
	
	wname = name;
	wname += "Button2";
	btn = static_cast<CEGUI::PushButton*>(win_mgr.createWindow("TaharezLook/Button", wname));
	message->addChildWindow(btn);
	btn->setProperty("NormalImage", "set:MainMenu image:SPBtnNormal"); 	 
	btn->setProperty("DisabledImage", "set:MainMenu image:SPBtnNormal"); 	 
	btn->setProperty("HoverImage", "set:MainMenu image:SPBtnHover"); 	 
	btn->setProperty("PushedImage", "set:MainMenu image:SPBtnPushed");
	btn->setPosition(CEGUI::UVector2(cegui_reldim(0.60f), cegui_reldim( 0.6f)));
	btn->setSize(CEGUI::UVector2(cegui_reldim(0.2f), cegui_reldim( 0.15f)));
	btn->subscribeEvent(CEGUI::PushButton::EventClicked, subscriber2);
	
	wname = name;
	wname += "Label";
	label = win_mgr.createWindow("TaharezLook/StaticText", wname);
	message->addChildWindow(label);
	label->setProperty("FrameEnabled", "false");
	label->setProperty("BackgroundEnabled", "false");
	label->setProperty("HorzFormatting", "HorzCentred");
	label->setPosition(CEGUI::UVector2(cegui_reldim(0.120732f), cegui_reldim(0.327987f)));
	label->setSize(CEGUI::UVector2(cegui_reldim(0.768301f), cegui_reldim( 0.111321f)));	
	
	m_name = name;
	updateTranslation();
	
}

void MessageQuestionWindow::updateTranslation()
{
	CEGUI::WindowManager& win_mgr = CEGUI::WindowManager::getSingleton();
	CEGUI::Window* label;
	
	std::string wname = m_name;
	wname += "Button1";
	CEGUI::PushButton* btn = static_cast<CEGUI::PushButton*>(win_mgr.getWindow( wname));
	btn->setText((CEGUI::utf8*) gettext(m_button1.c_str()));
	
	wname = m_name;
	wname += "Button2";
	btn = static_cast<CEGUI::PushButton*>(win_mgr.getWindow( wname));
	btn->setText((CEGUI::utf8*) gettext(m_button2.c_str()));
	
	wname = m_name;
	wname += "Label";
	label = win_mgr.getWindow(wname);
	label->setText((CEGUI::utf8*) gettext(m_question.c_str()));
}

void MessageQuestionWindow::setQuestion(std::string question)
{
	m_question = question;
	updateTranslation();
}


WarningDialogWindow::WarningDialogWindow (Document* doc)
:Window(doc)
{
	CEGUI::WindowManager& win_mgr = CEGUI::WindowManager::getSingleton();
	CEGUI::PushButton* btn;
	//CEGUI::Window* label; // 2011.10.23: found as unused.
	
	CEGUI::FrameWindow* warning_dialog = (CEGUI::FrameWindow*) win_mgr.loadWindowLayout("WarningDialogWindow.layout");
	m_window = warning_dialog;
	m_warning = "";
	
	btn = static_cast<CEGUI::PushButton*>(win_mgr.getWindow("WarningDialogConfirmButton"));
	btn->subscribeEvent(CEGUI::PushButton::EventClicked, CEGUI::Event::Subscriber(&WarningDialogWindow ::onConfirmed, this));
	
	updateTranslation();
}

void WarningDialogWindow::update()
{
}

void WarningDialogWindow::updateTranslation()
{
	CEGUI::WindowManager& win_mgr = CEGUI::WindowManager::getSingleton();
	CEGUI::Window* label;
	
	CEGUI::PushButton* btn = static_cast<CEGUI::PushButton*>(win_mgr.getWindow( "WarningDialogConfirmButton"));
	btn->setText((CEGUI::utf8*) gettext("Ok"));
	
	label = win_mgr.getWindow("WarningDialogLabel");
	label->setText((CEGUI::utf8*) gettext(m_warning.c_str()));
}


bool WarningDialogWindow::onConfirmed(const CEGUI::EventArgs& evt)
{
	m_window->hide();
	m_window->setModalState(false);
	return true;
}

void WarningDialogWindow::setWarning(std::string warning)
{
	m_warning = warning;
	updateTranslation();
}


ErrorDialogWindow::ErrorDialogWindow (Document* doc)
:Window(doc)
{
	CEGUI::WindowManager& win_mgr = CEGUI::WindowManager::getSingleton();
	CEGUI::PushButton* btn;
	//CEGUI::Window* label; // 2011.10.23: found as unused.
	
	CEGUI::FrameWindow* error_dialog = (CEGUI::FrameWindow*) win_mgr.loadWindowLayout("ErrorDialogWindow.layout");
	m_window = error_dialog;
	m_error = "Network connection timed out";
	
	btn = static_cast<CEGUI::PushButton*>(win_mgr.getWindow("ErrorDialogConfirmButton"));
	btn->subscribeEvent(CEGUI::PushButton::EventClicked, CEGUI::Event::Subscriber(&ErrorDialogWindow ::onConfirmed, this));
	
	updateTranslation();
}

void ErrorDialogWindow::update()
{
}

void ErrorDialogWindow::updateTranslation()
{
	CEGUI::WindowManager& win_mgr = CEGUI::WindowManager::getSingleton();
	CEGUI::Window* label;
	
	CEGUI::PushButton* btn = static_cast<CEGUI::PushButton*>(win_mgr.getWindow( "ErrorDialogConfirmButton"));
	btn->setText((CEGUI::utf8*) gettext("Ok"));
	
	label = win_mgr.getWindow("ErrorDialogLabel");
	label->setText((CEGUI::utf8*) gettext(m_error.c_str()));
}


bool ErrorDialogWindow::onConfirmed(const CEGUI::EventArgs& evt)
{
	m_document->onButtonErrorDialogConfirm();
	return true;
}

void ErrorDialogWindow::setError(std::string error)
{
	m_error = error;
	updateTranslation();
}

