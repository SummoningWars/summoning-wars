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

#include "sumwarshelper.h"

// Instance of the singleton.
template<> SumwarsHelper* Ogre::Singleton<SumwarsHelper>::ms_Singleton = 0;

/**
 * Constructor; will call the init function.
 */
SumwarsHelper::SumwarsHelper ()
	: portable_ (false)
{
	init ();
}


/**
* \brief Initialize the helper; this will also reload the list of supported features.
*/
void SumwarsHelper::init ()
{
	supportedFeatures_.clear ();

#if defined (_WIN32)
	supportedFeatures_.push_back ("non-exclusive-fullscreen"); // aka windowed (fullscreen) mode.

	// Feature to allow some settings to be pre-loaded from a different config file:
	// see getPreloadConfigFile.
	supportedFeatures_.push_back ("allows-preload");

	// Feature that allows the game to run in a portable mode (E.g. just like on a USB drive)
	// This requires that no data is stored anywhere outside the portable folder.
	// This will typically be the same folder from where the exe is running.
	supportedFeatures_.push_back ("portable-mode");

#elif defined (__unix__)
#elif defined (__APPLE__)
#endif
}



/**
 * Search in the list of stored features for a specific one.
 */
bool SumwarsHelper::hasFeature (const std::string& featureName) const
{
	for (std::vector <std::string>::const_iterator it = supportedFeatures_.begin ();
		it != supportedFeatures_.end (); ++ it)
	{
		if (*it == featureName)
		{
			return true;
		}
	}
	return false;
}


/**
 * \brief Retrieve the preload file name. Should not be used if the preload feature is not available.
 */
std::string SumwarsHelper::getPreloadFileName () const
{
	// TODO: investigate whether an OS specific file name should be used
	return std::string ("swpreload.cfg");
}


/**
 * \brief Get the path to use for storage.
 */
std::string SumwarsHelper::getStorageBasePath () const
{
	std::string pathToUse (PHYSFS_getUserDir ());

	// For portable applications, the default folder is the exe folder.
	if (portable_)
	{
		pathToUse = PHYSFS_getBaseDir ();
	}

	return pathToUse;
}



/**
 * \fn static Ogre::String userPath()
 * \brief Returns the writable sumwars directory in the users directory
 */
Ogre::String SumwarsHelper::userPath ()
{
	//Ogre::String path = PHYSFS_getUserDir ();
	Ogre::String path = SumwarsHelper::getSingletonPtr ()->getStorageBasePath ();
	
#if defined (_WIN32)
	path.append(".sumwars");
#elif defined (__unix__)
	path.append(".sumwars");
#elif defined (__APPLE__)
	path.append("/Library/Application Support/Sumwars");
#endif

	return path;
}




std::string SumwarsHelper::getUpdatedResolutionString (const std::string& initialString, int newWidth, int newHeight)
{
	std::string returnValue;
	returnValue = initialString;

	std::string sLeft, sRight;
	int nPos = returnValue.find (" ");
	if (nPos == std::string::npos)
	{
		// some error...
		return "";
	}

	sLeft = returnValue.substr (0, nPos);
	sRight= returnValue.substr (nPos + 3); // + 3 chars for " x "
	nPos = sRight.find (" ");
	std::string sAux;

	if (nPos == std::string::npos)
	{
		// we don't have a colour depth.
		sAux = sRight;
		sRight = "";
	}
	else
	{
		sAux = sRight.substr(0, nPos);
		sRight = sRight.substr(nPos);
	}

	// Use a string stream to parse the needed string.
	std::stringstream ss;
	ss << newWidth << " x " << newHeight << sRight;
	returnValue = ss.str ();

	return returnValue;
}


void SumwarsHelper::getSizesFromResolutionString (const std::string& initialString, int& videoModeWidth, int& videoModeHeight)
{
	std::string tempString;
	tempString = initialString;

	std::string sLeft, sRight;
	int nPos = tempString.find (" ");
	if (nPos == std::string::npos)
	{
		// some error...
		return;
	}

	sLeft = tempString.substr (0, nPos);
	sRight= tempString.substr (nPos + 3); // + 3 chars for " x "
	nPos = sRight.find (" ");
	std::string sAux;

	if (nPos == std::string::npos)
	{
		// we don't have a colour depth.
		sAux = sRight;
		sRight = "";
	}
	else
	{
		sAux = sRight.substr(0, nPos);
		sRight = sRight.substr(nPos);
	}

	videoModeWidth = atoi (sLeft.c_str ());
	videoModeHeight = atoi (sAux.c_str ());
}
