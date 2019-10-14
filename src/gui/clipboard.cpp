#include "clipboard.h"
#include "config.h"

#include "debug.h"

#if OGRE_PLATFORM == OGRE_PLATFORM_WIN32
#include "tchar.h"
#endif

// Utility for CEGUI cross-version compatibility
#include "ceguiutility.h"

template<> SWUtil::Clipboard* Ogre::Singleton<SWUtil::Clipboard>::SUMWARS_OGRE_SINGLETON = 0;

namespace SWUtil
{

	Clipboard::Clipboard ()
	{
		SW_DEBUG ("Clipboard costructor!");
	}


	bool Clipboard::copy (bool cutInsteadOfCopy) 
	{
		SW_DEBUG ("Reached Clipboard copy; cut? %d", cutInsteadOfCopy);
		CEGUI::Window* sheet = CEGUIUtility::getRootSheet (CEGUI::System::getSingletonPtr ());
		if (!sheet) return false;

		CEGUI::Window* active = sheet->getActiveChild();
		if (!active) return false;

		CEGUI::String seltext;
		const CEGUI::String& type = active->getType();

		if (type.find("/MultiLineEditbox") != CEGUI::String::npos)
		{
			CEGUI::MultiLineEditbox* edit = static_cast<CEGUI::MultiLineEditbox*>(active);
			CEGUI::String::size_type beg = edit->getSelectionStartIndex();
			CEGUI::String::size_type len = edit->getSelectionLength();
			seltext = edit->getText().substr( beg, len ).c_str();

			// are we cutting or just copying?
			if (cutInsteadOfCopy) 
			{
				if (edit->isReadOnly())
				{
					return false;
				}
				CEGUI::String newtext = edit->getText();
				edit->setText( newtext.erase( beg, len ) );
			}

		}
		else if (type.find("/Editbox") != CEGUI::String::npos)
		{
			CEGUI::Editbox* edit = static_cast<CEGUI::Editbox*>(active);
			CEGUI::String::size_type beg = edit->getSelectionStartIndex();
			CEGUI::String::size_type len = edit->getSelectionLength();
			seltext = edit->getText().substr( beg, len ).c_str();

			// are we cutting or just copying?
			if (cutInsteadOfCopy) 
			{
				if (edit->isReadOnly())
				{
					return false;
				}
				CEGUI::String newtext = edit->getText();
				edit->setText( newtext.erase( beg, len ) );
			}

		}
		else 
		{
			return false;
		}

		_copyToClipboard (std::string (seltext.c_str ()));
		return true;
	}
	

	bool Clipboard::cut ()
	{
		return copy (true);
	}


	bool Clipboard::paste ()
	{
		SW_DEBUG ("Reached clipboard paste...");
		
		std::string clipboardContents = _readFromClipboard ();
		SW_DEBUG ("Got clipboard text to be: %s", clipboardContents.c_str ());

		CEGUI::Window* sheet = CEGUIUtility::getRootSheet (CEGUI::System::getSingletonPtr ());
		if (!sheet) return false;

		CEGUI::Window* active = sheet->getActiveChild();
		if (!active) return false;

		const CEGUI::String& type = active->getType();

		if (type.find("/MultiLineEditbox") != CEGUI::String::npos)
		{
			CEGUI::MultiLineEditbox* edit = static_cast<CEGUI::MultiLineEditbox*>(active);
			if (edit->isReadOnly())
			{
				return false;
			}

			// Check to see if some text is selected; if pasting from selected text, remove selected text first.

			CEGUI::String::size_type beg = edit->getSelectionStartIndex ();
			CEGUI::String::size_type len = edit->getSelectionLength ();
			
			if (len > 0)
			{
				CEGUI::String newtext = edit->getText();
				edit->setText( newtext.erase( beg, len ) );
			}

			// Now paste the new text.
			// TODO: not at end;
			CEGUI::String newtext = edit->getText();
			newtext.append (clipboardContents);
			edit->setText( newtext.erase( beg, len ) );
		}
		else if (type.find("/Editbox") != CEGUI::String::npos)
		{
			CEGUI::Editbox* edit = static_cast<CEGUI::Editbox*>(active);
			if (edit->isReadOnly())
			{
				return false;
			}

			// Check to see if some text is selected; if pasting from selected text, remove selected text first.
			CEGUI::String::size_type beg = edit->getSelectionStartIndex ();
			CEGUI::String::size_type len = edit->getSelectionLength ();
			if (len > 0)
			{
				CEGUI::String newtext = edit->getText();
				edit->setText( newtext.erase( beg, len ) );
			}
			
			SW_DEBUG ("Selection start idx: %d", beg);

			// Now paste the new text.
			CEGUI::String newtext = edit->getText();
			newtext.insert (beg, clipboardContents);
			edit->setText (newtext);

			// Also update the caret location, so that it is behind the pasted text.
#ifdef CEGUI_07
			edit->setCaratIndex (beg + clipboardContents.length ());
#else
			// Ok, I won't create a function just for the caret name.
			edit->setCaretIndex (beg + clipboardContents.length ());
#endif

			SW_DEBUG ("updated edit box contents");
		}
		else 
		{
			return false;
		}
		return true;
	}



	bool Clipboard::_copyToClipboard (const std::string& textToCopy)
	{
		// Used as source example: http://msdn.microsoft.com/en-us/library/windows/desktop/ms649016%28v=vs.85%29.aspx#_win32_Copying_Information_to_the_Clipboard

#if OGRE_PLATFORM == OGRE_PLATFORM_WIN32
		// Open the clipboard for examination and prevent other applications from modifying the clipboard content.
		// Sending the parameter to 0 (NULL), means that the open clipboard is associated with the current task. 
		if (! OpenClipboard (0))
		{
			ERRORMSG ("Could not open clipboard");
			return false;
		}

		if (! EmptyClipboard ())
		{
			ERRORMSG ("Could not empty clipboard");
			CloseClipboard ();
			return false;
		}

		HGLOBAL hglbText;

		// TODO: check whether WCHAR is more appropriate (for unicode).
		hglbText = GlobalAlloc (GMEM_MOVEABLE, (textToCopy.length () + 1) * sizeof(TCHAR)); 
        if (hglbText == NULL) 
        { 
            CloseClipboard ();
            return false; 
        }

		// Lock the handle and copy the text to the buffer. 
 
		LPTSTR  lptstrText = 0;
		lptstrText = (LPTSTR) GlobalLock (hglbText);
		memcpy (lptstrText, textToCopy.data (), textToCopy.length () * sizeof(TCHAR)); 
		lptstrText[textToCopy.length ()] = (TCHAR) 0;    // null character 

		// Unlock the handle.
		GlobalUnlock (hglbText); 

		// Set the clipboard data.
		SetClipboardData (CF_TEXT, hglbText);

		// Regular clipboard close.
		CloseClipboard ();

#elif OGRE_PLATFORM == OGRE_PLATFORM_APPLE
		return false;
#elif OGRE_PLATFORM == OGRE_PLATFORM_LINUX
		return false;
#endif
		return true;
	}


	/**
	* \brief Internal operation of reading text from clipboard.
	*/
	std::string Clipboard::_readFromClipboard () const
	{
		std::string returnValue ("");

#if OGRE_PLATFORM == OGRE_PLATFORM_WIN32

		// Only allow pasting of text, no other data.
		if (!IsClipboardFormatAvailable(CF_TEXT)) 
		{
			return returnValue; 
		}

		// Open the clipboard for examination and prevent other applications from modifying the clipboard content.
		// Sending the parameter to 0 (NULL), means that the open clipboard is associated with the current task. 
		if (! OpenClipboard (0))
		{
			ERRORMSG ("Could not open clipboard");
			return returnValue;
		}

		HGLOBAL hglbText;
		LPTSTR    lptstrText; 
		hglbText = GetClipboardData (CF_TEXT); 
		if (hglbText != NULL) 
		{ 
			lptstrText = (LPTSTR) GlobalLock (hglbText); 
			if (lptstrText != NULL) 
			{ 
				// Call the application-defined ReplaceSelection 
				// function to insert the text and repaint the 
				// window. 
 
				int len = _tcslen (lptstrText);
				
				// TODO: this is an arbitrarily chosen hard-coded limit; should make the component more configurable.
				// Should also take into account the first newline maybe (and stop parsing there).
				if (len > 500) 
				{
					returnValue = "too much data!";
				}
				else
				{
					returnValue = lptstrText;
				}
				GlobalUnlock (hglbText); 
			} 
		} 

		// Regular clipboard close.
		CloseClipboard();

#elif OGRE_PLATFORM == OGRE_PLATFORM_APPLE
#elif OGRE_PLATFORM == OGRE_PLATFORM_LINUX
#endif

		return returnValue;
	}


}