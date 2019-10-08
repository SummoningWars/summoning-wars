#include "raknet/MessageIdentifiers.h"
#include "raknet/RakNetworkFactory.h"
#include "raknet/RakPeerInterface.h"
#include "raknet/RakNetTypes.h"
#include <iostream>

using namespace std;

 
unsigned char GetPacketIdentifier(Packet *p)
{
	if ((unsigned char)p->data[0] == ID_TIMESTAMP)
		return (unsigned char) p->data[sizeof(unsigned char) + sizeof(unsigned long)];
	else
		return (unsigned char) p->data[0];
}


int main()
{
	RakPeerInterface* peer = RakNetworkFactory::GetRakPeerInterface();
	
	SocketDeskriptor sock();
	
	peer->Startup(1,30,&SocketDescriptor(), 1);
	
}