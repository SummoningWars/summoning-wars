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

#ifndef __SUMWARS_CORE_CHARCONV_H__
#define __SUMWARS_CORE_CHARCONV_H__

#include <algorithm>
#include <string>
#include <cstring>
#include <iostream>

/**
 * \brief Abstract Class for writing data structures into a stream class and vice versa
 */
class CharConv
{
	public:
	
		/**
		* \brief Standard Constructor, creates an empty object
		*/
		CharConv()
		:	m_version(0)
		{
			
		}
		
		/**
		* \brief Destructor
		*/
		virtual ~CharConv()
		{
			
		};
		
		
		/**
		* \brief writes raw data to the buffer
		* \param data raw data
		* \param size length of data in bytes
		*/
		virtual void toBuffer(const char* data, unsigned int size)=0;
		
		
		/**
		* \brief reads raw data from buffer
		* \param data output buffer for raw data
		* \param size length of data in bytes
		*/
		virtual void fromBuffer(char* data, unsigned int size)=0;
		
		
		/**
		* \brief writes a string to the buffer
		* \param s String
		*/
		virtual void toBuffer(const std::string & s)=0;
		
		
		/**
		* \brief reads a string from the buffer
		* \param s String
		*/
		virtual void fromBuffer(std::string& s) =0;
		
		/**
		* \brief writes a double to the buffer
		* \param d double
		*/
		virtual void toBuffer(const double d)=0;
		
		
		/**
		* \brief reads a double from the buffer
		* \param d double
		*/
		virtual void fromBuffer(double& d) =0;
		
		/**
		* \brief writes a float to the buffer
		* \param f float
		*/
		virtual void toBuffer(const float f)=0;
		
		
		/**
		* \brief reads a float from the buffer
		* \param f float
		*/
		virtual void fromBuffer(float& f) =0;
		
		/**
		* \brief writes an int to the buffer
		* \param i int
		*/
		virtual void toBuffer(const int i)=0;
		
		
		/**
		* \brief reads an int from the buffer
		* \param i int
		*/
		virtual void fromBuffer(int& i) =0;
		
		/**
		* \brief writes a short to the buffer
		* \param s short
		*/
		virtual void toBuffer(const short s)=0;
		
		
		/**
		* \brief reads a short from the buffer
		* \param s short
		*/
		virtual void fromBuffer(short& s) =0;
		
		/**
		* \brief writes a char to the buffer
		* \param c char
		*/
		virtual void toBuffer(const char c)=0;
		
		
		/**
		* \brief reads a char from the buffer
		* \param c char
		*/
		virtual void fromBuffer(char& c) =0;
		
		/**
		* \brief writes an unsigned char to the buffer
		* \param c char
		*/
		virtual void toBuffer(const unsigned char c)=0;
		
		
		/**
		* \brief reads an unsigned char from the buffer
		* \param c char
		*/
		virtual void fromBuffer(unsigned char& c) =0;
		
		/**
		 * \brief writes a bool to the buffer
		 * \param b bool
		 */
		void toBuffer(bool b)
		{
			toBuffer(static_cast<char>(b));
		}
		
		
		/**
		 * \brief reads a bool from the buffer
		 * \param b bool
		 */
		void fromBuffer(bool& b)
		{
			char c;
			fromBuffer(c);
			b = static_cast<bool>(c);
		}
		
		/**
		* \brief toBuffer function as operator
		*/
		template<typename T> 
		CharConv& operator<<(const T data)
		{
			toBuffer(data);
			return *this;
		}
		
		/**
		* \brief fromBuffer function as operator
		*/
		template<typename T> 
		CharConv& operator>>(T& data)
		{
			fromBuffer(data);
			return *this;
		}
	
		/**
		* \brief returns the number of bits read from stream so far
		*/
		virtual int readBits() = 0;
	
		
		/**
		* \brief returns the number of bits written to stream so far
		*/
		virtual int writeBits() =0;
		
		
		/**
		* \brief Returns the delay of the packet in milliseconds. Return 0 if the data was not sent via network.
		*/
		virtual float getDelay() = 0;
		
		
		/**
		* \brief Prints a newline to the output. This is only used for stdlib streams.
		*/
		virtual void printNewline()
		{
			
		}
		
		
		/**
		* \brief Returns the version of the program that was used to write this data
		*/
		int getVersion()
		{
			return m_version;
		}
		
		/**
		* \brief Setzt die Version
		* \param version sets version of the program that was used to write this data
		*/
		void setVersion(int version)
		{
			m_version = version;
		}
		
		
	protected:
		
		/**
		 * \brief Version number of the creating program
		 */
		int m_version;
	
};



#endif // __SUMWARS_CORE_CHARCONV_H__
