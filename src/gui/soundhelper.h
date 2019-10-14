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

#ifndef __SUMWARS_GUI_SOUNDHELPER_H__
#define __SUMWARS_GUI_SOUNDHELPER_H__

//
// Includes
//

// string support
#include <string>

// XML help class.
#include "elementattrib.h"

// Geometry utility functions built into sumwars.
#include "geometry.h"


///
/// A helper class for sound operations that are not part of the used libraries and don't make sense to be included in the
/// libraries (E.g. if they would create additional dependencies).
///
class SoundHelper
{
public:

	/// Load a file (E.g. an XML file) containing sound group definitions.
	static bool loadSoundGroupsFromFile (const std::string& fileName);

	/// Return the full path to the music file (passing through Ogre resource management), from the received file name.
	static std::string getNameWithPathForMusicTrack (const std::string& trackName);

	/// Return the full path to the sound file (passing through Ogre resource management), from the received file name.
	static std::string getNameWithPathForSoundFile (const std::string& soundFile);

	/// Play a sound for the current player, at his location. (Basically a 2D sound).
	static void playAmbientSoundGroup (const std::string& soundGroupID);

	/// Play a sound group at a given location.
	static void playSoundGroupAtPosition (const std::string& soundGroupID, double posX, double posY, double posZ);
	
	/// Add a track to a playlist by using only the file name (without the path).
	/// @param playlistName The name of the playlist to add the music track to
	/// @param shortFileName The name of the file containing the track. It doesn't need a path.
	/// @return true of the track is added successfully, false otherwise.
	static bool addPlaylistTrackByShortName (const std::string& playlistName, const std::string& shortFileName);

	/// Load a playlist from an XML file.
	/// @param fileName The name of the XML file containing the playlist tracks.
	static bool loadPlaylistFromXMLFile (const std::string& fileName);

	/// Send a signal to the sound manager so that it knows that it needs to call elapseTime.
	/// Calling this function will do nothing if the sound manager is controlled in a separate thread.
	static void signalSoundManager ();

protected:
	/// Load content from an XML Node.
	/// @param node The XML node to load from. Note: currently tied to the tinyxml implementation.
	static void loadSoundInfos (TiXmlNode* node);

	/// Load content from an XML Node.
	/// @param node The XML node to load from. Note: currently tied to the tinyxml implementation.
	static bool loadPlaylistFromXMLNode (TiXmlNode* node);


private:
	/// Specify whether the sound manager is controlled and accessed in a single thread mode or not.
	/// Possible values:
	///	 - true: the sound manager is accessed in a single threaded way. All operations are called from the main thread.
	///  - false: the sound manager receives play commands from the main thread, but also has a dedicated thread to elapse time. This 
	///           allows it to also play while the game is loading data.
	static bool singleThreadedSoundAccess_;
};


#endif // __SUMWARS_GUI_SOUND_H__

