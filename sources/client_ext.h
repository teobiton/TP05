#ifndef CLIENT_EXT_H
#define CLIENT_EXT_H

#include "server.h"

typedef struct
{
   SOCKET sock;
   char name[BUF_SIZE];
}Client;

#endif /* guard */
