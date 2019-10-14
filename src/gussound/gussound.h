#ifndef GUS_SOUND_MAIN_HEADER_H
#define GUS_SOUND_MAIN_HEADER_H

//   This file is part of the gussound library, licensed under the terms of the MIT License.
//
//   The MIT License
//   Copyright (C) 2010-2013  Augustin Preda (thegusty999@gmail.com)
//
//   Permission is hereby granted, free of charge, to any person obtaining a copy
//   of this software and associated documentation files (the "Software"), to deal
//   in the Software without restriction, including without limitation the rights
//   to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
//   copies of the Software, and to permit persons to whom the Software is
//   furnished to do so, subject to the following conditions:
//   
//   The above copyright notice and this permission notice shall be included in
//   all copies or substantial portions of the Software.
//   
//   THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
//   IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
//   FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
//   AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
//   LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
//   OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
//   THE SOFTWARE.
//
//   Sound utility.

// std::map
#include <map>

// std::vector
#include <vector>

// std::stringstream
#include <sstream>

//
// Add guslib inclusions... unless we're dealing with the standalone version
//

#ifdef GUSSOUND_STANDALONE

// A timer
#include <gussoundutil/timer.h>

// Smart pointer
#include <gussoundutil/smartptr.h>

// Abstract factory
#include <gussoundutil/abstractfactory.hpp>

// Implementation of singleton
#include <gussoundutil/singleton.hpp>

// Another implementation of a singleton.
#include <gussoundutil/singletonholder.hpp>

#else

// A timer
#include <guslib/system/timer.h>

// Smart pointer
#include <guslib/common/smartptr.h>

// Abstract factory
#include <guslib/common/abstractfactory.hpp>

// Implementation of singleton
#include <guslib/common/singleton.hpp>

// Another implementation of a singleton.
#include <guslib/common/singletonholder.hpp>

#endif // GUSSOUND_STANDALONE

#ifndef GUSSNDTLVL // gussound trace level
#define GUSSNDTLVL 4
#endif

namespace gussound
{
	//
	// Forward class declarations.
	//
	class AbstractSound;
	class AudioDevice;
	class EffectableSound;
	class MusicPlayer;
	class MusicPlayerListener;
	class Playlist;
	class PlaylistListener;
	class SoundEffect;
	class SoundEffectListener;
	class SoundFinishingTrackerInterface;
	class SoundGroup;
	class SoundRepository;

	//
	// Enums
	//

	typedef enum SoundCategoryEnum
	{
		GSC_Effect = 0, // the standard sound category, most sounds should be in this category (menu events, game events)
		GSC_Music, // the music category, to group up the music related sounds.
		GSC_Voice, // the voice category, such as mission briefings, character speech, etc.
		GSC_Master, // this category affects all other categories. It shouldn't be assigned to sounds directly (if it is, it will be squared => you'll get a lower output volume)
		GSC_Custom1,
		GSC_Custom2,
		GSC_Custom3,
		GSC_None
	} SoundCategory;


	typedef enum PlayStateEnum
	{
		GPLS_Stopped=0, // the sound, list or player is stopped.
		GPLS_Playing, // sound is playing.
		GPLS_Paused // sound play is paused.
	}PlayState;


	typedef enum ActionIfPlayingEnum
	{
		ActionDoNothing,
		ActionRestart,
		ActionParallelPlay,
		ActionEnqueue
	} ActionIfPlaying;

	//
	// Typedefs
	// Redefine some types.
	//

	typedef GSmartPtr <SoundEffect> SoundEffectPtr;
	typedef GSmartPtr <AbstractSound> AbstractSoundSmartPtr;
	typedef GSmartPtr <EffectableSound> EffectableSoundSmartPtr;
	typedef GSmartPtr <AudioDevice> AudioDeviceSmartPtr;
	typedef GSmartPtr <SoundRepository> SoundRepositorySmartPtr;
	typedef GSmartPtr <Playlist> PlaylistSmartPtr;
	typedef GSmartPtr <MusicPlayer> MusicPlayerSmartPtr;
	typedef GSmartPtr <SoundGroup> SoundGroupSmartPtr;
	

	typedef double SoundDuration;
	typedef int SampleRate;
	typedef float SoundVolume;

	typedef std::map <std::string, std::string> TagList;
	typedef std::map <std::string, EffectableSoundSmartPtr> SoundMap;
	typedef std::map <std::string, PlaylistSmartPtr> PlaylistMapping;
	typedef std::map <std::string, SoundGroupSmartPtr> SoundGroupMapping;
	typedef std::vector <SoundEffectPtr> SoundEffectList;

	//
	// Constants.
	//

	const SoundVolume max_volume = 1.0;
	const SoundVolume min_volume = 0.0;
	const SoundDuration standard_fade_duration = 2000;
	const std::string EMPTY_SLOT_NAME = "_";
	const std::string EMPTY_SOUND_FILE = "";

	///
	/// An exception class.
	///
	class SoundException
		: public std::exception
	{
	private:
		std::string exceptionMessage_;

	public:
		SoundException (const char* text);
        virtual ~SoundException() throw();
        const char* what () const throw();
	};

	///
	///	An abstraction of audio devices.
	///
	class AudioDevice
	{
	public:
		AudioDevice ();
		virtual ~AudioDevice ();
	};

	///
	///	The abstract interface for a sound.
	///
	class AbstractSound
	{
	protected:
		///
		/// Try to load a sound from a device and a file name.
		///	@return true if loaded successfully, false otherwise.
		///	@note This function also sets some internal parameters (such as the sampleRate_).
		///	It also sets the volume for the sound to the object saved member.
		///
		virtual bool loadFrom (const gussound::AudioDeviceSmartPtr & devicePtr, 
			const std::string &fileName, bool loadInMemory, bool onlyOneInstance) = 0;

	public:

		/// virtual destructor required for smart ptr implementation.
		virtual ~AbstractSound () {}  

		/// Get the low-level output volume.
		virtual SoundVolume getOutputVolume () = 0;

		/// Get the duration of time played so far.
		virtual SoundDuration getSeekDuration () = 0;

		/// Tell whether the sound received the play command with the repeat flag, meaning it will
		/// play continuously until a stop is issued.
		virtual bool isRepeating () = 0;

		/// Pause the sound.
		virtual void pause () = 0;

		/// Commence the low-level sound play. Note that certain actions may be performed if the sound is already
		/// playing, but not all possible actions are supported at this low level.
		virtual void play2D (ActionIfPlayingEnum actionIfPlaying = ActionRestart, bool repeatContinuously = false) = 0;

		/// Commence the low-level sound play with 3D positioning. Will default to 2D play unless overridden.
		/// This is a fire & forget function. You cannot update the source of the sound after it is launched.
		virtual void play3D (double posX, double posY, double posZ, ActionIfPlayingEnum actionIfPlaying = ActionRestart, bool repeatContinuously = false);

		/// Commence the low-level sound play with 3D positioning. Will default to 2D play unless overridden.
		/// This allows you to update the source of the sound after the play command is launched.
		/// @return An identifier to the token or source of the sound. This can be used later to edit
		virtual int play3Dnamed (double posX, double posY, double posZ, ActionIfPlayingEnum actionIfPlaying = ActionRestart, bool repeatContinuously = false);

		/// Get the duration of the sound.
		virtual SoundDuration getDuration() = 0;

		/// Get the sample rate of the sound.
		virtual SampleRate getSampleRate() = 0;

		/// Get the current play state.
		virtual const PlayState & getState () const = 0;

		/// Seek to a certain time within the sound.
		virtual void seekToTime (const SoundDuration & duration) =0;

		/// Set the output volume. Normally, a sound's volume will be influenced by things such as:
		/// the global sound volume within the application, the sound for the category of this sound (such as music
		/// voice, interface) as well as a correction factor for the sound. All the influencing factors should be
		/// combined and the result passed to this function.
		virtual void setOutputVolume (const SoundVolume & volume) = 0;

		/// Stop the low-level sound.
		virtual void stop() = 0;
	};



	///
	/// Listener for sound effect events.
	///
	class SoundEffectListener
	{
	public:
		/// Callback for when the effect has ended.
		virtual void onEffectEnded (const SoundEffect & effect) = 0;

		/// Virtual dtor. Required for smart ptr implementation.
		virtual ~SoundEffectListener () {}
	};


	///
	/// A sound that supports effects being applied to it.
	///
	class EffectableSound
		: public AbstractSound
		, public SoundEffectListener
	{
	protected:
		/// Volume value of internally played sound.
		SoundVolume volume_; // this should be controlled from a higher level.
		SoundVolume categoryVolume_;
		SoundVolume effectVolume_;

		SoundEffectList tempEffectList_;
		SoundEffectList deletionList_;

		SoundFinishingTrackerInterface * tracker_;
		SoundDuration trackerTime_;
		SoundCategory category_;
		// Sound's play state_.
		PlayState state_;

		std::string id_;

		/// If this is set to true, only one instance of the sound is played. If it's false, each sound play will open a new stream.
		/// @see SoundEffectType usage from audiere.
		bool onlyOneInstance_;

		/// Specifies whether the sound is coming from a stream (being played as soon as it comes, without requiring the complete sound data to be available)
		/// or whether the sound is loaded into memory.
		bool streamed_;

	public:

		/// Constructor.
		/// @param id The ID to give to the sound.
		EffectableSound (const std::string & id = "");

		/// Destructor.
		virtual ~EffectableSound ();

		/// Getter for the sound ID.
		virtual const std::string & getId () const {return id_;}

		/// Getter for the sound category.
		virtual const SoundCategory & getCategory () const {return category_;}

		/// Getter for the play state of the sound.
		virtual const PlayState & getState () const {return state_;}

		/// Getter for the volume.
		virtual const SoundVolume & getVolume() const {return volume_;}

		/// Add an effect to the sound. 
        virtual void addEffect (SoundEffectPtr effect);

		/// Clear all effects from the current sound.
		virtual void clearAllEffects ();

		/// Time elapsing function. Make the sound react to time passing by. Will also apply assigned sound effects.
		/// @param time The amount of time that elapsed.
		virtual void elapseTime (const guslib::TimeUnits & time);

		/// Getter for the combined value of a sound volume.
		virtual SoundVolume getCombinedVolumeModifiers ();

		/// Callback for a finished sound effect.
		virtual void onEffectEnded (const SoundEffect & effect);

		/// Setter for the sound category.
		virtual void setCategory (const SoundCategory & cat) {category_ = cat;}

		/// Setter for the volume modifier of the sound category.
		virtual void setVolumeCategoryModifier (const SoundVolume & volume);

		/// Setter for the extra modifier for a sound. This is called internally by sound effects.
		virtual void setVolumeExtraModifier (const SoundVolume & volume);

		/// Set the main volume value. Note: the overall sound output value is also influenced by the category modifier and effect (extra) modifier.
		virtual void setVolume (const SoundVolume & volume);

		/// Set the listener (tracker) of the sound finishing effect to use.
		virtual void setTracker (SoundFinishingTrackerInterface* ptr, SoundDuration dur );

		/// Remove the listener (tracker)
		virtual void clearTracker ();

		friend class SoundRepository;
	};

	///
	/// Obtain the event of a sound play ending.
	///
	class SoundFinishingTrackerInterface
	{
	public:
		virtual void onSoundEvent () = 0;

		virtual ~SoundFinishingTrackerInterface () {} // virtual destructor required for smart ptr implementation.
	};


	///
	/// Listener for events from the music player.
	///
	class MusicPlayerListener
	{
	public:
		virtual void OnTrackPlay (const std::string & newTrack) = 0;
		virtual void OnTrackChange (const std::string & newTrack) = 0;
		virtual void OnPlaylistEnd () = 0; // will be encountered in non-repeat mode only.

		virtual ~MusicPlayerListener (){} // virtual destructor required for smart ptr implementation.
	};

	///
	/// Listener for events regarding the playlist.
	///
	class PlaylistListener
	{
	public:
		virtual void OnTrackPlay (const std::string & newTrack) = 0;
		virtual void OnTrackChange (const std::string & newTrack ) = 0;
		virtual void OnPlaylistEnd () = 0;

		virtual ~PlaylistListener () {} // virtual destructor required for smart ptr implementation.
	};


	///
	/// A general interface for a media player.
	///
	class MusicPlayerInterface
	{
	public:
		virtual void play () = 0; // just like clicking the play button in a media player application.

		virtual void stop () = 0; // just like clicking the stop button in a media player application.

		virtual void nextTrack () = 0; // next track in list.

		virtual void previousTrack () = 0; // previous track in list.

		virtual void pause () = 0; // the resuming can be done by either calling pause again or play.

		virtual void setListener (MusicPlayerListener * listener) = 0;

		virtual void switchToPlaylist (const std::string & playlistName, bool pauseExisting = true) = 0;

		virtual ~MusicPlayerInterface () {} // virtual destructor required for smart ptr implementation.
	};



	///
	///	Base class for sound effects (for advanced sounds).
	///
	class SoundEffect
	{
	protected:
		guslib::TimeUnits internalTimer_;
		guslib::TimeUnits startFrom_;
		guslib::TimeUnits stopAt_;
		guslib::TimeUnits targetLength_;
		EffectableSoundSmartPtr ptrToSound_;
		bool ended_;
	public:
		SoundEffect (EffectableSoundSmartPtr & target, guslib::TimeUnits startFrom, guslib::TimeUnits stopAt);

		virtual ~SoundEffect ();

		virtual void applyEffect ();
		virtual void elapseTime (const guslib::TimeUnits & time);
		/// Define action to be taken when the effect ends (for instance, a fade-out would stop the sound at the end.
		virtual void endEffect (bool force = false); 
		virtual bool equals (const SoundEffect & rhs);
		virtual void reset ();
		virtual void stop ();
	};

	

	///
	/// A simple, linear, fade-in effect
	///
	class FadeInEffect
		: public SoundEffect
	{
	protected:
		SoundVolume minVolume_;
	public:
		FadeInEffect (EffectableSoundSmartPtr & target
			, const guslib::TimeUnits & duration
			, const SoundVolume & minVolume = gussound::min_volume);

		FadeInEffect (EffectableSoundSmartPtr & target
			, const guslib::TimeUnits & fromTime
			, const guslib::TimeUnits & duration
			, const SoundVolume & minVolume);

		virtual ~FadeInEffect ();
		virtual void applyEffect ();

		/// Define action to be taken when the effect ends (for instance, a fade-in would make sure that the sound level is appropriate at the end.
		/// @param force Ignored
		virtual void endEffect (bool force = false);
	};


	///
	/// A simple, linear, fade out effect.
	///
	class FadeOutEffect
		: public SoundEffect
	{
	protected:
		SoundVolume minVolume_;
		bool stopOnEnd_; // true if the sound stops afterwards, false if the sound is paused.
	public:
		FadeOutEffect (EffectableSoundSmartPtr & target
			, const guslib::TimeUnits & duration
			, const SoundVolume & minVolume = gussound::min_volume
			, bool stopAtEnd = true);

		FadeOutEffect (EffectableSoundSmartPtr & target
			, const guslib::TimeUnits & startFrom
			, const guslib::TimeUnits & stopAt
			, const SoundVolume & minVolume
			, bool stopAtEnd = true);

		virtual ~FadeOutEffect ();

		virtual void applyEffect ();

		/// Define action to be taken when the effect ends (for instance, a fade-out would stop the sound at the end.
		/// @param force If the force flag is set to true, the stop commands for the sound are suppressed.
		/// This is performed in order to prevent fade-in and fade-out effects running for the same sound and performing
		/// an unplanned stop operation when the fade-out is finished.
		virtual void endEffect (bool force = false);
	};


	///
	///	Define a group of sounds. This is a utility class meant to help the user add randomness in the usage
	///	of sounds. For instance, you may want to use one of several sounds for highly repetitive actions. Imagine
	///	selecting a unit in an RTS/RPG game and ordering it to move. You would issue normally several move orders.
	///	It would be much more boring to just hear "yes" on each move command that hearing a combination of "yes", "okay",
	///	"roger", "on my way".
	///	In addition you may desire to not play all of them in the same ratio. Some sounds could be uses rarely to
	///	enhance the replay-ability. Imagine having a snake game and playing some sounds after picking a bonus.
	///	You may play a sound like "yummy" after each bonus pick-up, but use "delicious" every once in a while, something
	///	like 5% of the times, or maybe you want to play no sound most of the time and play a sound to remove the
	///	monotony every now and then.
	///
	class SoundGroup
	{
	protected:
		SoundGroup ();

		// the map will contain the correlation between a sound's name and the weight it has.
		std::map<std::string, int> sounds_;

		// the sum of all weights; kept in memory to prevent unnecessary re-calculations.
		int currentWeightSum_;

	public:
		virtual ~SoundGroup ();

		// add a non-sound (the option to play no sound).
		virtual void addEmptySlot (int weight);

		// add a sound. The odds of that sound actually playing are given by the weight parameter.
		virtual void addSound (const std::string & soundName, int weight);

		// remove the empty slot (the option to play no sound).
		virtual void removeEmptySlot ();

		// remove a sound from the group.
		virtual void removeSound (const std::string & soundName);

		// obtain the name of a random sound.
		virtual std::string getRandomSound () const;

		friend class SoundManagerUtil;
	};


	///	
	///	For a music player one can define a playlist.
	///	This will ensure that music is more varied. For instance, let's say you have a strategy game. At
	///	each point, you may choose to be on some terrain, having a different playlist for each terrain
	///	type. The playlist management should be made much easier using this class.
	///	
	class Playlist
		: public SoundFinishingTrackerInterface
	{
	protected:
		std::string currentTrack_;
		int currentTrackIdx_;

		bool autoSkipToNextTrack_;
		std::vector<std::string> tracks_;

		// Keep track of the number of times each track was played. This is especially
		// important for shuffle and repeat functionality.
		std::vector<int> playCount_;

		// The tracks in the order they were played in.
		std::vector<int> playOrder_;

		// specify whether to use a fade-in effect for all play commands.
		bool fadeInAtPlay_;

		// specify whether to use a fade-in effect for all stop commands.
		bool fadeOutAtStop_;

		// specify whether to use a fade-in effect for all pause commands.
		bool fadeOutAtPause_;

		// specify whether to use the repeat.
		bool repeat_;

		// specify whether to use shuffle or not.
		bool shuffle_;

		PlayState state_;

		SoundRepositorySmartPtr ptrToRepository_;
		PlaylistListener * ptrToListener_;

		std::string id_;

		SoundDuration fadeDuration_;

	public:
		Playlist ();

		Playlist (const SoundRepositorySmartPtr & repository_
			, PlaylistListener * listener
			, const std::string & id = "noname");

		Playlist (const Playlist & rhs);

		virtual ~Playlist ();

		virtual void addTrack (const std::string & trackName);

		virtual SoundDuration getFadeDuration () const { return fadeDuration_; }

		virtual void pause ();

		virtual void play ();

		virtual void removeTrack (const std::string & trackName);

		virtual void setFade (bool atPlay = true, bool atStop = true, bool atPause = true);

		virtual void setFadeDuration (SoundDuration dur) { fadeDuration_ = dur; }

		virtual void setRepeat (bool value);

		virtual void setShuffle (bool value);

		virtual void setRepository (const SoundRepositorySmartPtr & repository) {ptrToRepository_ = repository;}

		/// Move the iterator to the next item in the list. The play state is maintained.
		virtual void skipToNextTrack (); 

		virtual void skipToPreviousTrack (); 

		virtual void stop ();

		virtual void stop (bool fadeOut);

		virtual void setListener (PlaylistListener * listener) {ptrToListener_ = listener;}

		// override
		virtual void onSoundEvent ();
	};


	///	
	///	This is the class that will handle the loading and unloading of sounds. The sounds ca be loaded either into
	///	memory, or be streamed directly from the disk, but for any use, they need to be added into the repository 
	///	before being used.
	///	
	class SoundRepository
	{
	protected:
		SoundMap sounds_;

		std::vector<SoundVolume> categoryVolumes_;

		gussound::AudioDeviceSmartPtr audioDevice_;

		SoundRepository ();

		void setAudioDevice (const gussound::AudioDeviceSmartPtr & device) {audioDevice_ = device;}

	public:
		virtual ~SoundRepository ();

		virtual void addSound (const std::string &soundName, const std::string & fileName, bool loadIntoMemory = false, SoundCategory cat = GSC_None, bool onlyOneInstance = false);

		virtual EffectableSound * createSound (const std::string& id = "", const std::string& fileExtension = "") = 0;

		virtual EffectableSoundSmartPtr & getSound (const std::string &soundName);

		virtual void elapseTime (const guslib::TimeUnits & time);

		virtual SoundVolume getVolumeForCategory (SoundCategory categ) const;

		virtual void removeSound (const std::string &soundName);

		virtual void setVolumeForCategory (SoundCategory categ, SoundVolume vol);

		friend class SoundManagerUtil;
	};



	///	
	///	An implementation of a media player.
	///	This will be a utility class for handling the music for games. It should allow a more convenient
	///	playlist management, allowing the user to switch from one playlist to another, to reflect game changes
	///	(such as switching from a calm themed playlist to an action playlist when a player enters a dangerous
	///	zone in an RPG ).
	///	
	class MusicPlayer
		: public MusicPlayerInterface
		, public PlaylistListener
	{
	protected:
		PlaylistMapping playlists_;

		PlayState playstate_;

		MusicPlayerListener * listenerPtr_;

		bool shuffle_;

		bool repeat_;

		float volume_; // all items in the playlist will be affected by this value

		// allows the tracks to be changed automatically. Otherwise, when the track ends, you need to
		// switch manually to the next song.
		bool skipToNextTrackAutomatically_;

		// use a vector to tell which tracks were played so far, in order to skip them from double play.
		std::vector<bool> shuffleTracker_;

		// specify whether to use a fade-in effect for all play commands.
		bool fadeInAtPlay_;

		// specify whether to use a fade-in effect for all stop commands.
		bool fadeOutAtStop_;

		// specify whether to use a fade-in effect for all pause commands.
		bool fadeOutAtPause_;

		// the list that is currently being played.
		std::string currentList_;

		// self explanatory: if true, when the lists are changed, the play keeps active.
		bool keepPlayingAtListChange;

		SoundRepositorySmartPtr ptrToRepository_;

		std::string id_;
		SoundDuration fadeDuration_;

	protected:
		MusicPlayer (const std::string & id = "noname");

	public:
		virtual ~MusicPlayer ();

		virtual bool isPlaylistRegistered (const std::string& name);
		virtual void registerPlaylist (const std::string & name);
		virtual void unregisterPlaylist (const std::string & name);

		virtual void addTrackToPlaylist (const std::string & playlistName, const std::string & trackName);
		virtual void removeTrackFromPlaylist (const std::string & playlistName, const std::string & trackName);

		virtual void nextTrack (); // If the last track was just played, will call the listener's OnPlaylistEnd function.

		virtual void stop ();

		virtual void play ();

		virtual void pause ();

		virtual void previousTrack ();

		virtual SoundDuration getFadeDuration () const { return fadeDuration_; }
		virtual bool getRepeat () const { return repeat_;}
		virtual bool getShuffle () const { return shuffle_;}

		virtual void setRepository (const SoundRepositorySmartPtr & repository) {ptrToRepository_ = repository;}

		virtual void setFadePreferrences (bool atPlay = true, bool atStop = true, bool atPause = true);
		virtual void setFadeDuration (SoundDuration dur);
		virtual void setRepeat (bool value);
		virtual void setShuffle (bool value);

		virtual void setPlaylistRepeat (const std::string& playlistName, bool value);
		virtual void setPlaylistShuffle (const std::string& playlistName, bool value);

		virtual void switchToPlaylist (const std::string & playlistName, bool pauseExisting = true);

		// overrides from the playlist listener

		virtual void OnTrackPlay(const std::string & newTrack);
		virtual void OnTrackChange(const std::string & newTrack );
		virtual void OnPlaylistEnd();

		virtual void setListener (MusicPlayerListener * listener );
		friend class SoundManagerUtil;
	};


	///	
	///	This will be the main interface to the application.
	///	It is supposed to be a singleton class, allowing the user's access to the entire audio suite.
	///	
	class SoundManagerUtil
	{
	protected:
		SoundManagerUtil (const SoundRepositorySmartPtr & repos);

		SoundRepositorySmartPtr repository_;

		SoundGroupMapping groups_;

		MusicPlayerSmartPtr player_;

		guslib::Timer timer_; // internal timer.

	public:
		/// Destructor
		virtual ~SoundManagerUtil ();

		/// Access this instead of the constructor directly.
		static SoundManagerUtil* createSoundManager ();

		/// Create and add a sound group.
		virtual void addSoundGroup (const std::string & groupName);

		/// Add a track to a playlist: utility call that bypasses the repository access and music player access.
		/// Please note that the track name will be anonymous.
		virtual void addPlaylistTrack (const std::string & playlistName, const std::string & trackName);

		/// Process some time dependant internal logic.
		/// React to some time units having passed from the last time the function was called.
		/// This will generally be used to control the playlists.
		/// The user of the audio engine will have to call this function on a regular basis
		/// (i.e. in a loop in the main thread or in a separate thread).
		virtual void elapseTime ();

		/// Process some time dependant internal logic.
		/// React to some time units having passed from the last time the function was called.
		/// This will generally be used to control the playlists.
		/// The user of the audio engine will have to call this function on a regular basis
		/// (i.e. in a loop in the main thread or in a separate thread).
		/// @param timeUnits The amount of time units (milliseconds) elapsed since the last call.
		virtual void elapseTimeByMilliseconds (const guslib::TimeUnits& timeUnits);

		/// Access the music player.
		virtual MusicPlayerSmartPtr & getMusicPlayer () { return player_; }

		/// Access the sound repository.
		virtual SoundRepositorySmartPtr & getRepository () { return repository_; }

		/// Get a sound group (groups sounds with a common category).
		virtual SoundGroupSmartPtr & getSoundGroup (const std::string & groupName);
		
		/// Create and return a string containing the version of the sound library
		virtual std::string getVersion () const;

		/// Destroy a sound group.
		virtual void removeSoundGroup (const std::string & groupName);
	};


	///	
	///	A so-called hidden namespace to create anonymous track names.
	/// If the user does not provide a track name, just use an automatically generated name.
	///	
	namespace hidden
	{
		static int trackerCounter_ = 0;
		static std::string getAnonymousTrackName ()
		{
			std::stringstream ss;
			ss<<"track_"<<trackerCounter_;
			++trackerCounter_;
			return ss.str();
		}
	};


	// use this to create the sound manager util
	typedef guslib::Singleton <
		guslib::GeneralFactory <SoundManagerUtil, std::string, SoundManagerUtil* (*) ()>
	> SoundManagerFactory;


	// define an abstract singleton holder, SoundManager

	typedef guslib::SingletonHolder <SoundManagerUtil> SoundManager;

	///
	/// A class to help out with preparing logging
	///
	class SoundManagerLogger
	{
	public:
		/// Set the log file to be used (part of the tracing library).
		static void setLoggerTarget (const std::string& fileName, int level = 5);
	};


} // namespace gussound

//
// Macros for play control commands.
//
#define GSOUND_ADD_PLAYLIST(param) SoundManager::getPtr ()->getMusicPlayer ()->registerPlaylist(param)
#define GSOUND_ADD_SOUND(paramName,paramFile,paramCategory,paramLoad) SoundManager::getPtr ()->getRepository ()->addSound (paramName, paramFile, paramLoad, paramCategory)
#define GSOUND_ADD_TRACK_TO_PLAYLIST(paramList,paramTrack) {std::string tempName = gussound::hidden::getAnonymousTrackName ();\
SoundManager::getPtr ()->getRepository()->addSound(tempName, paramTrack, false, GSC_Music);\
SoundManager::getPtr ()->getMusicPlayer()->addTrackToPlaylist(paramList, tempName);}
#define GSOUND_ELAPSE_TIME() SoundManager::getPtr ()->elapseTime ()
#define GSOUND_PLAY2D(param) SoundManager::getPtr ()->getRepository ()->getSound (param)->play2D ()
#define GSOUND_SET_MUSIC_REPEAT(param) SoundManager::getPtr ()->getMusicPlayer ()->setRepeat (param)
#define GSOUND_SET_MUSIC_SHUFFLE(param) SoundManager::getPtr ()->getMusicPlayer ()->setShuffle (param)
#define GSOUND_SET_VOLUME_FOR_CATEGORY(paramCateg,paramVol) SoundManager::getPtr ()->getRepository ()->setVolumeForCategory (paramCateg,paramVol)
#define GSOUND_START_MUSIC() SoundManager::getPtr ()->getMusicPlayer ()->play ()
#define GSOUND_PAUSE_MUSIC() SoundManager::getPtr ()->getMusicPlayer ()->pause ()
#define GSOUND_SWITCH_TO_PLAYLIST(param) SoundManager::getPtr ()->getMusicPlayer ()->switchToPlaylist (param)



#endif // GUS_SOUND_MAIN_HEADER_H
