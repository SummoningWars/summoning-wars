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

#include "sound.h"

#include "elementattrib.h"
#include "random.h"
#include <OgreResourceGroupManager.h>

std::map<SoundName, SoundSystem::SoundSet> SoundSystem::m_sounds;

std::map<std::string, SoundObject*> SoundSystem::m_sound_objects;

float SoundSystem::m_sound_volume = 0.0f;
ALCdevice * SoundSystem::m_device;
ALCcontext* SoundSystem::m_context;
std::list<SoundObject*> SoundSystem::m_ambient_sounds;

Sound SoundSystem::getSound(SoundName sname)
{
	// Iterators for the first and last Sounds with the desired name.
	std::map<SoundName, SoundSet>::iterator it;
    it = m_sounds.find(sname);
    if ( it == m_sounds.end())
    {
        return AL_NONE;
    }
    SoundSet& sset = it->second;
    
    // roll whether to play a sound
    if (Random::random() > sset.m_probability)
    {
        return AL_NONE;
    }
    
    // check that there are sounds in the list
    if (sset.m_weights.size() == 0)
    {
        return AL_NONE;
    }


    int soundnr = Random::randDiscrete(sset.m_weights);
    DEBUGX("play sound %i for soundname %s",sset.m_sounds[soundnr], sname.c_str());
    return sset.m_sounds[soundnr];
}

void SoundSystem::loadSoundFile(std::string file, SoundName sname, float weight)
{
	Ogre::FileInfoListPtr files;
	Ogre::FileInfoList::iterator it;
	std::string filename;
	files = Ogre::ResourceGroupManager::getSingleton().findResourceFileInfo("sound",file);
	
	for (it = files->begin(); it != files->end(); ++it)
	{
		filename = it->archive->getName();
		filename += "/";
		filename += it->filename;
		
		DEBUGX("loading file %s",filename.c_str());
		
		int buffernr;
		buffernr = alutCreateBufferFromFile(filename.c_str());

		// Check for errors. We may get an error if the sound format is not supported.
		if (buffernr !=  AL_NONE)
		{
			m_sounds[sname].m_weights.push_back(weight);
            m_sounds[sname].m_sounds.push_back(buffernr);
		}
		else
		{
			ERRORMSG ("Could not load [%s]", filename.c_str ());
		}
	}
	checkErrors();
}

void SoundSystem::loadSoundData(const char* pFilename)
{
	TiXmlDocument doc(pFilename);
	bool loadOkay = doc.LoadFile();

	if (loadOkay)
	{
		loadSoundInfos(&doc);
	}
	else
	{
		ERRORMSG("Failed to load file %s", pFilename);
	}
}

void SoundSystem::loadSoundInfos(TiXmlNode* node)
{
	TiXmlNode* child;
	if (node->Type()==TiXmlNode::TINYXML_ELEMENT && !strcmp(node->Value(), "Sound"))
	{
		ElementAttrib attr;
		attr.parseElement(node->ToElement());
		
		std::string name,file;
		attr.getString("name",name);
        attr.getFloat("probability",m_sounds[name].m_probability, 1.0);

		
		for ( child = node->FirstChild(); child != 0; child = child->NextSibling())
		{
			if (child->Type()==TiXmlNode::TINYXML_ELEMENT)
			{
				attr.parseElement(child->ToElement());
				if (!strcmp(child->Value(), "Soundfile"))
				{
					attr.getString("source",file);
                    float weight;
                    attr.getFloat("weight",weight,1.0);
                    
					DEBUGX("loading sound file %s for sound %s",file.c_str(), name.c_str());
					loadSoundFile(file, name,weight);
				}
			}
		}
	}
	else
	{
		for ( child = node->FirstChild(); child != 0; child = child->NextSibling())
		{
			loadSoundInfos(child);
		}
	}
}

void SoundSystem::init()
{
	m_device = 0;
	m_context = 0;
	// variante 1, geht unter ubuntu 9.10 nicht
	/*
    m_device = alcOpenDevice("Generic Software");
    m_context = alcCreateContext(m_device, NULL);
    alcMakeContextCurrent(m_context);
	alutInitWithoutContext(0,0);
	*/
	
	// variante 2, geht unter win nicht
	//alutInit(0,0);
	
	// variante 3: zu testen
	/*
	const ALCchar *defaultDevice = alcGetString(NULL, ALC_DEFAULT_DEVICE_SPECIFIER);
	m_device = alcOpenDevice(defaultDevice);
	m_context = alcCreateContext(m_device, NULL);
	alcMakeContextCurrent(m_context);
	alutInitWithoutContext(0,0);
	*/
	
	// variante 4: zu testen
	
	m_device = alcOpenDevice(NULL);
    m_context = alcCreateContext(m_device, NULL);
    alcMakeContextCurrent(m_context);
	alutInitWithoutContext(NULL,NULL);
	
	alDistanceModel(AL_LINEAR_DISTANCE_CLAMPED);
	checkErrors();
}

void SoundSystem::setListenerPosition(Vector pos)
{
	ALfloat listenerPos[3]={0.0,0.0,0.0};
	listenerPos[0] = pos.m_x;
	listenerPos[1] = pos.m_y;

	alListenerfv(AL_POSITION,listenerPos);
	checkErrors();
}

void SoundSystem::update()
{
	std::list<SoundObject*>::iterator it;
	for (it = m_ambient_sounds.begin(); it != m_ambient_sounds.end(); )
	{
		(*it)->update();
		if ((*it)->isPlaying())
		{
			++it;
		}
		else
		{
			std::list<SoundObject*>::iterator todel = it;
			++it;
			deleteSoundObject(*todel);
			m_ambient_sounds.erase(todel);
		}
	}
}

void SoundSystem::cleanup()
{
	std::map<SoundName, SoundSet>::iterator it;
    std::vector<Sound>::iterator st;
	for (it = m_sounds.begin(); it != m_sounds.end(); ++it)
	{
        for (st = it->second.m_sounds.begin(); st != it->second.m_sounds.end(); ++st)
        {
            alDeleteBuffers(1,&(*st));
        }
	}
	m_sounds.clear();
	
	std::list<SoundObject*>::iterator jt;
	for (jt = m_ambient_sounds.begin(); jt != m_ambient_sounds.end(); ++jt)
	{
		deleteSoundObject(*jt);
	}
	m_ambient_sounds.clear();
		

	clearObjects();
}

void SoundSystem::cleanupBuffers()
{
	// stop the soundsobjects from using buffer contents
	std::list<SoundObject*>::iterator jt;
	for (jt = m_ambient_sounds.begin(); jt != m_ambient_sounds.end(); ++jt)
	{
		(*jt)->stop();
	}
	
	std::map<std::string, SoundObject*>::iterator it;
	for (it = m_sound_objects.begin(); it != m_sound_objects.end(); ++it)
	{
		it->second->stop();
	}
	
	std::vector<Sound>::iterator st;
	for (std::map<SoundName, SoundSet>::iterator it = m_sounds.begin(); it != m_sounds.end(); ++it)
	{
		for (st = it->second.m_sounds.begin(); st != it->second.m_sounds.end(); ++st)
		{
			alDeleteBuffers(1,&(*st));
		}
	}
	m_sounds.clear();
}

void SoundSystem::clearObjects()
{
	std::map<std::string, SoundObject*>::iterator it;
	for (it = m_sound_objects.begin(); it != m_sound_objects.end(); ++it)
	{
		delete it->second;
	}

	m_sound_objects.clear();
}


void SoundSystem::setSoundVolume(float vol)
{
	m_sound_volume = vol;
	
	// Apply new sound volume for existing sound objects.
	std::map<std::string, SoundObject*>::iterator it;
	for (it = m_sound_objects.begin(); it != m_sound_objects.end(); ++it)
	{
		it->second->setVolume(vol);
	}
}

SoundObject* SoundSystem::createSoundObject(std::string name)
{
	if (m_sound_objects.count(name)>0)
	{
		ERRORMSG("Sound object with name %s already exists",name.c_str());
		return getSoundObject(name);
	}
	DEBUGX("creating sound object with name %s",name.c_str());

	SoundObject* so;
	so = new SoundObject(name);
	so->setVolume(m_sound_volume);
	m_sound_objects.insert(std::make_pair(name,so));
	checkErrors();
	
	return so;

}

SoundObject* SoundSystem::getSoundObject(std::string name)
{
	std::map<std::string, SoundObject*>::iterator it;
	it = m_sound_objects.find(name);
	if (it ==m_sound_objects.end())
	{
		return 0;
	}

	return it->second;
}

void SoundSystem::deleteSoundObject(std::string name)
{
	std::map<std::string, SoundObject*>::iterator it;
	it = m_sound_objects.find(name);
	if (it !=m_sound_objects.end())
	{
		delete it->second;
		m_sound_objects.erase(it);
	}
	else
	{
		ERRORMSG("Cant delete Sound Object %s",name.c_str());
	}
}

void SoundSystem::deleteSoundObject(SoundObject* object)
{
	deleteSoundObject(object->getName());
}


bool SoundSystem::getSourceHandle(ALuint &handle)
{
	bool success = true;
	
	alGenSources(1, &handle);
	if (alGetError() != AL_NO_ERROR)
	{
		success = false;
	}
	else
	{
		//alSourcei(m_handle, AL_LOOPING, AL_TRUE);
		alSourcef(handle, AL_REFERENCE_DISTANCE, 5);
		alSourcef(handle, AL_ROLLOFF_FACTOR, 0.5);
		
		alSourcef(handle,AL_MAX_DISTANCE , 20);
	}
	return success;
}

void SoundSystem::playAmbientSound(SoundName sname, float volume, Vector* position)
{
	static int id=1;
	std::stringstream stream;
	stream << "ambient#"<<id;
	id ++;
	
	DEBUGX("play ambient sound %s volume %f",sname.c_str(), volume);
	
	SoundObject* sobj = createSoundObject(stream.str());
	sobj->setVolume(volume*m_sound_volume);
	sobj->setSound(sname);
	
	// if position is given: set it,
	// otherwise the object is directly attached to the listener
	if (position != 0)
	{
		sobj->setPosition(*position);
	}
	else
	{
		sobj->setPositionRelative(Vector(0,0));
	}
	m_ambient_sounds.push_back(sobj);
}

bool SoundSystem::checkErrors()
{
	int error = alGetError();

	if(error != AL_NO_ERROR)
	{
		ERRORMSG("AL error %i: %s",error, alGetString(error)) ;
		return true;
	}
	return false;
}





SoundObject::SoundObject(std::string name, Vector pos)
{
	/*
	m_handle_valid = SoundSystem::getSourceHandle(m_handle);
	setPosition(pos);
	SoundSystem::checkErrors();
	*/
	m_handle_valid = false;
	m_sound_name = "";
	m_name = name;
	m_global_volume = 1.0;
	m_volume = 1.0;
}

SoundObject::~SoundObject()
{
	if (m_handle_valid)
	{
		alDeleteSources(1,&m_handle);
	}
}

void SoundObject::setPosition(Vector pos)
{
	m_position = pos;
	

	if (m_handle_valid)
	{
		ALfloat spos[3]={0.0,0.0,0.0};
		spos[0] = pos.m_x;
		spos[1] = pos.m_y;
	
		alSourcefv(m_handle, AL_POSITION, spos);
		alSourcei (m_handle, AL_SOURCE_RELATIVE, AL_FALSE);
	}
}

void SoundObject::setPositionRelative(Vector pos)
{
	m_position = pos;
	
	if (m_handle_valid)
	{
		ALfloat spos[3]={0.0,0.0,0.0};
		spos[0] = pos.m_x;
		spos[1] = pos.m_y;
	
		alSourcefv(m_handle, AL_POSITION, spos);
		alSourcei (m_handle, AL_SOURCE_RELATIVE, AL_TRUE      );
	}
}

Vector SoundObject::getPosition()
{
	return m_position;
}

void SoundObject::setSound(Sound snd)
{
	// if this object has no valid handle, try to get a valid one
	if (!m_handle_valid)
	{
		m_handle_valid = SoundSystem::getSourceHandle(m_handle);
		setPosition(m_position);
		setVolume(m_volume);
	}
	
	if (m_handle_valid)
	{
		alSourcei(m_handle, AL_BUFFER, snd);
		SoundSystem::checkErrors();
	}
	else
	{
		m_sound_name = "";
	}
}

void SoundObject::setSound(SoundName sname)
{
	update();

	if (m_sound_name != "")
	{
		return;
	}

	if (sname == "")
	{
		m_sound_name = "";
		return;
	}


	Sound snd = SoundSystem::getSound(sname);
	if (snd != AL_NONE)
	{
		m_sound_name =sname;
		setSound(snd);
		play();
	}
}

void SoundObject::play()
{
	if (m_handle_valid)
	{
		alSourcePlay(m_handle);
		SoundSystem::checkErrors();
	}
}

void SoundObject::stop()
{
	if (m_handle_valid)
	{
		alSourceStop(m_handle);
		SoundSystem::checkErrors();
	}
	m_sound_name="";
}

void SoundObject::rewind()
{
	if (m_handle_valid)
	{
		alSourceRewind(m_handle);
		SoundSystem::checkErrors();
	}
}

void SoundObject::update()
{
	if (m_handle_valid)
	{
		int state;
		alGetSourcei(m_handle,AL_SOURCE_STATE,&state);

		if (state == AL_STOPPED)
		{
			m_sound_name = "";
			alDeleteSources(1, &m_handle);
			m_handle_valid = false;
		}
	}
}

bool SoundObject::isPlaying()
{
	if (!m_handle_valid)
		return false;
	
	int state;
	alGetSourcei(m_handle,AL_SOURCE_STATE,&state);
	if (state == AL_STOPPED)
		return false;
	
	return true;
}

