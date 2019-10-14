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

#ifndef __SUMWARS_CORE_SUMWARSHELPER_H__
#define __SUMWARS_CORE_SUMWARSHELPER_H__

#include "config.h"

#include <physfs.h>
#include <OgreString.h>

// The standard string
#include <string>
// The standard container for stl.
#include <vector>
// The used singleton manager.
#include "OgreSingleton.h"

/**
 * \brief Helper class. Used as a namespace to store utility static functions.
 * Functions provided here relate to parsing of the resolution string found in
 * various OGRE drivers and the OS specific path parsing to avoid adding #if/#ifdef clauses
 * all over the code.
 */
class SumwarsHelper
		: public Ogre::Singleton <SumwarsHelper>
{
protected:
	// A list of supported features.
	std::vector <std::string> supportedFeatures_;

	// The available aspect ratio folders. Will store only the name of the folder that depicts a certain aspect ratio (not full path).
	std::map <std::string, double> guiAspectRatios_;

	// The default fonts to use for each particular screen size. Note: only the height is relevant for this calculation.
	std::map <int, std::string> guiDefaultFonts_;

	// The available application internal configurable global vars.
	std::map <std::string, std::string> applicationGlobalVars_;

	// This is the set of resource group location that could not be loaded.
	std::map <std::string, std::string> incompleteResourceGroupDirs_;


public:
	/**
	 * Constructor; will call the init function.
	 */
	SumwarsHelper ();

	/**
	 * \brief Initialize the helper; this will also reload the list of supported features.
	 */
    bool init ();


	/**
	 * \brief Search in the list of stored features for a specific one.
	 * \param featureName The name of the feature to look for.
	 */
	bool hasFeature (const std::string& featureName) const;


	/**
	 * \brief Retrieve the preload file name. Should not be used if the preload feature is not available.
	 */
	std::string getPreloadFileName () const;

	/**
	 * \brief Get the path to use for storage.
	 */
	static const std::string& getStorageBasePath();


	/**
	 * \fn static Ogre::String gameDataPath()
	 *
	 * \brief Returns the path where the game data is stored (e.g.,
	 * /usr/share/sumwars in Unix)
	 */
	static Ogre::String gameDataPath()
	{
		Ogre::String path = userPath();

#if defined (__unix__)
		// redefine if necessary
		path = SUMWARS_PREFIX "/" SUMWARS_SHARE_DIR;
#elif defined (__APPLE__)
		// redefine if necessary
#elif defined (_WIN32)
		// redefine if necessary
#endif

		return path;
	}

	/**
	 * \fn static Ogre::String userPath()
	 * \brief Returns the writable sumwars directory in the users directory
	 */
	static Ogre::String userPath();

	/**
	 * \fn static Ogre::String savePath()
	 * \brief Returns the directory with sumwars save files
	 */
	static Ogre::String savePath();


	/**
	 * \brief Gets the updated resolution string from a base resolution string.
	 * Purpose: manage differences between Direct3D and OpenGL resolution strings.
	 *
	 * \param initialString The source resolution string.
	 * \param newWidth The new width to use.
	 * \param newHeight The new height to use.
	 * \return The full updated name of the entry for the resolution.
	 * \author Augustin Preda (if anything doesn't work, he's the one to bash).
	 */
	static std::string getUpdatedResolutionString (const std::string& initialString, int newWidth, int newHeight);

	/**
	 * \brief Gets the native resolution on the desktop.
	 * Purpose: Setting resolution on initial startup
	 *
	 * \return Full native desktop resolution
	 * \author Stefan Stammberger (if anything doesn't work, he's the one to bash).
	 */
	static std::string getNativeResolutionString ();

	/**
	 * \brief Gets the colour depth on the desktop.
	 * Purpose: Setting colour depth on initial startup for the D3D Ogre driver
	 *
	 * \return Return the colour depth on windows
	 * \author Stefan Stammberger (if anything doesn't work, he's the one to bash).
	 */
	static std::string getNativeColourDepthString ();

	/**
	 * \brief Parse a basic resolution string and get the width and height.
	 * Purpose: manage differences between Direct3D and OpenGL resolution strings.
	 *
	 * \param initialString The source resolution string.
	 * \param[out] newWidth The width to read.
	 * \param[out] newHeight The height to read.
	 * \return Nothing.
	 * \author Augustin Preda (if anything doesn't work, he's the one to bash).
	 */
	static void getSizesFromResolutionString (const std::string& initialString, int& videoModeWidth, int& videoModeHeight);

	/**
	 * Get the nearest aspect ratio compatible with the current resolution.
	 * This is very useful for windowed mode.
	 * @param width The width of the display window.
	 * @param height The height of the display window.
	 * @return A string containing the nearest folder name corresponding to the given window size.
	 * @author Augustin Preda.
	 */
	std::string getNearestAspectRatioStringForWindowSize (int width, int height);

	/**
	 * Get the nearest font to be used as a default font for the current resolution.
	 * @param width The width of the display window.
	 * @param height The height of the display window.
	 * @return A string containing the name of the recommended font to be used.
	 * @author Augustin Preda.
	 */
	std::string getRecommendedDefaultFontForWindowSize (int width, int height);

	/**
	 * Add a list of locations to a specified resource group.
	 * @param locList The list (vector actually) of locations.
	 * @param groupToAddTo The name of the resource group to add the locations to.
	 * @author Augustin Preda
	 */
	void addResourceLocationList (const std::vector<std::string> &locList, const std::string &groupToAddTo);

	/**
	 * Add a location to a specified resource group.
	 * @param location The location.
	 * @param groupToAddTo The name of the resource group to add the locations to.
	 * @author Augustin Preda
	 */
	void addResourceLocation (const std::string& location, const std::string& groupToAddTo);

	/**
	 * Add a location to a specified resource group.
	 * @param location The location.
	 * @param groupToAddTo The name of the resource group to add the locations to.
	 * @param resourceType The type of resource (E.g. "FileSystem")
	 * @author Augustin Preda
	 */
	void addResourceLocation (const std::string& location, const std::string& resourceType, const std::string& groupToAddTo);

	/**
	 * Get the associated value for an special application environment variable.
	 * This could be an OS environment variable.
	 * @param varName The name of the variable.
	 * @return The value of the variable, given as a string.
	 * @author Augustin Preda.
	 */
	std::string getCustomVariableValue (const std::string& varName) const;


	/**
	 * Obtain access to a mapping of lists of global variables.
	 * @author Augustin Preda.
	 */
	std::map <std::string, std::string>& getEditableApplicationVariablesMapping ()
	{
		return applicationGlobalVars_;
	}

	/**
	 * Set the preffered aspect ratio string. Uses getEditableApplicationVariablesMapping.
	 * @param aspectRatio The aspect ratio (as a string), in the same format retrieved by the function "getNearestAspectRatioStringForWindowSize".
	 * @author Augustin Preda.
	 */
	void setPrefferedAspectRatioString (const std::string& aspectRatio);

	/**
	 * Retry to add the directories/resource locations for all groups that failed so far.
	 * In order for this function to work, you must add resource locations using the functions defined in this class.
	 *  - addResourceLocation
	 *  - addResourceLocationList
	 * If you only add resource locations using the standard OGRE functions, this function will have no effect.
	 *
	 * This will be mainly used to load resource groups after the aspect ratio is calculated.
	 * The aspect ratio will be specified as a custom string in the resource paths. That custom string (E.g. "$(ASPECT_RATIO)") will need to be replaced by the string
	 * of the actual aspect ratio, corresponding to a folder name (E.g. "016_009");
	 * @author Augustin Preda
	 */
	void retryToAddIncompleteResourceDirs ();


#if defined (__APPLE__)
	// TODO: copied function here from application.h; Investigate integration with userPath function.
	// TODO: investigate removal of original function from application.h

	/**
	 * \fn Ogre::String macPath()
	 * \brief Returns the path to the Resources directory on mac.
	 */
	Ogre::String macPath();
#endif

};
#endif // __SUMWARS_CORE_SUMWARSHELPER_H__
