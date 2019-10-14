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

// The abstract sound interface
#include <gussound.h>

//
// Add guslib inclusions... unless we're dealing with the standalone version
//

#ifdef GUSSOUND_STANDALONE

// Tracing util
#include <gussoundutil/trace.h>

// Random numbers.
#include <gussoundutil/random.h>

// String parsing utility
#include <gussoundutil/stringutil.h>

#else

// Tracing util
#include <guslib/trace/trace.h>

// Random numbers.
#include <guslib/system/random.h>

// String parsing utility
#include <guslib/util/stringutil.h>

#endif // GUSSOUND_STANDALONE

#define GUSSOUND_MAJOR_VERSION 1
#define GUSSOUND_MINOR_VERSION 2
#define GUSSOUND_PATCH_VERSION 0

namespace gussound
{
	SoundException::SoundException (const char* text)
	{
        this->exceptionMessage_ = text;
    }

    SoundException::~SoundException() throw()
    {
    }

    const char* SoundException::what () const throw()
	{
		return this->exceptionMessage_.c_str ();
	}

	// ----------------------------------------------- AudioDevice -------------------------------------------------

	AudioDevice::AudioDevice ()
	{
	}

	AudioDevice::~AudioDevice ()
	{
		GTRACE (3, "Audio device dtor");
	}



	// --------------------------------------------- AbstractSound -------------------------------------------------



	///
	/// Commence the low-level sound play with 3D positioning. Will default to 2D play unless overridden.
	/// This is a fire & forget function. You cannot update the source of the sound after it is launched.
	///
	void AbstractSound::play3D (double , double , double , ActionIfPlayingEnum actionIfPlaying, bool repeatContinuously)
	{
		GTRACE (7, "Abstract sound play3D entered. Not supported. Forwarding to 2D implementation");
		play2D (actionIfPlaying, repeatContinuously);
	}


	///
	/// Commence the low-level sound play with 3D positioning. Will default to 2D play unless overridden.
	/// This allows you to update the source of the sound after the play command is launched.
	///
	int AbstractSound::play3Dnamed (double , double , double , ActionIfPlayingEnum actionIfPlaying, bool repeatContinuously)
	{
		GTRACE (7, "Abstract sound play3D entered. Not supported. Forwarding to 2D implementation");
		play2D (actionIfPlaying, repeatContinuously);
		return 0;
	}



	// ----------------------------------------------- SoundEffect -------------------------------------------------


	SoundEffect::SoundEffect (EffectableSoundSmartPtr & target, guslib::TimeUnits startFrom, guslib::TimeUnits stopAt)
		: ptrToSound_ (target)
		, startFrom_ (startFrom)
		, stopAt_ (stopAt)
		, internalTimer_ (startFrom)
		, ended_ (false)
		, targetLength_ (0)
	{
		GTRACE (6, "SoundEffect ctor from " << startFrom << " to " << stopAt << " for " << target->getId ());
	}

	SoundEffect::~SoundEffect ()
	{
		GTRACE(6, "SoundEffect dtor");
	}

	void SoundEffect::applyEffect ()
	{
		//dummy
	}

	void SoundEffect::elapseTime (const guslib::TimeUnits & time)
	{
		if (ended_)
		{
			return;
		}
		// see how much time has passed.
		internalTimer_ = (guslib::TimeUnits) ptrToSound_->getSeekDuration();

		// if the allowed time for the effect has ended, kill it.
		if (internalTimer_ >= stopAt_)
		{
			endEffect ();
			ended_ = true;
			ptrToSound_->onEffectEnded (*this);
			return;
		}

		// if the effect is not dead yet, check to see if it can be applied.
		if (internalTimer_ >= startFrom_ && internalTimer_ <= stopAt_)
		{
			applyEffect();
		}
	}


	void SoundEffect::endEffect(bool)
	{
		//dummy
	}


	bool SoundEffect::equals (const SoundEffect & rhs)
	{
		if (rhs.ended_ != ended_
			|| rhs.internalTimer_ != internalTimer_
			|| rhs.ptrToSound_ != ptrToSound_
			|| rhs.startFrom_ != startFrom_
			|| rhs.stopAt_ != stopAt_
			|| rhs.targetLength_ != targetLength_)
		{
			return false;
		}
		return true;
	}


	void SoundEffect::reset()
	{
		internalTimer_ = 0; 
		ended_ = false;
	}


	void SoundEffect::stop ()
	{
		internalTimer_ = 0;
		ended_ = true;
		ptrToSound_->onEffectEnded(*this);
	}


	//-------------------------------------- FadeInEffect --------------------------------

	FadeInEffect::FadeInEffect (EffectableSoundSmartPtr & target
		, const guslib::TimeUnits & duration
		, const SoundVolume & minVolume)

		: SoundEffect (target, 0, duration)
		, minVolume_ (minVolume)
	{
		GTRACE (6, "FadeInEffect ctor1: from " << 0 << ", to " << duration);
		GTRACE (6, "FadeInEffect ctor1: target (" << target->getId () << ") at " << target->getSeekDuration ());
	}

	FadeInEffect::FadeInEffect (EffectableSoundSmartPtr & target
		, const guslib::TimeUnits & fromTime
		, const guslib::TimeUnits & duration
		, const SoundVolume & minVolume)

		: SoundEffect (target, fromTime, fromTime + duration)
		, minVolume_ (minVolume)
	{
		GTRACE (6, "FadeInEffect ctor2: from " << fromTime << ", to " << fromTime + duration);
		GTRACE (6, "FadeInEffect ctor2: target (" << target->getId () << ") at " << target->getSeekDuration ());
	}


	FadeInEffect::~FadeInEffect ()
	{
		GTRACE (6, "FadeInEffect dtor.");
	}

	void FadeInEffect::applyEffect ()
	{
		GTRACE (7, "FadeInEffect applyEffect.");
		if (stopAt_ == startFrom_)
		{
			// nothing to apply.
			stop ();
			return;
		}

		// see how much time has passed and apply a liniar value based on it.
		SoundDuration fractionPassed;
		fractionPassed = (SoundDuration)(internalTimer_-startFrom_) / (SoundDuration)(stopAt_ - startFrom_);
		GTRACE (7, "FadeInEffect applyEffect, fraction passed=" << fractionPassed 
			<< "; itimer=" <<internalTimer_ << "; start:" <<startFrom_ <<"; stop:" << stopAt_);
		SoundVolume newVol = (SoundVolume) (fractionPassed * (max_volume - minVolume_));
		ptrToSound_->setVolumeExtraModifier (newVol);
	}


	/// Define action to be taken when the effect ends (for instance, a fade-in would make sure that the sound level is appropriate at the end.
	/// @param force Ignored
	void FadeInEffect::endEffect (bool)
	{
		GTRACE (7, "FadeInEffect endEffect; applying final volume")
		SoundVolume newVol = (SoundVolume) (max_volume - minVolume_);
		ptrToSound_->setVolumeExtraModifier (newVol);
	}


	//----------------
	FadeOutEffect::FadeOutEffect (EffectableSoundSmartPtr & target
		, const guslib::TimeUnits & duration
		, const SoundVolume & minVolume
		, bool stopAtEnd)

		: SoundEffect (target, (guslib::TimeUnits) (target->getDuration () - duration)
					, (guslib::TimeUnits)target->getDuration ())
		, stopOnEnd_ (stopAtEnd)
		, minVolume_ (minVolume)
	{
		GTRACE(6, "FadeOutEffect ctor1");
	}

	FadeOutEffect::FadeOutEffect (EffectableSoundSmartPtr & target
		, const guslib::TimeUnits & startFrom
		, const guslib::TimeUnits & stopAt
		, const SoundVolume & minVolume
		, bool stopAtEnd)

		: SoundEffect (target, startFrom, stopAt)
		, stopOnEnd_ (stopAtEnd)
		, minVolume_ (minVolume)
	{
		GTRACE (6, "FadeOutEffect ctor2");
	}

	FadeOutEffect::~FadeOutEffect ()
	{
		GTRACE (5, "FadeOutEffect Destr.");
	}

	void FadeOutEffect::applyEffect ()
	{
		GTRACE (7, "FadeOutEffect applyEffect.");
		if (stopAt_ <= startFrom_)
		{
			GTRACE (7, "fade-out: start and stop are the same or reverted; FadeOutEffect applyEffect.");
			stop ();
			return;
		}
		SoundDuration fractionPassed;
		fractionPassed = (SoundDuration)(internalTimer_ - startFrom_) / (SoundDuration)(stopAt_ - startFrom_);
		GTRACE (7, "FadeOutEffect applyEffect, fraction passed=" << fractionPassed 
			<< "; itimer=" <<internalTimer_ << "; start:" <<startFrom_ <<"; stop:" << stopAt_);
		SoundVolume newVol = (SoundVolume)((1-fractionPassed) * (max_volume - minVolume_));
		ptrToSound_->setVolumeExtraModifier( newVol );
	}


	///
	/// Define action to be taken when the effect ends (for instance, a fade-out would stop the sound at the end.
	/// @param force If the force flag is set to true, the stop commands for the sound are suppressed.
	/// This is performed in order to prevent fade-in and fade-out effects running for the same sound and performing
	/// an unplanned stop operation when the fade-out is finished.
	///
	void FadeOutEffect::endEffect (bool force)
	{
		GTRACE (GUSSNDTLVL, "fadeout effect ending...");
		ptrToSound_->setVolumeExtraModifier (1.0f);
		if (stopOnEnd_)
		{
			if (! force)
			{
				GTRACE (GUSSNDTLVL, "fadeout effect stopping on end.");
				ptrToSound_->stop ();
			}
			else
			{
				GTRACE (GUSSNDTLVL, "Force flag suppresses stop operation of effect.");
			}
		}
		else
		{
			GTRACE (GUSSNDTLVL, "fadeout effect pausing on end.");
			ptrToSound_->pause ();
		}
	}



	// ----------------------------------------------- Sound -------------------------------------------------


	EffectableSound::EffectableSound(const std::string & id)
		: volume_ (max_volume)
		, effectVolume_ (max_volume)
		, categoryVolume_ (max_volume)
		, tracker_ (NULL)
		, trackerTime_ (0)
		, category_ (GSC_None)
		, state_ (GPLS_Stopped)
		, id_ (id)
		, onlyOneInstance_ (true)
		, streamed_ (false)

	{
		GTRACE(7, "EffectableSound ctor " << id_);
	}


	EffectableSound::~EffectableSound()
	{
		GTRACE(7, "EffectableSound dtor " << id_);
	}


    void EffectableSound::addEffect( SoundEffectPtr effect )
	{
		GTRACE(6, " EffectableSound::addEffect... to " << id_);
		tempEffectList_.push_back( effect );
		effect->applyEffect();
	}


	void EffectableSound::clearAllEffects ()
	{
		GTRACE (6, "Clearing all effects... from " << id_);
		for (SoundEffectList::iterator it = tempEffectList_.begin ();
			it != tempEffectList_.end (); ++it)
		{
			(*it)->endEffect (true);
		}
		tempEffectList_.clear ();
		effectVolume_ = max_volume;
	}


	void EffectableSound::elapseTime (const guslib::TimeUnits & time)
	{
		if (state_ != GPLS_Playing)
			return;

		for (SoundEffectList::iterator iter = deletionList_.begin ();
			iter != deletionList_.end (); ++iter)
		{
			for( SoundEffectList::iterator iter2 = tempEffectList_.begin ();
				iter2 != tempEffectList_.end (); ++iter2 )
			{
				if(* iter == * iter2)
				{
					GTRACE(7, "EffectableSound::elapseTime removing effect");
					tempEffectList_.erase( iter2 );
					break;
				}
			}
			deletionList_.erase(iter);
			break;
		}

		for (SoundEffectList::iterator iter = tempEffectList_.begin ();
			iter != tempEffectList_.end (); ++iter)
		{
			(*iter)->elapseTime (time);
		}

		if (tracker_)
		{
			if (getSeekDuration () >= trackerTime_)
			{
				GTRACE (5, "seek dur=" <<getSeekDuration () << "; tracker time: " <<trackerTime_ << "; making sound event");

				SoundFinishingTrackerInterface * localTracker = tracker_;
				clearTracker ();
				if (0 != localTracker)
				{
					localTracker->onSoundEvent ();
				}
			}
		}
	}


	SoundVolume EffectableSound::getCombinedVolumeModifiers ()
	{
		SoundVolume temp = effectVolume_ * volume_ * categoryVolume_;
		return temp;
	}


	void EffectableSound::onEffectEnded (const SoundEffect & effect)
	{
		for( SoundEffectList::const_iterator iter = tempEffectList_.begin();
			iter != tempEffectList_.end(); ++iter )
		{
			if (*iter && (*iter)->equals (effect))
			{
				deletionList_.push_back (*iter);
				GTRACE(6, "EffectableSound::onEffectEnded (exit, effect added to deletion list)");
				return;
			}
		}
	}

	void EffectableSound::setTracker (SoundFinishingTrackerInterface * ptr, SoundDuration dur)
	{
		GTRACE(7, "EffectableSound tracker set for time =" << dur);
		tracker_ = ptr;
		trackerTime_ = dur;
	}


	void EffectableSound::clearTracker ()
	{
		 tracker_ = NULL;
	}


	void EffectableSound::setVolume (const SoundVolume & volume)
	{
		volume_ = volume;
		setOutputVolume (getCombinedVolumeModifiers ());
	}

	void EffectableSound::setVolumeCategoryModifier (const SoundVolume & volume)
	{
		GTRACE (6, id_ << " got volume for category " << getCategory () << " as " << volume);
		categoryVolume_ = volume;
		setOutputVolume (getCombinedVolumeModifiers ());
	}

	void EffectableSound::setVolumeExtraModifier (const SoundVolume & volume)
	{
		effectVolume_ = volume;
		GTRACE (7, id_ << " setting sound volume:" << getCombinedVolumeModifiers ());
		GTRACE (7, id_ << " effectVolume_:" << effectVolume_);
		GTRACE (7, id_ << " categoryVolume_:" << categoryVolume_);
		GTRACE (7, id_ << " volume_:" << volume_);
		setOutputVolume (getCombinedVolumeModifiers ());
	}

	// -------------------------------------- SoundGroup ----------------------------------------------
	
	SoundGroup::SoundGroup()
		: currentWeightSum_(0)
	{
	}

	SoundGroup::~SoundGroup()
	{
		GTRACE (5, "Sound group dtor");
		// release all map entries.
		sounds_.clear();
	}


	void SoundGroup::addEmptySlot (int weight)
	{
		GTRACE (4, "Adding an empty slot for sound " << " with weight " << weight);
		addSound (EMPTY_SLOT_NAME, weight);
	}


	void SoundGroup::addSound (const std::string & soundName, int weight)
	{
		std::map<std::string, int>::iterator iter = sounds_.find (soundName);
		if (iter != sounds_.end ())
		{
			throw SoundException ("SoundGroup: attempting to add an already existing sound!");
		}

		sounds_.insert (std::make_pair (soundName, weight));
		currentWeightSum_ += weight;
	}


	void SoundGroup::removeEmptySlot ()
	{
		removeSound ("_");
	}



	void SoundGroup::removeSound (const std::string & soundName)
	{
		std::map<std::string, int>::iterator iter = sounds_.find( soundName );
		if( iter == sounds_.end() )
		{
			throw SoundException ("SoundGroup: attempting to remove a non-existent sound!");
		}
		int weight = iter->second;
		sounds_.erase( iter );

		currentWeightSum_ -= weight;
	}


	std::string SoundGroup::getRandomSound() const
	{
		// get a random number between 0 and currentWeightSum_-1;

		long randomValue = 0;
		bool found = false;
		randomValue = guslib::Random::getPtr ()->getValueUpTo( currentWeightSum_ );

		// now simply decrease the weight for each item until the negative value is passed, or the first item is encountered.
		std::map<std::string, int>::const_iterator iter = sounds_.begin();
		//randomValue -= iter->second;
		//found = (randomValue < 0);
		while( iter != sounds_.end() && !found )
		{
			randomValue -= iter->second;
			found = (randomValue < 0);
			if( ! found )
				++iter;
		}

		if (! found)
		{
			return std::string (EMPTY_SOUND_FILE);
		}

		return iter->first;
	}

	// ----------------------------------------------- Playlist -------------------------------------------------

	Playlist::Playlist()
		: fadeInAtPlay_(true)
		, fadeOutAtPause_(true)
		, fadeOutAtStop_(true)
		, currentTrack_("")
		, currentTrackIdx_(-1)
		, repeat_(false)
		, shuffle_(false)
		, ptrToRepository_(NULL)
		, autoSkipToNextTrack_(true)
		, fadeDuration_(standard_fade_duration)
	{
		GTRACE(6, "Playlist ctor1");
		playCount_.clear();
		playOrder_.clear();
		tracks_.clear();
	}

	Playlist::Playlist (const SoundRepositorySmartPtr & repository_
		, PlaylistListener * listener
		, const std::string & id)
		: fadeInAtPlay_(true)
		, fadeOutAtPause_(true)
		, fadeOutAtStop_(true)
		, currentTrack_("")
		, currentTrackIdx_(-1)
		, repeat_(false)
		, shuffle_(false)
		, ptrToRepository_(repository_)
		, autoSkipToNextTrack_(true)
		, ptrToListener_ (listener)
		, id_ (id)
		, fadeDuration_(standard_fade_duration)
	{
		GTRACE(6, "Playlist ctor2");
		playCount_.clear();
		playOrder_.clear();
		tracks_.clear();
	}


	Playlist::Playlist( const Playlist & rhs )
		: fadeInAtPlay_(rhs.fadeInAtPlay_)
		, fadeOutAtPause_(rhs.fadeOutAtPause_)
		, fadeOutAtStop_(rhs.fadeOutAtStop_)
		, currentTrack_( rhs.currentTrack_)
		, currentTrackIdx_( rhs.currentTrackIdx_)
		, repeat_( rhs.repeat_ )
		, shuffle_(rhs.shuffle_ )
		, ptrToRepository_( rhs.ptrToRepository_ )
		, autoSkipToNextTrack_(true)
		, ptrToListener_(rhs.ptrToListener_)
	{
		tracks_ = rhs.tracks_;
		playCount_ = rhs.playCount_;
		GTRACE(6, "Playlist copyctor");
	}

	Playlist::~Playlist()
	{
		GTRACE(6, "Playlist dtor: " << id_);
	}

	void Playlist::addTrack(const std::string &trackName)
	{
		if( !ptrToRepository_ )
		{
			throw SoundException ("Failure by addTrack for playlist (no access to repository)");
		}
		if( ! tracks_.empty() )
		{
			for( std::vector<std::string>::iterator iter = tracks_.begin();
				iter != tracks_.end(); ++iter )
			{
				if( *iter == trackName )
				{
					throw SoundException ("Already using track ");
				}
			}
		}
		tracks_.push_back( trackName );
		playCount_.push_back(0);
		if( currentTrackIdx_ == -1 )
		{
			currentTrackIdx_ = 0;
		}
	}


	void Playlist::pause()
	{
		GTRACE (GUSSNDTLVL, "Playlist pause: " << id_);
		if( tracks_.empty() )
			return;
		if( currentTrackIdx_ < 0 )
			return;
		if( fadeOutAtPause_ )
		{
			GTRACE(GUSSNDTLVL, id_ << ": Pausing playlist with a fade-out effect.");
			SoundEffectPtr tempPtr = new FadeOutEffect
				(ptrToRepository_->getSound (tracks_[currentTrackIdx_])
				, (guslib::TimeUnits) ptrToRepository_->getSound (tracks_[currentTrackIdx_])->getSeekDuration ()
				, (guslib::TimeUnits) (ptrToRepository_->getSound(tracks_[currentTrackIdx_])->getSeekDuration() 
						+ getFadeDuration())
				, 0
				, false); 
			ptrToRepository_->getSound(tracks_[currentTrackIdx_] )->addEffect (tempPtr);
			//TODO: investigate possible conflicts
			state_ = GPLS_Paused;
		}
		else
		{
			GTRACE(GUSSNDTLVL, "Pausing playlist w/o effects.");
			ptrToRepository_->getSound (tracks_[currentTrackIdx_])->pause ();
			state_ = GPLS_Paused;
		}
	}


	void Playlist::play ()
	{
		GTRACE (GUSSNDTLVL, "[" << id_ << "] Playlist received play command");
		if (tracks_.empty ())
		{
			return;
		}

		if (currentTrackIdx_ < 0 || currentTrackIdx_ >= (int)tracks_.size ())
		{
			throw SoundException ("Playlist play with current track idx outside bounds!");
		}

		// clear all the previous effects that may be still active on this sound.

		ptrToRepository_->getSound (tracks_ [currentTrackIdx_])->clearAllEffects ();
		if (fadeInAtPlay_)
		{
			if (state_ == GPLS_Stopped || state_ == GPLS_Playing)
			{
				GTRACE (GUSSNDTLVL, id_ << ": Playlist playing (starting from stop) with fade-in");
				ptrToRepository_->getSound (tracks_ [currentTrackIdx_] )->addEffect (
					GSmartPtr <SoundEffect>(
						new FadeInEffect (ptrToRepository_->getSound (tracks_ [currentTrackIdx_])
						, (guslib::TimeUnits)getFadeDuration ())));
			}
			else // paused
			{
				//ptrToRepository_->getSound (tracks_ [currentTrackIdx_])->clearAllEffects (); // TODO: re-enable if needed.
				GTRACE (GUSSNDTLVL, id_ << " Playlist playing (resuming from pause) with fade-in");
				ptrToRepository_->getSound (tracks_ [currentTrackIdx_])->addEffect (
					GSmartPtr<SoundEffect>(
						new FadeInEffect (ptrToRepository_->getSound (tracks_ [currentTrackIdx_])
										, (guslib::TimeUnits) ptrToRepository_->getSound (tracks_ [currentTrackIdx_])->getSeekDuration ()
										, (guslib::TimeUnits) getFadeDuration ()
										, 0)));
			}
		}

		ptrToRepository_->getSound (tracks_ [currentTrackIdx_])->play2D ();
		if (state_ == GPLS_Stopped)
		{
			ptrToRepository_->getSound (tracks_ [currentTrackIdx_])->setTracker (this
				, ptrToRepository_->getSound (tracks_[currentTrackIdx_])->getDuration () - 
					ptrToRepository_->getSound (tracks_[currentTrackIdx_])->getSeekDuration () -
					getFadeDuration ());
		}
		else
		{
			ptrToRepository_->getSound (tracks_[currentTrackIdx_])->setTracker (this
				, ptrToRepository_->getSound (tracks_[currentTrackIdx_])->getDuration () - getFadeDuration ());
		}
		playCount_ [currentTrackIdx_] ++;

		if (playOrder_.empty ())
		{
			playOrder_.push_back (currentTrackIdx_);
		}

		state_ = GPLS_Playing;
		currentTrack_ = tracks_[currentTrackIdx_];
		if( ptrToListener_ )
		{
			ptrToListener_->OnTrackPlay (currentTrack_);
		}
	}



	void Playlist::removeTrack (const std::string & trackName)
	{
		// if a track is removed from the list, but playing, the play cmd is kept.
		int idx=-1;
		for( std::vector<std::string>::iterator iter = tracks_.begin();
			iter != tracks_.end(); ++iter, ++idx )
		{
			if( *iter == trackName )
			{
				tracks_.erase(iter);
				if( currentTrackIdx_ == idx )
				{
					currentTrackIdx_ --;
				}
				return;
			}
		}
		throw SoundException ("Not using track.");
	}



	void Playlist::setFade (bool atPlay, bool atStop, bool atPause)
	{
		fadeInAtPlay_ = atPlay;
		fadeOutAtStop_ = atStop;
		fadeOutAtPause_ = atPause;
	}


	/// Move the iterator to the next item in the list. The play state is maintained.
	void Playlist::skipToNextTrack ()
	{
		GTRACE(5, "Playlist::skipToNextTrack()");
		// if there are no tracks, there's nowhere to skip to.
		if( playCount_.empty() )
		{
			return;
		}

		// If the playlist contains a single track, skipping to the next track means playing the same track all over again.
		// The question is: how does one handle cross-fading in this scenario?
		// Let's just get rid of it in this case.
		if (playCount_.size () == 1)
		{
			if (state_ == GPLS_Playing)
			{
				stop (false);
        play ();
			}
			if (ptrToListener_)
			{
				ptrToListener_->OnTrackChange (currentTrack_);
			}
			return;
		}


		PlayState temp = state_;
		if( state_ == GPLS_Playing || state_ == GPLS_Paused )
		{
			GTRACE(5, "Playlist::skipToNextTrack() while in pause or play.");
			stop ();
		}

		int newPos = 0;
		if( shuffle_ )
		{
			// get the minimum value of playcounts.
			int minPlayCount = playCount_[0];
			for( unsigned i=0; i< playCount_.size(); ++i )
			{
				if( playCount_[i] < minPlayCount )
				{
					minPlayCount = playCount_[i];
				}
			}
			std::vector<int> indices;
			for( unsigned i=0; i<playCount_.size(); ++i )
			{
				if( playCount_[i] == minPlayCount )
				{
					indices.push_back(i);
				}
			}
			if( repeat_ )
			{
				newPos = guslib::Random::getPtr()->getValueUpTo( (long)indices.size() );
				// if the play record is full, remove the oldest item.
				if( playOrder_.size() >= playCount_.size() )
				{
					playOrder_.erase(playOrder_.begin());
				}
				newPos = indices[newPos];
				playOrder_.push_back(newPos);
			}
			else
			{
				// no repeat
				if( minPlayCount == playCount_[0] && minPlayCount == 0 )
				{
					currentTrackIdx_ = 0;
					if( ptrToListener_ )
					{
						ptrToListener_->OnPlaylistEnd();
					}
					return;
				}
				else
				{
					newPos = guslib::Random::getPtr ()->getValueUpTo( (long)indices.size() );
					playOrder_.push_back(indices[newPos]);
				}
			}
		}
		else
		{
			newPos = currentTrackIdx_ + 1;
			if( newPos >= (int)tracks_.size() )
			{
				newPos = 0;
				if( ! repeat_ )
				{
					currentTrackIdx_ = 0;
					if( ptrToListener_ )
					{
						ptrToListener_->OnPlaylistEnd();
					}
					return;
				}
			}
		}

		currentTrack_ = tracks_[newPos];
		currentTrackIdx_ = newPos;

		if( temp == GPLS_Playing )
		{
			play ();
		}
		if( ptrToListener_ )
		{
			ptrToListener_->OnTrackChange( currentTrack_ );
		}
	}


	void Playlist::skipToPreviousTrack()
	{
		GTRACE(5, "Playlist::skipToPrevTrack()");
		// if there are no tracks, there's nowhere to skip to.
		if( playCount_.empty() )
		{
			return;
		}

		PlayState temp = state_;
		if( state_ == GPLS_Playing || state_ == GPLS_Paused )
		{
			GTRACE(5, "Playlist::skipToPreviousTrack() while in pause or play.");
			stop ();
		}

		if( shuffle_ )
		{
			for( unsigned i=0; i<playOrder_.size(); ++i )
			{
				GTRACE(7, "(before) play order["<<i<<"]="<<playOrder_[i]<<"; out of "<<playOrder_.size());
			}
			// to see where the skip should occur, we should check the track before
			// the last position in the play order.
			if( playOrder_.size() >1 )
			{
				currentTrackIdx_ = playOrder_[playOrder_.size()-2];
				GTRACE(6, "Should skip to track:"<<currentTrackIdx_);
				playOrder_.pop_back();
			}
			else if( playOrder_.size() ==1 )
			{
				//there is only one element;remove it
				
				currentTrackIdx_ = playOrder_[playOrder_.size()-1];
				GTRACE(6, "Should skip to track:"<<currentTrackIdx_);
				playOrder_.pop_back();
				//also don't resume play
				ptrToListener_->OnPlaylistEnd();
				return;
			}

			//else nothing can be done.
			for( unsigned i=0; i<playOrder_.size(); ++i )
			{
				GTRACE(7, "(after) play order["<<i<<"]="<<playOrder_[i]<<"; out of "<<playOrder_.size());
			}
		}
		else // (non-shuffle)
		{
			currentTrackIdx_ --;
			if(currentTrackIdx_ < 0 )
			{
				currentTrackIdx_ = (int)playCount_.size()-1;
			}
			currentTrack_ = tracks_[currentTrackIdx_];
		}
		if( temp == GPLS_Playing )
		{
			GTRACE(7, "resuming play");
			play ();
		}
		if( ptrToListener_ )
		{
			ptrToListener_->OnTrackChange( currentTrack_ );
		}
	}


	void Playlist::setRepeat( bool value )
	{
		repeat_ = value;
	}

	void Playlist::setShuffle( bool value )
	{
		shuffle_ = value;
		playOrder_.clear();
		for( unsigned i=0; i<playCount_.size(); ++i )
		{
			playCount_[i] = 0;
		}
	}


	void Playlist::stop ()
	{
		GTRACE (GUSSNDTLVL, "[" << id_ << "] Playlist received stop command");
		if (tracks_.empty ())
		{
			return;
		}
		
		if (currentTrackIdx_ < 0 || currentTrackIdx_ >= (int)tracks_.size ())
		{
			return;
		}

		if (fadeOutAtStop_)
		{
			ptrToRepository_->getSound (tracks_ [currentTrackIdx_])->addEffect ( 
				GSmartPtr<SoundEffect> (
					new FadeOutEffect (ptrToRepository_->getSound(tracks_[currentTrackIdx_])
					, (guslib::TimeUnits) (ptrToRepository_->getSound(tracks_ [currentTrackIdx_])->getSeekDuration ())
					, (guslib::TimeUnits) (ptrToRepository_->getSound(tracks_ [currentTrackIdx_])->getSeekDuration () + getFadeDuration ())
					, 0
					, true) 
					) 
				);
			//state_ = GPLS_PlayingButStopping;
		}
		else
		{
			ptrToRepository_->getSound (tracks_[currentTrackIdx_])->stop ();
		}
		GTRACE (GUSSNDTLVL, id_ << " Playlist has stopped state.");
		state_ = GPLS_Stopped;
	}



	void Playlist::stop (bool fadeOut)
	{
		GTRACE (GUSSNDTLVL, "[" << id_ << "] Playlist received stop command");
		if (tracks_.empty ())
		{
			return;
		}
		
		if (currentTrackIdx_ < 0 || currentTrackIdx_ >= (int)tracks_.size ())
		{
			return;
		}

		if (fadeOut)
		{
			ptrToRepository_->getSound (tracks_ [currentTrackIdx_])->addEffect ( 
				GSmartPtr<SoundEffect> (
					new FadeOutEffect (ptrToRepository_->getSound(tracks_[currentTrackIdx_])
					, (guslib::TimeUnits) (ptrToRepository_->getSound(tracks_ [currentTrackIdx_])->getSeekDuration ())
					, (guslib::TimeUnits) (ptrToRepository_->getSound(tracks_ [currentTrackIdx_])->getSeekDuration () + getFadeDuration ())
					, 0
					, true) 
					) 
				);
			//state_ = GPLS_PlayingButStopping;
		}
		else
		{
			ptrToRepository_->getSound (tracks_[currentTrackIdx_])->stop ();
		}
		GTRACE (GUSSNDTLVL, id_ << " Playlist has stopped state.");
		state_ = GPLS_Stopped;
	}


	void Playlist::onSoundEvent()
	{
		if( state_ == GPLS_Stopped )
			return;
		GTRACE(5, "Playlist::onSoundEvent()");

		if (autoSkipToNextTrack_)
		{
			GTRACE(5, "Playlist::onSoundEvent() skip to next track");
			skipToNextTrack ();
		}
	}

	// ------------------------------------- SoundRepository ------------------------------------------


	SoundRepository::SoundRepository()
	{
		GTRACE(6, "SoundRepository ctor");
		for( int i=GSC_Effect; i<GSC_None; ++i )
		{
			categoryVolumes_.push_back(0.5);
		}
	}

	SoundRepository::~SoundRepository()
	{
		GTRACE(6, "SoundRepository dtor");
		sounds_.clear ();
		GTRACE (6, "done clearing list");
	}



	void SoundRepository::addSound (const std::string &soundName, const std::string & fileName, bool loadIntoMemory, SoundCategory cat, bool onlyOneInstance)
	{
		GTRACE (4, "Adding sound named [" << soundName << "] from file [" << fileName << "] in categ [" << cat << "]");
		// search for the given name.
		SoundMap::iterator iter = sounds_.find (soundName);
		if( iter != sounds_.end() )
		{
			throw SoundException ("Tried to add a sound with a name already in use!");
		}

		// Get the extension from the file name
		std::string extension = guslib::stringutil::getExtensionFromFileName (fileName);

		EffectableSoundSmartPtr mySound = createSound(soundName, extension);
		mySound->loadFrom (audioDevice_, fileName, loadIntoMemory, onlyOneInstance);
		mySound->setCategory (cat);

		// Also set the volume for the sound according to the category.
		SoundVolume vol = getVolumeForCategory(cat);
		if (cat != GSC_Master)
		{
			vol *= getVolumeForCategory(GSC_Master);
		}
		mySound->setVolumeCategoryModifier (vol);
		
		sounds_.insert( std::make_pair( soundName, mySound ) );
	}


	void SoundRepository::elapseTime (const guslib::TimeUnits & time)
	{
		GTRACE (9, "Repository, elapsing time for sounds: " << time);
		for (SoundMap::iterator iter = sounds_.begin (); iter != sounds_.end (); ++iter)
		{
			iter->second->elapseTime (time);
		}
	}


	EffectableSoundSmartPtr & SoundRepository::getSound (const std::string &soundName)
	{
		SoundMap::iterator iter = sounds_.find (soundName);
		if (iter == sounds_.end ())
		{
			GTRACE (2, "EXCEPTION! Tried to get an inexistent sound: [" << soundName << "]");
			// Also log the existing sounds. Might be useful for a comparison.
			if (soundName.compare (EMPTY_SOUND_FILE) != 0)
			{
				GTRACE (3, "Current list of sounds:");
				for (SoundMap::iterator it2 = sounds_.begin(); 
					it2 != sounds_.end(); ++it2 )
				{
					GTRACE(3, " -> [" << it2->first << "]");
				}
			}
			throw SoundException ("Tried to get an inexistent sound!");
		}
		return iter->second;
	}


	void SoundRepository::removeSound (const std::string &soundName)
	{
		SoundMap::iterator iter = sounds_.find (soundName);
		if (iter == sounds_.end ())
		{
			GTRACE (2, "EXCEPTION! Tried to remove an inexistent sound: " << soundName);
			throw SoundException ("Tried to remove an inexistent sound!");
		}

		sounds_.erase (iter);
	}


	SoundVolume SoundRepository::getVolumeForCategory (SoundCategory categ) const
	{
		return categoryVolumes_[categ];
	}


	void SoundRepository::setVolumeForCategory (SoundCategory categ, SoundVolume vol)
	{
		if (categ == GSC_None)
		{
			GTRACE (3, "WARNING: Trying to set volume for category [none]. Not allowed.");
			return;
		}

		// store it.
		categoryVolumes_[categ] = vol;
		GTRACE (5, "Volume for category [" << categ << " is now " << vol);

		// treat it differently: master will affect everything.

		if (categ == GSC_Master)
		{
			for (SoundMap::iterator iter = sounds_.begin();
				iter != sounds_.end(); ++iter)
			{
				SoundVolume catVol = categoryVolumes_[iter->second->getCategory()];
				iter->second->setVolumeCategoryModifier (vol * catVol);
			}
		}
		else
		{
			// non-master will only affect only the given category. Also take the master volume into account.

			SoundVolume masterVol = categoryVolumes_[GSC_Master];

			for (SoundMap::iterator iter = sounds_.begin();
				iter != sounds_.end(); ++iter)
			{
				if (iter->second->getCategory() == categ)
				{
					GTRACE (6, "Setting cat volume for " << iter->second->getId());
					iter->second->setVolumeCategoryModifier (vol * masterVol);
				}
			}
		}

	}




	// ----------------------------------------------- MusicPlayer -------------------------------------------------


	MusicPlayer::MusicPlayer (const std::string & id)
		: ptrToRepository_ (NULL)
		, playstate_ (GPLS_Stopped)
		, shuffle_ (false)
		, repeat_ (true)
		, volume_ (max_volume)
		, skipToNextTrackAutomatically_ (true)
		, fadeInAtPlay_ (true)
		, fadeOutAtStop_ (true)
		, fadeOutAtPause_ (true)
		, currentList_ ("")
		, keepPlayingAtListChange (true)
		, listenerPtr_ (NULL)
		, id_ (id)
		, fadeDuration_ (standard_fade_duration)
	{
		GTRACE(6, "MusicPlayer ctor");
	}

	MusicPlayer::~MusicPlayer ()
	{
		// handle the destruction of all managed lists.
		GTRACE (5, "MusicPlayer dtor");
		// make sure to manually call the playlists clear first
		// this is done because they have callbacks to the listener interface, which is our object.
		// 
		playlists_.clear ();
		GTRACE (5, "(destroying player: " << id_ << ")");
	}

	bool MusicPlayer::isPlaylistRegistered (const std::string& name)
	{
		PlaylistMapping::iterator iter = playlists_.find (name);
		return iter != playlists_.end ();
	}

	void MusicPlayer::registerPlaylist (const std::string & name)
	{
		GTRACE (5, "MusicPlayer registering playlist [" << name << "]");
		if (!ptrToRepository_)
		{
			throw SoundException ("MusicPlayer: not initialized! (no repository registered). Attempting to register a playlist.");
		}

		PlaylistMapping::iterator iter = playlists_.find (name);
		if (iter != playlists_.end ())
		{
			// There is a playlist already registered.
			throw SoundException ("MusicPlayer: attempting to re-register a playlist!");
		}

		GTRACE (6, "MusicPlayer registerPlaylist added [" << name << "]");
		// store a temporary pointer, so that we may assign some values to the playlist.
		// these values are general and belong to the music player, so they need to be transferred to the playlist.
		Playlist * myPlaylistPtr = new Playlist (ptrToRepository_, this, name);
		myPlaylistPtr->setFadeDuration (fadeDuration_);
		myPlaylistPtr->setRepeat (repeat_);
		playlists_.insert (std::make_pair (name, PlaylistSmartPtr (myPlaylistPtr)));
		

		// If this is the first list registered, make it the current list automatically

		if (currentList_ == "")
		{
			currentList_ = name;
		}
		GTRACE (6, "MusicPlayer registerPlaylist currentList_ [" << currentList_ << "]");
	}


	void MusicPlayer::unregisterPlaylist( const std::string & name )
	{
		PlaylistMapping::iterator iter = playlists_.find( name );
		if( iter == playlists_.end() )
		{
			throw SoundException ("MusicPlayer: attempting to un-register a non-existent playlist!");
		}

		// If we're unregistering the playlist that we're currently playing, make sure to set the current list to be empty
		// (so that we don't try to play it).
		if (currentList_ == name)
		{
			currentList_ = "";
		}

		playlists_.erase( iter );

		// If there are no more registered playlists left, clear the current list.

		if( playlists_.begin() == playlists_.end() )
		{
			currentList_ = "";
		}
	}


	void MusicPlayer::addTrackToPlaylist (const std::string & playlistName, const std::string & trackName)
	{
		PlaylistMapping::iterator iter = playlists_.find (playlistName);
		if (iter == playlists_.end ())
		{
			if (playlistName.compare ("") != 0)
			{
				// Also log the existing playlists. Might be useful for a comparison.
				GTRACE (3, "Could not find playlist [" << playlistName << "]. Current list of playlists:");
				for (PlaylistMapping::const_iterator it2 = playlists_.begin (); 
					it2 != playlists_.end (); ++it2 )
				{
					GTRACE (3, " -> [" << it2->first << "]");
				}
			}
			throw SoundException ("MusicPlayer: attempting to add a track to a non-existent playlist!");
		}

		playlists_[playlistName]->addTrack (trackName);
	}



	void MusicPlayer::removeTrackFromPlaylist( const std::string & playlistName, const std::string & trackName )
	{
		PlaylistMapping::iterator iter = playlists_.find( playlistName );
		if( iter == playlists_.end() )
		{
			throw SoundException("MusicPlayer: attempting to remove a track from a non-existent playlist!");
		}

		playlists_[playlistName]->removeTrack( trackName );
	}



	void MusicPlayer::nextTrack()
	{
		GTRACE(5, "MusicPlayer::nextTrack");
		// Ask for next track in the current playlist.

		if( currentList_ != "" )
		{
			playlists_[currentList_]->skipToNextTrack();
		}
	}

	void MusicPlayer::stop ()
	{
		GTRACE (GUSSNDTLVL, "MusicPlayer received stop command");

		// Stop the current playlist.

		if( currentList_ != "" )
		{
			playlists_[currentList_]->stop ();
		}
	}

	void MusicPlayer::play ()
	{
		GTRACE (GUSSNDTLVL, "MusicPlayer received play command");

		// Play the current playlist.

		if( currentList_ != "" )
		{
			playlists_[currentList_]->play ();
		}
	}

	void MusicPlayer::pause()
	{
		GTRACE (GUSSNDTLVL, "MusicPlayer received pause command");

		// Pause the current playlist.

		if( currentList_ != "" )
		{
			playlists_[currentList_]->pause();
		}
	}
	
	void MusicPlayer::previousTrack()
	{
		// Ask for previous track in the current playlist.

		if( currentList_ != "" )
		{
			playlists_[currentList_]->skipToPreviousTrack();
		}
	}

	void MusicPlayer::setFadePreferrences (bool atPlay, bool atStop, bool atPause)
	{
		fadeInAtPlay_ = atPlay;
		fadeOutAtStop_ = atStop;
		fadeOutAtPause_ = atPause;
	}


	void MusicPlayer::setFadeDuration (SoundDuration dur)
	{
		fadeDuration_ = dur;

		for( PlaylistMapping::iterator iter = playlists_.begin();
				iter != playlists_.end(); ++iter )
		{
			iter->second->setFadeDuration( dur );
		}
	}

	void MusicPlayer::setRepeat (bool value)
	{
		repeat_ = value;
		for( PlaylistMapping::iterator iter = playlists_.begin();
				iter != playlists_.end(); ++iter )
		{
			iter->second->setRepeat( value );
		}
	}


	void MusicPlayer::setShuffle (bool value)
	{
		for( PlaylistMapping::iterator iter = playlists_.begin();
				iter != playlists_.end(); ++iter )
		{
			iter->second->setShuffle( value );
		}
	}




	void MusicPlayer::setPlaylistRepeat (const std::string& playlistName, bool value)
	{
		PlaylistMapping::iterator iter = playlists_.find (playlistName);
		if (iter == playlists_.end ())
		{
			if (playlistName.compare ("") != 0)
			{
				// Also log the existing playlists. Might be useful for a comparison.
				GTRACE (3, "Could not find playlist [" << playlistName << "]. Current list of playlists:");
				for (PlaylistMapping::const_iterator it2 = playlists_.begin (); 
					it2 != playlists_.end (); ++it2 )
				{
					GTRACE (3, " -> [" << it2->first << "]");
				}
			}
			throw SoundException ("MusicPlayer: attempting set repeat property to a non-existent playlist!");
		}

		playlists_[playlistName]->setRepeat (value);
	}
	

	
	void MusicPlayer::setPlaylistShuffle (const std::string& playlistName, bool value)
	{
		PlaylistMapping::iterator iter = playlists_.find (playlistName);
		if (iter == playlists_.end ())
		{
			if (playlistName.compare ("") != 0)
			{
				// Also log the existing playlists. Might be useful for a comparison.
				GTRACE (3, "Could not find playlist [" << playlistName << "]. Current list of playlists:");
				for (PlaylistMapping::const_iterator it2 = playlists_.begin (); 
					it2 != playlists_.end (); ++it2 )
				{
					GTRACE (3, " -> [" << it2->first << "]");
				}
			}
			throw SoundException ("MusicPlayer: attempting set shuffle property to a non-existent playlist!");
		}

		playlists_[playlistName]->setShuffle (value);
	}



	void MusicPlayer::setListener (MusicPlayerListener * listener)
	{
		listenerPtr_ = listener;
	}


	void MusicPlayer::switchToPlaylist( const std::string & playlistName, bool pauseExisting )
	{
		PlaylistMapping::iterator iter = playlists_.find (playlistName);
		if (iter == playlists_.end ())
		{
			GTRACE (5, id_ << " MusicPlayer: attempting to switch to a non-existent playlist: " << playlistName);
			throw SoundException ("MusicPlayer: attempting to switch to a non-existent playlist!");
		}

		GTRACE (5, id_ << " MusicPlayer switching to playlist " << playlistName);

		if (playlistName == currentList_)
		{
			GTRACE (5, "MusicPlayer already playing from playlist " << playlistName);
			return;
		}

		if (currentList_ == "")
		{
			return;
		}

		if (pauseExisting)
		{
			GTRACE (5, "Pausing existing playlist: " << currentList_);
			playlists_[currentList_]->pause ();
		}
		else
		{
			GTRACE (5, "Stopping existing playlist: " << currentList_);
			playlists_[currentList_]->stop ();
		}

		currentList_ = playlistName;
		GTRACE (5, "MusicPlayer issuing play to current playlist " << currentList_);
		play ();
	}

	void MusicPlayer::OnTrackPlay (const std::string & newTrack)
	{
		GTRACE (7, "MusicPlayer notifying listener of a new track being played: " << newTrack);

		if (listenerPtr_)
		{
			listenerPtr_->OnTrackPlay (newTrack);
		}
	}

	void MusicPlayer::OnTrackChange (const std::string & newTrack)
	{
		GTRACE (7, "MusicPlayer notifying listener of a track change occuring: " << newTrack);
		if (listenerPtr_)
		{
			listenerPtr_->OnTrackChange (newTrack);
		}
	}


	void MusicPlayer::OnPlaylistEnd ()
	{
		GTRACE (6, "MusicPlayer notifying listener of a playlist end");
		if (listenerPtr_)
		{
			listenerPtr_->OnPlaylistEnd ();
		}
	}


	// ------------------------------ SoundManagerUtil -----------------------------------------------

	SoundManagerUtil::SoundManagerUtil (const SoundRepositorySmartPtr & repos)
		: repository_ (repos)
		, player_ (MusicPlayerSmartPtr (new MusicPlayer ("s-util")))
	{
		GTRACE(6, "SoundManagerUtil ctor");
		player_->setRepository (repository_);
		GTRACE(6, "SoundManagerUtil ctor opened device, prepared repository");
	}

	SoundManagerUtil::~SoundManagerUtil()
	{
		GTRACE(6, "SoundManagerUtil dtor");
		groups_.clear ();
		GTRACE (6, "SoundManagerUtil cleared custom.");
	}


	void SoundManagerUtil::addSoundGroup (const std::string& groupName)
	{
		SoundGroupMapping::iterator iter = groups_.find( groupName );
		if( iter != groups_.end() )
		{
			throw SoundException ("Tried to re-create existent group!");
		}
		groups_.insert (std::make_pair (groupName, SoundGroupSmartPtr(new SoundGroup ())));
	}

	void SoundManagerUtil::addPlaylistTrack (const std::string & playlistName, const std::string & trackName)
	{
		GTRACE (4, "Adding anon. track [" << trackName << "] to playlist [" << playlistName << "]");
		std::string tempName = gussound::hidden::getAnonymousTrackName ();
		getRepository ()->addSound (tempName, trackName, false, GSC_Music, true);
		getMusicPlayer ()->addTrackToPlaylist (playlistName, tempName);
	}



	SoundGroupSmartPtr & SoundManagerUtil::getSoundGroup (const std::string& groupName)
	{
		SoundGroupMapping::iterator iter = groups_.find (groupName);
		if (iter == groups_.end ())
		{
			GTRACE (3, "Current list of groups:");
			for (SoundGroupMapping::const_iterator it2 = groups_.begin ();
				it2 != groups_.end (); ++it2)
			{
				GTRACE (3, " -> [" << it2->first << "]");
			}

			throw SoundException ("Tried to access inexistent group!");
		}
		return iter->second;
	}



	/// Create and return a string containing the version of the sound library
	std::string SoundManagerUtil::getVersion () const
	{
		std::stringstream ss;
		ss << "gussound-" << GUSSOUND_MAJOR_VERSION << "." << GUSSOUND_MINOR_VERSION << "." << GUSSOUND_PATCH_VERSION;
		return ss.str ();
	}


	void SoundManagerUtil::removeSoundGroup (const std::string& groupName)
	{
		SoundGroupMapping::iterator iter = groups_.find( groupName );
		if( iter == groups_.end() )
		{
			throw SoundException ("Tried to remove inexistent group!");
		}
		groups_.erase( iter );
	}



	///
	/// Process some time dependant internal logic.
	/// React to some time units having passed from the last time the function was called.
	/// This will generally be used to control the playlists.
	/// The user of the audio engine will have to call this function on a regular basis
	/// (i.e. in a loop in the main thread or in a separate thread).
	///
	void SoundManagerUtil::elapseTime ()
	{
		guslib::TimeUnits duration = timer_.renew ();
		repository_->elapseTime (duration);
	}



	///
	/// Process some time dependant internal logic.
	/// React to some time units having passed from the last time the function was called.
	/// This will generally be used to control the playlists.
	/// The user of the audio engine will have to call this function on a regular basis
	/// (i.e. in a loop in the main thread or in a separate thread).
	/// @param timeUnits The amount of time units (milliseconds) elapsed since the last call.
	///
	void SoundManagerUtil::elapseTimeByMilliseconds (const guslib::TimeUnits& timeUnits)
	{
		repository_->elapseTime (timeUnits);
	}



	///
	/// Set the log file to be used (part of the tracing library).
	///
	void SoundManagerLogger::setLoggerTarget (const std::string& fileName, int level)
	{
		GSTARTTRACING (fileName, level);
	}


}

