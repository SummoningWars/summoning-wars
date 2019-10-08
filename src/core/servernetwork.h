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

#ifdef SERVERNETWORK_H
#else
#define SERVERNETWORK_H

#include "network.h"
#include <queue>
#include <map>

// Debuggin
#include "debug.h"

class ServerNetwork;

/**
 * \class NetworkSlot
 * \brief Abstract class for a single server - client connection
 */
class NetworkSlot
{
	public:
		/**
		 * \brief Constructor
		 * \param server_network pointer to the parent ServerNetwork object
		 */
		NetworkSlot(ServerNetwork* server_network)
		{
			m_server_network = server_network;
		}

		/**
		 * \brief Destructor
		 */
		virtual ~NetworkSlot();

		/**
		 * \brief Adds the packet to the queue of received packets
		 * \param packet received packet
		 */
		void pushReceivedPacket(NetworkPacket* packet)
		{
			m_received_packets.push(packet);
		}

		/**
		 * \brief Removes the first packet from the receive queue and returns it. Returns NULL if the the receive queue is empty
		 */
		NetworkPacket* popReceivedPacket();

		/**
		 * \brief Returns the number of packets in the receive queue
		 */
		int numberMessages()
		{
			return m_received_packets.size();
		}
		
		/**
		 * \brief Returns the status of the slot
		 */
		NetStatus getStatus()
		{
			return m_status;
		}

		/**
		 * \brief Sets the status of the slot
		 * \param s neuer Zustand
		 */
		void setStatus(NetStatus s)
		{
			m_status = s;
		}
		

	private:
		/**
		 * \brief Queue of received Packets
		 */
		PacketQueue m_received_packets;

		/**
		 * \brief Status of the slot
		 */
		NetStatus m_status;
		
		/**
		 * \brief pointer to the parent ServerNetwork object
		 */
		ServerNetwork* m_server_network;
};

/**
 * \class ServerNetwork
 * \brief Abstract class for Network representation on server side
 */
class ServerNetwork: public Network
{
	public:
		/**
		 * \brief Constructor
		 * \param max_slots maximal number of client connections that are accepted by the server
		 */
		ServerNetwork(int max_slots);

		/**
		 * \brief Destructor
		 */
		virtual ~ServerNetwork();

		/**
		* \brief Initializes network interface, starts listening on the port
		* \param auth_port port for listening
		* \return network status
		*
		*/
		virtual NetStatus init( int auth_port ) =0;

		/**
		 * \brief Shuts down Network library and does all cleanup work
		 */
		virtual void kill();
		
		/**
		 * \brief Closes a connection to a client
		 * \param slot slot number
		 */
		virtual void clientDisconnect(int slot)=0;
		
		/**
		 * \brief returns if the slotnumber refers to a valid, connected slot
		 * \param slot slot number
		 */
		bool isConnectedSlot( int slot)
		{
			return (slot >=0 && slot < m_max_slots && m_slots[slot] != 0 && m_slots[slot]->getStatus() == NET_CONNECTED);
		}
		
		/**
		 * \brief Returns the Number of packets that are in the receive queue for the specified slot
		 * \return number of packets in the specified slot
		 * \param slot ID of a Slot
		 *
		 * Returns the Number of packets that are in the receive queue for the specified slot. 
		 */
		virtual int numberSlotMessages(int slot=0)
		{
			if (isConnectedSlot(slot))
			{
				return m_slots[slot]->numberMessages();
			}
			return 0;
		}
		
		
		/**
		 * \brief Returns the next received packet from the specified slot and removes it from the queue.
		 * \param data Received NetworkPacket, returns a NULL pointer, if packet queue is empty
		 * \param slot ID of a Slot
		 * 
		 * Returns the next received packet from the specified slot and removes it from the queue. If no packet was received from the specified slot, a NULL pointer is returned. Delete the Packet with \ref deallocatePacket at the end. 
		 */
		virtual void popSlotMessage( NetworkPacket* &data, int slot=0)
		{
			if (isConnectedSlot(slot) && m_slots[slot]->numberMessages()>0 )
			{
				data = m_slots[slot]->popReceivedPacket();
			}
			else
			{
				data = 0;
			}
		}
	
		/**
		 * \brief Return the status of a NetworkSlot
		 * \param slot ID of a slot
		 * \return status of a slot
		 *
		 */
		virtual NetStatus getSlotStatus( int slot=0 )
		{
			if (isConnectedSlot(slot))
			{
				return m_slots[slot]->getStatus();
			}
			return NET_ERROR;
		}
		
		/**
		 * \brief Returns the first element from the new login slot queue
		 */
		int popNewLoginSlot();
		
	protected:

		/**
		 * \brief Adds a new slot to the new login slot queue
		 * \param slot slot number
		 */
		void pushNewLoginSlot(int slot)
		{
			m_new_login_slots.push(slot);
		}

		/**
		* \brief Tells if the server is open
		*/
		bool m_active;

		/**
		* \brief Array of Connection slots
		*/
		std::vector<NetworkSlot*> m_slots;

		/**
		 * \brief maximal number of allowed connections
		 */
        int m_max_slots;

		/**
		 * \brief List of new clients, that have connected to the server, but have not been noticed by the world yet
		 */
		std::queue<int> m_new_login_slots;


		
};


#endif

