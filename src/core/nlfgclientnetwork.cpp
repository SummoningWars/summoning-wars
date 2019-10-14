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


#include "nlfgclientnetwork.h"
#include "nlfg.h"

#ifdef WIN32
#include <windows.h>
#endif

#include "math.h"

NLFGClientNetwork::NLFGClientNetwork()
	: ClientNetwork()
{
	m_server_address.peer = 0;
	m_status = NET_CLOSE;
    nlfg_init_client();
}

NLFGClientNetwork::~NLFGClientNetwork()
{
	kill();
}

void NLFGClientNetwork::serverConnect(const char* hostname, int req_port )
{
	srand ( time(NULL) );
	
	unsigned success = nlfg_connect(hostname, req_port);
	if (!success)
	{
        SW_DEBUG("error connecting to server");
		m_status = NET_ERROR;
	}
	else
	{
		m_status = NET_OK;
	}
}

void NLFGClientNetwork::serverDisconnect()
{
	SW_DEBUG("closing connection");
	nlfg_disconnect();
	// wait to deliver disconnect message
#ifdef WIN32
	Sleep( 10 );
#else
	usleep(10000 );
#endif
	m_status = NET_CLOSE;
}

void NLFGClientNetwork::kill()
{
	if (nlfg_isConnected())
	{
		nlfg_disconnect();
	}
}


void NLFGClientNetwork::update()
{
	// Receive all packets from server
	int packets = nlfg_process();
	unsigned int id;
	while (packets !=0)
	{
        NLFG_Message *packet = nlfg_getMessage();
		id = nlfg_getId(packet);
        
        if (id < ID_USER_PACKET_ENUM)
        {
            switch (id)
            {
                case NLFG_CONNECTED:
                    SW_DEBUG("connected to server");
					m_status = NET_CONNECTED;
                    break;
                case NLFG_DISCONNECTED:
                    SW_DEBUG("connection timed out");
					m_status =NET_TIMEOUT;
                    break;
            }
            
            nlfg_destroyPacket(packet);
        }
        else
        {
            
            DEBUGX("packet received");
            // create a new NetPacket from the Raknet packet
            // must not delete packet, because netpacket uses the packets internal data
            NetworkPacket* netpacket = new NLFGNetworkPacket(packet);
            m_received_packets.push(netpacket);
        }
        --packets;
    }
}


void NLFGClientNetwork::pushSlotMessage(const NetworkPacket* data, int slot, NetReliability reliability) 
{
	// check correct type
	const NLFGNetworkPacket* nlfgpacket = dynamic_cast<const NLFGNetworkPacket*>(data);
	if (nlfgpacket == 0)
	{
		ERRORMSG("Tried to send a NetworkPacket that is not a NLFGClientNetwork");
		return;
	}
	
  unsigned int rel;

	if (reliability == NET_RELIABLE)
	{
		rel = RELIABLE;
	}
	else if (reliability == NET_RELIABLE_ORDERED)
	{
		rel = RELIABLE;
	}
	else if (reliability == NET_UNRELIABLE)
	{
		rel = UNRELIABLE;
	}
	else
	{
		ERRORMSG("invalid value for reliability");
		return;
	}
    
    nlfg_setReliability(nlfgpacket->getPacket(), rel);
	
    nlfg_sendPacket(nlfgpacket->getPacket());
    
	// send it!
	//m_peer->Send(&bs,MEDIUM_PRIORITY,rel , true);
}

NetworkPacket* NLFGClientNetwork::createPacket()
{
	NetworkPacket* nlfgpacket = new NLFGNetworkPacket();
	
	// set the version number and write it to the packet
	nlfgpacket->setVersion(getPacketVersion());
	nlfgpacket->toBuffer(getPacketVersion()); 	// This number is read by the RakClientNetwork(packet) constructor
	
	return nlfgpacket;
}

void NLFGClientNetwork::deallocatePacket(NetworkPacket* packet)
{
	// check correct type
	NLFGNetworkPacket* nlfgpacket = dynamic_cast<NLFGNetworkPacket*>(packet);
	if (nlfgpacket == 0)
	{
		ERRORMSG("Tried to delete a NetworkPacket that is not a NLFGClientNetwork");
		delete packet;	// really delete it?
		return;
	}
	
	// first delete Packet, and then the object itself
	NLFG_Message* p = nlfgpacket->getPacket();
	nlfg_destroyPacket(p);
	delete packet;
}
