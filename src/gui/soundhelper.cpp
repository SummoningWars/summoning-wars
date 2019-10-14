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

//
// Includes
//

// Own header
#include "soundhelper.h"

// Debugging helper
#include "debug.h"

// Resource management and control for Ogre.
#include "OgreArchive.h"
#include "OgreResourceGroupManager.h"

// Sound library
#include "gussound.h"


// Allow a shorter name for the sound manager.
using gussound::SoundManager;



bool SoundHelper::singleThreadedSoundAccess_ = true;


///
/// Load a file (E.g. an XML file) containing sound group definitions.
///
bool SoundHelper::loadSoundGroupsFromFile (const std::string& fileName)
{
	// TODO: get extension from file. Create parsing implementation based on extension (E.g. XML).
	// Current assumption: the file is an XML file.

	// Load XML document.
	TiXmlDocument doc (fileName.c_str ());
	bool loadOkay = doc.LoadFile();

	if (! loadOkay)
	{
		// Failed to load.
		ERRORMSG ("Failed to load file %s", fileName.c_str ());
		return false;
	}

	loadSoundInfos (&doc);


	return true;
}


///
/// Load content from an XML Node.
///
void SoundHelper::loadSoundInfos (TiXmlNode* node)
{
	// File was loaded... time to parse the contents.
	TiXmlNode* child;
	if (node->Type () == TiXmlNode::TINYXML_ELEMENT && !strcmp (node->Value (), "Sound"))
	{
		ElementAttrib attr;
		attr.parseElement (node->ToElement ());
		
		std::string groupName;
		float probability = 0.0f;
		int sumOfMemberWeights = 0;

		attr.getString ("name", groupName);
		attr.getFloat ("probability", probability, 1.0);

		// Add the group to the repository.
		SoundManager::getPtr ()->addSoundGroup (groupName);

		for (child = node->FirstChild (); child != 0; child = child->NextSibling ())
		{
			if (child->Type () == TiXmlNode::TINYXML_ELEMENT)
			{
				attr.parseElement (child->ToElement ());
				if (! strcmp (child->Value (), "Soundfile"))
				{
					std::string fileName;
					int weight;

					attr.getString ("source", fileName);
					attr.getInt ("weight", weight, 1.0);

					sumOfMemberWeights += weight;
                    
					DEBUGX ("loading sound file %s for sound %s", fileName.c_str (), groupName.c_str ());

					// Add/load the sound file
					try
					{
						std::string fullPath = getNameWithPathForSoundFile (fileName);
						SoundManager::getPtr ()->getRepository ()->addSound (fileName.c_str ()  // name
																		, fullPath.c_str ()  // file & path
																		, true // load entire sound into memory? don't do it for songs! only for small sounds.
																		, gussound::GSC_Effect  // a category for the sound; you can change the volume for all sounds in a category.
																		, false); // want to make sure this sound is only played from one source at one time?
					}
					catch (std::exception&)
					{
						// An exception is thrown if the sound already exists.
						// That's ok for us, as we're using the file name as the sound name, so if it exists, it means it's already in the system.
					}

					// Assign the sound file to the group.
					SoundManager::getPtr ()->getSoundGroup (groupName)->addSound (fileName.c_str (), weight);
				}
			}
		}

		if (probability < 1.0f)
		{
			// the probability is provided only for playable sounds.
			// we need to add a no-sound entry in this case.
			int emptySoundWeight = 1;
			// probability ... sumOfMemberWeights
			// 1.0 - probability       ... emptySoundWeight
			emptySoundWeight = sumOfMemberWeights * ((1.0 - probability) / probability);
			// we need to calculate the weight for this one.
			SoundManager::getPtr ()->getSoundGroup (groupName)->addEmptySlot (emptySoundWeight);

			SW_DEBUG ("Added for group [%s], probability %f,  member weights %d, final sound weight %d", groupName.c_str (), probability, sumOfMemberWeights, emptySoundWeight);
		}
	}
	else
	{
		// Run through the entire tree
		for ( child = node->FirstChild(); child != 0; child = child->NextSibling())
		{
			loadSoundInfos(child);
		}
	}
}



///
/// Return the full path to the music file (passing through Ogre resource management), from the received file name.
///
std::string SoundHelper::getNameWithPathForMusicTrack (const std::string& trackName)
{
	std::string result (trackName);

	Ogre::FileInfoListPtr files;
	Ogre::FileInfoList::iterator it;
	files = Ogre::ResourceGroupManager::getSingleton ().findResourceFileInfo ("music", trackName);

	it = files->begin ();
	if (it != files->end ())
	{
		std::string filename;
		result = it->archive->getName ();
		result += "/";
		result += it->filename;
	}

	return result;
}



///
/// Add a track to a playlist by using only the file name (without the path).
/// @param playlistName The name of the playlist to add the music track to
/// @param shortFileName The name of the file containing the track. It doesn't need a path.
/// @return true of the track is added successfully, false otherwise.
///
bool SoundHelper::addPlaylistTrackByShortName (const std::string& playlistName, const std::string& shortFileName)
{
	std::string fileName;
	try
	{
		fileName = SoundHelper::getNameWithPathForMusicTrack (shortFileName);
		// One could call the following function to add an "anonymous" track (with a name such as "track_01").
		SoundManager::getPtr ()->addPlaylistTrack (playlistName, fileName);
	}
	catch (std::exception& e)
	{
		SW_DEBUG ("Caught exception while trying to add to playlist [%s] track file [%s]: %s", playlistName.c_str (), fileName.c_str (), e.what ());
		return false;
	}
	return true;
}




///
/// Return the full path to the sound file (passing through Ogre resource management), from the received file name.
///
std::string SoundHelper::getNameWithPathForSoundFile (const std::string& soundFile)
{
	std::string result (soundFile);

	Ogre::FileInfoListPtr files;
	Ogre::FileInfoList::iterator it;
	files = Ogre::ResourceGroupManager::getSingleton ().findResourceFileInfo ("sound", soundFile);

	it = files->begin ();
	if (it != files->end ())
	{
		std::string filename;
		result = it->archive->getName ();
		result += "/";
		result += it->filename;
	}

	return result;
}



///
/// Load a playlist from an XML file.
/// @param fileName The name of the XML file containing the playlist tracks.
///
bool SoundHelper::loadPlaylistFromXMLFile (const std::string& fileName)
{
	// Load XML document.
	TiXmlDocument doc (fileName.c_str ());
	bool loadOkay = doc.LoadFile();

	if (! loadOkay)
	{
		// Failed to load.
		ERRORMSG ("Failed to load file %s", fileName.c_str ());
		return false;
	}

	// Call the actual loading.
	return loadPlaylistFromXMLNode (&doc);
}



///
/// Load content from an XML Node.
/// @param node The XML node to load from. Note: currently tied to the tinyxml implementation.
///
bool SoundHelper::loadPlaylistFromXMLNode (TiXmlNode* node)
{
	bool result = true;

	// File was loaded... time to parse the contents.
	TiXmlNode* child;
	if (node->Type () == TiXmlNode::TINYXML_ELEMENT && !strcmp (node->Value (), "Playlist"))
	{
		ElementAttrib attr;
		attr.parseElement (node->ToElement ());
		
		std::string playlistName;
		bool repeatOption;
		bool shuffleOption;
		//float probability;

		attr.getString ("name", playlistName);
		attr.getBool ("repeat", repeatOption, true);
		attr.getBool ("shuffle", shuffleOption, false);

		// Register the playlist
		SoundManager::getPtr ()->getMusicPlayer ()->registerPlaylist (playlistName);
		SoundManager::getPtr ()->getMusicPlayer ()->setPlaylistRepeat (playlistName, repeatOption);
		SoundManager::getPtr ()->getMusicPlayer ()->setPlaylistShuffle (playlistName, shuffleOption);

		SW_DEBUG ("Got playlist: %s, repeat: %d, shuffle: %d", playlistName.c_str (), repeatOption, shuffleOption);

		for (child = node->FirstChild (); child != 0; child = child->NextSibling ())
		{
			if (child->Type () == TiXmlNode::TINYXML_ELEMENT)
			{
				attr.parseElement (child->ToElement ());
				if (! strcmp (child->Value (), "Track"))
				{
					// We only need the file name for each playlist track.
					std::string fileName;
					attr.getString ("source", fileName);
                    
					result &= SoundHelper::addPlaylistTrackByShortName (playlistName, fileName);
				}
			}
		}
	}
	else
	{
		// Run through the entire tree
		for ( child = node->FirstChild(); child != 0; child = child->NextSibling())
		{
			result &= loadPlaylistFromXMLNode(child);
		}
	}
	return result;
}



///
/// Play a sound for the current player, at his location. (Basically a 2D sound).
///
void SoundHelper::playAmbientSoundGroup (const std::string& soundGroupID)
{
	try
	{
		std::string soundToPlay;
		soundToPlay = SoundManager::getPtr ()->getSoundGroup (soundGroupID)->getRandomSound ();
		if (soundToPlay != gussound::EMPTY_SLOT_NAME)
		{
			SoundManager::getPtr ()->getRepository ()->getSound (soundToPlay)->play2D ();
		}
	}
	catch (std::exception& e)
	{
		SW_DEBUG ("Encountered exception: %s", e.what ());
	}
}



///
/// Play a sound group at a given location.
///
void SoundHelper::playSoundGroupAtPosition (const std::string& soundGroupID, double posX, double posY, double posZ)
{
	try
	{
		std::string soundToPlay;
		soundToPlay = SoundManager::getPtr ()->getSoundGroup (soundGroupID)->getRandomSound ();
		if (soundToPlay != gussound::EMPTY_SLOT_NAME && soundToPlay != gussound::EMPTY_SOUND_FILE)
		{
			SoundManager::getPtr ()->getRepository ()->getSound (soundToPlay)->play3D (posX, posY, posZ);
		}
	}
	catch (std::exception& e)
	{
		// This can happen if the group name is not valid. Or if the group has missing files. Or if the group is empty.
		SW_DEBUG ("Caught exception while trying to play sound group [%s]: %s", soundGroupID.c_str (), e.what ());
	}
}


/// Send a signal to the sound manager so that it knows that it needs to call elapseTime.
/// Calling this function will do nothing if the sound manager is controlled in a separate thread.
void SoundHelper::signalSoundManager ()
{
	if (singleThreadedSoundAccess_)
	{
		SoundManager::getPtr ()->elapseTime ();
	}
}
