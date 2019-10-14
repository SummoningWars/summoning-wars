#pragma once

//   This file is part of the gussound library, licensed under the terms of the MIT License.
//
//   The MIT License
//   Copyright (C) 2010-2013  Augustin Preda (thegusty999@gmail.com)
//
//    Gusty's Serpents is free software: you can redistribute it and/or modify
//    it under the terms of the GNU General Public License as published by
//    the Free Software Foundation, either version 3 of the License, or
//    (at your option) any later version.
//
//    This program is distributed in the hope that it will be useful,
//    but WITHOUT ANY WARRANTY; without even the implied warranty of
//    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
//    GNU General Public License for more details.
//
//    You should have received a copy of the GNU General Public License
//    along with this program.  If not, see <http://www.gnu.org/licenses/>.
//
//    Audio utility class.

// The API abstraction layer.
#include <gussound.h>

// The implementation is based on OpenAL.
#include <al.h>
#include <alc.h>

// Add support for ogg files.
#include <vorbis/vorbisfile.h>

// Add support for vectors
#include <vector>


// definition for the float type expected to be used by the underlying sound lib.

using namespace gussound;
namespace GOpenAl
{
	// forward class defs
	class OpenAlSoundRepository;

	///
	/// A helper class to encapsulate Ogg streaming functionality.
	/// Note: using some of the guidelines and techniques mentioned here:
	/// http://devmaster.net/posts/2895/openal-lesson-8-oggvorbis-streaming-using-the-source-queue
	///
	class GOggStreamedSound
		: public EffectableSound
	{
	public:
		/// Constructor
		GOggStreamedSound (OpenAlSoundRepository* repoPtr, const std::string & id = "");
		
		/// Destructor
		~GOggStreamedSound ();
    
		/// Open a file.
		void open (const std::string& fileName);

		void release();
		void display ();

		/// Commence the low-level sound play. Note that certain actions may be performed if the sound is already
		/// playing, but not all possible actions are supported at this low level.
		virtual void play2D (ActionIfPlayingEnum actionIfPlaying = ActionRestart, bool repeatContinuously = false);

		bool playing ();
		bool update ();
		
		void setVolume (float volume)
		{
			if (playing ())
			{
				alSourcef (source_, AL_GAIN,volume);
			}
			volume_ = volume;
		}


		const SoundDuration getEstimatedDurationInSeconds () const
		{
			return estimatedDurationInSeconds_;
		}

		virtual SoundVolume getOutputVolume(){ return volume_; }

		virtual SoundDuration getSeekDuration();

		/// Tell whether the sound received the play command with the repeat flag, meaning it will
		/// play continuously until a stop is issued.
		virtual bool isRepeating () { return false; }

		/// Pause the sound.
		virtual void pause ();

		virtual SoundDuration getDuration();

		virtual SampleRate getSampleRate(){ return sampleRate_; }

		virtual void seekToTime (const SoundDuration & duration);

		virtual void setOutputVolume (const SoundVolume & volume);

		virtual void stop();

		virtual void rewind ();

	protected:

		/// A pointer to the sound repository is stored here. 
		/// Do not change this to a smart pointer, as the repository is likely to store the sounds as smart pointers and that would produce issues.
		OpenAlSoundRepository* repositoryPtr_;

		std::string sourceFile_;

		bool stream (ALuint buffer);

		/// Empty the currently loaded buffers.
		void emptyBuffers ();

		/// Perform some check-ups for ogg stream commands.
		void oggcheck (const std::string& comment);

		virtual bool loadFrom (const gussound::AudioDeviceSmartPtr & devicePtr
					, const std::string &fileName, bool loadInMemory, bool onlyOneInstance = true);
		/// Utility function to retrieve the time that is currently elapsed from the stream.
		virtual SoundDuration getStreamElapsedTime ();

		virtual void elapseTime (const guslib::TimeUnits & time);

		virtual void doPlay (bool repeatContinuously = false);
		virtual void doResume (bool repeatContinuously = false);

		std::string makeAlErrorString (ALuint code);
		std::string makeOggErrorString (int code);

		SoundDuration estimatedDurationInSeconds_;

		/// The sample rate for the loaded sound.
		SampleRate sampleRate_;

		/// The duration in milliseconds that passed since this sound began playing.
		SoundDuration elapsedFromStart_;

	private:

		FILE*           oggFilePtr_;
		OggVorbis_File  oggStream_;
		vorbis_info*    vorbisInfo_;
		vorbis_comment* vorbisComment_;

		// For ogg, we have 2 buffers dedicated to the stream
		// There is a buffer being played and one waiting to be played. When the buffer being played has finished the next one starts. 
		// While the next buffer is being played, the first one is refilled with a new chunk of data from the stream and is set to play 
		// once the one playing is finished.
		ALuint buffers_[2];

		ALuint source_;
		ALenum format_;
		float volume_;
		bool m_initialized;
	};


}

