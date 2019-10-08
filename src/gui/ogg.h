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

#ifndef __ogg_h__
#define __ogg_h__

#include <string>
#include <iostream>
using namespace std;

#ifdef WIN32
	#include <al.h>
#else
	#include <AL/al.h>
#endif

#include <ogg/ogg.h>
#include <vorbis/vorbisfile.h>



#define BUFFER_SIZE (4096 * 16)



class ogg_stream
{
    public:
	ogg_stream()
	{
		m_volume = 1;
		m_initialized = false;
	}
	
	~ogg_stream()
	{ 
		if (m_initialized)
		{
			alDeleteSources(1, &source);
			oggcheck();
		}
	}
    
        void open(string path);
        void release();
        void display();
        bool playback();
        bool playing();
        bool update();
		
		void setVolume(float volume)
		{
			if (playing())
			{
				alSourcef (source, AL_GAIN,volume);
			}
			m_volume = volume;
		}

    protected:

        bool stream(ALuint buffer);
        void empty();
        void oggcheck();
        string errorString(int code);

    private:

        FILE*           oggFile;
        OggVorbis_File  oggStream;
        vorbis_info*    vorbisInfo;
        vorbis_comment* vorbisComment;

        ALuint buffers[2];
        ALuint source;
        ALenum format;
		float m_volume;
		bool m_initialized;
};


#endif // __ogg_h__
