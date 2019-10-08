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

#ifndef MESSAGEBOXES_H
#define MESSAGEBOXES_H


#include "window.h"


/**
 * \class SaveExitWindow
 * \brief Fenster Spiel beenden
 */
class SaveExitWindow : public Window
{
	public:
	/**
	 * \fn SaveExitWindow (Document* doc
	 * \brief Konstruktor
	 */
		SaveExitWindow (Document* doc);
	
		/**
		 * \fn virtual void update()
		 * \brief aktualisiert den Inhalt des Fensters
		 */
		virtual void update();
		
		/**
		 * \fn virtual void updateTranslation
		 * \brief aktualisiert die Uebersetzungen
		 */
		virtual void updateTranslation();
	
	private:
		/**
		 * \fn bool onExitGameConfirmed(const CEGUI::EventArgs& evt)
		 * \brief Behandelt Klick auf Button Spiel beenden
		 */
		bool onExitGameConfirmed(const CEGUI::EventArgs& evt);
		
		/**
		 * \fn bool onExitGameAborted(const CEGUI::EventArgs& evt)
		 * \brief Behandelt Klick auf Button Spiel beenden
		 */
		bool onExitGameAborted(const CEGUI::EventArgs& evt);
};

/**
 * \class MessageQuestion
 * \brief Fenster fuer eine Ja / Nein Frage
 */
class MessageQuestionWindow : public Window
{

	public:
		/**
		 * \fn MessageQuestionWindow (Document* doc
		* \brief Konstruktor
		*/
		MessageQuestionWindow (Document* doc, std::string name, std::string question, std::string button1,CEGUI::Event::Subscriber subscriber1,  std::string button2, CEGUI::Event::Subscriber subscriber2);
	
		/**
		 * \fn virtual void update()
		 * \brief aktualisiert den Inhalt des Fensters
		 */
		virtual void update()
		{
			
		}
		
		/**
		 * \fn virtual void updateTranslation
		 * \brief aktualisiert die Uebersetzungen
		 */
		virtual void updateTranslation();
		
		/**
		 * \fn void setQuestion(std::string question)
		 * \brief Aendert den Text der Frage
		 */
		void setQuestion(std::string question);
	
	private:
		/**
		 * \var std::string m_name
		 * \brief Name des Fensters
		 */
		std::string m_name;
		
		/**
		 * \var std::string m_question
		 * \brief Text der Frage
		 */
		std::string m_question;
		
		/**
		 * \var std::string m_button1
		 * \brief Beschriftung des ersten Buttons
		 */
		std::string m_button1;
		
		/**
		 * \var std::string m_button2
		 * \brief Beschriftung des zweiten Buttons
		 */
		std::string m_button2;
		
};

/**
 * \class WarningDialogWindow
 * \brief Dialog for Warnings
 */
class WarningDialogWindow : public Window
{
public:
	/**
	 * \brief Konstruktor
	 */
    WarningDialogWindow (Document* doc);
	
    /**
     * \brief aktualisiert den Inhalt des Fensters
     */
    virtual void update();
    
    /**
     * \brief aktualisiert die Uebersetzungen
     */
    virtual void updateTranslation();
    
    /**
     * \brief Set the warning text
     */
    void setWarning(std::string warning);
	
private:
    /**
     * \brief User clicked OK
     */
    bool onConfirmed(const CEGUI::EventArgs& evt);
    
    /**
     * \brief The warning text
     */
    std::string m_warning;
};

/**
 * \class ErrorDialogWindow
 * \brief Dialog for Errors
 */
class ErrorDialogWindow : public Window
{
public:
	/**
	 * \fn SaveExitWindow (Document* doc
	 * \brief Konstruktor
	 */
    ErrorDialogWindow (Document* doc);
	
    /**
     * \fn virtual void update()
     * \brief aktualisiert den Inhalt des Fensters
     */
    virtual void update();
    
    /**
     * \fn virtual void updateTranslation
     * \brief aktualisiert die Uebersetzungen
     */
    virtual void updateTranslation();
    
    /**
     * \fn void setError(std::string error)
     * \brief Set the error text
     */
    void setError(std::string error);
	
private:
    /**
     * \fn bool onConfirmed(const CEGUI::EventArgs& evt)
     * \brief User clicked OK
     */
    bool onConfirmed(const CEGUI::EventArgs& evt);
    
    /**
     * \var std::string m_error
     * \brief The error text
     */
    std::string m_error;
};


#endif

