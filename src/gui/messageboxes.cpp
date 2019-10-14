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

// Sound operations helper.
#include "soundhelper.h"

// Utility for CEGUI cross-version compatibility
#include "ceguiutility.h"


SaveExitWindow::SaveExitWindow (Document* doc)
	:Window(doc)
{
	CEGUI::PushButton* btn;
	
	CEGUI::FrameWindow* save_exit = (CEGUI::FrameWindow*) CEGUIUtility::loadLayoutFromFile ("saveexitwindow.layout");
	m_window = save_exit;
	
	btn = static_cast<CEGUI::PushButton*>(CEGUIUtility::getWindowForLoadedLayoutEx (m_window, "GameExitConfirmButton"));
	btn->subscribeEvent(CEGUI::PushButton::EventClicked, CEGUI::Event::Subscriber(&SaveExitWindow ::onExitGameConfirmed, this));
	btn->subscribeEvent(CEGUIUtility::EventMouseEntersPushButtonArea (), CEGUI::Event::Subscriber(&SaveExitWindow::onGUIItemHover, this));
	
	btn = static_cast<CEGUI::PushButton*>(CEGUIUtility::getWindowForLoadedLayoutEx (m_window, "GameExitAbortButton"));
	btn->subscribeEvent(CEGUI::PushButton::EventClicked, CEGUI::Event::Subscriber(&SaveExitWindow ::onExitGameAborted, this));
	btn->subscribeEvent(CEGUIUtility::EventMouseEntersPushButtonArea (), CEGUI::Event::Subscriber(&SaveExitWindow::onGUIItemHover, this));

	// The panel should also have an auto-close button; connect it to the Cancel/Abort event.
	btn = static_cast<CEGUI::PushButton*>( CEGUIUtility::getWindowForLoadedLayoutEx (m_window, "__auto_closebutton__"));
	if (btn)
	{
		btn->subscribeEvent(CEGUI::PushButton::EventClicked, CEGUI::Event::Subscriber(&SaveExitWindow::onExitGameAborted, this));
		btn->subscribeEvent(CEGUIUtility::EventMouseEntersPushButtonArea (), CEGUI::Event::Subscriber(&SaveExitWindow::onGUIItemHover, this));
	}
	
	updateTranslation();
}

void SaveExitWindow::update()
{
}

void SaveExitWindow::updateTranslation()
{
	CEGUI::Window* label;
	
	CEGUI::PushButton* btn = static_cast<CEGUI::PushButton*>(CEGUIUtility::getWindowForLoadedLayoutEx (m_window, "GameExitConfirmButton"));
	btn->setText((CEGUI::utf8*) gettext("Ok"));
	
	btn = static_cast<CEGUI::PushButton*>(CEGUIUtility::getWindowForLoadedLayoutEx (m_window, "GameExitAbortButton"));
	btn->setText((CEGUI::utf8*) gettext("Abort"));
	
	label = CEGUIUtility::getWindowForLoadedLayoutEx (m_window, "SaveExitWindow");
	if (label->isPropertyPresent ("Text"))
	{
		label->setProperty ("Text", (CEGUI::utf8*) gettext("Save and Exit?"));
	}
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


/**
 * \fn bool onGUIItemHover(const CEGUI::EventArgs& evt)
 * \brief Handle the hovering of gui items.
 */
bool SaveExitWindow::onGUIItemHover (const CEGUI::EventArgs& evt)
{
	SoundHelper::playAmbientSoundGroup ("main_menu_hover_item");
	return true;
}



//
// ----------------------------------------- Message Question Window (a window containing a question and two buttons) ----------------------------------------------
//


MessageQuestionWindow::MessageQuestionWindow (Document* doc
							, const std::string& layoutName
							, const std::string& question
							, const std::string& button1Text
							, CEGUI::Event::Subscriber subscriberButton1Callback
							, const std::string& button2Text
							, CEGUI::Event::Subscriber subscriberButton2Callback)
	: Window (doc)
	, m_button1 (button1Text)
	, m_button2 (button2Text)
	, m_layoutName (layoutName)
{
	m_question = question;
	
	CEGUI::PushButton* btn;
	
	CEGUI::FrameWindow* message = (CEGUI::FrameWindow*) CEGUIUtility::loadLayoutFromFile (layoutName.c_str ());
	m_window = message;

	CEGUI::String widgetName (CEGUIUtility::getNameForWidget("QuestionAnswerButton1"));
	btn = static_cast<CEGUI::PushButton*>(CEGUIUtility::getWindowForLoadedLayoutEx (m_window, widgetName));
	if (btn)
	{
		btn->subscribeEvent(CEGUI::PushButton::EventClicked, subscriberButton1Callback);
	}

	widgetName = CEGUIUtility::getNameForWidget("QuestionAnswerButton2");
	btn = static_cast<CEGUI::PushButton*>(CEGUIUtility::getWindowForLoadedLayoutEx (m_window, widgetName));
	if (btn)
	{
		btn->subscribeEvent(CEGUI::PushButton::EventClicked, subscriberButton2Callback);
	}

	updateTranslation ();
}


MessageQuestionWindow::~MessageQuestionWindow ()
{
}


void MessageQuestionWindow::updateTranslation()
{
	CEGUI::Window* widget;

	CEGUI::String widgetName (CEGUIUtility::getNameForWidget("QuestionAnswerButton1"));
	widget = CEGUIUtility::getWindowForLoadedLayout(m_window, widgetName);
	if (widget)
	{
		widget->setText ((CEGUI::utf8*) m_button1.c_str ());
	}
	widgetName = CEGUIUtility::getNameForWidget("QuestionAnswerButton2");
	widget = CEGUIUtility::getWindowForLoadedLayout(m_window, widgetName);
	if (widget)
	{
		widget->setText ((CEGUI::utf8*) m_button2.c_str ());
	}
	widgetName = CEGUIUtility::getNameForWidget("QuestionInfoLabel");
	widget = CEGUIUtility::getWindowForLoadedLayout(m_window, widgetName);
	if (widget)
	{
		widget->setText ((CEGUI::utf8*) m_question.c_str ());
	}
}

void MessageQuestionWindow::setQuestion(std::string question)
{
	m_question = question;
	updateTranslation();
}


//
// ----------------------------------------- Warning Window (a window containing a notification and a button) ----------------------------------------------
//


WarningDialogWindow::WarningDialogWindow (Document* doc)
:Window(doc)
{
	CEGUI::PushButton* btn;
	
	CEGUI::FrameWindow* warning_dialog = (CEGUI::FrameWindow*) CEGUIUtility::loadLayoutFromFile("warningdialogwindow.layout");
	m_window = warning_dialog;
	m_warning = "";
	
	btn = static_cast<CEGUI::PushButton*>(CEGUIUtility::getWindowForLoadedLayoutEx (m_window, "WarningDialogConfirmButton"));
	btn->subscribeEvent(CEGUI::PushButton::EventClicked, CEGUI::Event::Subscriber(&WarningDialogWindow ::onConfirmed, this));
	
	updateTranslation();
}

void WarningDialogWindow::update()
{
}

void WarningDialogWindow::updateTranslation()
{
	CEGUI::Window* label;
	
	CEGUI::PushButton* btn = static_cast<CEGUI::PushButton*>(CEGUIUtility::getWindowForLoadedLayoutEx (m_window, "WarningDialogConfirmButton"));
	btn->setText((CEGUI::utf8*) gettext("Ok"));
	
	label = CEGUIUtility::getWindowForLoadedLayoutEx (m_window, "WarningDialogLabel");
	label->setText((CEGUI::utf8*) gettext(m_warning.c_str()));
}


bool WarningDialogWindow::onConfirmed(const CEGUI::EventArgs& evt)
{
	m_document->hideWarning ();
	return true;
}

void WarningDialogWindow::setWarning(std::string warning)
{
	m_warning = warning;
	updateTranslation();
}



//
// ----------------------------------------- Error Dialog Window (a window containing a notification and a button) ----------------------------------------------
//


ErrorDialogWindow::ErrorDialogWindow (Document* doc)
:Window(doc)
{
	CEGUI::PushButton* btn;

	CEGUI::FrameWindow* error_dialog = (CEGUI::FrameWindow*) CEGUIUtility::loadLayoutFromFile ("errordialogwindow.layout");
	m_window = error_dialog;
	m_error = "Network connection timed out";
	
	btn = static_cast<CEGUI::PushButton*>(CEGUIUtility::getWindowForLoadedLayoutEx (m_window, "ErrorDialogConfirmButton"));
	btn->subscribeEvent(CEGUI::PushButton::EventClicked, CEGUI::Event::Subscriber(&ErrorDialogWindow ::onConfirmed, this));
	
	updateTranslation();
}

void ErrorDialogWindow::update()
{
}

void ErrorDialogWindow::updateTranslation()
{
	CEGUI::Window* label;
	
	CEGUI::PushButton* btn = static_cast<CEGUI::PushButton*>(CEGUIUtility::getWindowForLoadedLayoutEx (m_window, "ErrorDialogConfirmButton"));
	btn->setText((CEGUI::utf8*) gettext("Ok"));
	
	label = CEGUIUtility::getWindowForLoadedLayoutEx (m_window, "ErrorDialogLabel");
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

