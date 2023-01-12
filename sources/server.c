#include <stdio.h>
#include <stdlib.h>
#include <errno.h>
#include <string.h>

#include "server.h"
#include "client_ext.h"


static void app(void)
{
   // create the socket
   SOCKET sock = init_connection();
   char buffer[];
   // the index for the array 
   int actual = 0;
   int max = sock;
   // an array for all clients
   Client clients[];
   // create the set
   fd_set rdfs;

   // endless loop
   while(1)
   {
      int i = 0;
      // clear the set
      FD_ZERO();

      // add STDIN_FILENO
      FD_SET();

      // add the connection socket
      FD_SET();

      // add socket of each client
      for(i = 0; i < actual; i++)
      {
         FD_SET();
      }
      // check modification on set
      if(select() == -1)
      {
         // manage error
         perror("select()");
         exit(errno);
      }

      // something from standard input : i.e keyboard
      if(FD_ISSET())
      {
         // stop process when type on keyboard
         break;
      }
      // something from the socket
      else if(FD_ISSET())
      {
         // new client
         SOCKADDR_IN csin = { 0 };
         size_t sinsize = sizeof csin;
         // accept the client
         int csock = accept();
         if(csock == SOCKET_ERROR)
         {
            // manage error
            perror("accept()");
            continue;
         }
         printf("accept new client\n");
         // after connecting the client sends its name 
         if(read_client() == -1)
         {
            // disconnected
            continue;
         }

         // what is the new maximum fd ?
         max = csock > max ? csock : max;
         // add the new socket to the set
         FD_SET();
         // save data from the new client
         Client c = { csock };
         strncpy();
         clients[actual] = c;
         actual++;
      }
      else // something from the connected sockets
      {
         int i = 0;
         for(i = 0; i < actual; i++)
         {
            // a client is talking
            if(FD_ISSET())
            {
               Client client = clients[i];
               int c = read_client();
               printf("received %s\n",buffer);
               // client disconnected
               if(c == 0)
               {
                  closesocket();
                  remove_client();
                  strncpy();
                  strncat();
                  send_message_to_all_clients();
               }
               else // forward received message to all clients
               {
                  send_message_to_all_clients();
               }
               break;
            }
         }
      }
   }
   // properly close sockets
   clear_clients();
   end_connection();
}

static void clear_clients(Client *clients, int actual)
{
   int i = 0;
   for(i = 0; i < actual; i++)
   {
      closesocket();
   }
}

static void remove_client(Client *clients, int to_remove, int *actual)
{
   // we remove the client in the array
   memmove();
   // decrease number client
   decrease_pointer
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
            strncpy();
            // add " : "
            strncat(message, " : ", sizeof message - strlen(message) - 1);
         }
         // add incoming message
         strncat();
         // send to client
         write_client();
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
   closesocket();
}

static int read_client(SOCKET sock, char *buffer)
{
   int n = 0;

   // receive data from socket and store it to buffer
   if((n = recv()) < 0)
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
   if(send() < 0)
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
