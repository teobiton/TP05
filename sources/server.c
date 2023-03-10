#include <stdio.h>
#include <stdlib.h>
#include <errno.h>
#include <string.h>

#include "server.h"
#include "client_ext.h"

#define NB_CLIENTS 5

static void app(void)
{
   // create the socket
   SOCKET sock = init_connection();
   char buffer[BUF_SIZE];
   // the index for the array 
   int actual = 0;
   int max = sock;
   // an array for all clients
   Client clients[NB_CLIENTS];
   // create the set
   fd_set rdfs;

   // endless loop
   while(1)
   {
      
      int i = 0;
      // clear the set
      FD_ZERO(&rdfs);

      // add STDIN_FILENO
      FD_SET(STDIN_FILENO, &rdfs);

      // add the connection socket
      FD_SET(sock, &rdfs);

      // add socket of each client
      for(i = 0; i < actual; i++)
      {
         FD_SET(clients[i].sock, &rdfs);
      }
      // check modification on set
      if(select(max + 1, &rdfs, NULL, NULL, NULL) == -1)
      {
         // manage error
         perror("select()");
         exit(errno);
      }

      // something from standard input : i.e keyboard
      if(FD_ISSET(STDIN_FILENO, &rdfs))
      {
         // stop process when type on keyboard
         break;
      }
      // something from the socket
      else if(FD_ISSET(sock, &rdfs))
      {
         // new client
         SOCKADDR_IN csin = { 0 };
         size_t sinsize = sizeof csin;
         // accept the client
         int csock = accept(sock, (struct sockaddr *) &csin, (socklen_t *) &sinsize);
         if(csock == SOCKET_ERROR)
         {
            // manage error
            perror("accept()");
            continue;
         }

         // after connecting the client sends its name 
         if(read_client(csock, buffer) == -1)
         {
            // disconnected
            continue;
         }

         // what is the new maximum fd ?
         max = csock > max ? csock : max;
         // add the new socket to the set
         FD_SET(csock, &rdfs);
         // save data from the new client
         Client c = { csock };
         strncpy(c.name, buffer, BUF_SIZE); /* copy the name (lou) */
         clients[actual] = c;
         actual++;
         printf("Client name: %s\n", buffer);
      }
      else // something from the connected sockets
      {
         int i = 0;
         for(i = 0; i < actual; i++)
         {
            // a client is talking
            if(FD_ISSET(clients[i].sock, &rdfs))
            {
               Client client = clients[i];
               int c = read_client(client.sock, buffer);
               
               // client disconnected
               if(c == 0)
               {
                  closesocket(client.sock);
                  remove_client(clients, i, &actual);
                  strncpy(buffer, client.name, BUF_SIZE - 1);
                  strncat(buffer, " disconnected.",  BUF_SIZE - strlen(buffer) - 1);
                  printf("%s disconnected.\n", client.name);
                  send_message_to_all_clients(clients, client, actual, buffer, 1);
               }
               else // forward received message to all clients
               {
                  printf("Received from %s : %s\n", client.name, buffer);
                  send_message_to_all_clients(clients, client, actual, buffer, 0);
               }
               break;
            }
         }
      }
   }
   // properly close sockets
   clear_clients(clients, actual);
   end_connection(sock);
}

static void clear_clients(Client *clients, int actual)
{
   int i = 0;
   for(i = 0; i < actual; i++)
   {
      closesocket(clients[i].sock);
   }
}

static void remove_client(Client *clients, int to_remove, int *actual)
{

   const size_t CLIENT_SIZE = sizeof(Client);
   void * dest = clients + to_remove;
   void * src = clients + (to_remove + 1);
   size_t n_bytes = (*actual - to_remove - 1)*CLIENT_SIZE;

   memmove(dest, src, n_bytes);

   // decrease number client
   (*actual)--;

}

static void send_message_to_all_clients(Client *clients, Client sender, int actual, const char *buffer, char from_server)
{
   int i = 0;
   char message[BUF_SIZE];
   message[0] = 0;
   for(i = 0; i < actual; i++)
   {
      // we don't send message to the sender
      if(sender.sock != clients[i].sock)
      {
         if(from_server == 0)
         {
            // create string and set the client name
            strncpy(message, sender.name, BUF_SIZE - 1);
            // add " : "
            strncat(message, " : ", sizeof message - strlen(message) - 1);
         }
         // add incoming message
         strncat(message, buffer , sizeof message - strlen(message) - 1);
         // send to client
         write_client(clients[i].sock, message);
      }
   }
}

static int init_connection(void)
{
   // create socket
   SOCKET sock = socket(AF_INET, SOCK_STREAM, 0);
   SOCKADDR_IN sin = { 0 };

   // check the return value
   if(sock == INVALID_SOCKET)
   {
      // manage error
      perror("socket()");
      exit(errno);
   }

   // initialize sockaddr_in structure
   sin.sin_addr.s_addr = htonl(INADDR_ANY);
   sin.sin_port = htons(PORT);
   sin.sin_family = AF_INET;

   // bind the socket
   if(bind(sock,(SOCKADDR *) &sin, sizeof sin) == SOCKET_ERROR)
   {
      perror("bind()");
      exit(errno);
   }

   // listen for incoming clients
   if(listen(sock, MAX_CLIENTS) == SOCKET_ERROR)
   {
      // manage error
      perror("listen()");
      exit(errno);
   }
   
   return sock;
}

static void end_connection(int sock)
{
   closesocket(sock);
}

static int read_client(SOCKET sock, char *buffer)
{
   int n = 0;

   // receive data from socket and store it to buffer
   if((n = recv(sock, buffer, BUF_SIZE, 0)) < 0)
   {
      // manage error
      perror("recv()");
      // if recv error we disonnect the client
      n = 0;
   }
   // add \0 as the last character to end the ASCII flow
   buffer[n] = 0;

   return n;
}

static void write_client(SOCKET sock, const char *buffer)
{
   // send data from buffer to socket
   // TODO change flag
   if(send(sock, buffer, strlen(buffer), 0) < 0)
   {
      // manage error
      perror("send()");
      exit(errno);
   }
}

int main(int argc, char **argv)
{
   
   // start application
   app();

   return EXIT_SUCCESS;
}
