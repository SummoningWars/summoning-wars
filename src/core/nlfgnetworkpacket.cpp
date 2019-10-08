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


#include "nlfgnetworkpacket.h"

NLFGNetworkPacket::NLFGNetworkPacket()
	: m_data(NULL)
{
	m_timestamp = time(NULL);
    m_packet = new NLFG_Message;
    nlfg_init_packet(m_packet);
    
	nlfg_writeByte(m_packet, ID_TIMESTAMP);
	nlfg_writeInteger(m_packet, m_timestamp);
	nlfg_setId(m_packet, ID_USER_PACKET_ENUM);
}

// NLFGNetworkPacket does not copy the data from RakNet packet
// direct content from packet is used instead
NLFGNetworkPacket::NLFGNetworkPacket(NLFG_Message* packet)
{
	m_data = packet->data;
	m_length = packet->size;

	if (nlfg_readByte(packet) == ID_TIMESTAMP)
	{
		m_timestamp = nlfg_readInteger(packet);
	}
	else
	{
		m_timestamp = time(NULL);
	}
	
	m_version = nlfg_readInteger(packet);	// this is written by Network::createPacket
	
	m_packet = packet;
}
