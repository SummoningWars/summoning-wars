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

#include "optionswindow.h"
//#include "sound.h"
#include "listitem.h"
//#include "music.h"
#include "sumwarshelper.h"

// Allow the use of the sound manager.
#include "gussound.h"

// The following includes are added to support video mode changes in the options window.
#include <OgreConfigFile.h>
#include <OgreException.h>
#include <OgreResourceGroupManager.h>

// Utility for CEGUI cross-version compatibility
#include "ceguiutility.h"

// Sound operations helper.
#include "soundhelper.h"

using namespace std;

using gussound::SoundManager;

OptionsWindow::OptionsWindow (Document* doc, OIS::Keyboard *keyboard, const std::string& ceguiSkinName)
	: Window (doc)
	, m_ceguiSkinName (ceguiSkinName)
{
	CEGUI::PushButton* btn;

	m_keyboard = keyboard;

	SW_DEBUG ("OptionsWindow being created using cegui skin [%s]", m_ceguiSkinName.c_str ());

  // Generate GUI Elements
	// Load the base options window - containing the actual elements.
	CEGUI::FrameWindow* options = (CEGUI::FrameWindow*) CEGUIUtility::loadLayoutFromFile ("optionswindow.layout");
	if (!options)
	{
		SW_DEBUG ("WARNING: Failed to load [%s]", "optionswindow.layout");
	}

	// Load the holder layout. This should be diffeent for each aspect ratio.
	CEGUI::Window* options_holder = CEGUIUtility::loadLayoutFromFile ("options_holder.layout");
	if (!options_holder)
	{
		SW_DEBUG ("WARNING: Failed to load [%s]", "options_holder.layout");
	}

	CEGUI::Window* wndHolder = CEGUIUtility::getWindowForLoadedLayoutEx (options_holder, "OptionsWindow_Holder");
	CEGUI::Window* wndHeldWindow = CEGUIUtility::getWindowForLoadedLayoutEx (options, "OptionsWindow");
	if (wndHolder && wndHeldWindow)
	{
		CEGUIUtility::addChildWidget (wndHolder, wndHeldWindow);
	}
	else
	{
		if (!wndHolder) SW_DEBUG ("ERROR: Unable to get the window holder for options window.");
		if (!wndHeldWindow) SW_DEBUG ("ERROR: Unable to get the window for options window.");
	}


	options->setAlwaysOnTop(true);
	options_holder->setAlwaysOnTop (true);
	m_window = options_holder;

	SW_DEBUG ("OptionsWindow (ctor) loaded layout");

	// Rahmen fuer das Menue Savegame auswaehlen
	CEGUI::TabControl* optionstab = (CEGUI::TabControl*) CEGUIUtility::getWindowForLoadedLayoutEx (options, "OptionsWindowTab");
	optionstab->subscribeEvent(CEGUI::Window::EventMouseButtonDown, CEGUI::Event::Subscriber(&OptionsWindow::onAreaMouseButtonPressed, this));
	CEGUI::Window* label;
	CEGUI::Scrollbar* slider;

	label = CEGUIUtility::getWindowForLoadedLayoutEx (optionstab, "__auto_TabPane__Buttons/__auto_btnOptionsShortkeys");
	label->subscribeEvent (CEGUIUtility::EventMouseEntersPushButtonArea (), CEGUI::Event::Subscriber (&OptionsWindow::onButtonItemHover, this));
	label->subscribeEvent (CEGUI::PushButton::EventMouseClick, CEGUI::Event::Subscriber (&OptionsWindow::onGUIItemClicked, this));
	
	label->setInheritsAlpha(false);
	
	label = CEGUIUtility::getWindowForLoadedLayoutEx (optionstab, "__auto_TabPane__Buttons/__auto_btnOptionsGameplay");
	label->subscribeEvent (CEGUIUtility::EventMouseEntersPushButtonArea (), CEGUI::Event::Subscriber (&OptionsWindow::onButtonItemHover, this));
	label->subscribeEvent (CEGUI::PushButton::EventMouseClick, CEGUI::Event::Subscriber (&OptionsWindow::onGUIItemClicked, this));
	label->setInheritsAlpha(false);
	
	label = CEGUIUtility::getWindowForLoadedLayoutEx (optionstab, "__auto_TabPane__Buttons/__auto_btnOptionsSound");
	label->subscribeEvent (CEGUIUtility::EventMouseEntersPushButtonArea (), CEGUI::Event::Subscriber (&OptionsWindow::onButtonItemHover, this));
	label->subscribeEvent (CEGUI::PushButton::EventMouseClick, CEGUI::Event::Subscriber (&OptionsWindow::onGUIItemClicked, this));
	label->setInheritsAlpha(false);
	
	label = CEGUIUtility::getWindowForLoadedLayoutEx (optionstab, "__auto_TabPane__Buttons/__auto_btnOptionsGraphic");
	label->subscribeEvent (CEGUIUtility::EventMouseEntersPushButtonArea (), CEGUI::Event::Subscriber (&OptionsWindow::onButtonItemHover, this));
	label->subscribeEvent (CEGUI::PushButton::EventMouseClick, CEGUI::Event::Subscriber (&OptionsWindow::onGUIItemClicked, this));
	label->setInheritsAlpha(false);
	
	label = CEGUIUtility::getWindowForLoadedLayoutEx (optionstab, "__auto_TabPane__Buttons/__auto_btnOptionsMisc");
	label->subscribeEvent (CEGUIUtility::EventMouseEntersPushButtonArea (), CEGUI::Event::Subscriber (&OptionsWindow::onButtonItemHover, this));
	label->subscribeEvent (CEGUI::PushButton::EventMouseClick, CEGUI::Event::Subscriber (&OptionsWindow::onGUIItemClicked, this));
	label->setInheritsAlpha(false);
	
	SW_DEBUG ("OptionsWindow (ctor) adding labels");

	int targets[9] = {SHOW_INVENTORY, SHOW_CHARINFO, SHOW_SKILLTREE, SHOW_PARTYMENU, SHOW_CHATBOX, SHOW_QUESTINFO, SHOW_MINIMAP, SWAP_EQUIP, SHOW_ITEMLABELS};

	std::ostringstream stream;
	for (int i=0; i<9; ++i)
	{
		stream.str("");
		stream << "__auto_TabPane__/OptionsShortkeys/";
		stream << "ShortkeyLabel"<<i;

		label = CEGUIUtility::getWindowForLoadedLayoutEx (optionstab, stream.str());
		label->setID(targets[i]);
		label->setWantsMultiClickEvents(false);

		stream.str("");
		stream << "__auto_TabPane__/OptionsShortkeys/";
		stream << "ShortkeyValueLabel"<<i;

		label = CEGUIUtility::getWindowForLoadedLayoutEx (optionstab, stream.str());
		label->setID(targets[i]);
		label->setWantsMultiClickEvents(false);
		label->subscribeEvent(CEGUI::Window::EventMouseButtonDown, CEGUI::Event::Subscriber(&OptionsWindow::onShortkeyLabelClicked,  this));
		//label->subscribeEvent(CEGUIUtility::EventMouseEntersWindowArea (), CEGUI::Event::Subscriber(&OptionsWindow::onButtonItemHover,  this));
	}
	
	SW_DEBUG ("Adding difficulty combo-box");//xxx
	CEGUI::Combobox* diffcbo = static_cast<CEGUI::Combobox*>(CEGUIUtility::getWindowForLoadedLayoutEx (m_window, "OptionsWindow/OptionsWindowTab/__auto_TabPane__/OptionsGameplay/DifficultyBox"));
	diffcbo->addItem(new ListItem((CEGUI::utf8*) m_ceguiSkinName.c_str (), (CEGUI::utf8*) gettext("Easy"),Options::EASY));
	diffcbo->addItem(new ListItem((CEGUI::utf8*) m_ceguiSkinName.c_str (), (CEGUI::utf8*) gettext("Normal"),Options::NORMAL));
	diffcbo->addItem(new ListItem((CEGUI::utf8*) m_ceguiSkinName.c_str (), (CEGUI::utf8*) gettext("Hard"),Options::HARD));
	diffcbo->addItem(new ListItem((CEGUI::utf8*) m_ceguiSkinName.c_str (), (CEGUI::utf8*) gettext("Insane"),Options::INSANE));
	diffcbo->setReadOnly(true);
	diffcbo->setItemSelectState((size_t) 0,true);
	diffcbo->handleUpdatedListItemData();
	diffcbo->subscribeEvent(CEGUI::Combobox::EventListSelectionAccepted, CEGUI::Event::Subscriber(&OptionsWindow::onDifficultyChanged, this));
	diffcbo->setItemSelectState( (size_t) (Options::getInstance()->getDifficulty()-1),true);
	connectWidgetSoundEvents ("OptionsWindow/OptionsWindowTab/__auto_TabPane__/OptionsGameplay/DifficultyBox", "combobox");
	
	SW_DEBUG ("Adding sliders");
	slider = static_cast<CEGUI::Scrollbar*>(CEGUIUtility::getWindowForLoadedLayoutEx (m_window, "OptionsWindow/OptionsWindowTab/__auto_TabPane__/OptionsGameplay/TextSpeedSlider"));
	slider->setPageSize (0.01f);
	slider->setDocumentSize(1.0f);
	slider->setStepSize(0.01f);
	slider->setWantsMultiClickEvents(false);
	slider->subscribeEvent(CEGUI::Scrollbar::EventScrollPositionChanged, CEGUI::Event::Subscriber(&OptionsWindow::onTextSpeedChanged,  this));
	slider->subscribeEvent(CEGUIUtility::EventMouseEntersScrollbarArea (), CEGUI::Event::Subscriber(&OptionsWindow::onButtonItemHover, this));
	
	slider = static_cast<CEGUI::Scrollbar*>(CEGUIUtility::getWindowForLoadedLayoutEx (m_window, "OptionsWindow/OptionsWindowTab/__auto_TabPane__/OptionsSound/MusicVolumeSlider"));
	slider->setPageSize (0.01f);
	slider->setDocumentSize(1.0f);
	slider->setStepSize(0.01f);
	slider->setWantsMultiClickEvents(false);
	slider->subscribeEvent(CEGUI::Scrollbar::EventScrollPositionChanged, CEGUI::Event::Subscriber(&OptionsWindow::onMusicVolumeChanged,  this));
	slider->subscribeEvent(CEGUIUtility::EventMouseEntersScrollbarArea (), CEGUI::Event::Subscriber(&OptionsWindow::onButtonItemHover, this));

	slider = static_cast<CEGUI::Scrollbar*>(CEGUIUtility::getWindowForLoadedLayoutEx (m_window, "OptionsWindow/OptionsWindowTab/__auto_TabPane__/OptionsSound/SoundVolumeSlider"));
	slider->setPageSize (0.01f);
	slider->setDocumentSize(1.0f);
	slider->setStepSize(0.01f);
	slider->setWantsMultiClickEvents(false);
	slider->subscribeEvent(CEGUI::Scrollbar::EventScrollPositionChanged, CEGUI::Event::Subscriber(&OptionsWindow::onSoundVolumeChanged,  this));
	slider->subscribeEvent(CEGUIUtility::EventMouseEntersScrollbarArea (), CEGUI::Event::Subscriber(&OptionsWindow::onButtonItemHover, this));

	CEGUI::Combobox* ehlcbo = static_cast<CEGUI::Combobox*>(CEGUIUtility::getWindowForLoadedLayoutEx (m_window, "OptionsWindow/OptionsWindowTab/__auto_TabPane__/OptionsGraphic/EHColorBox"));
	ehlcbo->addItem(new StrListItem((CEGUI::utf8*) m_ceguiSkinName.c_str (), (CEGUI::utf8*) gettext("White"), "white", 0));
	ehlcbo->addItem(new StrListItem((CEGUI::utf8*) m_ceguiSkinName.c_str (), (CEGUI::utf8*) gettext("Black"), "black", 0));
	ehlcbo->addItem(new StrListItem((CEGUI::utf8*) m_ceguiSkinName.c_str (), (CEGUI::utf8*) gettext("Red"), "red", 0));
	ehlcbo->addItem(new StrListItem((CEGUI::utf8*) m_ceguiSkinName.c_str (), (CEGUI::utf8*) gettext("Green"), "green", 0));
	ehlcbo->addItem(new StrListItem((CEGUI::utf8*) m_ceguiSkinName.c_str (), (CEGUI::utf8*) gettext("Blue"), "blue", 0));
	ehlcbo->addItem(new StrListItem((CEGUI::utf8*) m_ceguiSkinName.c_str (), (CEGUI::utf8*) gettext("Yellow"), "yellow", 0));
	ehlcbo->addItem(new StrListItem((CEGUI::utf8*) m_ceguiSkinName.c_str (), (CEGUI::utf8*) gettext("Magenta"), "magenta", 0));
	ehlcbo->addItem(new StrListItem((CEGUI::utf8*) m_ceguiSkinName.c_str (), (CEGUI::utf8*) gettext("Cyan"), "cyan", 0));
	ehlcbo->addItem(new StrListItem((CEGUI::utf8*) m_ceguiSkinName.c_str (), (CEGUI::utf8*) gettext("Orange"), "orange", 0));
	ehlcbo->addItem(new StrListItem((CEGUI::utf8*) m_ceguiSkinName.c_str (), (CEGUI::utf8*) gettext("Pink"), "pink", 0));
	ehlcbo->addItem(new StrListItem((CEGUI::utf8*) m_ceguiSkinName.c_str (), (CEGUI::utf8*) gettext("Purple"), "purple", 0));
	ehlcbo->addItem(new StrListItem((CEGUI::utf8*) m_ceguiSkinName.c_str (), (CEGUI::utf8*) gettext("Cornflower Blue"), "cornflower_blue", 0));

	ehlcbo->setReadOnly(true);
	ehlcbo->setItemSelectState((size_t) getColorSelectionIndex("red"), true);
	ehlcbo->handleUpdatedListItemData();
	ehlcbo->subscribeEvent(CEGUI::Combobox::EventListSelectionAccepted, CEGUI::Event::Subscriber(&OptionsWindow::onEnemyHighlightChanged, this));
	ehlcbo->subscribeEvent(CEGUIUtility::EventMouseEntersComboboxArea (), CEGUI::Event::Subscriber(&OptionsWindow::onButtonItemHover, this));
	ehlcbo->setItemSelectState((size_t) (getColorSelectionIndex(Options::getInstance()->getEnemyHighlightColor())), true);

	connectWidgetSoundEvents ("OptionsWindow/OptionsWindowTab/__auto_TabPane__/OptionsGraphic/EHColorBox", "combobox");


	CEGUIUtility::ToggleButton *chkbox = static_cast<CEGUIUtility::ToggleButton*> (CEGUIUtility::getWindowForLoadedLayoutEx (
		m_window, "OptionsWindow/OptionsWindowTab/__auto_TabPane__/OptionsGraphic/GrabMouseInWindowedModeBox"));
	chkbox->setSelected(Options::getInstance()->getGrabMouseInWindowedMode());
	chkbox->subscribeEvent(CEGUIUtility::EventToggleButtonStateChanged (), CEGUI::Event::Subscriber(&OptionsWindow::onGrabMouseChanged, this));
	chkbox->subscribeEvent(CEGUIUtility::ToggleButton::EventMouseClick, CEGUI::Event::Subscriber(&OptionsWindow::onGUIItemClicked, this));
	chkbox->subscribeEvent(CEGUIUtility::EventMouseEntersToggleButtonArea (), CEGUI::Event::Subscriber(&OptionsWindow::onButtonItemHover, this));
	
	
	btn = static_cast<CEGUI::PushButton*>(CEGUIUtility::getWindowForLoadedLayoutEx (m_window, "OptionsWindow/OptionsCloseButton"));
	btn->setID(5);
	btn->subscribeEvent(CEGUI::PushButton::EventClicked, CEGUI::Event::Subscriber(&OptionsWindow::onButtonOkClicked, this));
	btn->subscribeEvent(CEGUIUtility::EventMouseEntersPushButtonArea (), CEGUI::Event::Subscriber(&OptionsWindow::onButtonItemHover, this));

	// todo: analyze this
	try
	{
		label = CEGUIUtility::getWindowForLoadedLayoutEx (m_window, "OptionsWindow/__auto_closebutton__");
		if (label)
		{
			label->subscribeEvent(CEGUI::Window::EventMouseClick, CEGUI::Event::Subscriber(&OptionsWindow::onButtonCancelClicked, this));
			label->subscribeEvent(CEGUIUtility::EventMouseEntersPushButtonArea (), CEGUI::Event::Subscriber(&OptionsWindow::onButtonItemHover, this));
		}
	}
	catch (std::exception& e)
	{
		SW_DEBUG ("Caught exception while creating events for optional object. Carrying on after the exception [%s]", e.what());
	}
	

	CEGUI::Combobox* cbo = static_cast<CEGUI::Combobox*>(CEGUIUtility::getWindowForLoadedLayoutEx (m_window, "OptionsWindow/OptionsWindowTab/__auto_TabPane__/OptionsMisc/LanguageBox"));
	cbo->addItem(new StrListItem((CEGUI::utf8*) m_ceguiSkinName.c_str (), (CEGUI::utf8*) gettext("System default"),"",0));
	cbo->addItem(new StrListItem((CEGUI::utf8*) m_ceguiSkinName.c_str (), "German","de_DE",0));
	cbo->addItem(new StrListItem((CEGUI::utf8*) m_ceguiSkinName.c_str (), "English GB","en_GB",0));
	cbo->addItem(new StrListItem((CEGUI::utf8*) m_ceguiSkinName.c_str (), "English US","en_US",0));
	cbo->addItem(new StrListItem((CEGUI::utf8*) m_ceguiSkinName.c_str (), "Italian IT","it_IT",0));
	cbo->addItem(new StrListItem((CEGUI::utf8*) m_ceguiSkinName.c_str (), "Russian RU","ru_RU",0));
	cbo->addItem(new StrListItem((CEGUI::utf8*) m_ceguiSkinName.c_str (), "Ukrainian UK","uk_UA",0));
	cbo->addItem(new StrListItem((CEGUI::utf8*) m_ceguiSkinName.c_str (), "Spanish ES","es_ES",0));

	cbo->setReadOnly(true);
	connectWidgetSoundEvents ("OptionsWindow/OptionsWindowTab/__auto_TabPane__/OptionsMisc/LanguageBox", "combobox");


	int selection = 0;
	std::string locale = Options::getInstance()->getLocale();
	int pos = locale.find(".");
	if (pos != std::string::npos)
	{
		locale = locale.substr(0,pos);
	}

	SW_DEBUG ("OptionsWindow (ctor) current locale is [%s]", locale.c_str ());
	
	for (int i=0; i < (int)cbo->getItemCount(); i++)
	{
		StrListItem* item = static_cast<StrListItem*>(cbo->getListboxItemFromIndex(i));
		if (locale == item->m_data)
		{
			selection = i;
		}
	}
	cbo->setItemSelectState((size_t) selection,true);
	cbo->handleUpdatedListItemData();

	cbo->subscribeEvent(CEGUI::Combobox::EventListSelectionAccepted, CEGUI::Event::Subscriber(&OptionsWindow::onLanguageSelected, this));

	// TODO: add comments.
	cbo = static_cast<CEGUI::Combobox*>(CEGUIUtility::getWindowForLoadedLayoutEx(m_window, "OptionsWindow/OptionsWindowTab/__auto_TabPane__/OptionsGraphic/DisplayModeBox"));
	cbo->addItem(new StrListItem((CEGUI::utf8*) m_ceguiSkinName.c_str (), (CEGUI::utf8*) gettext("Fullscreen (Exclusive Mode)"),"",0));
	cbo->addItem(new StrListItem((CEGUI::utf8*) m_ceguiSkinName.c_str (), (CEGUI::utf8*) gettext("Window (With Borders)"),"",0));

	if (Options::getInstance ()->isWindowedFullscreenModeSupported ())
	{
		SW_DEBUG ("Windowed fullscreen mode supported! Adding combobox item.");
		std::string modeName = gettext("Window (Fullscreen)");
		std::string recommended = gettext (" - recommended");
		std::string sum = modeName + recommended;

		cbo->addItem(new StrListItem((CEGUI::utf8*) m_ceguiSkinName.c_str (), (CEGUI::utf8*) sum.c_str(),"",0));
	}
	else
	{
		SW_DEBUG ("Windowed fullscreen mode NOT supported!");
	}

	cbo->setReadOnly(true);
	DisplayModes myDisplayMode = Options::getInstance ()->getUsedDisplayMode ();
	if (myDisplayMode == WINDOWED_FULLSCREEN && !Options::getInstance ()->isWindowedFullscreenModeSupported ())
	{
		myDisplayMode = WINDOWED_WITH_BORDER;
	}
	cbo->setItemSelectState((size_t) myDisplayMode, true);
	cbo->handleUpdatedListItemData();
	cbo->subscribeEvent(CEGUI::Combobox::EventListSelectionAccepted, CEGUI::Event::Subscriber(&OptionsWindow::onDisplayModeSelected, this));

	connectWidgetSoundEvents ("OptionsWindow/OptionsWindowTab/__auto_TabPane__/OptionsGraphic/DisplayModeBox", "combobox");


	//
	// Add the list of video drivers. --------------------------------------------------------------
	//

	// Get the list of available drivers to populate the list.
	std::vector <std::string> videoDrivers = Options::getInstance ()->getEditableAvailableVideoDrivers ();

	// Get the driver that is currently in use; this will be the selected entry in the list
	std::string selectedVideoDriver = Options::getInstance ()->getUsedVideoDriver ();

	// Start adding the driver items to the list
	cbo = static_cast<CEGUI::Combobox*>(CEGUIUtility::getWindowForLoadedLayoutEx (m_window, "OptionsWindow/OptionsWindowTab/__auto_TabPane__/OptionsGraphic/VideoDriverBox"));
	for (std::vector <std::string>::const_iterator it = videoDrivers.begin (); it != videoDrivers.end (); ++ it)
	{
		std::string myName = *it;
		cbo->addItem (new StrListItem ((CEGUI::utf8*) m_ceguiSkinName.c_str (), (CEGUI::utf8*) myName.c_str (), "", 0));
		if (myName == selectedVideoDriver)
		{
			cbo->setItemSelectState (cbo->getItemCount () - 1, true);
		}
	}
	// Set the list to read-only, so that the user can't type in entries.
	cbo->setReadOnly (true);
	
	// Since we may update the selected item, it's necessary to also call the internal update.
	cbo->handleUpdatedListItemData ();

	cbo->subscribeEvent(CEGUI::Combobox::EventListSelectionAccepted, CEGUI::Event::Subscriber(&OptionsWindow::onVideoDriverSelected, this));
	connectWidgetSoundEvents ("OptionsWindow/OptionsWindowTab/__auto_TabPane__/OptionsGraphic/VideoDriverBox", "combobox");


	//
	// Add the resolution list. Also select the currently used resolution.  ------------------------
	//

	// Get the resolution in the current options set.
	std::string usedResolution = Options::getInstance ()->getUsedResolution ();
	SW_DEBUG ("Used resolution is: [%s]", usedResolution.c_str ());

	// Get the list of available resolutions for the selected video driver.
	std::vector <std::string> resolutions = Options::getInstance ()->getEditableResolutionsMapping ()[selectedVideoDriver];

	// Start adding the resolutions as items to the combo-box, one by one.
	cbo = static_cast<CEGUI::Combobox*>(CEGUIUtility::getWindowForLoadedLayoutEx (m_window, "OptionsWindow/OptionsWindowTab/__auto_TabPane__/OptionsGraphic/ResolutionBox"));
	for (std::vector <std::string>::const_iterator it = resolutions.begin (); it != resolutions.end (); ++ it)
	{
		cbo->addItem (new StrListItem ((CEGUI::utf8*) m_ceguiSkinName.c_str (), (CEGUI::utf8*) it->c_str (), "", 0));
		if (*it == usedResolution)
		{
			cbo->setItemSelectState (cbo->getItemCount () - 1, true);
		}
	}
	// Set the list to read-only, so that the user can't type in entries.
	cbo->setReadOnly (true);

	// For windowed (fullscreen) mode, make sure the resolution can't be modified. Why? Because it's the desktop size.
	// What happens when you change your desktop size? That's another story...
	if (myDisplayMode == WINDOWED_FULLSCREEN)
	{
		cbo->setEnabled (false);

		// Clear the selection flag?
		//if (cbo->getSelectedItem () != 0)
		//{
		//	cbo->getSelectedItem ()->setSelected (false);
		//}
		//cbo->setText ("");
	}

	cbo->handleUpdatedListItemData();
	connectWidgetSoundEvents ("OptionsWindow/OptionsWindowTab/__auto_TabPane__/OptionsGraphic/ResolutionBox", "combobox");


	//
	// Add the shadow options into the list. Also select the currently used shadow mode ---------------------------------------------
	//
	Options::ShadowMode usedShadowMode = Options::getInstance ()->getShadowMode ();

	// Start adding the shadow options as items to the combo-box, one by one.
	cbo = static_cast<CEGUI::Combobox*>(CEGUIUtility::getWindowForLoadedLayoutEx (m_window, "OptionsWindow/OptionsWindowTab/__auto_TabPane__/OptionsGraphic/ShadowsDropDownList"));
	if (cbo)
	{
		cbo->addItem (new StrListItem ((CEGUI::utf8*) m_ceguiSkinName.c_str (), (CEGUI::utf8*) gettext("Off"), "", 0));
		cbo->addItem (new StrListItem ((CEGUI::utf8*) m_ceguiSkinName.c_str (), (CEGUI::utf8*) gettext("Simple (Higher quality)"), "", 0));
		cbo->addItem (new StrListItem ((CEGUI::utf8*) m_ceguiSkinName.c_str (), (CEGUI::utf8*) gettext("Simple (Lower quality)"), "", 0));
		cbo->addItem (new StrListItem ((CEGUI::utf8*) m_ceguiSkinName.c_str (), (CEGUI::utf8*) gettext("Simulated (Experimental 1)"), "", 0));
		cbo->addItem (new StrListItem ((CEGUI::utf8*) m_ceguiSkinName.c_str (), (CEGUI::utf8*) gettext("Simulated (Experimental 2)"), "", 0));

		if (usedShadowMode >= Options::SM_NONE && usedShadowMode < Options::SM_COUNT)
		{
			if (cbo->getItemCount () > (size_t)usedShadowMode)
			{
				// Select the currently used option.
				cbo->setItemSelectState ((int) usedShadowMode, true);
			}
		}

		// Set the list to read-only, so that the user can't type in entries.
		cbo->setReadOnly (true);

		// refresh the object.
		cbo->handleUpdatedListItemData();

		cbo->subscribeEvent(CEGUI::Combobox::EventListSelectionAccepted, CEGUI::Event::Subscriber(&OptionsWindow::onShadowModeSelected, this));
	}
	connectWidgetSoundEvents ("OptionsWindow/OptionsWindowTab/__auto_TabPane__/OptionsGraphic/ShadowsDropDownList", "combobox");

	// Select the 1st available tab.
	// This is a pre-emptive work-around for CEGUI 0.8+ support (which uses hierarchical items)
	std::string widgetName =  CEGUIUtility::getNameForWidget ("OptionsWindow/OptionsWindowTab");

	try
	{
		CEGUI::TabControl* tc = static_cast <CEGUI::TabControl*> (CEGUIUtility::getWindowForLoadedLayoutEx (m_window, widgetName));
		if (tc)
		{
			if (tc->getTabCount () > 0)
			{
				tc->setSelectedTabAtIndex(0);
				tc->invalidate (true);
				tc->invalidateRenderingSurface ();
			}
		}
		else
		{
			SW_DEBUG ("Options Window init: could not find item named %s", widgetName.c_str());
		}
	}
	catch(std::exception& e)
	{
		SW_DEBUG ("Caught exception and continuing [%s]", e.what ());
	}

	reset();
	updateTranslation();
}

void OptionsWindow::update()
{
	CEGUI::Window* label;
	Options* options = Options::getInstance();

	std::ostringstream stream;
	KeyCode key;
	std::string keyname;
	// Schleife ueber die Labels der Kurztasten
	for (int i=0; i<9; i++)
	{
		stream.str("");
		stream << "OptionsWindow/OptionsWindowTab/__auto_TabPane__/OptionsShortkeys/";
		stream << "ShortkeyValueLabel"<<i;
		label = CEGUIUtility::getWindowForLoadedLayoutEx (m_window, stream.str());

		if (m_key_destination == (int) label->getID())
		{
			// die Kurztaste soll gerade eingegeben werden
			keyname = "_";
		}
		else
		{
			key = options->getMappedKey( (ShortkeyDestination) label->getID());
			keyname = m_keyboard->getAsString ( (OIS::KeyCode) key);
		}

		if (label->getText() != keyname)
		{
			label->setText(keyname);
		}
	}

	CEGUI::Scrollbar* slider = static_cast<CEGUI::Scrollbar*>(CEGUIUtility::getWindowForLoadedLayoutEx (m_window, "OptionsWindow/OptionsWindowTab/__auto_TabPane__/OptionsSound/SoundVolumeSlider"));
	//if ( fabs ( slider->getScrollPosition() - SoundSystem::getSoundVolume()) > 0.01f)
	if ( fabs ( slider->getScrollPosition() - SoundManager::getPtr ()->getRepository ()->getVolumeForCategory (gussound::GSC_Effect)) > 0.01f)
	{
		//slider->setScrollPosition(SoundSystem::getSoundVolume());
		slider->setScrollPosition (SoundManager::getPtr ()->getRepository ()->getVolumeForCategory (gussound::GSC_Effect));
	}

	slider = static_cast<CEGUI::Scrollbar*>(CEGUIUtility::getWindowForLoadedLayoutEx (m_window, "OptionsWindow/OptionsWindowTab/__auto_TabPane__/OptionsSound/MusicVolumeSlider"));
	//if ( fabs ( slider->getScrollPosition() - MusicManager::instance().getMusicVolume()) > 0.01f)
	if ( fabs ( slider->getScrollPosition() - SoundManager::getPtr ()->getRepository ()->getVolumeForCategory (gussound::GSC_Music)) > 0.01f)
	{
		//slider->setScrollPosition(MusicManager::instance().getMusicVolume());
		slider->setScrollPosition (SoundManager::getPtr ()->getRepository ()->getVolumeForCategory (gussound::GSC_Music));
	}
	
	slider = static_cast<CEGUI::Scrollbar*>(CEGUIUtility::getWindowForLoadedLayoutEx (m_window, "OptionsWindow/OptionsWindowTab/__auto_TabPane__/OptionsGameplay/TextSpeedSlider"));
	float slidpos = (2.0 -Options::getInstance()->getTextSpeed()) / 1.4;
	if ( fabs ( slider->getScrollPosition() - slidpos) > 0.01f)
	{
		slider->setScrollPosition(slidpos);
	}

}

void OptionsWindow::updateTranslation()
{
	CEGUI::Window* label;
	CEGUIUtility::ToggleButton* box;
	

	CEGUI::DefaultWindow* keys = static_cast<CEGUI::DefaultWindow*> (CEGUIUtility::getWindowForLoadedLayoutEx (m_window, "OptionsWindow/OptionsWindowTab/__auto_TabPane__Buttons/__auto_btnOptionsShortkeys"));
	keys->setText((CEGUI::utf8*) gettext("Shortkeys"));
	CEGUI::DefaultWindow* gameplay = static_cast<CEGUI::DefaultWindow*> (CEGUIUtility::getWindowForLoadedLayoutEx (m_window, "OptionsWindow/OptionsWindowTab/__auto_TabPane__Buttons/__auto_btnOptionsGameplay"));
	gameplay->setText((CEGUI::utf8*) gettext("Gameplay"));
	CEGUI::DefaultWindow* sound = static_cast<CEGUI::DefaultWindow*> (CEGUIUtility::getWindowForLoadedLayoutEx (m_window, "OptionsWindow/OptionsWindowTab/__auto_TabPane__Buttons/__auto_btnOptionsSound"));
	sound->setText((CEGUI::utf8*) gettext("Audio"));
	CEGUI::DefaultWindow* graphic = static_cast<CEGUI::DefaultWindow*> (CEGUIUtility::getWindowForLoadedLayoutEx (m_window, "OptionsWindow/OptionsWindowTab/__auto_TabPane__Buttons/__auto_btnOptionsGraphic"));
	graphic->setText((CEGUI::utf8*) gettext("Graphic"));
	CEGUI::DefaultWindow* misc =static_cast<CEGUI::DefaultWindow*> (CEGUIUtility::getWindowForLoadedLayoutEx (m_window, "OptionsWindow/OptionsWindowTab/__auto_TabPane__Buttons/__auto_btnOptionsMisc"));
	misc->setText((CEGUI::utf8*) gettext("Language"));

	label = CEGUIUtility::getWindowForLoadedLayoutEx (m_window, "OptionsWindow");
	if (label->isPropertyPresent ("Text"))
	{
		label->setProperty ("Text", (CEGUI::utf8*) gettext("Options"));
	}

	label = CEGUIUtility::getWindowForLoadedLayoutEx (m_window, "OptionsWindow/OptionsWindowTab/__auto_TabPane__/OptionsShortkeys/ShortkeyLabel0");
	label->setText((CEGUI::utf8*) gettext("Inventory"));

	label = CEGUIUtility::getWindowForLoadedLayoutEx (m_window, "OptionsWindow/OptionsWindowTab/__auto_TabPane__/OptionsShortkeys/ShortkeyLabel1");
	label->setText((CEGUI::utf8*) gettext("Character screen"));

	label = CEGUIUtility::getWindowForLoadedLayoutEx (m_window, "OptionsWindow/OptionsWindowTab/__auto_TabPane__/OptionsShortkeys/ShortkeyLabel2");
	label->setText((CEGUI::utf8*) gettext("Skilltree"));

	label = CEGUIUtility::getWindowForLoadedLayoutEx (m_window, "OptionsWindow/OptionsWindowTab/__auto_TabPane__/OptionsShortkeys/ShortkeyLabel3");
	label->setText((CEGUI::utf8*) gettext("Party screen"));

	label = CEGUIUtility::getWindowForLoadedLayoutEx (m_window, "OptionsWindow/OptionsWindowTab/__auto_TabPane__/OptionsShortkeys/ShortkeyLabel4");
	label->setText((CEGUI::utf8*) gettext("Chatbox"));

	label = CEGUIUtility::getWindowForLoadedLayoutEx (m_window, "OptionsWindow/OptionsWindowTab/__auto_TabPane__/OptionsShortkeys/ShortkeyLabel5");
	label->setText((CEGUI::utf8*) gettext("Quests"));

	label = CEGUIUtility::getWindowForLoadedLayoutEx (m_window, "OptionsWindow/OptionsWindowTab/__auto_TabPane__/OptionsShortkeys/ShortkeyLabel6");
	label->setText((CEGUI::utf8*) gettext("Minimap"));

	label = CEGUIUtility::getWindowForLoadedLayoutEx (m_window, "OptionsWindow/OptionsWindowTab/__auto_TabPane__/OptionsShortkeys/ShortkeyLabel7");
	label->setText((CEGUI::utf8*) gettext("Swap equipement"));

	label = CEGUIUtility::getWindowForLoadedLayoutEx (m_window, "OptionsWindow/OptionsWindowTab/__auto_TabPane__/OptionsShortkeys/ShortkeyLabel8");
	label->setText((CEGUI::utf8*) gettext("Item Labels"));

	label = CEGUIUtility::getWindowForLoadedLayoutEx (m_window, "OptionsWindow/OptionsWindowTab/__auto_TabPane__/OptionsGameplay/GameplayDifficultyLabel");
	label->setText((CEGUI::utf8*) gettext("Difficulty"));
	
	label = CEGUIUtility::getWindowForLoadedLayoutEx (m_window, "OptionsWindow/OptionsWindowTab/__auto_TabPane__/OptionsGameplay/TextSpeedLabel");
	label->setText((CEGUI::utf8*) gettext("Text Speed"));
	
	label = CEGUIUtility::getWindowForLoadedLayoutEx (m_window, "OptionsWindow/OptionsWindowTab/__auto_TabPane__/OptionsSound/SoundVolumeLabel");
	label->setText((CEGUI::utf8*) gettext("Sound"));

	label = CEGUIUtility::getWindowForLoadedLayoutEx (m_window, "OptionsWindow/OptionsWindowTab/__auto_TabPane__/OptionsSound/MusicVolumeLabel");
	label->setText((CEGUI::utf8*) gettext("Music"));

	label = CEGUIUtility::getWindowForLoadedLayoutEx (m_window, "OptionsWindow/OptionsWindowTab/__auto_TabPane__/OptionsGraphic/EnemyHighlightLabel");
	label->setText((CEGUI::utf8*) gettext("Enemy Highlight Color"));

	label = CEGUIUtility::getWindowForLoadedLayoutEx (m_window, "OptionsWindow/OptionsWindowTab/__auto_TabPane__/OptionsMisc/LanguageLabel");
	label->setText((CEGUI::utf8*) gettext("Language"));
	
	box = static_cast<CEGUIUtility::ToggleButton*>(CEGUIUtility::getWindowForLoadedLayoutEx (m_window, "OptionsWindow/OptionsWindowTab/__auto_TabPane__/OptionsGraphic/GrabMouseInWindowedModeBox"));
	box->setText((CEGUI::utf8*) gettext("Grab mouse in windowed mode (needs restart)"));
	
	label = CEGUIUtility::getWindowForLoadedLayoutEx (m_window, "OptionsWindow/OptionsWindowTab/__auto_TabPane__/OptionsGraphic/VideoDriver");
	label->setText((CEGUI::utf8*) gettext("Video Driver"));
	
	label = CEGUIUtility::getWindowForLoadedLayoutEx (m_window, "OptionsWindow/OptionsWindowTab/__auto_TabPane__/OptionsGraphic/DisplayMode");
	label->setText((CEGUI::utf8*) gettext("Display Mode"));
	
	label = CEGUIUtility::getWindowForLoadedLayoutEx (m_window, "OptionsWindow/OptionsWindowTab/__auto_TabPane__/OptionsGraphic/Resolution");
	label->setText((CEGUI::utf8*) gettext("Resolution"));

	label = CEGUIUtility::getWindowForLoadedLayoutEx (m_window, "OptionsWindow/OptionsWindowTab/__auto_TabPane__/OptionsGraphic/ShadowsLabel");
	if (label) label->setText ((CEGUI::utf8*) gettext("Shadows"));
	
	// Set the close button to "ok"
	CEGUI::PushButton* btn = static_cast<CEGUI::PushButton*>(CEGUIUtility::getWindowForLoadedLayoutEx (m_window, "OptionsWindow/OptionsCloseButton"));
	btn->setText((CEGUI::utf8*) gettext("Ok"));

	//btn = static_cast<CEGUI::PushButton*>(CEGUIUtility::getWindow ("ResetGraphicsButton"));
	//btn->setText((CEGUI::utf8*) gettext("Reset graphic options")); // TODO: clean-up code.
}


void OptionsWindow::reset()
{
	m_key_destination = NO_KEY;
}


/**
 * Connect specific sound events for the widget.
 */
void OptionsWindow::connectWidgetSoundEvents (const std::string& widgetName, const std::string& widgetType)
{
	try
	{
		if (widgetType == "combobox")
		{
			try
			{
				CEGUI::PushButton* btn;
				std::string subWidgetName (widgetName);
				subWidgetName.append ("/__auto_button__");
				btn = static_cast<CEGUI::PushButton*> (CEGUIUtility::getWindowForLoadedLayoutEx (m_window, subWidgetName));
				btn->subscribeEvent (CEGUI::PushButton::EventMouseClick, CEGUI::Event::Subscriber (&OptionsWindow::onGUIItemClicked, this));
				btn->subscribeEvent (CEGUIUtility::EventMouseEntersPushButtonArea (), CEGUI::Event::Subscriber (&OptionsWindow::onButtonItemHover, this));

				CEGUI::ComboDropList* drop;
				subWidgetName = widgetName;
				subWidgetName.append ("/__auto_droplist__");
				drop = static_cast<CEGUI::ComboDropList*> (CEGUIUtility::getWindowForLoadedLayoutEx (m_window, subWidgetName));
				drop->subscribeEvent (CEGUI::ComboDropList::EventMouseClick, CEGUI::Event::Subscriber (&OptionsWindow::onGUIItemClicked, this));
				drop->subscribeEvent (CEGUIUtility::EventMouseEntersComboDroplistArea (), CEGUI::Event::Subscriber (&OptionsWindow::onButtonItemHover, this));

				CEGUI::Editbox* ebox;
				subWidgetName = widgetName;
				subWidgetName.append ("/__auto_editbox__");
				ebox = static_cast<CEGUI::Editbox*> (CEGUIUtility::getWindowForLoadedLayoutEx (m_window, subWidgetName));
				ebox->subscribeEvent (CEGUI::Editbox::EventMouseClick, CEGUI::Event::Subscriber (&OptionsWindow::onGUIItemClicked, this));
				ebox->subscribeEvent (CEGUIUtility::EventMouseEntersEditboxArea (), CEGUI::Event::Subscriber (&OptionsWindow::onButtonItemHover, this));
			}
			catch (std::exception& e)
			{
				SW_DEBUG ("Caught an exception while connecting widget sound events (will continue) [%s]", e.what ());
			}

		}
		else
		{
			SW_DEBUG ("Developer message: widget type [%s] not recognized in connecting widget sound events", widgetType.c_str ());
		}
	}
	catch (std::exception& e)
	{
		SW_DEBUG ("Caught exception: %s", e.what ());
	}
}


/**
 * \fn bool onButtonItemClicked(const CEGUI::EventArgs& evt)
 * \brief Handle the click of gui items.
 */
bool OptionsWindow::onGUIItemClicked (const CEGUI::EventArgs& evt)
{
	SoundHelper::playAmbientSoundGroup ("main_menu_click_item");
	return true;
}



/**
 * \fn bool onButtonItemHover(const CEGUI::EventArgs& evt)
 * \brief Handle the hovering of gui items.
 */
bool OptionsWindow::onButtonItemHover (const CEGUI::EventArgs& evt)
{
	SoundHelper::playAmbientSoundGroup ("main_menu_hover_item");
	return true;
}



bool OptionsWindow::onShortkeyLabelClicked(const CEGUI::EventArgs& evt)
{
	const CEGUI::MouseEventArgs& we =
			static_cast<const CEGUI::MouseEventArgs&>(evt);
	int id = (int) we.window->getID();

	if (m_key_destination == id)
	{
		m_key_destination = NO_KEY;
	}
	else
	{
		m_key_destination =(ShortkeyDestination) id;
	}
	return true;
}

void OptionsWindow::setKeyCode(KeyCode kc)
{
	if (requestsForKey())
	{
		ShortkeyMap& ablt_map = m_document->getAbilityShortkeys();
		ablt_map.erase(kc);
		
		Options::getInstance()->setShortkey(kc,m_key_destination);
		m_key_destination = NO_KEY;
	}
}

bool OptionsWindow::onAreaMouseButtonPressed(const CEGUI::EventArgs& evt)
{
	m_key_destination = NO_KEY;
	return true;
}


bool OptionsWindow::onButtonCancelClicked (const CEGUI::EventArgs& evt)
{
	// Play the sound
	SoundHelper::playAmbientSoundGroup ("main_menu_click_item");

	m_document->onButtonOptionsClicked();
	return true;
}

bool OptionsWindow::onButtonOkClicked (const CEGUI::EventArgs& evt)
{
	SW_DEBUG ("[Ok] button clicked in options. Attempting to save them");
	// TODO: move video mode checks to separate function? or just wait for the full integration with the rest of the options?

	// Play the sound
	SoundHelper::playAmbientSoundGroup ("main_menu_click_item");

	// Check to see if the display mode was updated. The display mode is stored in a different file at the moment.
	// TODO: move towards integrated settings file (single file for most settings).
	{
		// Flag to keep track of whether we need updating of the settings.
		bool someVideoSettingsWereChanged (false);

		// Check the driver for changes. ---------------
		CEGUI::Combobox* cbo = static_cast<CEGUI::Combobox*>(CEGUIUtility::getWindowForLoadedLayoutEx (m_window, "OptionsWindow/OptionsWindowTab/__auto_TabPane__/OptionsGraphic/VideoDriverBox"));
		if (cbo == 0)
		{
			SW_DEBUG ("Could not find widget [VideoDriverBox]");
			return false;
		}

		CEGUI::ListboxItem* item = cbo->getSelectedItem();
		if (item == 0)
		{
			SW_DEBUG ("Could not find selected item in widget [VideoDriverBox]. Searching via text values. (slower)");
			CEGUI::String currentText = cbo->getText();

			for (unsigned i = 0; i < cbo->getItemCount (); i ++)
			{
				if (cbo->getListboxItemFromIndex (i)->getText () == currentText)
				{
					item = cbo->getListboxItemFromIndex (i);
				}
			}
		}

		if (item == 0)
		{
			SW_DEBUG ("Could not find selected item in widget [VideoDriverBox]. Unable to store any changes!!!");
		}

		std::string selectedDriverName (Options::getInstance ()->getUsedVideoDriver ());

		if (item != 0)
		{
			selectedDriverName = item->getText ().c_str ();
			someVideoSettingsWereChanged  |= (selectedDriverName != Options::getInstance ()->getUsedVideoDriver ());
		}

		// Check the display mode for changes. ---------------
		cbo = static_cast<CEGUI::Combobox*>(CEGUIUtility::getWindowForLoadedLayoutEx (m_window, "OptionsWindow/OptionsWindowTab/__auto_TabPane__/OptionsGraphic/DisplayModeBox"));
		if (cbo == 0)
		{
			SW_DEBUG ("Could not find widget [DisplayModeBox]");
			return false;
		}

		item = cbo->getSelectedItem();
		if (item == 0)
		{
			SW_DEBUG ("Could not find selected item in widget [DisplayModeBox]. Searching via text values. (slower)");
			CEGUI::String currentText = cbo->getText();
			
			for (unsigned i = 0; i < cbo->getItemCount (); i ++)
			{
				if (cbo->getListboxItemFromIndex (i)->getText () == currentText)
				{
					item = cbo->getListboxItemFromIndex (i);
				}
			}
		}
		if (item == 0)
		{
			SW_DEBUG ("Could not find selected item in widget [DisplayModeBox]. Unable to store any changes!!!");
		}

		DisplayModes userMode = Options::getInstance ()->getUsedDisplayMode ();

		if (item != 0)
		{
			size_t selectionIndex = (int)cbo->getItemIndex (item);

			SW_DEBUG ("selected display mode %s",item->getText().c_str());
		
			userMode = (DisplayModes)selectionIndex;
			SW_DEBUG ("Index of combobox selection is: %d. New display mode set to %d", selectionIndex, userMode);
			someVideoSettingsWereChanged |= (userMode != Options::getInstance ()->getUsedDisplayMode ());
		}

		// Check the resolution for changes -----------------------
		std::string userResolution;
		cbo = static_cast<CEGUI::Combobox*>(CEGUIUtility::getWindowForLoadedLayoutEx (m_window, "OptionsWindow/OptionsWindowTab/__auto_TabPane__/OptionsGraphic/ResolutionBox"));

		if (cbo == 0)
		{
			SW_DEBUG ("Could not find widget [ResolutionBox]");
			return false;
		}

		userResolution = cbo->getText().c_str();
		if (userResolution.empty())
		{
			userResolution = Options::getInstance ()->getUsedResolution ();
		}
		SW_DEBUG ("selected userResolution %s", userResolution.c_str());

		if (userMode == WINDOWED_FULLSCREEN)
		{
#if OGRE_PLATFORM == OGRE_PLATFORM_WIN32
			int desktopWidth = (int)GetSystemMetrics (SM_CXSCREEN);
			int desktopHeight = (int)GetSystemMetrics (SM_CYSCREEN);
			userResolution = SumwarsHelper::getUpdatedResolutionString (userResolution, desktopWidth, desktopHeight);
#endif
			SW_DEBUG ("Updated selected userResolution to [%s]", userResolution.c_str());
		}

		someVideoSettingsWereChanged |= (userResolution != Options::getInstance ()->getUsedResolution ());

		// Check the shadow mode for changes -----------------------------------
		
		cbo = static_cast<CEGUI::Combobox*>(CEGUIUtility::getWindowForLoadedLayoutEx (m_window, "OptionsWindow/OptionsWindowTab/__auto_TabPane__/OptionsGraphic/ShadowsDropDownList"));
		if (cbo == 0)
		{
			SW_DEBUG ("Could not find widget [ShadowsDropDownList]");
			return false;
		}

		item = cbo->getSelectedItem();
		if (item == 0)
		{
			SW_DEBUG ("Could not find selected item in widget [ShadowsDropDownList]. Searching via text values. (slower)");
			CEGUI::String currentText = cbo->getText();
			
			for (unsigned i = 0; i < cbo->getItemCount (); i ++)
			{
				if (cbo->getListboxItemFromIndex (i)->getText () == currentText)
				{
					item = cbo->getListboxItemFromIndex (i);
				}
			}
		}

		if (item == 0)
		{
			SW_DEBUG ("Could not find selected item in widget [ShadowsDropDownList]. Unable to store any changes!!!");
		}

		Options::ShadowMode newShadowSettings = Options::getInstance ()->getShadowMode ();

		if (item != 0)
		{
			size_t selectionIndex = (int)cbo->getItemIndex (item);
			// Limit the values to the available options.
			if (selectionIndex >= Options::SM_COUNT) selectionIndex = Options::SM_COUNT - 1;
			if (selectionIndex < Options::SM_NONE) selectionIndex = Options::SM_NONE;
			newShadowSettings = static_cast<Options::ShadowMode> (selectionIndex);
		}


		SW_DEBUG ("Used shadow mode (old - new) : %d - %d", Options::getInstance ()->getShadowMode (), newShadowSettings);
	
		someVideoSettingsWereChanged |= (newShadowSettings != Options::getInstance ()->getShadowMode ());

		// Perform the actual settings save operation. ----------------------------

		if (someVideoSettingsWereChanged)
		{
			// Store it into the options; for the time being it is only saved there, not read from there!
			// The read is performed from the actual settings!
			// TODO: initialize the graphics subsystem with the settings in the options.xml file instead of the default ogre file.
			Options::getInstance ()->setUsedDisplayMode (userMode);
			Options::getInstance ()->setUsedResolution (userResolution);
			Options::getInstance ()->setUsedVideoDriver (selectedDriverName);
			Options::getInstance ()->setShadowMode (newShadowSettings);

			std::string configpath;
#if defined (__APPLE__)
			configpath = SumwarsHelper::macPath() + "/ogre.cfg";
#else
			configpath = SumwarsHelper::getStorageBasePath() + "/" + SumwarsHelper::userPath() + "/ogre.cfg";
#endif

			// TODO: move handling to specialized function.
			Ogre::ConfigFile cfg;
			try
			{
				// Don't trim whitespace
				cfg.load (configpath, "\t:=", false);
			}
			catch (Ogre::Exception& e)
			{
				if (e.getNumber () == Ogre::Exception::ERR_FILE_NOT_FOUND)
				{
					SW_DEBUG ("Could not find OGRE config file");
					return false;
				}
				else
				{
					throw;
				}
			}

			// Prepare the list of values to write.

			Ogre::String fullScreenValueToSet ("No");
			if (Options::getInstance ()->getUsedDisplayMode () == FULLSCREEN_EX)
			{
				fullScreenValueToSet = "Yes";
			}

			Ogre::String resolutionValueToSet ("");
			resolutionValueToSet = Options::getInstance ()->getUsedResolution ();
			SW_DEBUG ("Saving options; will set resolution to : [%s]", resolutionValueToSet.c_str());

			// Update the settings; 
			Ogre::ConfigFile::SectionIterator iSection = cfg.getSectionIterator ();
			while (iSection.hasMoreElements ())
			{
				const Ogre::String& renderSystemName = iSection.peekNextKey ();
				Ogre::ConfigFile::SettingsMultiMap& settings = *iSection.getNext();

				Ogre::ConfigFile::SettingsMultiMap::iterator it;

				// Check the main selected item.
				it = settings.find ("Render System");
				if (it != settings.end ())
				{
					it->second = Options::getInstance ()->getUsedVideoDriver ();
				}

				// Note: this will update for ALL render systems.
				it = settings.find ("Full Screen");
				if (it != settings.end ())
				{
					it->second = fullScreenValueToSet;
				}


				// Note: this will update only for the selected render systems.
				if (renderSystemName == Options::getInstance ()->getUsedVideoDriver ())
				{
					it = settings.find ("Video Mode");
					if (it != settings.end ())
					{
						it->second = resolutionValueToSet;
						DEBUGX ("Set resolution entry");
					}
				}
			}

			// ----------------------------------------------------------------
			// Write the Ogre.cfg file
			std::ofstream of (configpath.c_str());

			if (!of)
			{
				ERRORMSG ("Cannot write config");
			}
	
			iSection = cfg.getSectionIterator ();
			while (iSection.hasMoreElements ())
			{
				const Ogre::String& renderSystemName = iSection.peekNextKey ();
				if (!renderSystemName.empty())
				{
					of << "[" << renderSystemName << "]" << std::endl;
				}
				Ogre::ConfigFile::SettingsMultiMap& settings = *iSection.getNext ();

				Ogre::ConfigFile::SettingsMultiMap::const_iterator it;
				for (it = settings.begin (); it != settings.end (); ++ it)
				{
					of << it->first << "=" << it->second << std::endl;
				}
				of << std::endl;
			}

			of.close();
			// ----------------------------------------------------------------

			// Show a notification, but not for a running game. We should basically have only 2 cases here:
			if (m_document->getState () != Document::RUNNING)
			{
				m_document->showWarning (gettext ("Please restart the game\n for the changes to take effect!"));
			}
		} // user display mode changed.
	}

	m_document->onButtonOptionsClicked();
	return true;
}

bool OptionsWindow::onSoundVolumeChanged(const CEGUI::EventArgs& evt)
{
	const CEGUI::MouseEventArgs& we =
			static_cast<const CEGUI::MouseEventArgs&>(evt);

	SoundHelper::playAmbientSoundGroup ("main_menu_click_item");

	CEGUI::Scrollbar* slider = static_cast<CEGUI::Scrollbar*>(we.window);
	float vol = slider->getScrollPosition();
	SW_DEBUG("sound volume change to %f",vol);
	//SoundSystem::setSoundVolume(vol);
	SoundManager::getPtr ()->getRepository ()->setVolumeForCategory (gussound::GSC_Effect, vol);
	SoundManager::getPtr ()->getRepository ()->setVolumeForCategory (gussound::GSC_Master, 1.0);
	return true;
}

bool OptionsWindow::onDifficultyChanged(const CEGUI::EventArgs& evt)
{
	const CEGUI::MouseEventArgs& we = 
			static_cast<const CEGUI::MouseEventArgs&>(evt);
	
	SoundHelper::playAmbientSoundGroup ("main_menu_click_item");
	
	CEGUI::Combobox* cbo = static_cast<CEGUI::Combobox*>(we.window);
	
	CEGUI::ListboxItem* item = cbo->getSelectedItem();
	if (item != 0)
	{
		Options::getInstance()->setDifficulty(static_cast<Options::Difficulty>(item->getID()));
		SW_DEBUG ("Difficulty changed. %s", item->getText ().c_str ());
	}
	else
	{
		SW_DEBUG ("Difficulty changed. Unknown!");
	}
	
	return true;
}

bool OptionsWindow::onTextSpeedChanged(const CEGUI::EventArgs& evt)
{
	const CEGUI::MouseEventArgs& we =
			static_cast<const CEGUI::MouseEventArgs&>(evt);
	
	SoundHelper::playAmbientSoundGroup ("main_menu_click_item");

	CEGUI::Scrollbar* slider = static_cast<CEGUI::Scrollbar*>(we.window);
	float speed = 2.0 - slider->getScrollPosition()*1.4;
	DEBUGX("text speed changed to %f",speed);
	Options::getInstance()->setTextSpeed(speed);
	return true;
}

bool OptionsWindow::onMusicVolumeChanged(const CEGUI::EventArgs& evt)
{
	const CEGUI::MouseEventArgs& we =
			static_cast<const CEGUI::MouseEventArgs&>(evt);

	SoundHelper::playAmbientSoundGroup ("main_menu_click_item");

	CEGUI::Scrollbar* slider = static_cast<CEGUI::Scrollbar*>(we.window);
	float vol = slider->getScrollPosition();
	DEBUGX("music volume changed to %f",vol);
	//MusicManager::instance().setMusicVolume(vol);
	SoundManager::getPtr ()->getRepository ()->setVolumeForCategory (gussound::GSC_Music, vol);
	SoundManager::getPtr ()->getRepository ()->setVolumeForCategory (gussound::GSC_Master, 1.0);
	return true;
}

bool OptionsWindow::onEnemyHighlightChanged(const CEGUI::EventArgs& evt)
{
	const CEGUI::MouseEventArgs& we =
			static_cast<const CEGUI::MouseEventArgs&>(evt);

	CEGUI::Combobox* cbo = static_cast<CEGUI::Combobox*>(we.window);
	CEGUI::ListboxItem* item = cbo->getSelectedItem();

	if (item != 0)
	{
		DEBUGX("enemy highlight color changed to %s", item->getText().c_str());
		StrListItem* sitem = static_cast<StrListItem*>(item);
		Options::getInstance()->setEnemyHighlightColor(sitem->m_data.c_str());
	}

	return true;
}


bool OptionsWindow::onShadowModeSelected(const CEGUI::EventArgs& evt)
{
	//const CEGUI::MouseEventArgs& we =
	//		static_cast<const CEGUI::MouseEventArgs&>(evt);

	//CEGUI::Combobox* cbo = static_cast<CEGUI::Combobox*>(we.window);
	//CEGUI::ListboxItem* item = cbo->getSelectedItem();

	//if (item != 0)
	//{
	//	DEBUG ("shadow mode to %s", item->getText().c_str());

	//	size_t itemIndex = cbo->getSelectionStartIndex ();
	//	Options::ShadowMode modeToUse = static_cast<Options::ShadowMode> (itemIndex);
	//	Options::getInstance()->setShadowMode (modeToUse);
	//	
	//	return true;
	//}
	return true;
}


/*
	// TODO: get rid of obsolete code.
bool OptionsWindow::onResetGraphics(const CEGUI::EventArgs& evt)
{
	std::string configpath;
#if defined (_WIN32)
	configpath = SumwarsHelper::userPath() + "/ogre.cfg";
#elif defined (__APPLE__)
	configpath = SumwarsHelper::macPath() + "/ogre.cfg";
#elif defined (__unix__)
	configpath = SumwarsHelper::userPath() + "/ogre.cfg";
#endif

	remove(configpath.c_str());
	return true;
}
*/

bool OptionsWindow::onLanguageSelected(const CEGUI::EventArgs& evt)
{

	const CEGUI::MouseEventArgs& we =
			static_cast<const CEGUI::MouseEventArgs&>(evt);

	CEGUI::Combobox* cbo = static_cast<CEGUI::Combobox*>(we.window);

	CEGUI::ListboxItem* item = cbo->getSelectedItem();

	if (item != 0)
	{
		DEBUGX("selected Language %s",item->getText().c_str());
		StrListItem* sitem = static_cast<StrListItem*>(item);

		// Call the options set locale; this will call in turn the Gettext set locale
		Options::getInstance ()->setLocale (sitem->m_data.c_str());

#if OGRE_PLATFORM == OGRE_PLATFORM_WIN32
		// A re-init seems to be needed on Windows.
		// TODO:XXX: investigate - Augustin Preda, 2011.10.31
		Ogre::StringVectorPtr path = Ogre::ResourceGroupManager::getSingleton().findResourceLocation("translation", "*");
		Gettext::init(sitem->m_data.c_str(), path.get()->at(0));
#endif
	}

	return true;
}



bool OptionsWindow::onDisplayModeSelected (const CEGUI::EventArgs& evt)
{
	SW_DEBUG ("Display mode selected");
	const CEGUI::MouseEventArgs& we =
			static_cast<const CEGUI::MouseEventArgs&>(evt);

	CEGUI::Combobox* cbo = static_cast<CEGUI::Combobox*>(we.window);

	if (cbo == 0)
	{
		SW_DEBUG ("CEGUI ERROR: source widget is NULL");
		return false;
	}

	CEGUI::ListboxItem* item = cbo->getSelectedItem();

	if (item == 0)
	{
		SW_DEBUG ("CEGUI ERROR: No valid selection in source widget");
		return false;
	}

	size_t selectionIndex = (int)cbo->getItemIndex (item);

	//DEBUG ("selected display mode %s",item->getText().c_str());
	//DEBUG ("idx of selection is: %d", selectionIndex);
	//Options::getInstance ()->setUsedDisplayMode ((DisplayModes)selectionIndex);

	DisplayModes myDisplayMode = (DisplayModes)selectionIndex;

	// For windowed (fullscreen) mode, make sure the resolution can't be edited.

	std::string widgetName = CEGUIUtility::getNameForWidget ("OptionsWindow/OptionsWindowTab/__auto_TabPane__/OptionsGraphic/ResolutionBox");
	cbo = static_cast<CEGUI::Combobox*>(CEGUIUtility::getWindowForLoadedLayoutEx (m_window, widgetName));
	if (myDisplayMode == WINDOWED_FULLSCREEN)
	{
		cbo->setEnabled (false);
		
		// Set the selection to the desktop size resolution.

#if OGRE_PLATFORM == OGRE_PLATFORM_WIN32
		int desktopWidth = (int)GetSystemMetrics (SM_CXSCREEN);
		int desktopHeight = (int)GetSystemMetrics (SM_CYSCREEN);

		string userResolution = cbo->getText().c_str();
		if (userResolution.empty())
		{
			userResolution = Options::getInstance ()->getUsedResolution ();
		}

		userResolution = SumwarsHelper::getUpdatedResolutionString (userResolution, desktopWidth, desktopHeight);
		cbo->setText (userResolution);
#endif

		cbo->handleUpdatedListItemData ();
	}
	else
	{
		cbo->setEnabled (true);

		std::string usedResolution = Options::getInstance ()->getUsedResolution ();
		
		SW_DEBUG ("Will try to show that the used resolution is [%s]", usedResolution.c_str ());

		for (size_t i = 0; i < cbo->getItemCount (); ++ i)
		{
			CEGUI::ListboxItem* item = cbo->getListboxItemFromIndex (i);
			CEGUI::String itemText = item->getText ();
			if (usedResolution == itemText)
			{
				cbo->setItemSelectState(i, true);
				
				SW_DEBUG ("Located item [%s] at [%d]", itemText.c_str (), i);
			}
			SW_DEBUG ("Compared with [%s]", itemText.c_str ());
		}
		cbo->handleUpdatedListItemData();
	}

	return true;
}



bool OptionsWindow::onVideoDriverSelected (const CEGUI::EventArgs& evt)
{
	SW_DEBUG ("Video Driver selected");
	const CEGUI::MouseEventArgs& we =
			static_cast<const CEGUI::MouseEventArgs&>(evt);

	CEGUI::Combobox* cbo = static_cast<CEGUI::Combobox*>(we.window);

	if (cbo == 0)
	{
		return false;
	}

	CEGUI::ListboxItem* item = cbo->getSelectedItem();

	if (item == 0)
	{
		return false;
	}

	std::string videoDriver = cbo->getText().c_str(); // it's safe to get the text, they are read-only.
	std::string usedResolution;

	// Get the current width and height. This is to attempt to keep the resolution (E.g. if we have 800x600 in Direct3D, let's keep it for OpenGL as well).
	int currentWidth (0), currentHeight (0);
	SumwarsHelper::getSizesFromResolutionString (Options::getInstance ()->getUsedResolution (), currentWidth, currentHeight);

	SW_DEBUG ("Selected video driver: %s", videoDriver.c_str ());

	// Get the list of available resolutions for the selected video driver.
	std::vector <std::string> resolutions = Options::getInstance ()->getEditableResolutionsMapping ()[videoDriver];
	
	// Use the last item as basis...
	if (!resolutions.empty())
	{
		usedResolution = resolutions[resolutions.size () - 1];
		usedResolution = SumwarsHelper::getUpdatedResolutionString (usedResolution, currentWidth, currentHeight);
	}

	// Start adding the resolutions as items to the combo-box, one by one.
	cbo = static_cast<CEGUI::Combobox*>(CEGUIUtility::getWindowForLoadedLayoutEx (m_window, "OptionsWindow/OptionsWindowTab/__auto_TabPane__/OptionsGraphic/ResolutionBox"));
	cbo->resetList ();


	for (std::vector <std::string>::const_iterator it = resolutions.begin (); it != resolutions.end (); ++ it)
	{
		SW_DEBUG ("New mode: %s", it->c_str ());
		cbo->addItem (new StrListItem ((CEGUI::utf8*) m_ceguiSkinName.c_str (), (CEGUI::utf8*) it->c_str (), "", 0));
		if (*it == usedResolution)
		{
			cbo->setItemSelectState (cbo->getItemCount () - 1, true);
		}
	}

	cbo->handleUpdatedListItemData ();

	if (CEGUIUtility::isWindowPresent ("OptionsWindow/OptionsWindowTab"))
	{
		CEGUI::TabControl* tc = static_cast <CEGUI::TabControl*> (CEGUIUtility::getWindow ("OptionsWindow/OptionsWindowTab"));
		if (tc)
		{
			if (tc->getTabCount () > 0)
			{
				tc->invalidate (true);
				tc->invalidateRenderingSurface ();
			}
		}
	}

	return true;
}



bool OptionsWindow::onGrabMouseChanged(const CEGUI::EventArgs& evt)
{
	const CEGUI::MouseEventArgs& we =
			static_cast<const CEGUI::MouseEventArgs&>(evt);

	CEGUIUtility::ToggleButton* cbo = static_cast<CEGUIUtility::ToggleButton*>(we.window);
	
	Options::getInstance()->setGrabMouseInWindowedMode(cbo->isSelected());
	
	return true;
}


unsigned int OptionsWindow::getColorSelectionIndex(std::string name)
{
	std::string list[] = {
			"white",
			"black",
			"red",
			"green",
			"blue",
			"yellow",
			"magenta",
			"cyan",
			"orange",
			"pink",
			"purple",
			"cornflower_blue"
	};

	for (unsigned int i = 0; i < 12; i++)
	{
		if (name == list[i])
		{
			return i;
		}
	}

	return 0;
}

