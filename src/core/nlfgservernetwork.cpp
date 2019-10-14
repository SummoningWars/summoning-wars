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



#include "nlfgservernetwork.h"

///////////////////////////////////////////
// NLFGServerNetwork functions

NLFGServerNetwork::NLFGServerNetwork(int max_slots)
	: ServerNetwork(max_slots)
{
	
}

NetStatus NLFGServerNetwork::init( int auth_port )
{
    if (nlfg_init_server(auth_port) == 1)
    {
    	return NET_OK;
    }

	return NET_ERROR;
}

NLFGServerNetwork::~NLFGServerNetwork()
{
	kill();
}

void NLFGServerNetwork::kill()
{
	nlfg_disconnect();
	ServerNetwork::kill();
}

void NLFGServerNetwork::clientDisconnect(int slot)
{
	if (isConnectedSlot(slot))
	{
		//m_peer->remove(static_cast<NLFGNetworkSlot*>(m_slots[slot])->getPlayerID());
		delete m_slots[slot];
		m_slots[slot]=0;
	}
}

void NLFGServerNetwork::update()
{
	int packets = nlfg_process();
	unsigned int id;
	int slot;
	while (packets !=0)
	{
        NLFG_Message *packet = nlfg_getMessage();
		id = nlfg_getId(packet);
        slot = getSlotByAddress(packet->addr);

		if (id <  ID_USER_PACKET_ENUM)
		{
			switch(id)
			{
				case NLFG_CONNECTED:
					slot = insertNewSlot(packet->addr);
					pushNewLoginSlot(slot);
					SW_DEBUG("connection accepted for slot %i", slot);
					break;

				case NLFG_DISCONNECTED:
					SW_DEBUG("slot %i disconnected",slot);
					delete m_slots[slot];
					m_slots[slot]=0;
					break;
					
				default:
					SW_DEBUG("unknown id: %i",id);
					break;
			}

			nlfg_destroyPacket(packet);
		}
		else
		{
			if (slot != NOSLOT)
			{
				// create a new NetPacket from the Raknet packet
				// must not delete packet, because netpacket uses the packets internal data
				NetworkPacket* netpacket = new NLFGNetworkPacket(packet);
				m_slots[slot]->pushReceivedPacket(netpacket);
			}
			else
			{
				ERRORMSG("Received message from unknown sender");
				nlfg_destroyPacket(packet);
			}
		}
		--packets;
	}
}

int  NLFGServerNetwork::insertNewSlot(NLFG_Address address)
{
	for (int i=0; i < m_max_slots; i++)
	{
		if (m_slots[i] == 0)
		{
			m_slots[i] = new NLFGNetworkSlot(this, address);
			m_slots[i]->setStatus(NET_CONNECTED);
			return i;
		}
	}
	return NOSLOT;
}

int NLFGServerNetwork::getSlotByAddress(NLFG_Address address)
{
	for (int i=0;i< m_max_slots;i++)
	{
		if (m_slots[i] == 0)
			continue;
		
        NLFG_Address addr = static_cast<NLFGNetworkSlot*>(m_slots[i])->getPlayerID();
		if (m_slots[i] != 0  && nlfg_matchAddress(&addr, &address))
		{
			return i;
		}
	}
	return NOSLOT;
}


void NLFGServerNetwork::pushSlotMessage(const NetworkPacket* p, int slot, NetReliability reliability)
{
	if (!isConnectedSlot(slot))
	{
		return;
	}

	// check correct type
	const NLFGNetworkPacket* nlfgpacket = dynamic_cast<const NLFGNetworkPacket*>(p);
	if (nlfgpacket == 0)
	{
		ERRORMSG("Tried to send a NetworkPacket that is not a NLFGNetworkPacket");
		return;
	}
	
	// convert reliability into raknet enums
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
    
    NLFG_Address addr = static_cast<NLFGNetworkSlot*>(m_slots[slot])->getPlayerID();
    nlfg_sendPacketToClient(nlfgpacket->getPacket(), &addr);
	
	//m_peer->Send(&bs, MEDIUM_PRIORITY, rel, 0, static_cast<NLFGNetworkSlot*>(m_slots[slot])->getPlayerID(), false);
}


NetworkPacket* NLFGServerNetwork::createPacket()
{
	NetworkPacket* nlfgpacket = new NLFGNetworkPacket();
	
	// set the version number and write it to the packet
	nlfgpacket->setVersion(getPacketVersion());
	nlfgpacket->toBuffer(getPacketVersion()); 	// This number is read by the NLFGNetworkPacket(packet) constructor
	
	return nlfgpacket;
}

void NLFGServerNetwork::deallocatePacket(NetworkPacket* packet)
{
	// check correct type
	NLFGNetworkPacket* nlfgpacket = dynamic_cast<NLFGNetworkPacket*>(packet);
	if (nlfgpacket == 0)
	{
		ERRORMSG("Tried to delete a NetworkPacket that is not a NLFGNetworkPacket");
		delete packet;	// really delete it?
		return;
	}
	
	// first delete NLFG Packet, and then the object itself
	NLFG_Message* p = nlfgpacket->getPacket();
	nlfg_destroyPacket(p);
	delete packet;
}
