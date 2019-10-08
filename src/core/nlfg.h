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

#pragma once

#define RELIABLE (1 << 0)
#define UNRELIABLE (1 << 1)

#define NLFG_CONNECTED      1
#define NLFG_DISCONNECTED   2

#ifdef __cplusplus
 extern "C" {
 #endif

struct _ENetPeer;

struct NLFG_Address
{
    struct _ENetPeer *peer;
};

typedef struct _NLFG_Message
{
    unsigned int id;
    unsigned int position;
    unsigned int size;
    unsigned int reliability;
    struct NLFG_Address addr;
    char *data;
    struct _NLFG_Message *child;
} NLFG_Message;

/* NLFG Initialisation
   Returns whether it was successful
*/
int nlfg_init_client();
int nlfg_init_server(unsigned int port);

/* NLFG Init Message */
void nlfg_init_packet(NLFG_Message *msg);

/* NLFG Connect */
unsigned int nlfg_connect(const char *hostname, unsigned int port);

/* NLFG Disconnect */
void nlfg_disconnect();

/* NLFG Is connected
   Returns whether connection established
*/
int nlfg_isConnected();

/* NLFG Processing
   Returns if theres a message available
*/
int nlfg_process();

/* NLFG Get Message
   Returns a pointer to a message
*/
NLFG_Message* nlfg_getMessage();

/* NLFG Get Position */
unsigned int nlfg_getPosition(NLFG_Message *msg);

/* NLFG Get Size */
unsigned int nlfg_getSize(NLFG_Message *msg);

/* NLFG Get Id */
unsigned int nlfg_getId(NLFG_Message *msg);

/* NLFG Set Id */
void nlfg_setId(NLFG_Message *msg, unsigned int id);

/* NLFG Set Reliability */
void nlfg_setReliability(NLFG_Message *msg, unsigned int rel);

/* NLFG Read byte */
char nlfg_readByte(NLFG_Message *msg);

/* NLFG Read short */
int nlfg_readShort(NLFG_Message *msg);

/* NLFG Read int */
int nlfg_readInteger(NLFG_Message *msg);

/* NLFG Read float */
float nlfg_readFloat(NLFG_Message *msg);

/* NLFG Read Double */
double nlfg_readDouble(NLFG_Message *msg);

/* NLFG Read String */
char* nlfg_readString(NLFG_Message *msg);
/* Assume data is already allocated memory, pass in the size of data to read */
void nlfg_readStringWithSize(NLFG_Message *msg, char *data, int size);

/* NLFG Write byte */
void nlfg_writeByte(NLFG_Message *msg, char c);

/* NLFG Write int */
void nlfg_writeShort(NLFG_Message *msg, int i);

/* NLFG Write int */
void nlfg_writeInteger(NLFG_Message *msg, int i);

/* NLFG Write float */
void nlfg_writeFloat(NLFG_Message *msg, float f);

/* NLFG Write Double */
void nlfg_writeDouble(NLFG_Message *msg, double d);

/* NLFG Write String */
void nlfg_writeString(NLFG_Message *msg, const char *s);
void nlfg_writeStringWithSize(NLFG_Message *msg, const char *s, int size);

/* NLFG Destroy Packet */
void nlfg_destroyPacket(NLFG_Message *msg);

/* NLFG Send Packet */
void nlfg_sendPacket(NLFG_Message *msg);

/* NLFG Send Packet To Client */
void nlfg_sendPacketToClient(NLFG_Message *msg, struct NLFG_Address *addr);

/* Copy a NLFG Address */
void nlfg_copyAddress(struct NLFG_Address *dest, const struct NLFG_Address *src);

/* Match 2 NLFG Addresses */
int nlfg_matchAddress(const struct NLFG_Address *addr1, const struct NLFG_Address *addr2);

#ifdef __cplusplus
}
 #endif
