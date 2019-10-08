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

#include "ogg.h"

#include "debug.h"


void ogg_stream::open(string path)
{
	if (!m_initialized)
	{
		alGenSources(1, &source);
		int error = alGetError();

		if(error != AL_NO_ERROR)
		{
			return;
		}
		m_initialized = true;
	}
    int result;
    
    if(!(oggFile = fopen(path.c_str(), "rb")))
        throw string("Could not open Ogg file.");

    if((result = ov_open(oggFile, &oggStream, NULL, 0)) < 0)
    {
        fclose(oggFile);
        
        throw string("Could not open Ogg stream. ") + errorString(result);
    }
	
	vorbisInfo = ov_info(&oggStream, -1);
    vorbisComment = ov_comment(&oggStream, -1);

    if(vorbisInfo->channels == 1)
        format = AL_FORMAT_MONO16;
    else
        format = AL_FORMAT_STEREO16;
        
    alGenBuffers(2, buffers);
	oggcheck();
    alSource3f(source, AL_POSITION,        0.0, 0.0, 0.0);
    alSource3f(source, AL_VELOCITY,        0.0, 0.0, 0.0);
    alSource3f(source, AL_DIRECTION,       0.0, 0.0, 0.0);
    alSourcef (source, AL_ROLLOFF_FACTOR,  0.0          );
    alSourcei (source, AL_SOURCE_RELATIVE, AL_TRUE      );
	alSourcef (source, AL_GAIN,m_volume);
	
}




void ogg_stream::release()
{
    alSourceStop(source);
    empty();
    alDeleteBuffers(2, buffers);
    oggcheck();

    ov_clear(&oggStream);
}




void ogg_stream::display()
{
    cout
        << "version         " << vorbisInfo->version         << "\n"
        << "channels        " << vorbisInfo->channels        << "\n"
        << "rate (hz)       " << vorbisInfo->rate            << "\n"
        << "bitrate upper   " << vorbisInfo->bitrate_upper   << "\n"
        << "bitrate nominal " << vorbisInfo->bitrate_nominal << "\n"
        << "bitrate lower   " << vorbisInfo->bitrate_lower   << "\n"
        << "bitrate window  " << vorbisInfo->bitrate_window  << "\n"
        << "\n"
        << "vendor " << vorbisComment->vendor << "\n";
        
    for(int i = 0; i < vorbisComment->comments; i++)
        cout << "   " << vorbisComment->user_comments[i] << "\n";
        
    cout << endl;
}




bool ogg_stream::playback()
{
    if(playing())
        return true;
        
    if(!stream(buffers[0]))
        return false;
        
    if(!stream(buffers[1]))
        return false;
    
    alSourceQueueBuffers(source, 2, buffers);
    alSourcePlay(source);
    
    return true;
}




bool ogg_stream::playing()
{
    ALenum state;
    
    alGetSourcei(source, AL_SOURCE_STATE, &state);
    
    return (state == AL_PLAYING);
}




bool ogg_stream::update()
{
    int processed;
    bool active = true;

    alGetSourcei(source, AL_BUFFERS_PROCESSED, &processed);
	oggcheck();
	while(processed>0)
    {
		processed--;
		DEBUGX("processed %i",processed+1);
        ALuint buffer;
        
        alSourceUnqueueBuffers(source, 1, &buffer);
		DEBUGX("unqueued buffer %i", buffer);
        oggcheck();

        active = stream(buffer);

        alSourceQueueBuffers(source, 1, &buffer);
        oggcheck();
    }

	if (active && !playing())
	{
		alSourcePlay(source);
	}

    return active;
}




bool ogg_stream::stream(ALuint buffer)
{
	DEBUGX("streaming buffer %i",buffer);
	char pcm[BUFFER_SIZE];
    int  size = 0;
    int  section;
    int  result;

    while(size < BUFFER_SIZE)
    {
        result = ov_read(&oggStream, pcm + size, BUFFER_SIZE - size, 0, 2, 1, &section);
    
        if(result > 0)
            size += result;
        else
            if(result < 0)
                throw errorString(result);
            else
                break;
    }
    
    if(size == 0)
	{
		return false;
	}
    
	if (!alIsBuffer(buffer))
	{
		DEBUG("specified number is not a buffer");
	}
    alBufferData(buffer, format, pcm, size, vorbisInfo->rate);
    oggcheck();
    
    return true;
}




void ogg_stream::empty()
{
    int queued;
    
    alGetSourcei(source, AL_BUFFERS_QUEUED, &queued);
    
    while(queued--)
    {
        ALuint buffer;
    
        alSourceUnqueueBuffers(source, 1, &buffer);
        oggcheck();
    }
}




void ogg_stream::oggcheck()
{
	int error = alGetError();

	if(error != AL_NO_ERROR)
	{
		ERRORMSG("AL error message %s",alGetString(error));
	}
}



string ogg_stream::errorString(int code)
{
    switch(code)
    {
        case OV_EREAD:
            return string("Read from media.");
        case OV_ENOTVORBIS:
            return string("Not Vorbis data.");
        case OV_EVERSION:
            return string("Vorbis version mismatch.");
        case OV_EBADHEADER:
            return string("Invalid Vorbis header.");
        case OV_EFAULT:
            return string("Internal logic fault (bug or heap/stack corruption.");
        default:
            return string("Unknown Ogg error.");
    }
}
