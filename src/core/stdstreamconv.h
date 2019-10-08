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

#ifndef STD_STREAM_CONV_H
#define STD_STREAM_CONV_H

#include "charconv.h"
#include "debug.h"

/**
 * \class StdStreamConv
 * \brief Class for writing and data to and reading from a stdlib stream
 */
class StdStreamConv : public CharConv
{
	public:
		/**
		 * \brief Constructor
		 * \param stream Stream object that is used for reading and writing
		 */
		StdStreamConv(std::iostream* stream)
		{
			m_stream = stream;
		}
		
		/**
		 * \brief Destructor
		 */
		virtual ~StdStreamConv()
		{
			
		};
		
		/**
		 * \brief writes raw data to the buffer
		 * \param data raw data
		 * \param size length of data in bytes
		 */
		virtual void toBuffer(const char* data, unsigned int size)
		{
			m_stream->write(data,size);
		}
		
		
		/**
		 * \brief reads raw data from buffer
		 * \param data output buffer for raw data
		 * \param size length of data in bytes
		 */
		virtual void fromBuffer(char* data, unsigned int size)
		{
			m_stream->read(data,size);
		}
		
		
		/**
		 * \brief writes a string to the buffer
		 * \param s String
		 */
		virtual void toBuffer(const std::string s);
		
		
		/**
		 * \brief reads a string from the buffer
		 * \param s String
		 */
		virtual void fromBuffer(std::string& s);
		
		/**
		 * \brief writes a double to the buffer
		 * \param d double
		 */
		virtual void toBuffer(const double d)
		{
			*m_stream << d << " ";
		}
		
		
		/**
		 * \brief reads a double from the buffer
		 * \param d double
		 */
		virtual void fromBuffer(double& d)
		{
			*m_stream >> d;
		}
		
		/**
		 * \brief writes a float to the buffer
		 * \param f float
		 */
		virtual void toBuffer(const float f)
		{
			*m_stream << f << " ";
		}
		
		
		/**
		 * \brief reads a float from the buffer
		 * \param f float
		 */
		virtual void fromBuffer(float& f)
		{
			*m_stream >> f;
		}
		
		/**
		 * \brief writes an int to the buffer
		 * \param i int
		 */
		virtual void toBuffer(const int i)
		{
			*m_stream << i << " ";
		}
		
		
		/**
		 * \brief reads an int from the buffer
		 * \param i int
		 */
		virtual void fromBuffer(int& i)
		{
			*m_stream >> i;
		}
		
		/**
		 * \brief writes a short to the buffer
		 * \param s short
		 */
		virtual void toBuffer(const short s)
		{
			*m_stream << s << " ";
		}
		
		
		/**
		 * \brief reads a short from the buffer
		 * \param s short
		 */
		virtual void fromBuffer(short& s)
		{
			*m_stream >> s;
		}
		
		/**
		 * \brief writes a char to the buffer
		 * \param c char
		 */
		virtual void toBuffer(const char c)
		{
			// Save it as int
			// Saving as char creates issues with spaces, control characters etc.
			*m_stream << static_cast<int>(c) << " ";
		}
		
		
		/**
		 * \brief reads a char from the buffer
		 * \param c char
		 */
		virtual void fromBuffer(char& c)
		{
			int i;
			*m_stream >> i;
			c = static_cast<char>(i);
		}
		
		/**
		 * \brief writes an unsigned  char to the buffer
		 * \param c char
		 */
		virtual void toBuffer(const unsigned char c)
		{
			// Save it as int
			// Saving as char creates issues with spaces, control characters etc.
			*m_stream << static_cast<int>(c) << " ";
		}
		
		
		/**
		 * \brief reads an unsigned char from the buffer
		 * \param c char
		 */
		virtual void fromBuffer(unsigned char& c)
		{
			int i;
			*m_stream >> i;
			c = static_cast<char>(i);
		}
		
		/**
		 * \brief returns the number of bits read from stream so far
		 */
		virtual int readBits()
		{
			ERRORMSG("readBits is  not supported by StdStreamConv");
			return 0;
		}
	
		
		/**
		 * \brief returns the number of bits written to stream so far
		 */
		virtual int writeBits()
		{
			ERRORMSG("writeBits is  not supported by StdStreamConv");
			return 0;
		}
		
		
		/**
		 * \brief Returns the delay of the packet in milliseconds. Return 0 if the data was not sent via network.
		 */
		virtual float getDelay()
		{
			return 0;
		};
		
		
		/**
		 * \brief Prints a newline to the output. This is only used for stdlib streams.
		 */
		virtual void printNewline()
		{
			*m_stream << "\n";
		}
	
		/**
		 * \brief Returns the internal stream object
		 */
		std::iostream* getStream()
		{
			return m_stream;
		}
	private:
	
		/**
		* \brief IO stream that is used for writing and reading
		*/
		std::iostream* m_stream;
};


#endif

