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

#ifndef NLFGNETWORKPACKET_H
#define NLFGNETWORKPACKET_H

#define ID_TIMESTAMP                    23

#include "networkpacket.h"
#include "nlfg.h"
#include <time.h>
#include "network.h"

/**
 * Class for data sent or received with NLFG
 */
class NLFGNetworkPacket : public NetworkPacket
{
	public:
		/**
		 * \brief Constructor
		 * Creates a new packet and writes the current time as timestamp
		 */
		NLFGNetworkPacket();

		/**
		* \brief Creates a new packet from a RakNet::Packet
		* \param packet RakNet Packet
		*/
		NLFGNetworkPacket(NLFG_Message* packet);

		/**
		* \brief writes raw data to the buffer
		* \param data raw data
		* \param size length of data in bytes
		*/
		virtual void toBuffer(const char* data, unsigned int size)
		{
			nlfg_writeStringWithSize(m_packet, data, size);
		}


		/**
		* \brief reads raw data from buffer
		* \param data output buffer for raw data
		* \param size length of data in bytes
		*/
		virtual void fromBuffer(char* data, unsigned int size)
		{
			nlfg_readStringWithSize(m_packet, data, size);
		}

		/**
		* \brief writes a string to the buffer
		* \param s String
		*/
		virtual void toBuffer(const std::string s)
		{
			toBuffer(static_cast<int>(s.size()));
			toBuffer(s.data(),s.size());
		}


		/**
		* \brief reads a string from the buffer
		* \param s String
		*/
		virtual void fromBuffer(std::string& s)
		{
			int len;
			fromBuffer(len);

			char * data = new char[len];
			fromBuffer(data,len);
			s.assign(data,len);
			delete data;
		}

		/**
		* \brief writes a double to the buffer
		* \param d double
		*/
		virtual void toBuffer(const double d)
		{
			nlfg_writeDouble(m_packet, d);
		}


		/**
		* \brief reads a double from the buffer
		* \param d double
		*/
		virtual void fromBuffer(double& d)
		{
			d = nlfg_readDouble(m_packet);
		}

		/**
		* \brief writes a float to the buffer
		* \param f float
		*/
		virtual void toBuffer(const float f)
		{
			nlfg_writeFloat(m_packet, f);
		}


		/**
		* \brief reads a float from the buffer
		* \param f float
		*/
		virtual void fromBuffer(float& f)
		{
			f = nlfg_readFloat(m_packet);
		}

		/**
		* \brief writes an int to the buffer
		* \param i int
		*/
		virtual void toBuffer(const int i)
		{
			nlfg_writeInteger(m_packet, i);
		}


		/**
		* \brief reads an int from the buffer
		* \param i int
		*/
		virtual void fromBuffer(int& i)
		{
			i = nlfg_readInteger(m_packet);
		}

		/**
		* \brief writes a short to the buffer
		* \param s short
		*/
		virtual void toBuffer(const short s)
		{
			nlfg_writeShort(m_packet, s);
		}


		/**
		* \brief reads a short from the buffer
		* \param s short
		*/
		virtual void fromBuffer(short& s)
		{
			s = nlfg_readShort(m_packet);
		}

		/**
		* \brief writes a char to the buffer
		* \param c char
		*/
		virtual void toBuffer(const char c)
		{
			nlfg_writeByte(m_packet, c);
		}


		/**
		* \brief reads a char from the buffer
		* \param c char
		*/
		virtual void fromBuffer(char& c)
		{
			c = nlfg_readByte(m_packet);
		}

        /**
        * \brief writes a char to the buffer
        * \param c char
        */
        virtual void toBuffer(const unsigned char c)
        {
            nlfg_writeByte(m_packet, c);
        }


        /**
        * \brief reads a char from the buffer
        * \param c char
        */
        virtual void fromBuffer(unsigned char& c)
        {
            c = nlfg_readByte(m_packet);
        }

		/**
		* \brief returns the number of bits read from stream so far
		*/
		virtual int readBits()
		{
			// the offset is for counteracting prepended internal data
			return nlfg_getPosition(m_packet) - 13;
		}


		/**
		* \brief returns the number of bits written to stream so far
		*/
		virtual int writeBits()
		{
			// the offset is for counteracting prepended internal data
			return nlfg_getPosition(m_packet) - 9;
		}

		/**
		* \brief returns the delay of the packet in milliseconds
		*/
		virtual float getDelay()
		{
			return std::max(0.0f,static_cast<float>(time(NULL))-static_cast<float>(m_timestamp));
		}

		/**
		 * \brief Returns the NLFG packet that was used to create the object
		 */
		NLFG_Message* getPacket() const
		{
			return m_packet;
		}

		/**
		 * \brief Returns the underlying bitstream (for internal use only)
		 */
		char* getData()
		{
			return m_packet->data;
		}
	private:
		/**
		* \brief stream for reading and writing the data
		*/
		char *m_data;
		int m_length;

		/**
		 * \brief Time when the packet was created
		 */
		unsigned int m_timestamp;

		/**
		 * \brief Pointer to the RakNet Packet that was used to create this object
		 */
		NLFG_Message* m_packet;
};

#endif
