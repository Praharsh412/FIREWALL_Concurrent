#ifndef _server_h_
#define _server_h_

#include <iostream>
#include <vector>

#include <cstdio>
#include <cstdlib>
#include <cstring>

#include <sys/socket.h>
#include <netinet/in.h>

#include "mythread.h"
#include "client.h"
#include <pthread.h>
#include <thread>

#define PORT 30666

using namespace std;

class Server {

  private:
    static vector<Client> clients;

    //Socket stuff
    int serverSock, clientSock;
    static struct sockaddr_in serverAddr, clientAddr;
    char buff[256];
    static string ipaddr;
    static thread IP_check;
    static thread Port_check;
    static thread censor_check;

  public:
    Server();
    void AcceptAndDispatch(vector<string> Ip, vector<int> ports, vector<string> censored);
    static void * HandleClient(void *args);

  private:
    static void ListClients();
    static void SendToAll(char *message,int id);
    static int FindClientIndex(Client *c); 
};

#endif
