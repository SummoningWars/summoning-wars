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

#ifndef __SUMWARS_CORE_NLFGSERVERNETWORK_H__
#define __SUMWARS_CORE_NLFGSERVERNETWORK_H__

#include "servernetwork.h"
#include "nlfgnetworkpacket.h"

#include <queue>
#include <map>

// Debuggin
#include "debug.h"

/**
 * \brief Class for nlfg server-client connection
 */
class NLFGNetworkSlot : public NetworkSlot
{
	public:
		/**
		 * \brief Initialises the slot
		 * \param client_address Network address of the client
		 * \param peer Network server interface class
		 * \param server_network server network wrapper class
		 */
		NLFGNetworkSlot(ServerNetwork *server_network, NLFG_Address client_address)
			: NetworkSlot(server_network)
		{
			nlfg_copyAddress(&m_system_address, &client_address);
		}

		/**
		 * \fn ~NLFGNetworkSlot()
		 * \brief Destructor
		 */
		~NLFGNetworkSlot()
		{
		}

		
		/**
		 * \fn PlayerID getPlayerID()
		 * \brief Gibt die Adresse des mit diesem Slot verbundenen Client aus
		 */
		NLFG_Address getPlayerID()
		{
			return m_system_address;
		}

	private:
		/**
		 * \brief Network address of the client that is connected to this slot
		 */
		NLFG_Address m_system_address;
};

/**
 * \class NLFGServerNetwork
 * \brief \brief server-side network abstraction layer implemented with NLFG
 */
class NLFGServerNetwork: public ServerNetwork
{
	public:
		/**
		 * \fn NLFGServerNetwork(int max_slots)
		 * \brief Constructor
		 * \param max_slots maximal number of client connections that are accepted by the server
		 */
		NLFGServerNetwork(int max_slots);

		/**
		 * \fn ~NLFGServerNetwork()
		 * \brief Destructor
		 */
		virtual ~NLFGServerNetwork();

		/**
		 * \brief Initializes network interface, starts listening on the port
		 * \param auth_port port for listening
		 * \return network status
		 *
		 */
		virtual NetStatus init( int auth_port );

		/**
		 * \fn void kill()
		 * \brief Shuts down network connections
		 */
		virtual void kill();
		
		/**
		 * \brief Closes a connection to a client
		 * \param slot slot number
		 */
		virtual void clientDisconnect(int slot);

		/**
		 * \fn  void update()
		 * \brief Updates internal state of the wrapper. This function is called once per frame.
		 */
		virtual void update();

		/**
		 * \brief Sends a packet to the specified slot
		 * \param data Network packet to be sent
		 * \param slot Slot the packet is sent to
		 * \param reliability Reliability of the packet
		 * 
		 * Sends the packet to the specified slot. The slot number is only relevant on server side, client network class will ignore it and send the packet to the server. The packet is still valid after this operation and may be sent to other slots.
		 */
		virtual void pushSlotMessage(const NetworkPacket* data, int slot=0, NetReliability reliability= NET_RELIABLE) ;
		
		/**
		 * \brief creates a new NetworkPacket
		 * \return new NetworkPacket
		 */
		virtual NetworkPacket* createPacket();
		
		/**
		 * \param packet packet to be deleted
		 * \brief Deletes the packet.
		 * Deletes the packet. Always use this function to delete NetworkPackets. The pointer is invalid after this operation.
		 */
		virtual void deallocatePacket(NetworkPacket* packet);
		
	private:

		/**
		 * \brief Returns Identifier of a Raknet Packet
		 * \param p Raknet Package
		 */
		unsigned char getPacketIdentifier(NLFG_Message *p)
		{
			unsigned char timestamp = nlfg_readByte(p);
			if (timestamp == ID_TIMESTAMP)
				return nlfg_readInteger(p);
			else
				return timestamp;
		}

		/**
		 * \fn int insertNewSlot(PlayerID addr)
		 * \brief Fuegt einen neuen Client hinzu
		 * \param addr Netzwerkadresse des Client
		 */
		int insertNewSlot(NLFG_Address addr);

		/**
		 * \fn int getSlotByAddress(PlayerID address)
		 * \brief ermittelt auf der Netzwerkadresse einen Client den Slot auf dem er angemeldet ist
		 * \param address Netzwerkadresse des Client
		 */
		int getSlotByAddress(NLFG_Address address);
};


#endif // __SUMWARS_CORE_NLFGSERVERNETWORK_H__
