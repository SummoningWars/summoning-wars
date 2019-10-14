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
//    Audio utility class.

// The abstract sound interface
#include <gussound.h>

//
// Add guslib inclusions... unless we're dealing with the standalone version
//

#ifdef GUSSOUND_STANDALONE

// Tracing utility
#include <gussoundutil/trace.h>

// String parsing utility.
#include <gussoundutil/stringutil.h>

#else

// Tracing utility
#include <guslib/trace/trace.h>

// String parsing utility.
#include <guslib/util/stringutil.h>

#endif // GUSSOUND_STANDALONE

#include <AL/alut.h>

// The OGG bound implementation.
#include <goggstreamedsound.h>

// Some of the definitions are generic to GOpenAl
#include <gopenal.h>


//
// Default trace level. TODO: remove?
//
#define ATLVL 4

namespace GOpenAl
{

	
	// ------------------------------------------ GOggStreamedSound -----------------------------------------

	/// Constructor
	GOggStreamedSound::GOggStreamedSound (OpenAlSoundRepository* repoPtr, const std::string & id)
		: EffectableSound (id)
		, sampleRate_ (0)
		, oggFilePtr_ (0)
		, vorbisInfo_ (0)
		, vorbisComment_ (0)
		, source_ (0)
		, estimatedDurationInSeconds_ (0)
		, volume_ (1.0f)
		, m_initialized (false)
		, elapsedFromStart_ (0)
		, repositoryPtr_ (repoPtr)
	{
		GTRACE (4, "GOggStreamedSound CTor");
		buffers_ [0] = 0;
		buffers_ [1] = 0;
	}
#if 0
		, buffer_ (0)
		, cachedBufferLength_ (0)
#endif

	/// Destructor
	GOggStreamedSound::~GOggStreamedSound ()
	{
		if (m_initialized)
		{
			oggcheck ("dtor");
		}
		GTRACE (4, "GOggStreamedSound DTor");
	}


	void GOggStreamedSound::open (const std::string& fileName)
	{
		GTRACE (4, "GOggStreamedSound opening [" << fileName << "]");
		sourceFile_ = fileName;

		int error = alGetError ();

		if (error != AL_NO_ERROR)
		{
			std::string errMessage ("Error on opening Ogg stream: ");
			errMessage.append (makeAlErrorString (error));
			throw SoundException (errMessage.c_str ());
		}

		int result;
		oggFilePtr_ = fopen (fileName.c_str (), "rb");
		if (! oggFilePtr_)
		{
			std::string errMessage ("Failed to load Ogg file: ");
			errMessage.append (fileName);
			throw SoundException (errMessage.c_str ());
		}

		if((result = ov_open (oggFilePtr_, &oggStream_, NULL, 0)) < 0)
		{
			fclose (oggFilePtr_);
        
			std::string errText ("Could not open Ogg stream. ");
			errText.append (makeOggErrorString (result));
			throw SoundException (errText.c_str ());
		}
	
		vorbisInfo_ = ov_info (&oggStream_, -1);
		vorbisComment_ = ov_comment (&oggStream_, -1);
		if (vorbisComment_)
		{
			GTRACE (6, "Displaying vorbis comment info");
			GTRACE (7, "Vendor: [" << vorbisComment_->vendor << "]");
			for (int i = 0; i < vorbisComment_->comments; i ++)
			{
				GTRACE (7, "Comment [" << i << "] is : [" << (vorbisComment_->user_comments[i]) << "]");
			}
		}

		GTRACE (4, "Loaded vorbis file information for file [" << fileName << "]");

		long myBitRate = ov_bitrate (&oggStream_, -1);
		GTRACE(5, "Bitrate = " << myBitRate);

		// get the file size.
		std::ifstream inFile (fileName.c_str (), std::ifstream::in | std::ifstream::binary);
	    inFile.seekg (0, std::ifstream::end);
		GTRACE (5, "File size = " << inFile.tellg ());

		// The bit rate is in bits, the file size is in bytes, so adjust by a factor of 8.
		if (myBitRate)
		{
			estimatedDurationInSeconds_ = 8 * inFile.tellg () / myBitRate;
			GTRACE (5, "Ogg content duration: " << estimatedDurationInSeconds_ << " (sec.)");
		}

		if (vorbisInfo_->channels == 1)
		{
			format_ = AL_FORMAT_MONO16;
		}
		else
		{
			format_ = AL_FORMAT_STEREO16;
		}
        
		GTRACE (5, "GOggStreamedSound creating 2 buffers for [" << getId () << "]");
		alGenBuffers (2, buffers_);

		oggcheck ("open");
	}


	void GOggStreamedSound::release ()
	{
		alSourceStop (source_);
		oggcheck ("release");
		
		emptyBuffers ();
		oggcheck ("release(1)");
		
		alDeleteBuffers (2, buffers_);
		oggcheck ("release(2)");

		ov_clear (&oggStream_);
		oggcheck ("release(3)");
	}


	void GOggStreamedSound::play2D (ActionIfPlayingEnum actionIfPlaying, bool repeatContinuously)
	{
		GTRACE (7, "GOggStreamedSound::playback for [" << getId () << "]. Buffers are [" << buffers_[0] << "] and [" << buffers_[1] << "]");
		if (0 == repositoryPtr_)
		{
			GTRACE (2, "ERROR: NULL repository ptr");
			return;
		}
		if (repeatContinuously)
		{
			GTRACE (2, "WARNING: GOggStreamedSound::play received parameter [repeatContinuously] as true, but function will ignore it and treat is as hardcoded FALSE");
		}

		try
		{
			// Get a free source (an exception is thrown if none is available).
			source_ = repositoryPtr_->getAvailableSourceForPlay (GSC_Music);

			alSource3f (source_, AL_POSITION,        0.0, 0.0, 0.0);
			alSource3f (source_, AL_VELOCITY,        0.0, 0.0, 0.0);
			alSource3f (source_, AL_DIRECTION,       0.0, 0.0, 0.0);
			alSourcef (source_, AL_ROLLOFF_FACTOR,  0.0          );
			alSourcei (source_, AL_SOURCE_RELATIVE, AL_TRUE      );
			SoundVolume myVol = getCombinedVolumeModifiers ();
			alSourcef (source_, AL_GAIN, myVol);
		}
		catch (std::exception &e)
		{
			GTRACE (2, "OggStream failed to play. " << e.what ());
		}

		switch (getState ())
		{
		case GPLS_Playing:
			//if (playing ())
			//{
				switch( actionIfPlaying )
				{
				case ActionRestart:
					GTRACE (6, "Sound [" << getId () << "] already playing. Rewiding (close & reopen).");
					rewind ();
					doPlay (repeatContinuously);
					state_ = GPLS_Playing;
					break;
				case ActionParallelPlay:
					//not supported at this level
					break;
				case ActionEnqueue:
					//not supported at this level
					break;
				case ActionDoNothing:
				default:
					break;
				}
			//}
			//else
			//{
			//	GTRACE(2, "WARNING: Sound internal state corruption (obj state=playing; stream not playing)!");
			//	// normally this shouldn't happen, we have some internal state problems.
			//	doPlay( repeatContinuously );
			//	state_ = GPLS_Playing;
			//}
			return; // already playing.
		case GPLS_Stopped:
			{
				GTRACE (6, "Sound [" << getId () << "] was stopped. Rewiding (close & reopen).");
				rewind ();
				doPlay( repeatContinuously );
				state_ = GPLS_Playing;
				return;
			}
		case GPLS_Paused:
			{
				doResume ();
				state_ = GPLS_Playing;
				return;
			}
		default:
			return;
		}

		return;// true;
	}


	void GOggStreamedSound::doPlay (bool repeatContinuously)
	{
		GTRACE (5, "GOggStreamedSound::doPlay triggered. Buffers are [" << buffers_[0] << "] and [" << buffers_[1] << "]");

		if (!stream (buffers_[0]))
		{
			return;// false;
		}
        
		if (!stream (buffers_[1]))
		{
			return;// false;
		}
    
		alSourceQueueBuffers (source_, 2, buffers_);
		alSourcePlay (source_);
		oggcheck ("doPlay");
	}

	void GOggStreamedSound::doResume (bool repeatContinuously)
	{
		alSourcePlay (source_);
		oggcheck ("doResume");
		/*
		alSourcei (source_, AL_BUFFERS_PROCESSED, &processed);
		*/
		//doPlay (repeatContinuously);
	}


	// TODO: is this function obsolete?
	bool GOggStreamedSound::playing ()
	{
		ALenum state;
    
		alGetSourcei (source_, AL_SOURCE_STATE, &state);
    
		return (state == AL_PLAYING);
	}


	bool GOggStreamedSound::update ()
	{
		//GTRACE (8, "GOggStreamedSound::update [" << getId () << "]");
		int processed = 0;
		bool active = true;

		alGetSourcei (source_, AL_BUFFERS_PROCESSED, &processed);
		GTRACE (8, "GOggStreamedSound::update [" << getId () << "], processed so far from source: "<< processed);
		oggcheck ("update");
		while (processed>0)
		{
			processed--;
			ALuint buffer;
        
			alSourceUnqueueBuffers (source_, 1, &buffer);
			oggcheck ("update (2)");

			active = stream (buffer);

			alSourceQueueBuffers (source_, 1, &buffer);
			oggcheck ("update (3)");
		}

		if (state_ == GPLS_Playing)
		{
			if (! playing ())
			{
				GTRACE (8, "[" << getId () << "] active and not playing... triggering play");
				alSourcePlay (source_);
				ALenum myErr = alGetError ();
				if (myErr)
				{
					GTRACE (4, "ERROR while playing: " << makeAlErrorString (myErr));
				}
			}
		}
#if 0
		if (active && !playing ())
		{
			GTRACE (8, "[" << getId () << "] active and not playing... triggering play");
			//XXX, TODO, play
			alSourcePlay (source_);
			ALenum myErr = alGetError ();
			if (myErr)
			{
				GTRACE (4, "ERROR while playing: " << makeAlErrorString (myErr));
			}
		}
#endif

		return active;
	}


	bool GOggStreamedSound::stream (ALuint buffer)
	{
		static const int BUFFER_SIZE = 4096 * 16;
		//static const int BUFFER_SIZE = 4096;
		//TODO:XXX:investigate buffer sizes.

		char pcm [BUFFER_SIZE];
		int  size = 0;
		int  section;

		while(size < BUFFER_SIZE)
		{
			int  result;
			result = ov_read (&oggStream_, pcm + size, BUFFER_SIZE - size, 0, 2, 1, &section);
    
			if(result > 0)
				size += result;
			else
				if(result < 0)
					throw makeOggErrorString (result);
				else
					break;
		}
    
		if (size == 0)
		{
			return false;
		}
    
		if (! alIsBuffer (buffer))
		{
			GTRACE (3, "Ogg Stream received non-buffer data to stream from!");
			return false;
		}
		alBufferData (buffer, format_, pcm, size, vorbisInfo_->rate);
		oggcheck ("stream");
    
		return true;
	}



	void GOggStreamedSound::rewind ()
	{
		GTRACE (8, "GOggStreamedSound::rewind ()");
#if 0
		alSourceRewind (source_);
#else
		release ();

		open (sourceFile_);
#endif
	}



	void GOggStreamedSound::stop()
	{
		GTRACE (6, "GOggStreamedSound::stop()");

		alSourceStop (source_);
		state_ = GPLS_Stopped;
		elapsedFromStart_ = 0;
		oggcheck ("stop");
	}


	void GOggStreamedSound::setOutputVolume (const SoundVolume & volume)
	{
		SoundVolume sentVolume (volume); // the volume to send to the device.
		if (sentVolume < gussound::min_volume) sentVolume = gussound::min_volume;
		if (sentVolume > gussound::max_volume) sentVolume = gussound::max_volume;

		if (source_)
		{
			GTRACE (8, "Setting low level output volume for [" << getId () << "] to: " << sentVolume);
			alSourcef (source_, AL_GAIN, sentVolume);
		}
	}


	void GOggStreamedSound::seekToTime (const SoundDuration & duration)
	{
		GTRACE(6, "GOggStreamedSound::seekToTime ()");
		//TODO: implement

		//if (outstreamptr_)
		//{
		//	if (outstreamptr_->isSeekable() && sampleRate_)
		//	{
		//		if (getDuration() == 0)
		//		{
		//			return;
		//		}
		//		SoundDuration ftemp = outstreamptr_->getLength() * duration / getDuration();
		//		int nPosition = (int)ftemp;
		//		GTRACE (6, "inner calc: seek to pos=" << nPosition << ", from len=" << outstreamptr_->getLength ());
		//		outstreamptr_->setPosition (nPosition);
		//	}
		//}
	}

	SoundDuration GOggStreamedSound::getDuration ()
	{
		// TODO: simplify
		return 1000 * getEstimatedDurationInSeconds ();
	}


	SoundDuration GOggStreamedSound::getSeekDuration ()
	{
		if (getDuration () == 0)
		{
			return 0;
		}

		return elapsedFromStart_;

		//SoundDuration rValue = getStreamElapsedTime (); // OR: elapsedtime * length / length ?
		//return rValue;
	}



	void GOggStreamedSound::emptyBuffers ()
	{
		int queued;
    
		alGetSourcei (source_, AL_BUFFERS_QUEUED, &queued);
    
		while (queued --)
		{
			ALuint buffer;
    
			alSourceUnqueueBuffers (source_, 1, &buffer);
			oggcheck ("emptybuf");
		}
	}



	void GOggStreamedSound::pause ()
	{
		if (getState () == GPLS_Playing)
		{
			alSourcePause (source_);
			state_ = GPLS_Paused;
			GTRACE (ATLVL, "Paused low level sound." << id_);
		}
		else
		{
			GTRACE (ATLVL, id_ << "; Warning: received command to pause while state is: " << getState ());
		}
	}



	/// Utility function to retrieve the time that is currently elapsed from the stream.
	SoundDuration GOggStreamedSound::getStreamElapsedTime ()
	{
		float elapsed;
		alGetSourcef (source_, AL_SEC_OFFSET, &elapsed);
		return elapsed;
	}


	bool GOggStreamedSound::loadFrom (const gussound::AudioDeviceSmartPtr & devicePtr, const std::string &fileName, bool loadInMem, bool onlyOneInstance )
	{
		GTRACE (2, "GOggStreamedSound. Loading from file [" << fileName << "]");
		if (loadInMem)
		{
			GTRACE (2, "WARNING: GOggStreamedSound::loadFrom received param [loadInMem] as TRUE, but param is hard-coded by design to be FALSE in this class");
			return false;
		}

		if (!onlyOneInstance)
		{
			GTRACE (2, "WARNING: GOggStreamedSound::loadFrom received param [onlyOneInstance] as FALSE, but param is hard-coded by design to be TRUE in this class");
			return false;
		}

		onlyOneInstance_ = onlyOneInstance;
		streamed_ = true;
		
		this->open (fileName.c_str ());
		return true; // TODO: make checks
	}

	void GOggStreamedSound::elapseTime (const guslib::TimeUnits & time)
	{
		GTRACE (9, "GOggStreamedSound::elapseTime, " << getId () << ", units: " << time);
		// Call parent implementation.
		EffectableSound::elapseTime (time);
		
		if (GPLS_Playing == state_)
		{
			GTRACE (9, "Elapsed from start for ["<< getId () << "] is: " << elapsedFromStart_);
			elapsedFromStart_ += time;// = getStreamElapsedTime ();
			
			update ();
		}
	}



	void GOggStreamedSound::oggcheck (const std::string& comment)
	{
		int error = alGetError ();

		if (error != AL_NO_ERROR)
		{
			// TODO: add error msg.
			GTRACE (2, "oggcheck failed (" << makeAlErrorString (error) << "); " << this->getId () << "; " << comment);
		}
	}


	std::string GOggStreamedSound::makeOggErrorString (int code)
	{
		// Show a bit more than alGetString.
		switch(code)
		{
			case OV_EREAD:
				return std::string ("Read from media.");
			case OV_ENOTVORBIS:
				return std::string ("Not Vorbis data.");
			case OV_EVERSION:
				return std::string ("Vorbis version mismatch.");
			case OV_EBADHEADER:
				return std::string ("Invalid Vorbis header.");
			case OV_EFAULT:
				return std::string ("Internal logic fault (bug or heap/stack corruption.");
			default:
				{
					std::stringstream ss;
					ss << "Unknown Ogg error (" << code << ")";
					return ss.str ();
				}
		}
	}



	std::string GOggStreamedSound::makeAlErrorString (ALuint code)
	{
		// Show a bit more than alGetString.
		switch(code)
		{
			case AL_INVALID_NAME:
				return std::string ("Invalid name parameter.");
			case AL_INVALID_ENUM:
				return std::string ("Invalid enum parameter value.");
			case AL_INVALID_VALUE:
				return std::string ("Invalid parameter.");
			case AL_INVALID_OPERATION:
				return std::string ("Illegal call.");
			case AL_OUT_OF_MEMORY:
				return std::string ("Unable to allocate memory.");
			default:
				{
					std::stringstream ss;
					ss << "Unknown OpenAL error (" << code << ")";
					return ss.str ();
				}
		}
	}

}
