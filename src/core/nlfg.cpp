/**
Copyright (c) 2010 David Athay

Permission is hereby granted, free of charge, to any person obtaining a copy
of this software and associated documentation files (the "Software"), to deal
in the Software without restriction, including without limitation the rights
to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
copies of the Software, and to permit persons to whom the Software is
furnished to do so, subject to the following conditions:

The above copyright notice and this permission notice shall be included in
all copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
THE SOFTWARE.
**/

#include "nlfg.h"

#include <enet/enet.h>
#include <string.h>
#include <sys/types.h>
#include <stdint.h>

const int MAX_QUEUE_SIZE = 32;

struct NLFG_Queue
{
    NLFG_Message *head;
} nlfgQueue;

ENetHost *host;
ENetPeer *peer;
int connected = 0;

void nlfg_expand(NLFG_Message *msg, int size)
{
    if (size > msg->size)
    {
        char *tempData;
        int oldSize = msg->size;

        while (msg->size < size)
        {
            msg->size = msg->size << 1;
        }
        tempData = (char*)malloc(oldSize);
        memcpy(tempData, msg->data, oldSize);
        free(msg->data);
        msg->data = (char*)malloc(msg->size);
        memcpy(msg->data, tempData, oldSize);
        free(tempData);
    }
}

int nlfg_addMessage(NLFG_Message *msg)
{
    if (nlfgQueue.head == 0)
    {
        nlfgQueue.head = msg;
        msg->child = 0;
    }
    else
    {
        NLFG_Message *next = nlfgQueue.head;
        while (next->child != 0)
        {
            next = next->child;
        }
        next->child = msg;
		msg->child = 0;
    }
    return 0;
}

int nlfg_init_client()
{
    enet_initialize();
    atexit(enet_deinitialize);
    peer = 0;

#ifdef ENET_VERSION
    // enet 1.3
    host = enet_host_create (NULL,
                    1 /* only allow 1 outgoing connection */,
		    0,
                    57600 / 8 /* 56K modem with 56 Kbps downstream bandwidth */,
                    14400 / 8 /* 56K modem with 14 Kbps upstream bandwidth */);
#else
    // enet 1.2
    host = enet_host_create (NULL,
                    1 /* only allow 1 outgoing connection */,
                    57600 / 8 /* 56K modem with 56 Kbps downstream bandwidth */,
                    14400 / 8 /* 56K modem with 14 Kbps upstream bandwidth */);
#endif
    nlfgQueue.head = 0;
    return 1;
}

int nlfg_init_server(unsigned int port)
{
    enet_initialize();
    atexit(enet_deinitialize);

    ENetAddress addr;
    addr.host = ENET_HOST_ANY;
    addr.port = port;

#ifdef ENET_VERSION
    // enet 1.3
    host = enet_host_create (&addr,
                    32 /* only allow 32 incoming connections */,
                    0, 0, 0);
#else
    // enet 1.2
    host = enet_host_create (&addr,
                    32 /* only allow 32 incoming connections */,
                    0, 0);
#endif

    nlfgQueue.head = 0;
    return 1;
}

void nlfg_init_packet(NLFG_Message *msg)
{
    msg->data = (char*)malloc(5);
    msg->size = 5;
    msg->position = 0;
    msg->id = 0;
    msg->reliability = RELIABLE;
    msg->child = 0;
}

unsigned int nlfg_connect(const char *hostname, unsigned int port)
{
    ENetAddress address;
    enet_address_set_host(&address, hostname);
    address.port = port;
#ifdef ENET_VERSION
    // enet 1.3
    peer = enet_host_connect(host, &address, 0, 1);
#else
    // enet 1.2
    peer = enet_host_connect(host, &address, 1);
#endif
    if (peer)
        return 1;
    return 0;
}

void nlfg_disconnect()
{
    if (peer)
        enet_peer_disconnect(peer, 0);
}

int nlfg_isConnected()
{
    return connected;
}

int nlfg_process()
{
    ENetEvent event;
    int packets = 0;

    // check for data
    while (enet_host_service(host, &event, 0) > 0)
    {
        switch (event.type)
        {
        case ENET_EVENT_TYPE_CONNECT:
            {
                connected = 1;
                NLFG_Message *msg = (NLFG_Message*)(malloc(sizeof(NLFG_Message)));
                msg->addr.peer = event.peer;
                msg->data = 0;
                msg->size = 0;
                msg->position = 0;
                msg->reliability = ENET_PACKET_FLAG_RELIABLE;
                msg->id = NLFG_CONNECTED;
                nlfg_addMessage(msg);
                ++packets;
            } break;

        case ENET_EVENT_TYPE_RECEIVE:
            {
                NLFG_Message *msg = (NLFG_Message*)(malloc(sizeof(NLFG_Message)));
                msg->addr.peer = event.peer;
                msg->data = (char*)malloc(event.packet->dataLength);
                memcpy(msg->data, event.packet->data, event.packet->dataLength);
                msg->size = event.packet->dataLength;
                msg->position = 0;
                msg->reliability = ENET_PACKET_FLAG_RELIABLE;
                msg->id = nlfg_readInteger(msg);
                nlfg_addMessage(msg);
                enet_packet_destroy (event.packet);
                ++packets;
            } break;

        case ENET_EVENT_TYPE_DISCONNECT:
            {
                connected = 0;
                NLFG_Message *msg = (NLFG_Message*)(malloc(sizeof(NLFG_Message)));
                msg->addr.peer = event.peer;
                msg->data = 0;
                msg->size = 0;
                msg->position = 0;
                msg->reliability = ENET_PACKET_FLAG_RELIABLE;
                msg->id = NLFG_DISCONNECTED;
                nlfg_addMessage(msg);
                ++packets;
            } break;
        }
    }

    return packets;

}

NLFG_Message* nlfg_getMessage()
{
    if (nlfgQueue.head == 0)
        return NULL;
    NLFG_Message *msg = nlfgQueue.head;
    nlfgQueue.head = msg->child;
    return msg;
}

unsigned int nlfg_getPosition(NLFG_Message *msg)
{
    return msg->position;
}

unsigned int nlfg_getSize(NLFG_Message *msg)
{
    return msg->size;
}

unsigned int nlfg_getId(NLFG_Message *msg)
{
    return msg->id;
}

void nlfg_setId(NLFG_Message *msg, unsigned int id)
{
    msg->id = id;
}

void nlfg_setReliability(NLFG_Message *msg, unsigned int rel)
{
    msg->reliability = rel;
}

char nlfg_readByte(NLFG_Message *msg)
{
    int c = -1;
    if (msg->position < msg->size)
    {
        c = (unsigned char) msg->data[msg->position];
    }
    msg->position += 1;
    return c;
}

int nlfg_readShort(NLFG_Message *msg)
{
    int num = -1;
    if (msg->position + 2 <= msg->size)
    {
        uint16_t t;
        memcpy(&t, msg->data + msg->position, 2);
        num = ENET_NET_TO_HOST_16(t);
    }
    msg->position += 2;
    return num;
}

int nlfg_readInteger(NLFG_Message *msg)
{
    int num = -1;
    if (msg->position + 4 <= msg->size)
    {
        uint32_t t;
        memcpy(&t, &msg->data[msg->position], 4);
        num = ENET_NET_TO_HOST_32(t);
    }
    msg->position += 4;
    return num;
}

float nlfg_readFloat(NLFG_Message *msg)
{
    float f = -1.0f;
    int i = nlfg_readInteger(msg);
    f = *((float*)(&i));
    return f;
}

double nlfg_readDouble(NLFG_Message *msg)
{
    double d = -1.0f;
    int i1 = nlfg_readInteger(msg);
    int i2 = nlfg_readInteger(msg);
    uint64_t i = (uint64_t)i1 << 32 | i2;
    d = *((double*)(&i));
    return d;
}

char* nlfg_readString(NLFG_Message *msg)
{
    char *s = (char*)(malloc(256));
    int pos = 0;

    while (msg->position < msg->size && msg->data[msg->position] != '\0' && pos < 255)
    {
        s[pos] = msg->data[msg->position];
        ++(msg->position);
        ++pos;
    }

    if (msg->data[msg->position] == '\0')
        ++(msg->position);

    return s;
}

void nlfg_readStringWithSize(NLFG_Message *msg, char *data, int size)
{
    if (msg->position + size > msg->size)
    {
        data = NULL;
        return;
    }

    memcpy(data, &msg->data[msg->position], size);

    msg->position += size;
}

void nlfg_writeByte(NLFG_Message *msg, char c)
{
    nlfg_expand(msg, msg->position + 1);
    msg->data[msg->position] = c;
    msg->position += 1;
}

void nlfg_writeShort(NLFG_Message *msg, int i)
{
    nlfg_expand(msg, msg->position + 2);
    uint32_t t = ENET_HOST_TO_NET_16(i);
    memcpy(msg->data + msg->position, &t, 2);
    msg->position += 2;
}

void nlfg_writeInteger(NLFG_Message *msg, int i)
{
    nlfg_expand(msg, msg->position + 4);
    uint32_t t = ENET_HOST_TO_NET_32(i);
    memcpy(&msg->data[msg->position], &t, 4);
    msg->position += 4;
}

void nlfg_writeFloat(NLFG_Message *msg, float f)
{
    unsigned int i = *((unsigned int *)(&f));
    nlfg_writeInteger(msg, i);
}

void nlfg_writeDouble(NLFG_Message *msg, double d)
{
    uint64_t i = *((uint64_t*)(&d));
    uint32_t i2 = (uint32_t)i;
    uint32_t i1 = (uint32_t)(i >> 32);
    nlfg_writeInteger(msg, i1);
    nlfg_writeInteger(msg, i2);
}

void nlfg_writeString(NLFG_Message *msg, const char *s)
{
    /* string requires null terminator */
    int size = strlen(s)+1;
    nlfg_expand(msg, msg->position + size);
    memcpy(msg->data + msg->position, s, size);
    msg->position += size;
}

void nlfg_writeStringWithSize(NLFG_Message *msg, const char *s, int size)
{
    nlfg_expand(msg, msg->position + size);
    memcpy(&msg->data[msg->position], s, size);
    msg->position += size;
}

void nlfg_destroyPacket(NLFG_Message *msg)
{
    free(msg->data);
    free(msg);
}

void nlfg_sendPacket(NLFG_Message *msg)
{
    if (msg && nlfg_isConnected())
    {
        char *data = (char*)malloc(msg->position + 4);
        memcpy(data, &msg->id, 4);
        memcpy(&data[4], msg->data, msg->position);
        ENetPacket *p = enet_packet_create(data, msg->position + 4, msg->reliability);
        enet_peer_send(peer, 0, p);
        enet_host_flush(host);
        free(data);
    }
}

void nlfg_sendPacketToClient(NLFG_Message *msg, struct NLFG_Address *addr)
{
    if (msg && nlfg_isConnected())
    {
        char *data = (char*)malloc(msg->position + 4);
        memcpy(data, &msg->id, 4);
        memcpy(&data[4], msg->data, msg->position);
        ENetPacket *p = enet_packet_create(data, msg->position + 4, msg->reliability);
        enet_peer_send(addr->peer, 0, p);
        enet_host_flush(host);
        free(data);
    }
}

void nlfg_copyAddress(struct NLFG_Address *dest, const struct NLFG_Address *src)
{
    dest->peer = src->peer;
}

int nlfg_matchAddress(const struct NLFG_Address *addr1, const struct NLFG_Address *addr2)
{
    if (addr1->peer->address.host != addr2->peer->address.host || addr1->peer->address.port != addr2->peer->address.port)
    {
        /* no match */
        return 0;
    }

    return 1;
}
