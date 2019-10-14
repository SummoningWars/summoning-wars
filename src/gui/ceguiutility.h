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

#ifndef __SUMWARS_GUI_CEGUIUTILITY_H__
#define __SUMWARS_GUI_CEGUIUTILITY_H__

// Define the CEGUI version. TODO: move to a global definition.
//#define CEGUI_07

#ifdef CEGUI_07
#include "CEGUI.h"
#include "CEGUI/CEGUIUDim.h"
#include "CEGUI/CEGUIFont.h"
#else
#include "CEGUI/CEGUI.h"
#endif

#include <list>
#include <string>
#include <OIS.h>

struct FormatedText
{
public:
	CEGUI::String text;
	size_t lines;
};

#define LINE_ENDING "\n"
#define ERASE_CNT 1


/**
	A class to hold a custom exception.
	This is a copy of a CEGUI exception.
*/
class CEGUIUtilityNoWidgetException
	: public std::exception
{
public:
	CEGUIUtilityNoWidgetException (const char* exceptionText)
        : std::exception()
	{
	};

	CEGUIUtilityNoWidgetException (const std::string& exceptionText)
        : std::exception()
	{
	};
};

/**
	A class to hold a custom exception.
*/
class CEGUIUtilityNullPointerException
	: public std::exception
{
public:
	CEGUIUtilityNullPointerException (const char* exceptionText)
        : std::exception()
	{
	};

	CEGUIUtilityNullPointerException (const std::string& exceptionText)
        : std::exception()
	{
	};
};


/**
	A class to be used as a namespace placeholder for most CEGUI functionality.
	This is to alleviate some of the issues caused by a change in the API syntax between various versions of CEGUI.

*/

class CEGUIUtility
{
	public:

		/*!
		\brief
		Enumerated type that contains valid formatting types that can be specified when rendering text into a Rect area (the formatting Rect).
		*/
		enum TextFormatting
		{
			/// All text is printed on a single line.  The left-most character is aligned with the left edge of the formatting Rect.
			LeftAligned,
			/// All text is printed on a single line.  The right-most character is aligned with the right edge of the formatting Rect.
			RightAligned,
			/// All text is printed on a single line.  The text is centred horizontally in the formatting Rect.
			Centred,
			/// All text is printed on a single line.  The left-most and right-most characters are aligned with the edges of the formatting Rect.
			Justified,
			/// Text is broken into multiple lines no wider than the formatting Rect.  The left-most character of each line is aligned with the left edge of the formatting Rect.
			WordWrapLeftAligned,
			/// Text is broken into multiple lines no wider than the formatting Rect.  The right-most character of each line is aligned with the right edge of the formatting Rect.
			WordWrapRightAligned,
			/// Text is broken into multiple lines no wider than the formatting Rect.  Each line is centred horizontally in the formatting Rect.
			WordWrapCentred,
			/// Text is broken into multiple lines no wider than the formatting Rect.  The left-most and right-most characters of each line are aligned with the edges of the formatting Rect.
			WordWrapJustified
		};

		static const CEGUI::String EventMouseEntersWindowArea ();
		static const CEGUI::String EventMouseEntersPushButtonArea ();
		static const CEGUI::String EventMouseEntersListBoxArea ();
		static const CEGUI::String EventMouseEntersScrollbarArea ();
		static const CEGUI::String EventMouseEntersComboboxArea ();
		static const CEGUI::String EventMouseEntersComboDroplistArea ();
		static const CEGUI::String EventMouseEntersEditboxArea ();
		static const CEGUI::String EventMouseEntersToggleButtonArea ();
		static const CEGUI::String EventToggleButtonStateChanged ();
		static const CEGUI::String EventMouseLeavesWindowArea ();

#ifdef CEGUI_07
		typedef CEGUI::Rect Rectf ;
#else
		typedef CEGUI::Rectf Rectf;
#endif

//#ifdef CEGUI_07
//		typedef CEGUI::Rect CachedRectf ;
//#else
//		typedef CEGUI::Element::CachedRectf CachedRectf;
//#endif


#ifdef CEGUI_07
		typedef CEGUI::Checkbox ToggleButton ;
#else
		typedef CEGUI::ToggleButton ToggleButton ;
#endif

#ifdef CEGUI_07
		typedef CEGUI::Point Vector2f ;
#else
		typedef CEGUI::Vector2f Vector2f;
#endif

#ifdef CEGUI_07
		typedef CEGUI::Size Size;
#else
		typedef CEGUI::Size<float> Size;
#endif


		enum Colours
		{
			Red = 1,
			Green,
			Blue,
			White,
			Black,
		};
		
		/**
		* \fn static CEGUI::UVector2 getWindowSizeForText(std::list<std::string> list, CEGUI::Font *font, std::string &added)
		* \brief Calculates the onscreen size of a text
		* \param list A std::list with each line of the text separated
		* \param font the CEGUI::Fonts thats used
		* \param added The complete list joined together as one string is writen into this argument
		* \returns The size of the text as a CEGUI::UVector2
		*/
		static CEGUI::UVector2 getWindowSizeForText(std::list<std::string> list, const CEGUI::Font *font, std::string &added);

		/**
		* \fn static CEGUI::UVector2 getWindowSizeForText(std::string text, CEGUI::Font *font)
		* \brief Calculates the size of a text
		* \param list The text as a std::string
		* \param font the CEGUI::Fonts thats used
		* \returns The size of the text as a CEGUI::UVector2
		*/
		static CEGUI::UVector2 getWindowSizeForText(std::string text, const CEGUI::Font *font);

		/**
		* \fn static std::list<std::string> getTextAsList(std::string text)
		* \brief Returns the input text as a std::list<std::string> separated by line breaks
		* \param list The text as a std::string
		* \returns The input text as a stringlist
		*/
		static std::list<std::string> getTextAsList(const std::string &text);
		
		/**
		* \fn static std::string stripColours(const std::string &input)
		* \brief Removes CEGUI colour tags from the string
		* \param Input text
		* \returns Input text without the colour information
		*/
		static std::string stripColours(const std::string &input);

		/**
		* \fn static std::string getStdColourAsString(std::string &col)
		* \brief Returns a string with the colour formated for use with CEGUI
		* \param Input Colour name
		* \returns A string with the colour formated for use with CEGUI
		*/
		static std::string getStdColourAsString(int col);

		/**
		* \fn static std::string getStdColourAsString(std::string &col)
		* \brief Colourizes a text string with the given colours
		* \param colour The colour
		* \param text The text to colourise
		* \param colourAfter colour after the string
		* \returns the input string formated with the colour for use with CEGUI
		*/
		static std::string getColourizedString(int colour, std::string text, int colourAfter);

		/**
		* \fn static size_t fitTextToWindow (const CEGUI::String& text, const CEGUI::Rect& format_area,TextFormatting fmt, CEGUI::Font *font, float x_scale = 1.0f, CEGUI::String& out_string = "")
		* \brief Returns the number of lines the text will have within a given CEGUI window
		* \param text Fits the text to a window
                * \param maxWidth The maximum width
		* \param fmt The text format
		* \param font The CEGUI::Font
		* \param x_scale Optional text scale
		* \returns the formated text and the line count
		*/
		static FormatedText fitTextToWindow (const CEGUI::String& text, float maxWidth, TextFormatting fmt, const CEGUI::Font *font, float x_scale = 1.0f);
		
		/**
			Add a child widget to a CEGUI widget
			@param parentPtr The parent pointer to the CEGUI Window
			@param childPtr The child pointer to the CEGUI Window.
		*/
		static void addChildWidget (CEGUI::Window* parentPtr, CEGUI::Window* childPtr);

		/**
			Remove a child widget from a CEGUI widget
			@param parentPtr The parent pointer to the CEGUI Window
			@param childPtr The child pointer to the CEGUI Window.
		*/
		static void removeChildWidget (CEGUI::Window* parentPtr, CEGUI::Window* childPtr);

#ifdef CEGUI_07
		static CEGUI::ImagesetManager& getImageManager ();
#else
		static CEGUI::ImageManager& getImageManager ();
#endif

		static void loadImageset (const CEGUI::String& name);

		static void addManagedImageFromFile (const CEGUI::String& name, const CEGUI::String& fileName, const CEGUI::String& resourceGroup = "");

		static CEGUI::Window* getWindow (const CEGUI::String& name);
		static CEGUI::Window* getWindowForSystem (CEGUI::System* sys, const CEGUI::String& name);
		static CEGUI::Window* getWindowForLoadedLayout (CEGUI::Window* parentWnd, const CEGUI::String& name);
		static CEGUI::Window* getWindowForSystemNoEx (CEGUI::System* sys, const CEGUI::String& name);

		/// @param startingWindow: the window from where the dump to log shall commence. For 
		/// a full system dump, use the root window.
		/// @param level: the amount of indentation levels to use.
		static void dumpFullWindowToLog (CEGUI::Window* startingWindow, int level = 0);

		/// Same as getWindowForLoadedLayout, but also throws an exception if the window ptr is NULL.
		static CEGUI::Window* getWindowForLoadedLayoutEx (CEGUI::Window* parentWnd, const CEGUI::String& name);

		/// 
		static CEGUI::String getMatchingPath (CEGUI::Window* parentWnd, const CEGUI::String& fullWidgetPath);
		static std::vector <std::string> SplitByChar (const std::string& source, const char delimiter);

		static CEGUI::Window* getWindowContainingMouse (CEGUI::System* sys);

		static bool isWindowPresent (const CEGUI::String& name);
		static bool isWindowPresentForSystem (CEGUI::System* sys, const CEGUI::String& name);

		static CEGUI::Window* loadLayoutFromFile (const CEGUI::String& name, const CEGUI::String& resGroup = "");

		static bool injectKeyDown (OIS::KeyCode myKey);
		static bool injectKeyDown (CEGUI::System* sys, OIS::KeyCode myKey);

		static bool injectKeyUp (OIS::KeyCode myKey);
		static bool injectKeyUp (CEGUI::System* sys, OIS::KeyCode myKey);
		
		static bool injectChar (int myKey);
		static bool injectChar (CEGUI::System* sys, int myKey);
		
		static bool injectMousePosition (float x, float y);
		static bool injectMousePosition (CEGUI::System* sys, float x, float y);

		static bool injectMouseWheelChange (float pos);
		static bool injectMouseWheelChange (CEGUI::System* sys, float pos);

		static bool injectMouseButtonDown (OIS::MouseButtonID btn);
		static bool injectMouseButtonDown (CEGUI::System* sys, OIS::MouseButtonID btn);

		static bool injectMouseButtonUp (OIS::MouseButtonID btn);
		static bool injectMouseButtonUp (CEGUI::System* sys, OIS::MouseButtonID btn);

		static void injectTimePulse (float timeElapsed);
		static void injectTimePulse (CEGUI::System* sys, float timeElapsed);

		static void addFont (const CEGUI::String& fontName, const CEGUI::String& resGroup);
		static void loadScheme (const CEGUI::String& schemeName, const CEGUI::String& resGroup);

		static CEGUI::MouseButton convertOISButtonToCegui (int buttonID);

		// Make sure that the FULL widget name (including path) can be used easily in CEGUI_07 as well.
		static std::string getNameForWidget (const std::string& name);

		// Retrieve the widget with a given skin.
		static std::string getWidgetWithSkin (const std::string& skin, const std::string& name);

		static std::string getImageNameWithSkin (const std::string& skinName, const std::string& imageName);

		static void setScrollPositionForWidget (const CEGUI::String& widgetName, float newScrollPosition);


		static void setDefaultFont (const CEGUI::String& fontName);
		static CEGUI::Font* getDefaultFont ();

		static void setDefaultMouseCursor (CEGUI::System* sys, const std::string& skinName, const std::string& cursorName);
		static void setDefaultTooltip (CEGUI::System* sys, const std::string& skinName, const std::string& cursorName);

		static CEGUIUtility::Vector2f getMouseCursorPosition (CEGUI::System* sys);
		static CEGUI::UVector2 getWidgetSize (CEGUI::Window* widget);

		static void setWidgetSizeRel (CEGUI::Window* widget, float width, float height);
		static void setWidgetSizeAbs (CEGUI::Window* widget, float width, float height);
		static void setWidgetSize (CEGUI::Window* widget, CEGUI::UVector2 size);
		static void setWidgetMaxSize (CEGUI::Window* widget, CEGUI::UVector2 size);


		static void setRootSheet (CEGUI::System* sys, CEGUI::Window* widget);
		static CEGUI::Window* getRootSheet (CEGUI::System* sys);

		static void createImage (const CEGUI::String& imageName);

private:
		/**
		* \fn static const size_t getNextWord(const CEGUI::String& in_string, size_t start_idx, CEGUI::String& out_string)
		* \brief 
		* \param in_string String to search
		* \param start_idx The start index
		* \param out_string The word get written into this string
		* \returns The new index
		*/
		static const size_t getNextWord(const CEGUI::String& in_string, size_t start_idx, CEGUI::String& out_string);
		
};

#endif // __SUMWARS_GUI_CEGUIUTILITY_H__
